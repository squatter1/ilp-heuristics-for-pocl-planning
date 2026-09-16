// Copyright (C) 2024 Scott Howsam
//
// This file is part of VHPOP. It was added to VHPOP as part of the
// project "ILP-Based Delete Relaxation Heuristics for Partial Order
// Causal Link (POCL) Planning" (ANU COMP3770, 2023--2024) and is
// distributed under the same licence as the rest of VHPOP.
//
// VHPOP is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// VHPOP is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
// License for more details.
//
// You should have received a copy of the GNU General Public License
// along with VHPOP; if not, write to the Free Software Foundation,
// Inc., #59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// ILP creation and solving functions for the ILP and ILPC heuristics.

#include "ilp.h"
#include "actions.h"
#include "formulas.h"
#include "problems.h"
#include "plans.h"

#include <iostream>
#include <set>
#include <string>
#include <algorithm>
#include <cmath>

#include <ilcplex/ilocplex.h>

size_t IlpAction::next_id = 0;

IlpAction::IlpAction(const std::string& name)
    : id_(next_id++),
      name_(name) { }

IlpAction::IlpAction(const Action& action, const std::string& name)
    : id_(next_id++),
      name_(name) {
  // Iterate through the conjuncts in the preconditions
  const Formula& condition_formula = action.condition();
  const Conjunction* precondition_conjunctions = dynamic_cast<const Conjunction*>(&condition_formula);
  if (precondition_conjunctions) {
    // Access conjuncts_ directly as member variable
    for (FormulaList::const_iterator it = precondition_conjunctions->conjuncts().begin();
         it != precondition_conjunctions->conjuncts().end(); ++it) {
      const Formula* conjunct = *it;
      // Get the atom
      const Atom* atom = dynamic_cast<const Atom*>(conjunct);
      if (atom) {
        add_condition(PredicateTable::name((*atom).predicate()));
      } else {
        std::cerr << "Not an atom ERROR" << std::endl;
        continue; // Not an atom
      }
    }
  } else {
    // Check if the condition is an atom
    const Atom* atom = dynamic_cast<const Atom*>(&condition_formula);
    if (atom) {
      add_condition(PredicateTable::name((*atom).predicate()));
    } else {
      condition_formula.print(std::cerr, 0, Bindings::EMPTY);
      std::cerr << "Not a conjunction ERROR" << std::endl;
      return; // Not a conjunction
    }
  }

  // Iterate through the EffectList
  const EffectList& effect_list = action.effects();
  for (EffectList::const_iterator ei = effect_list.begin(); ei != effect_list.end();
       ei++) {
    // Get the literal of this effect
    const Literal& literal = (*ei)->literal();
    // Get the atom of this literal
    const Atom* atom = &literal.atom();
    // Check for negation
    const Negation* negation = dynamic_cast<const Negation*>(&literal);
    if (negation) {
      add_neg_effect(PredicateTable::name((*atom).predicate()));
    } else {
      add_pos_effect(PredicateTable::name((*atom).predicate()));
    }
  }
}

IlpAction::~IlpAction() { }

void IlpAction::add_condition(const std::string condition) {
  conditions_.insert(condition);
}

void IlpAction::add_pos_effect(const std::string effect) {
  posEffects_.insert(effect);
}

void IlpAction::add_neg_effect(const std::string effect) {
  negEffects_.insert(effect);
}

void IlpAction::print(std::ostream& os, std::string prefix) const {
  os << prefix << "ILP Action Name: " << name_ << std::endl;
  // Print the conditions
  os << prefix << "Conditions:";
  for (std::set<std::string>::const_iterator ai = conditions_.begin();
       ai != conditions_.end(); ai++) {
    os << ' ' << *ai;
  }
  os << std::endl;
  // Print the positive effect atoms
  os << prefix << "Add Effects:";
  for (std::set<std::string>::const_iterator ai = posEffects_.begin();
       ai != posEffects_.end(); ai++) {
    os << ' ' << *ai;
  }
  os << std::endl;
  // Print the negative effect atoms
  os << prefix << "Del Effects:";
  for (std::set<std::string>::const_iterator ai = negEffects_.begin();
       ai != negEffects_.end(); ai++) {
    os << ' ' << *ai;
  }
  os << std::endl;
}

IlpProblem::IlpProblem(const Problem& problem)
    : name_(problem.name()) {
  // Add the initial atoms
  AtomSet init_atoms = problem.init_atoms();
  for (AtomSet::const_iterator ai = init_atoms.begin();
       ai != init_atoms.end(); ai++) {
    add_init_prop(PredicateTable::name((*ai)->predicate()));
  }

  // Add the goal atoms
  const Formula& goal_formula = problem.goal();
  const Conjunction* goal_conjunctions = dynamic_cast<const Conjunction*>(&goal_formula);
  if (goal_conjunctions) {
    // Access conjuncts_ directly as member variable
    for (FormulaList::const_iterator it = goal_conjunctions->conjuncts().begin();
         it != goal_conjunctions->conjuncts().end(); ++it) {
      const Formula* conjunct = *it;
      // Get the atom
      const Atom* atom = dynamic_cast<const Atom*>(conjunct);
      if (atom) {
        add_goal_prop(PredicateTable::name((*atom).predicate()));
      } else {
        std::cerr << "Not an atom ERROR" << std::endl;
        continue; // Not an atom
      }
    }
  } else {
    // Check if the goal is an atom
    const Atom* atom = dynamic_cast<const Atom*>(&goal_formula);
    if (atom) {
      add_goal_prop(PredicateTable::name((*atom).predicate()));
    } else {
      std::cerr << "Not a conjunction ERROR" << std::endl;
      return; // Not a conjunction
    }
  }

  // Add the propositions
  const std::map<std::string, Predicate> predicates = problem.domain().predicates().predicates();
  // Iterate through the atoms
  for (std::map<std::string, Predicate>::const_iterator ai =
           predicates.begin();
       ai != predicates.end(); ai++) {
    add_prop((*ai).first);
  }

  // Add the actions
  for (std::map<std::string, const ActionSchema*>::const_iterator ai =
             problem.domain().actions().begin();
         ai != problem.domain().actions().end(); ai++) {
    add_action(new IlpAction(*(*ai).second));
  }
}

IlpProblem::~IlpProblem() { 
  // Delete all created action pointers
  for (std::map<std::string, IlpAction*>::const_iterator ai =
           actions_.begin();
       ai != actions_.end(); ai++) {
    delete (*ai).second;
  }
}

void IlpProblem::add_action(IlpAction* action) {
  actions_[action->name()] = action;
}

IlpAction* IlpProblem::get_action(const std::string& name) const {
  std::map<std::string, IlpAction*>::const_iterator ai =
      actions_.find(name);
  if (ai != actions_.end()) {
    return (*ai).second;
  } else {
    return NULL;
  }
}

void IlpProblem::add_prop(std::string prop) {
  props_.insert(prop);
}

void IlpProblem::add_init_prop(std::string prop) {
  init_.insert(prop);
}

void IlpProblem::add_goal_prop(std::string prop) {
  goal_.insert(prop);
}

std::ostream& operator<<(std::ostream& os, const IlpProblem& p) {
  os << "ILP Problem Name: " << p.name() << std::endl;
  // Print the list of all propositions
  os << "Propositions:";
  for (std::set<std::string>::const_iterator ai = p.props().begin();
       ai != p.props().end(); ai++) {
    os << ' ' << *ai;
  }
  os << std::endl;
  // Print the initial state
  os << "Initial State:";
  for (std::set<std::string>::const_iterator ai = p.init().begin();
       ai != p.init().end(); ai++) {
    os << ' ' << *ai;
  }
  os << std::endl;
  // Print the goal state
  os << "Goal State:";
  for (std::set<std::string>::const_iterator ai = p.goal().begin();
       ai != p.goal().end(); ai++) {
    os << ' ' << *ai;
  }
  os << std::endl;
  // Print the actions
  os << "Actions:" << std::endl;
  for (std::map<std::string, IlpAction*>::const_iterator ai =
             p.actions_.begin();
       ai != p.actions_.end(); ai++) {
    os << (*ai).first << std::endl;
    (*ai).second->print(os, "  ");
  }
  return os;
}

IlpPlan::IlpPlan(const Plan& plan)
    : id_(plan.serial_no()) {
  // Add the plan steps
  const Chain<Step>* steps = plan.steps();
  for (const Chain<Step>* sc = steps; sc != NULL; sc = sc->tail) {
    // If the action name is <init 0> or <goal 0> then skip it
    const Action& action = sc->head.action();
    if (action.name() == "<init 0>" || action.name() == "") {
      continue;
    }

    add_step(sc->head.id(), new IlpAction(action, action.name() + '-' + std::to_string(sc->head.id())));
  }
  // Add the plan causal links
  const Chain<Link>* links = plan.links();
  for (const Chain<Link>* lc = links; lc != NULL; lc = lc->tail) {
    const Link& link = lc->head;
    add_causal_link(link.from_id(), link.to_id(), PredicateTable::name((*(&link.condition().atom())).predicate()));
  }
  // Add the plan orderings
  const BinaryOrderings& orderings = dynamic_cast<const BinaryOrderings&>(plan.orderings());
  for (size_t i = 1; i <= orderings.size(); i++) {
    for (size_t j = 1; j <= orderings.size(); j++) {
      if (orderings.before(i, j)) {
        add_ordering(i, j);
      }
    }
  }
}

IlpPlan::~IlpPlan() { 
  // Delete all created action pointers
  for (std::map<size_t, IlpAction*>::const_iterator ai =
           steps_.begin();
       ai != steps_.end(); ai++) {
    delete (*ai).second;
  }
}

void IlpPlan::add_step(const size_t id, IlpAction* action) {
  steps_[id] = action;
}

IlpAction* IlpPlan::get_action(const size_t id) const {
  std::map<size_t, IlpAction*>::const_iterator ai =
      steps_.find(id);
  if (ai != steps_.end()) {
    return (*ai).second;
  } else {
    return NULL;
  }
}

void IlpPlan::add_causal_link(const size_t from, const size_t to, const std::string& condition) {
  links_.insert(std::make_pair(from, std::make_pair(to, condition)));
}

void IlpPlan::clear_causal_links() {
  links_.clear();
}

void IlpPlan::add_ordering(const size_t before, const size_t after) {
  orderings_.insert(std::make_pair(before, after));
}

std::ostream& operator<<(std::ostream& os, const IlpPlan& p) {
  os << "ILP Plan ID: " << p.serial_no() << std::endl;
  // Print the steps
  os << "Steps:" << std::endl;
  for (std::map<size_t, IlpAction*>::const_iterator ai =
             p.steps_.begin();
       ai != p.steps_.end(); ai++) {
    os << "Step ID: " << (*ai).first << std::endl;
    (*ai).second->print(os, "  ");
  }
  // Print the causal links
  os << "Causal Links:" << std::endl;
  for (std::multimap<size_t, std::pair<size_t, std::string>>::const_iterator ai =
             p.links_.begin();
       ai != p.links_.end(); ai++) {
    os << "  " << (*ai).second.second << ": ";
    if ((*ai).first == 0) {
      os << "INIT -> ";
    } else if ((*ai).first == Plan::GOAL_ID) {
      os << "GOAL -> ";
    } else {
      IlpAction* from = p.get_action((*ai).first);
      os << "  " << from->name() << " -> ";
    }
    if ((*ai).second.first == 0) {
      os << "INIT" << std::endl;
    } else if ((*ai).second.first == Plan::GOAL_ID) {
      os << "GOAL" << std::endl;
    } else {
      IlpAction* to = p.get_action((*ai).second.first);
      os << to->name() << std::endl;
    }
  }
  // Print the orderings
  os << "Orderings:" << std::endl;
  for (std::set<std::pair<size_t, size_t>>::const_iterator ai =
             p.orderings_.begin();
       ai != p.orderings_.end(); ai++) {
    if ((*ai).first == 0) {
      os << "  INIT -> ";
    } else if ((*ai).first == Plan::GOAL_ID) {
      os << "  GOAL -> ";
    } else {
      IlpAction* before = p.get_action((*ai).first);
      os << "  " << before->name() << " -> ";
    }
    if ((*ai).second == 0) {
      os << "INIT" << std::endl;
    } else if ((*ai).second == Plan::GOAL_ID) {
      os << "GOAL" << std::endl;
    } else {
      IlpAction* after = p.get_action((*ai).second);
      os << after->name() << std::endl;
    }
  }
  return os;
}

IlpNode::IlpNode(const Problem& problem, const Plan& plan)
    : id_(plan.serial_no()) {
  problem_ = new IlpProblem(problem);
  plan_ = new IlpPlan(plan);
  remove_causal_links();
}

IlpNode::~IlpNode() { 
  // Delete the plan and problem
  delete problem_;
  delete plan_;
}

// Variables of the ILP and ILPC models of a node
struct IlpVars {
  IloModel model;
  std::map<std::string, int> prop, propTime, action, actionTime, stepTime, addEffect;
  IloArray<IloNumVarArray> propVars, propTimeVars, actionVars, actionTimeVars, addEffectVars;
  IloNumVarArray stepTimeVars;
  size_t upperBound;
  size_t M;

  IlpVars(IloEnv env)
      : model(env), propVars(env), propTimeVars(env), actionVars(env), actionTimeVars(env), addEffectVars(env), stepTimeVars(env) { }

  // Creates the variables and objective, clamping ub to the default horizon
  void build(const IlpNode& n, bool lp_relax, size_t lb, size_t& ub, std::ostream& os, short int verbosity);
  // Constraints of the exact delete relaxation
  void add_ilp_constraints(const IlpNode& n);
  // Constraints of the counting relaxation
  void add_ilpc_constraints(const IlpNode& n);
  // Solves the model, printing the relaxed plan at verbosity 1 and all variables at verbosity 3
  size_t run(const IlpNode& n, std::ostream& os, short int verbosity, size_t lb, size_t seconds);
};

void IlpVars::build(const IlpNode& n, bool lp_relax, size_t lb, size_t& ub, std::ostream& os, short int verbosity) {
  IloEnv env = model.getEnv();
  const IlpProblem* problem = n.problem();
  const IlpPlan* plan = n.plan();
  const IloNumVar::Type boolType = lp_relax ? IloNumVar::Float : IloNumVar::Bool;
  const IloNumVar::Type intType = lp_relax ? IloNumVar::Float : IloNumVar::Int;

  // Each deletion of a prop by a plan step adds an instance of that prop
  std::map<std::string, int> propInstances;
  for (std::set<std::string>::const_iterator ai = problem->props().begin();
       ai != problem->props().end(); ai++) {
    propInstances[(*ai)] = 1;
  }
  int planDelEffects = 0;
  for (std::map<size_t, IlpAction*>::const_iterator ai = plan->steps().begin();
       ai != plan->steps().end(); ai++) {
    planDelEffects += (*ai).second->neg_effects().size();
    for (std::set<std::string>::const_iterator ei = (*ai).second->neg_effects().begin();
         ei != (*ai).second->neg_effects().end(); ei++) {
      propInstances[(*ei)]++;
    }
  }

  // Use upper bound of $min(|A|,|p|)+|del_P|+|P|$ if none is provided
  const size_t steps = plan->steps().size();
  const size_t maxBound = std::min(problem->actions().size(), problem->props().size()) + planDelEffects + problem->props().size();
  if (ub > maxBound || ub + steps > maxBound) {
    ub = maxBound > steps ? maxBound - steps : 0;
  }
  upperBound = ub + steps;
  M = upperBound + 2; // + 2 to avoid issues where we are comparing times with a difference of 1
  if (verbosity >= 2) {
    os << "Upper bound: " << upperBound << std::endl;
  }

  // Create the prop and propTime variables
  for (std::set<std::string>::const_iterator ai = problem->props().begin();
       ai != problem->props().end(); ai++) {
    propVars.add(IloNumVarArray(env, propInstances[(*ai)], 0, 1, boolType));
    prop["PA-" + (*ai)] = propVars.getSize() - 1;
    propTimeVars.add(IloNumVarArray(env, propInstances[(*ai)], 0, upperBound, intType));
    propTime["PT-" + (*ai)] = propTimeVars.getSize() - 1;
  }

  // Create stepTime variables, as well as associated addEffect variables
  for (std::map<size_t, IlpAction*>::const_iterator ai = plan->steps().begin();
       ai != plan->steps().end(); ai++) {
    stepTimeVars.add(IloNumVar(env, 0, upperBound, intType));
    stepTime["ST-" + (*ai).second->name()] = stepTimeVars.getSize() - 1;
    for (std::set<std::string>::const_iterator ei = (*ai).second->pos_effects().begin();
         ei != (*ai).second->pos_effects().end(); ei++) {
      // Only one instance of each plan step, so only one add effect variable
      addEffectVars.add(IloNumVarArray(env, 1, 0, 1, boolType));
      addEffect["SE-" + (*ai).second->name() + "->" + (*ei)] = addEffectVars.getSize() - 1;
    }
  }

  // Create the action and actionTime variables, as well as associated addEffect variables
  for (std::map<std::string, IlpAction*>::const_iterator ai = problem->actions().begin();
       ai != problem->actions().end(); ai++) {
    // Determine the number of instances of this action required (1 + the number of times an add effect of this action is deleted)
    int actionInstances = 1;
    for (std::set<std::string>::const_iterator ei = (*ai).second->pos_effects().begin();
         ei != (*ai).second->pos_effects().end(); ei++) {
      actionInstances += propInstances[(*ei)] - 1;
    }
    actionVars.add(IloNumVarArray(env, actionInstances, 0, 1, boolType));
    action["AU-" + (*ai).first] = actionVars.getSize() - 1;
    actionTimeVars.add(IloNumVarArray(env, actionInstances, 0, upperBound, intType));
    actionTime["AT-" + (*ai).first] = actionTimeVars.getSize() - 1;
    for (std::set<std::string>::const_iterator ei = (*ai).second->pos_effects().begin();
         ei != (*ai).second->pos_effects().end(); ei++) {
      addEffectVars.add(IloNumVarArray(env, actionInstances, 0, 1, boolType));
      addEffect["AE-" + (*ai).first + "->" + (*ei)] = addEffectVars.getSize() - 1;
    }
  }

  // Objective is to minimise the number of actions used, within the given bounds
  IloExpr actionUsage(env);
  for (int i = 0; i < actionVars.getSize(); i++) {
    actionUsage += IloSum(actionVars[i]);
  }
  model.add(IloMinimize(env, actionUsage));
  model.add(actionUsage >= static_cast<IloNum>(lb));
  model.add(actionUsage <= static_cast<IloNum>(ub));
}

void IlpVars::add_ilp_constraints(const IlpNode& n) {
  IloEnv env = model.getEnv();
  const IlpProblem* problem = n.problem();
  const IlpPlan* plan = n.plan();

  // Constraint set 1: For each goal proposition, ensure that there is at least one instance achieved after all of its deletions.
  for (std::set<std::string>::const_iterator ai = problem->goal().begin();
       ai != problem->goal().end(); ai++) {
    IloOr goalAchieved(env);
    // For at least one instance of the prop, it must be equal to 1 and all of its deletions must be prior to it
    for (int i = 0; i < propVars[prop["PA-" + (*ai)]].getSize(); i++) {
      IloAnd instanceAchieved(env);
      IloConstraint instanceUsed = propVars[prop["PA-" + (*ai)]][i] == 1;
      instanceAchieved.add(instanceUsed);
      // For each step, if it deletes the prop, it must be before the instance
      for (std::map<size_t, IlpAction*>::const_iterator si =
               plan->steps().begin();
           si != plan->steps().end(); si++) {
        if ((*si).second->neg_effects().count(*ai) > 0) {
          IloConstraint deletionBeforeInstance = stepTimeVars[stepTime["ST-" + (*si).second->name()]] - propTimeVars[propTime["PT-" + (*ai)]][i] + 1 <= 0;
          instanceAchieved.add(deletionBeforeInstance);
        }
      }
      goalAchieved.add(instanceAchieved);
    }
    model.add(goalAchieved);
  }

  // Constraint set 2 not required

  // Constraint set 3: An action can be the first achiever only if it is used
  for (std::map<std::string, IlpAction*>::const_iterator ai =
           problem->actions().begin();
       ai != problem->actions().end(); ai++) {
    for (std::set<std::string>::const_iterator ei = (*ai).second->pos_effects().begin();
         ei != (*ai).second->pos_effects().end(); ei++) {
      for (int i = 0; i < actionVars[action["AU-" + (*ai).first]].getSize(); i++) {
        IloConstraint c3 = actionVars[action["AU-" + (*ai).first]][i] >= addEffectVars[addEffect["AE-" + (*ai).first + "->" + (*ei)]][i];
        model.add(c3);
        c3.setName(("C3-A_" + (*ai).first + "-E_" + (*ei)).c_str());
      }
    }
  }

  // Constraint set 4 not required

  // Constraint set 5: For each precondition, ensure one instance is achieved beforehand with no deletions in-between the prop time and action time
  for (std::map<std::string, IlpAction*>::const_iterator ai =
           problem->actions().begin();
       ai != problem->actions().end(); ai++) {
    for (std::set<std::string>::const_iterator ci = (*ai).second->conditions().begin();
         ci != (*ai).second->conditions().end(); ci++) {
      for (int i = 0; i < actionVars[action["AU-" + (*ai).first]].getSize(); i++) {
        IloOr preconditionSatisfied(env);
        // Either this action instance isn't used
        IloConstraint actionNotUsed = actionVars[action["AU-" + (*ai).first]][i] == 0;
        preconditionSatisfied.add(actionNotUsed);
        // Or there is an instance of the precondition that is satisfied before the action without being deleted
        for (int j = 0; j < propVars[prop["PA-" + (*ci)]].getSize(); j++) {
          IloAnd preconditionSatisfiedInstance(env);
          // Precondition must be used
          IloConstraint preconditionUsed = propVars[prop["PA-" + (*ci)]][j] == 1;
          preconditionSatisfiedInstance.add(preconditionUsed);
          // The precondition must precede the action
          IloConstraint preconditionBeforeAction = propTimeVars[propTime["PT-" + (*ci)]][j] - actionTimeVars[actionTime["AT-" + (*ai).first]][i] <= 0;
          preconditionSatisfiedInstance.add(preconditionBeforeAction);
          // For each step which deletes the precondition, it must either be before the prop or after the action
          for (std::map<size_t, IlpAction*>::const_iterator si =
                   plan->steps().begin();
               si != plan->steps().end(); si++) {
            if ((*si).second->neg_effects().count(*ci) > 0) {
              IloOr noDeletion(env);
              // First case: the step is before the prop
              IloConstraint deletionBeforeProp = stepTimeVars[stepTime["ST-" + (*si).second->name()]] - propTimeVars[propTime["PT-" + (*ci)]][j] + 1 <= 0;
              noDeletion.add(deletionBeforeProp);
              // Second case: the step is after the action
              IloConstraint deletionAfterAction = stepTimeVars[stepTime["ST-" + (*si).second->name()]] - actionTimeVars[actionTime["AT-" + (*ai).first]][i] >= 1;
              noDeletion.add(deletionAfterAction);
              preconditionSatisfiedInstance.add(noDeletion);
            }
          }
          preconditionSatisfied.add(preconditionSatisfiedInstance);
        }
        model.add(preconditionSatisfied);
      }
    }
  }
  for (std::map<size_t, IlpAction*>::const_iterator ai =
           plan->steps().begin();
       ai != plan->steps().end(); ai++) {
    for (std::set<std::string>::const_iterator ci = (*ai).second->conditions().begin();
         ci != (*ai).second->conditions().end(); ci++) {
      IloOr preconditionSatisfied(env);
      // There must be an instance of the precondition that is satisfied before the step without being deleted
      for (int j = 0; j < propVars[prop["PA-" + (*ci)]].getSize(); j++) {
        IloAnd preconditionSatisfiedInstance(env);
        // Precondition must be used
        IloConstraint preconditionUsed = propVars[prop["PA-" + (*ci)]][j] == 1;
        preconditionSatisfiedInstance.add(preconditionUsed);
        // The precondition must precede the step
        IloConstraint preconditionBeforeStep = propTimeVars[propTime["PT-" + (*ci)]][j] - stepTimeVars[stepTime["ST-" + (*ai).second->name()]] <= 0;
        preconditionSatisfiedInstance.add(preconditionBeforeStep);
        // For each step which deletes the precondition, it must either be before the prop or after the step
        for (std::map<size_t, IlpAction*>::const_iterator si =
                 plan->steps().begin();
             si != plan->steps().end(); si++) {
          // If this is the same step, skip it
          if ((*si).second->name() == (*ai).second->name()) {
            continue;
          }
          if ((*si).second->neg_effects().count(*ci) > 0) {
            IloOr noDeletion(env);
            // First case: the step is before the prop
            IloConstraint deletionBeforeProp = stepTimeVars[stepTime["ST-" + (*si).second->name()]] - propTimeVars[propTime["PT-" + (*ci)]][j] + 1 <= 0;
            noDeletion.add(deletionBeforeProp);
            // Second case: the step is after the step
            IloConstraint deletionAfterStep = stepTimeVars[stepTime["ST-" + (*si).second->name()]] - stepTimeVars[stepTime["ST-" + (*ai).second->name()]] >= 1;
            noDeletion.add(deletionAfterStep);
            preconditionSatisfiedInstance.add(noDeletion);
          }
        }
        preconditionSatisfied.add(preconditionSatisfiedInstance);
      }
      model.add(preconditionSatisfied);
    }
  }

  // Constraint set 6: For each proposition instance, ensure that the time step prior to the instance being achieved contains an achiever for the proposition.
  for (std::set<std::string>::const_iterator pi = problem->props().begin();
       pi != problem->props().end(); pi++) {
    for (int i = 0; i < propVars[prop["PA-" + (*pi)]].getSize(); i++) {
      // Set init prop instances to time 0
      if (problem->init().count(*pi) > 0 && i == 0) {
        model.add(propVars[prop["PA-" + (*pi)]][i] == 1);
        model.add(propTimeVars[propTime["PT-" + (*pi)]][i] == 0);
        continue;
      }
      IloOr propAchieved(env);
      // Either the instance is false, or it is true and there is an action that achieves it
      IloConstraint propFalse = propVars[prop["PA-" + (*pi)]][i] == 0;
      propAchieved.add(propFalse);
      // For at least one achiever of the prop, it must be equal to 1 and one step before the instance
      for (std::map<std::string, IlpAction*>::const_iterator ai =
               problem->actions().begin();
           ai != problem->actions().end(); ai++) {
        if ((*ai).second->pos_effects().count(*pi) > 0) {
          for (int j = 0; j < actionVars[action["AU-" + (*ai).first]].getSize(); j++) {
            IloAnd instanceAchieved(env);
            // Action effect = 1
            IloConstraint actionEffect = addEffectVars[addEffect["AE-" + (*ai).first + "->" + (*pi)]][j] == 1;
            instanceAchieved.add(actionEffect);
            // Prop time - Action time = 1
            IloConstraint propAchievedBeforeAction = propTimeVars[propTime["PT-" + (*pi)]][i] - actionTimeVars[actionTime["AT-" + (*ai).first]][j] - 1 == 0;
            instanceAchieved.add(propAchievedBeforeAction);
            propAchieved.add(instanceAchieved);
          }
        }
      }
      // Also check for steps that achieve the proposition
      for (std::map<size_t, IlpAction*>::const_iterator ai =
               plan->steps().begin();
           ai != plan->steps().end(); ai++) {
        if ((*ai).second->pos_effects().count(*pi) > 0) {
          IloAnd instanceAchieved(env);
          // Action effect = 1
          IloConstraint actionEffect = addEffectVars[addEffect["SE-" + (*ai).second->name() + "->" + (*pi)]][0] == 1;
          instanceAchieved.add(actionEffect);
          // Prop time - Step time = 1
          IloConstraint propAchievedBeforeStep = propTimeVars[propTime["PT-" + (*pi)]][i] - stepTimeVars[stepTime["ST-" + (*ai).second->name()]] - 1 == 0;
          instanceAchieved.add(propAchievedBeforeStep);
          propAchieved.add(instanceAchieved);
        }
      }
      model.add(propAchieved);
    }
  }

  // Constraint set 7: Step ordering constraints
  for (std::set<std::pair<size_t, size_t>>::const_iterator oi = plan->orderings().begin();
       oi != plan->orderings().end(); oi++) {
    IloConstraint c7 = stepTimeVars[stepTime["ST-" + plan->steps().at((*oi).first)->name()]] 
                     - stepTimeVars[stepTime["ST-" + plan->steps().at((*oi).second)->name()]] + 1 <= 0;
    model.add(c7);
    c7.setName(("C7-S_" + plan->steps().at((*oi).first)->name() + "-S_" + plan->steps().at((*oi).second)->name()).c_str());
  }

  // Constraint set 8: Step interference
  for (std::map<size_t, IlpAction*>::const_iterator ai =
           plan->steps().begin();
       ai != plan->steps().end(); ai++) {
    for (std::map<size_t, IlpAction*>::const_iterator bi =
             plan->steps().begin();
         bi != plan->steps().end(); bi++) {
      if (ai == bi) {
        continue;
      }
      // For each precondition of ai, and each delete effect of bi
      bool interference = false;
      for (std::set<std::string>::const_iterator ci = (*ai).second->conditions().begin();
           ci != (*ai).second->conditions().end(); ci++) {
        for (std::set<std::string>::const_iterator di = (*bi).second->neg_effects().begin();
             di != (*bi).second->neg_effects().end(); di++) {
          // If the precondition of ai is the delete effect of bi, then add a constraint
          if ((*ci) == (*di)) {
            interference = true;
            IloConstraint c9 = (stepTimeVars[stepTime["ST-" + (*ai).second->name()]] - stepTimeVars[stepTime["ST-" + (*bi).second->name()]] != 0);
            model.add(c9);
            c9.setName(("C9-S_" + (*ai).second->name() + "-S_" + (*bi).second->name()).c_str());
            break;
          }
        }
        if (interference) {
          break;
        }
      }
    }
  }

  // Constraint set 9: Step inconsistent effects
  for (std::map<size_t, IlpAction*>::const_iterator ai =
           plan->steps().begin();
       ai != plan->steps().end(); ai++) {
    for (std::map<size_t, IlpAction*>::const_iterator bi =
             plan->steps().begin();
         bi != plan->steps().end(); bi++) {
      if (ai == bi) {
        continue;
      }
      // For each add effect of ai, and each delete effect of bi
      bool inconsistent = false;
      for (std::set<std::string>::const_iterator ci = (*ai).second->pos_effects().begin();
           ci != (*ai).second->pos_effects().end(); ci++) {
        for (std::set<std::string>::const_iterator di = (*bi).second->neg_effects().begin();
             di != (*bi).second->neg_effects().end(); di++) {
          // If the add effect of ai is the delete effect of bi, then add a constraint
          if ((*ci) == (*di)) {
            inconsistent = true;
            IloConstraint c10 = (stepTimeVars[stepTime["ST-" + (*ai).second->name()]] - stepTimeVars[stepTime["ST-" + (*bi).second->name()]] != 0);
            model.add(c10);
            c10.setName(("C10-S_" + (*ai).second->name() + "-S_" + (*bi).second->name()).c_str());
            break;
          }
        }
        if (inconsistent) {
          break;
        }
      }
    }
  }

}

void IlpVars::add_ilpc_constraints(const IlpNode& n) {
  IloEnv env = model.getEnv();
  const IlpProblem* problem = n.problem();
  const IlpPlan* plan = n.plan();

  // Constraint set 1: The goals must be achieved
  for (std::set<std::string>::const_iterator ai = problem->goal().begin();
       ai != problem->goal().end(); ai++) {
    // Get the number of steps which have this prop as a neg effect
    size_t deletions = 0;
    for (std::map<size_t, IlpAction*>::const_iterator si =
             plan->steps().begin();
         si != plan->steps().end(); si++) {
      if ((*si).second->neg_effects().count(*ai) > 0) {
        deletions++;
      }
    }
    IloConstraint c1 = IloSum(propVars[prop["PA-" + (*ai)]]) - deletions >= 1;
    model.add(c1);
    c1.setName(("C1-" + (*ai)).c_str());
  }

  // Constraint set 2: Actions require their preconditions
  for (std::map<std::string, IlpAction*>::const_iterator ai =
           problem->actions().begin();
       ai != problem->actions().end(); ai++) {
    for (std::set<std::string>::const_iterator ci = (*ai).second->conditions().begin();
         ci != (*ai).second->conditions().end(); ci++) {
      // For each instance of the action, sum of propVars for each precondition must be greater than or equal to the actionVar
      for (int i = 0; i < actionVars[action["AU-" + (*ai).first]].getSize(); i++) {
        IloConstraint c2 = IloSum(propVars[prop["PA-" + (*ci)]]) >= actionVars[action["AU-" + (*ai).first]][i];
        model.add(c2);
        c2.setName(("C2-A_" + (*ai).first + "-P_" + (*ci)).c_str());
      }
    }
  }
  for (std::map<size_t, IlpAction*>::const_iterator ai =
           plan->steps().begin();
       ai != plan->steps().end(); ai++) {
    for (std::set<std::string>::const_iterator ci = (*ai).second->conditions().begin();
         ci != (*ai).second->conditions().end(); ci++) {
      IloConstraint c2 = IloSum(propVars[prop["PA-" + (*ci)]]) >= 1;
      model.add(c2);
      c2.setName(("C2-S_" + (*ai).second->name() + "-P_" + (*ci)).c_str());
    }
  }

  // Constraint set 3: An action can be the first achiever only if it is used
  for (std::map<std::string, IlpAction*>::const_iterator ai =
           problem->actions().begin();
       ai != problem->actions().end(); ai++) {
    for (std::set<std::string>::const_iterator ei = (*ai).second->pos_effects().begin();
         ei != (*ai).second->pos_effects().end(); ei++) {
      for (int i = 0; i < actionVars[action["AU-" + (*ai).first]].getSize(); i++) {
        IloConstraint c3 = actionVars[action["AU-" + (*ai).first]][i] >= addEffectVars[addEffect["AE-" + (*ai).first + "->" + (*ei)]][i];
        model.add(c3);
        c3.setName(("C3-A_" + (*ai).first + "-E_" + (*ei)).c_str());
      }
    }
  }

  // Constraint set 4: If a proposition is achieved, it must be true in the initial state or be the effect of some action
  for (std::set<std::string>::const_iterator pi = problem->props().begin();
       pi != problem->props().end(); pi++) {
    // Create an expression for the sum of add effects
    IloNumVarArray propAddEffects(env);
    for (std::map<std::string, IlpAction*>::const_iterator ai =
             problem->actions().begin();
         ai != problem->actions().end(); ai++) {
      if ((*ai).second->pos_effects().count(*pi) > 0) {
        for (int i = 0; i < actionVars[action["AU-" + (*ai).first]].getSize(); i++) {
          propAddEffects.add(addEffectVars[addEffect["AE-" + (*ai).first + "->" + (*pi)]][i]);
        }
      }
    }
    for (std::map<size_t, IlpAction*>::const_iterator ai =
             plan->steps().begin();
         ai != plan->steps().end(); ai++) {
      if ((*ai).second->pos_effects().count(*pi) > 0) {
        propAddEffects.add(addEffectVars[addEffect["SE-" + (*ai).second->name() + "->" + (*pi)]][0]);
      }
    }
    IloConstraint c4 = IloSum(propVars[prop["PA-" + (*pi)]]) <= problem->init().count(*pi) + IloSum(propAddEffects);
    model.add(c4);
    c4.setName(("C4-" + (*pi)).c_str());
  }

  // Constraint set 5: Actions must be preceded by the satisfaction of their preconditions
  for (std::map<std::string, IlpAction*>::const_iterator ai =
           problem->actions().begin();
       ai != problem->actions().end(); ai++) {
    for (std::set<std::string>::const_iterator ci = (*ai).second->conditions().begin();
         ci != (*ai).second->conditions().end(); ci++) {
      for (int i = 0; i < actionVars[action["AU-" + (*ai).first]].getSize(); i++) {
        // Get the number of instances of the precondition that are satisfied before the action
        IloNumVarArray preconditionSatisfied(env);
        for (int j = 0; j < propVars[prop["PA-" + (*ci)]].getSize(); j++) {
          IloNumVar instanceSatisfied(env, 0, 1, IloNumVar::Bool);
          model.add(instanceSatisfied == (propTimeVars[propTime["PT-" + (*ci)]][j] 
                                        - actionTimeVars[actionTime["AT-" + (*ai).first]][i] 
                                        + M * (1 - propVars[prop["PA-" + (*ci)]][j]) <= 0));
          preconditionSatisfied.add(instanceSatisfied);
        }
        // Get the number of steps which delete the precondition that occur before the action
        IloNumVarArray preconditionDeleted(env);
        for (std::map<size_t, IlpAction*>::const_iterator si =
                 plan->steps().begin();
             si != plan->steps().end(); si++) {
          if ((*si).second->neg_effects().count(*ci) > 0) {
            IloNumVar instanceDeleted(env, 0, 1, IloNumVar::Bool);
            model.add(instanceDeleted == (stepTimeVars[stepTime["ST-" + (*si).second->name()]] 
                                        - actionTimeVars[actionTime["AT-" + (*ai).first]][i] < 0));
            preconditionDeleted.add(instanceDeleted);
          }
        }
        // The precondition must be satisfied more than it is deleted
        IloConstraint c5 = IloSum(preconditionSatisfied) - IloSum(preconditionDeleted) + M * (1 - actionVars[action["AU-" + (*ai).first]][i]) >= 1;
        model.add(c5);
        c5.setName(("C5-A_" + (*ai).first + "-P_" + (*ci)).c_str());
      }
    }
  }
  
  for (std::map<size_t, IlpAction*>::const_iterator ai =
           plan->steps().begin();
       ai != plan->steps().end(); ai++) {
    for (std::set<std::string>::const_iterator ci = (*ai).second->conditions().begin();
         ci != (*ai).second->conditions().end(); ci++) {
      // Get the number of instances of the precondition that are satisfied before the action
      IloNumVarArray preconditionSatisfied(env);
      for (int j = 0; j < propVars[prop["PA-" + (*ci)]].getSize(); j++) {
        IloNumVar instanceSatisfied(env, 0, 1, IloNumVar::Bool);
        model.add(instanceSatisfied == (propTimeVars[propTime["PT-" + (*ci)]][j] 
                                      - stepTimeVars[stepTime["ST-" + (*ai).second->name()]] 
                                      + M * (1 - propVars[prop["PA-" + (*ci)]][j]) <= 0));
        preconditionSatisfied.add(instanceSatisfied);
      }
      // Get the number of steps which delete the precondition that occur before the action
      IloNumVarArray preconditionDeleted(env);
      for (std::map<size_t, IlpAction*>::const_iterator si =
               plan->steps().begin();
           si != plan->steps().end(); si++) {
        // If this is the same step as the action, then skip it
        if ((*si).second->name() == (*ai).second->name()) {
          continue;
        }
        if ((*si).second->neg_effects().count(*ci) > 0) {
          IloNumVar instanceDeleted(env, 0, 1, IloNumVar::Bool);
          model.add(instanceDeleted == (stepTimeVars[stepTime["ST-" + (*si).second->name()]] 
                                      - stepTimeVars[stepTime["ST-" + (*ai).second->name()]] <= 0));
          preconditionDeleted.add(instanceDeleted);
        }
      }
      // The precondition must be satisfied more than it is deleted
      model.add(IloSum(preconditionSatisfied) - IloSum(preconditionDeleted) > 0);
    }
  }

  // Constraint set 6: each non init instance of p must be associated with an achiever of p
  for (std::set<std::string>::const_iterator pi = 
           problem->props().begin();
        pi != problem->props().end(); pi++) {
    for (int i = 0; i < propVars[prop["PA-" + (*pi)]].getSize(); i++) {
      // Set init prop instances to time 0
      if (problem->init().count(*pi) > 0 && i == 0) {
        model.add(propVars[prop["PA-" + (*pi)]][i] == 1);
        model.add(propTimeVars[propTime["PT-" + (*pi)]][i] == 0);
        continue;
      }
      // Sum the number of actions which achieve this prop
      IloNumVarArray propAchieved(env);
      for (std::map<std::string, IlpAction*>::const_iterator ai =
               problem->actions().begin();
           ai != problem->actions().end(); ai++) {
        if ((*ai).second->pos_effects().count(*pi) > 0) {
          for (int j = 0; j < actionVars[action["AU-" + (*ai).first]].getSize(); j++) {
            IloNumVar instanceAchieved(env, 0, 1, IloNumVar::Bool);
            model.add(instanceAchieved == (propTimeVars[propTime["PT-" + (*pi)]][i]
                                        - actionTimeVars[actionTime["AT-" + (*ai).first]][j]  
                                        - M * (1 - addEffectVars[addEffect["AE-" + (*ai).first + "->" + (*pi)]][j]) > 0));
            propAchieved.add(instanceAchieved);
          }
        }
      }
      for (std::map<size_t, IlpAction*>::const_iterator ai =
               plan->steps().begin();
           ai != plan->steps().end(); ai++) {
        if ((*ai).second->pos_effects().count(*pi) > 0) {
          IloNumVar instanceAchieved(env, 0, 1, IloNumVar::Bool);
          model.add(instanceAchieved == (propTimeVars[propTime["PT-" + (*pi)]][i]
                                      - stepTimeVars[stepTime["ST-" + (*ai).second->name()]]
                                      - M * (1 - addEffectVars[addEffect["SE-" + (*ai).second->name() + "->" + (*pi)]][0]) > 0));
          propAchieved.add(instanceAchieved);
        }
      }
      // Sum the number of instances of the prop that are satisfied before or at the same time as this instance
      IloNumVarArray propSatisfied(env);
      for (int j = 0; j < propVars[prop["PA-" + (*pi)]].getSize(); j++) {
        // If j=0 and init prop, then continue, or if j=i then continue
        if ((j == 0 && problem->init().count(*pi) > 0) || j == i) {
          continue;
        }
        // Else, instanceSatisifed is 1 if the prop is satisfied before or at the same time as this instance
        IloNumVar instanceSatisfied(env, 0, 1, IloNumVar::Bool);
        model.add(instanceSatisfied == (propTimeVars[propTime["PT-" + (*pi)]][j] 
                                    - propTimeVars[propTime["PT-" + (*pi)]][i] <= 0));
        propSatisfied.add(instanceSatisfied);
      }
      // Prop must be not satisfied, or have more achievers than satisfied instances
      IloConstraint propHasAchiever = IloSum(propAchieved) - IloSum(propSatisfied) + M * (1 - propVars[prop["PA-" + (*pi)]][i]) > 0;
      model.add(propHasAchiever);
      propHasAchiever.setName(("C6-P_" + (*pi) + "-" + std::to_string(i)).c_str());
    }
  }

  // Constraint set 7: Step ordering constraints
  for (std::set<std::pair<size_t, size_t>>::const_iterator oi = plan->orderings().begin();
       oi != plan->orderings().end(); oi++) {
    IloConstraint c7 = stepTimeVars[stepTime["ST-" + plan->steps().at((*oi).first)->name()]] 
                     - stepTimeVars[stepTime["ST-" + plan->steps().at((*oi).second)->name()]] < 0;
    model.add(c7);
    c7.setName(("C7-S_" + plan->steps().at((*oi).first)->name() + "-S_" + plan->steps().at((*oi).second)->name()).c_str());
  }

  // Constraint set 8: Step interference
  for (std::map<size_t, IlpAction*>::const_iterator ai =
           plan->steps().begin();
       ai != plan->steps().end(); ai++) {
    for (std::map<size_t, IlpAction*>::const_iterator bi =
             plan->steps().begin();
         bi != plan->steps().end(); bi++) {
      if (ai == bi) {
        continue;
      }
      // For each precondition of ai, and each delete effect of bi
      bool interference = false;
      for (std::set<std::string>::const_iterator ci = (*ai).second->conditions().begin();
           ci != (*ai).second->conditions().end(); ci++) {
        for (std::set<std::string>::const_iterator di = (*bi).second->neg_effects().begin();
             di != (*bi).second->neg_effects().end(); di++) {
          // If the precondition of ai is the delete effect of bi, then add a constraint
          if ((*ci) == (*di)) {
            interference = true;
            IloConstraint c9 = (stepTimeVars[stepTime["ST-" + (*ai).second->name()]] - stepTimeVars[stepTime["ST-" + (*bi).second->name()]] != 0);
            model.add(c9);
            c9.setName(("C9-S_" + (*ai).second->name() + "-S_" + (*bi).second->name()).c_str());
            break;
          }
        }
        if (interference) {
          break;
        }
      }
    }
  }
}

size_t IlpVars::run(const IlpNode& n, std::ostream& os, short int verbosity, size_t lb, size_t seconds) {
  IloEnv env = model.getEnv();
  const IlpPlan* plan = n.plan();
  IloCplex cplex(model);
  if (verbosity < 2) cplex.setOut(env.getNullStream());
  cplex.setParam(IloCplex::Param::MIP::Limits::LowerObjStop, lb);
  cplex.setParam(IloCplex::Param::TimeLimit, seconds);
  cplex.solve();
  if (cplex.getStatus() == IloAlgorithm::Infeasible) {
    return IlpNode::INFEASIBLE;
  }
  const size_t objectiveValue = static_cast<size_t>(cplex.getObjValue());
  if (verbosity >= 3) {
    env.out() << "Solution status = " << cplex.getStatus() << std::endl;
    env.out() << "Solution value  = " << objectiveValue << std::endl;
    // Iterate through both dimensions of the propVars array
    for (std::map<std::string, int>::const_iterator ai = prop.begin();
         ai != prop.end(); ai++) {
      for (int i = 0; i < propVars[(*ai).second].getSize(); i++) {
        try {
          // Use cplex.getValue to get the IloNum value of the variable
          IloNum val = cplex.getValue(propVars[(*ai).second][i]);
          os << "Prop " << (*ai).first << "[" << i << "] = " << val << std::endl;
        } catch (IloException& e) {
          os << "Prop " << (*ai).first << "[" << i << "] = " << "?" << std::endl;
        }
      }
    }
    // Iterate through both dimensions of the propTimeVars array
    for (std::map<std::string, int>::const_iterator ai = propTime.begin();
         ai != propTime.end(); ai++) {
      for (int i = 0; i < propTimeVars[(*ai).second].getSize(); i++) {
        try {
          IloNum val = cplex.getValue(propTimeVars[(*ai).second][i]);
          os << "Prop Time " << (*ai).first << "[" << i << "] = " << val << std::endl;
        } catch (IloException& e) {
          os << "Prop Time " << (*ai).first << "[" << i << "] = " << "?" << std::endl;
        }
      }
    }
    // Iterate through both dimensions of the actionVars array
    for (std::map<std::string, int>::const_iterator ai = action.begin();
         ai != action.end(); ai++) {
      for (int i = 0; i < actionVars[(*ai).second].getSize(); i++) {
        try {
          IloNum val = cplex.getValue(actionVars[(*ai).second][i]);
          os << "Action " << (*ai).first << "[" << i << "] = " << val << std::endl;
        } catch (IloException& e) {
          os << "Action " << (*ai).first << "[" << i << "] = " << "?" << std::endl;
        }
      }
    }
    // Iterate through both dimensions of the actionTimeVars array
    for (std::map<std::string, int>::const_iterator ai = actionTime.begin();
         ai != actionTime.end(); ai++) {
      for (int i = 0; i < actionTimeVars[(*ai).second].getSize(); i++) {
        try {
          IloNum val = cplex.getValue(actionTimeVars[(*ai).second][i]);
          os << "Action Time " << (*ai).first << "[" << i << "] = " << val << std::endl;
        } catch (IloException& e) {
          os << "Action Time " << (*ai).first << "[" << i << "] = " << "?" << std::endl;
        }
      }
    }
    // Iterate through both dimensions of the addEffectVars array
    for (std::map<std::string, int>::const_iterator ai = addEffect.begin();
         ai != addEffect.end(); ai++) {
      for (int i = 0; i < addEffectVars[(*ai).second].getSize(); i++) {
        try {
          IloNum val = cplex.getValue(addEffectVars[(*ai).second][i]);
          os << "Add Effect " << (*ai).first << "[" << i << "] = " << val << std::endl;
        } catch (IloException& e) {
          os << "Add Effect " << (*ai).first << "[" << i << "] = " << "?" << std::endl;
        }
      }
    }
    // Iterate through the single dimension of the stepVars array
    for (std::map<size_t, IlpAction*>::const_iterator ai =
           plan->steps().begin();
       ai != plan->steps().end(); ai++) {
      os << "Step " << (*ai).second->name() << " = 1" << std::endl;
    }
    // Iterate through the single dimension of the stepTimeVars array
    for (std::map<std::string, int>::const_iterator ai = stepTime.begin();
         ai != stepTime.end(); ai++) {
      try {
        IloNum val = cplex.getValue(stepTimeVars[(*ai).second]);
        os << "Step Time " << (*ai).first << " = " << val << std::endl;
      } catch (IloException& e) {
        os << "Step Time " << (*ai).first << " = " << "?" << std::endl;
      }
    }
  }
  if (verbosity >= 1) {
    // Iterate through both dimensions of the actionVars array
    std::map<std::string, int> actionVals;
    for (std::map<std::string, int>::const_iterator ai = action.begin();
         ai != action.end(); ai++) {
      for (int i = 0; i < actionVars[(*ai).second].getSize(); i++) {
        try {
          IloNum val = cplex.getValue(actionVars[(*ai).second][i]);
          actionVals[(*ai).first.substr(3) + '-' + std::to_string(i)] = std::round(val);
        } catch (IloException& e) {
          actionVals[(*ai).first.substr(3) + '-' + std::to_string(i)] = 0;
        }
      }
    }
    // Iterate through both dimensions of the actionTimeVars array
    std::map<int, std::vector<std::string>> actionTimeVals;
    for (std::map<std::string, int>::const_iterator ai = actionTime.begin();
         ai != actionTime.end(); ai++) {
      for (int i = 0; i < actionTimeVars[(*ai).second].getSize(); i++) {
        if (actionVals[(*ai).first.substr(3) + '-' + std::to_string(i)] == 1) {
          try { 
            IloNum val = cplex.getValue(actionTimeVars[(*ai).second][i]);
            actionTimeVals[std::round(val)].push_back((*ai).first.substr(3) + '-' + std::to_string(i));
          } catch (IloException& e) {
            actionTimeVals[0].push_back((*ai).first.substr(3) + '-' + std::to_string(i));
          }
        }
      }
    }
    // Iterate through the single dimension of the stepTimeVars array
    for (std::map<std::string, int>::const_iterator ai = stepTime.begin();
         ai != stepTime.end(); ai++) {
      try {
        IloNum val = cplex.getValue(stepTimeVars[(*ai).second]);
        actionVals["STEP:" + (*ai).first.substr(3)] = std::round(val);
        actionTimeVals[std::round(val)].push_back("STEP:" + (*ai).first.substr(3));
      } catch (IloException& e) {
        actionVals["STEP:" + (*ai).first.substr(3)] = 0;
        actionTimeVals[0].push_back("STEP:" + (*ai).first.substr(3));
      }
    }
    // Iterate through the single dimension of the propVars array
    std::map<std::pair<std::string, int>, int> propVals;
    // For each prop and instance, get the value
    for (std::map<std::string, int>::const_iterator ai = prop.begin();
         ai != prop.end(); ai++) {
      for (int i = 0; i < propVars[(*ai).second].getSize(); i++) {
        // If (*ai).first contains the substring 'link', then skip
        if ((*ai).first.find("link") != std::string::npos) {
          continue;
        }
        try {
          IloNum val = cplex.getValue(propVars[(*ai).second][i]);
          propVals[std::make_pair((*ai).first.substr(3), i)] = std::round(val);
        } catch (IloException& e) {
          propVals[std::make_pair((*ai).first.substr(3), i)] = 0;
        }
      }
    }
    // Iterate through the single dimension of the propTimeVars array
    std::map<int, std::vector<std::pair<std::string, int>>> propTimeVals;
    // For each prop and instance, get the time value
    for (std::map<std::string, int>::const_iterator ai = propTime.begin();
         ai != propTime.end(); ai++) {
      for (int i = 0; i < propTimeVars[(*ai).second].getSize(); i++) {
        if (propVals[std::make_pair((*ai).first.substr(3), i)] == 1) {
          try {
            IloNum val = cplex.getValue(propTimeVars[(*ai).second][i]);
            propTimeVals[std::round(val)].push_back(std::make_pair((*ai).first.substr(3), i));
          } catch (IloException& e) {
            propTimeVals[0].push_back(std::make_pair((*ai).first.substr(3), i));
          }
        }
      }
    }

    // Print the sorted actions and props
    os << "INIT" << std::endl;
    for (size_t i = 0; i <= upperBound; i++) {
      if (propTimeVals.count(i) > 0) {
        os << i << ": ";
        for (std::pair<std::string, int> prop : propTimeVals[i]) {
          os << prop.first << "[" << prop.second << "] ";
        }
        os << std::endl;
      }
      if (actionTimeVals.count(i) > 0) {
        os << i << ": ";
        for (std::string action : actionTimeVals[i]) {
          os << action << " ";
        }
        os << std::endl;
      }
    }
    os << "GOAL" << std::endl;
  }

  return objectiveValue;
}

size_t IlpNode::solve_model(bool counting, std::ostream& os, short int verbosity, bool lp_relax, size_t lb, size_t ub, size_t seconds) const {
  IloEnv env;
  size_t result = SOLVER_ERROR;
  try {
    IlpVars vars(env);
    vars.build(*this, lp_relax, lb, ub, os, verbosity);
    if (counting) {
      vars.add_ilpc_constraints(*this);
    } else {
      vars.add_ilp_constraints(*this);
    }
    result = vars.run(*this, os, verbosity, lb, seconds);
  }
  catch (IloException& e) {
    std::cerr << "CPLEX Concert exception caught: " << e << std::endl;
  }
  catch (...) {
    std::cerr << "Unknown ILP exception caught" << std::endl;
  }
  env.end();
  return result;
}

size_t IlpNode::solve(std::ostream& os, short int verbosity, bool lp_relax, size_t lb, size_t ub, size_t seconds) const {
  return solve_model(false, os, verbosity, lp_relax, lb, ub, seconds);
}

size_t IlpNode::counting_solve(std::ostream& os, short int verbosity, bool lp_relax, size_t lb, size_t ub, size_t seconds) const {
  return solve_model(true, os, verbosity, lp_relax, lb, ub, seconds);
}

void IlpNode::remove_causal_links() {
  // Iterate through the causal links
  for (std::multimap<size_t, std::pair<size_t, std::string>>::const_iterator li =
           plan_->links().begin();
       li != plan_->links().end(); li++) {
    // Get the from and to actions
    IlpAction* from = plan_->get_action((*li).first);
    IlpAction* to = plan_->get_action((*li).second.first);
    std::string condition = (*li).second.second;
    std::string link_prop = (*li).second.second + "-link-" + std::to_string((*li).first) + "-" + std::to_string((*li).second.first);
    problem_->add_prop(link_prop);
    // If from is not init state, add link_prop to problem init state and delete at start of link
    if (from) {
      problem_->add_init_prop(link_prop);
      from->add_neg_effect(link_prop);
    }
    // If to is not goal state, add link_prop at end of link
    if (to) {
      to->add_pos_effect(link_prop);
    }
    // For each action in problem, if the actions neg effects contains the condition, add link_prop to the preconditions
    for (std::map<std::string, IlpAction*>::const_iterator ai =
             problem_->actions().begin();
         ai != problem_->actions().end(); ai++) {
      if ((*ai).second->neg_effects().count(condition) > 0) {
        (*ai).second->add_condition(link_prop);
      }
    }
    // For each step in plan, if the steps neg effects contains the condition, add link_prop to the preconditions
    for (std::map<size_t, IlpAction*>::const_iterator ai =
             plan_->steps().begin();
         ai != plan_->steps().end(); ai++) {
      // Skip if this is the to action
      if ((*li).second.first == (*ai).first) {
        continue;
      }
      if ((*ai).second->neg_effects().count(condition) > 0) {
        (*ai).second->add_condition(link_prop);
      }
    }
  }
  // Clear the causal links
  plan_->clear_causal_links();
}

std::ostream& operator<<(std::ostream& os, const IlpNode& n) {
  os << "Ilp Node ID: " << n.serial_no() << std::endl;
  os << *n.problem();
  os << *n.plan();
  return os;
}
