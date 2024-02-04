#include "ilp.h"
#include "actions.h"
#include "formulas.h"
#include "problems.h"

#include <iostream>
#include <set>
#include <string>

#include <ilcplex/ilocplex.h>

size_t IlpAction::next_id = 0;

IlpAction::IlpAction(const std::string& name)
    : id_(next_id++),
      name_(name) { }

IlpAction::IlpAction(const Action& action)
    : id_(next_id++),
      name_(action.name()) {
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
        std::cout << "Not an atom ERROR" << std::endl;
        continue; // Not an atom
      }
    }
  } else {
    std::cout << "Not a conjunction ERROR" << std::endl;
    return; // Not a conjunction
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
  os << prefix << "ID: " << id_ << std::endl;
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

IlpProblem::IlpProblem(const std::string& name)
    : name_(name) { }

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
        continue; // Not an atom
      }
    }
  } else {
    return; // Not a conjunction
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
  for (std::map<std::string, const IlpAction*>::const_iterator ai =
           actions_.begin();
       ai != actions_.end(); ai++) {
    delete (*ai).second;
  }
}

void IlpProblem::add_action(const IlpAction* action) {
  actions_[action->name()] = action;
}

const IlpAction* IlpProblem::find_action(const std::string& name) const {
  std::map<std::string, const IlpAction*>::const_iterator ai =
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

const size_t IlpProblem::solve(std::ostream& os, short int verbosity) const {
  IloEnv env;
  try {
    IloModel model(env);  

    std::map<std::string, int> prop;
    std::map<std::string, int> propTime;
    std::map<std::string, int> action;
    std::map<std::string, int> actionTime;
    std::map<std::string, int> addEffect;

    IloNumVarArray propVars(env);
    IloNumVarArray propTimeVars(env);
    IloNumVarArray actionVars(env);
    IloNumVarArray actionTimeVars(env);
    IloNumVarArray addEffectVars(env);
  
    IloEnv env = model.getEnv();  

    // Get the number of actions, to be used as an upper bound
    int numActions = actions_.size();

    for (std::set<std::string>::const_iterator ai = props_.begin();
         ai != props_.end(); ai++) {
      IloNumVar propVar(env, 0, 1, IloNumVar::Bool);
      propVars.add(propVar);
      prop[("PA-" + (*ai)).c_str()] = propVars.getSize() - 1;
      IloNumVar propTimeVar(env, 0, numActions, IloNumVar::Int);
      propTimeVars.add(propTimeVar);
      propTime[("PT-" + (*ai)).c_str()] = propTimeVars.getSize() - 1;
    }

    // For each action add a usage variable, a time variable, and add effect variables
    for (std::map<std::string, const IlpAction*>::const_iterator ai =
             actions_.begin();
         ai != actions_.end(); ai++) {
      IloNumVar actionVar(env, 0, 1, IloNumVar::Bool);
      actionVars.add(actionVar);
      action[("AU-" + (*ai).first).c_str()] = actionVars.getSize() - 1;
      IloNumVar actionTimeVar(env, 0, numActions, IloNumVar::Int);
      actionTimeVars.add(actionTimeVar);
      actionTime[("AT-" + (*ai).first).c_str()] = actionTimeVars.getSize() - 1;
      for (std::set<std::string>::const_iterator ei = (*ai).second->pos_effects().begin();
           ei != (*ai).second->pos_effects().end(); ei++) {
        IloNumVar addEffectVar(env, 0, 1, IloNumVar::Bool);
        addEffectVars.add(addEffectVar);
        addEffect[("AE-" + (*ai).first + "->" + (*ei)).c_str()] = addEffectVars.getSize() - 1;
      }
    } 
  
    // Objective is to minimise the number of actions used (sum of action usage action variables)
    model.add(IloMinimize(env, IloSum(actionVars)));

    // Constraint set 1: The goals must be achieved
    for (std::set<std::string>::const_iterator ai = goal_.begin();
         ai != goal_.end(); ai++) {
      model.add(propVars[prop[("PA-" + (*ai)).c_str()]] == 1);
    }

    // Constraint set 2: Actions require their preconditions
    for (std::map<std::string, const IlpAction*>::const_iterator ai =
             actions_.begin();
         ai != actions_.end(); ai++) {
      for (std::set<std::string>::const_iterator ci = (*ai).second->conditions().begin();
           ci != (*ai).second->conditions().end(); ci++) {
        model.add(propVars[prop[("PA-" + (*ci)).c_str()]] >= actionVars[action[("AU-" + (*ai).first).c_str()]]);
      }
    }

    // Constraint set 3: An action can be the first achiever only if it is used
    for (std::map<std::string, const IlpAction*>::const_iterator ai =
             actions_.begin();
         ai != actions_.end(); ai++) {
      for (std::set<std::string>::const_iterator ei = (*ai).second->pos_effects().begin();
           ei != (*ai).second->pos_effects().end(); ei++) {
        model.add(actionVars[action[("AU-" + (*ai).first).c_str()]] >= addEffectVars[addEffect[("AE-" + (*ai).first + "->" + (*ei)).c_str()]]);
      }
    }

    // Constraint set 4: If a proposition is achieved, it must be true in the initial state or be the effect of some action
    for (std::set<std::string>::const_iterator pi = props_.begin();
         pi != props_.end(); pi++) {
      // Create an expression for the sum of add effects
      IloNumExprArray propAddEffects(env);
      for (std::map<std::string, const IlpAction*>::const_iterator ai =
               actions_.begin();
           ai != actions_.end(); ai++) {
        if ((*ai).second->pos_effects().count(*pi) > 0) {
          propAddEffects.add(addEffectVars[addEffect[("AE-" + (*ai).first + "->" + (*pi)).c_str()]]);
        }
      }
      model.add(propVars[prop[("PA-" + (*pi)).c_str()]] == init_.count(*pi) + IloSum(propAddEffects));
    }

    // Constraint set 5: Actions must be preceded by the satisfaction of their preconditions
    for (std::map<std::string, const IlpAction*>::const_iterator ai =
             actions_.begin();
         ai != actions_.end(); ai++) {
      for (std::set<std::string>::const_iterator ci = (*ai).second->conditions().begin();
           ci != (*ai).second->conditions().end(); ci++) {
        model.add(actionTimeVars[actionTime[("AT-" + (*ai).first).c_str()]] >= propTimeVars[propTime[("PT-" + (*ci)).c_str()]]);
      }
    }

    // Constraint set 6: If a is the first achiever of p, then a must precede p
    for (std::map<std::string, const IlpAction*>::const_iterator ai =
             actions_.begin();
         ai != actions_.end(); ai++) {
      for (std::set<std::string>::const_iterator ei = (*ai).second->pos_effects().begin();
           ei != (*ai).second->pos_effects().end(); ei++) {
        const size_t M = numActions + 1;
        model.add(actionTimeVars[actionTime[("AT-" + (*ai).first).c_str()]] + 1 <=
                  propTimeVars[propTime[("PT-" + (*ei)).c_str()]] + M * (1 - addEffectVars[addEffect[("AE-" + (*ai).first + "->" + (*ei)).c_str()]]));
      }
    }

    IloCplex cplex(model);
    if (verbosity == 0) cplex.setOut(env.getNullStream());

    // Optimize the problem and obtain solution.
    if ( !cplex.solve() ) {
      env.error() << "Failed to optimize LP" << std::endl;
      throw(-1);
    }

    IloNumArray vals(env);
    const size_t objectiveValue = static_cast<size_t>(cplex.getObjValue());
    if (verbosity >= 2) {
      env.out() << "Solution status = " << cplex.getStatus() << std::endl;
      env.out() << "Solution value  = " << objectiveValue << std::endl;
      cplex.getValues(vals, propVars);
      env.out() << "Prop vals" << vals << std::endl;
      for (std::map<std::string, int>::const_iterator ai = prop.begin();
           ai != prop.end(); ai++) {
        os << "Prop " << (*ai).first << " = " << vals[(*ai).second] << std::endl;
      }
      cplex.getValues(vals, propTimeVars);
      env.out() << "Prop Time vals" << vals << std::endl;
      for (std::map<std::string, int>::const_iterator ai = propTime.begin();
           ai != propTime.end(); ai++) {
        os << "Prop Time " << (*ai).first << " = " << vals[(*ai).second] << std::endl;
      }
      cplex.getValues(vals, actionVars);
      env.out() << "Action vals" << vals << std::endl;
      for (std::map<std::string, int>::const_iterator ai = action.begin();
           ai != action.end(); ai++) {
        os << "Action " << (*ai).first << " = " << vals[(*ai).second] << std::endl;
      }
      cplex.getValues(vals, actionTimeVars);
      env.out() << "Action Time vals" << vals << std::endl;
      for (std::map<std::string, int>::const_iterator ai = actionTime.begin();
           ai != actionTime.end(); ai++) {
        os << "Action Time " << (*ai).first << " = " << vals[(*ai).second] << std::endl;
      }
      cplex.getValues(vals, addEffectVars);
      env.out() << "Add Effect vals" << vals << std::endl;
      for (std::map<std::string, int>::const_iterator ai = addEffect.begin();
           ai != addEffect.end(); ai++) {
        os << "Add Effect " << (*ai).first << " = " << vals[(*ai).second] << std::endl;
      }
    }
    if (verbosity >= 1) {
      cplex.getValues(vals, actionVars);
      std::map<std::string, int> actionVals;
      for (std::map<std::string, int>::const_iterator ai = action.begin();
           ai != action.end(); ai++) {
        actionVals[(*ai).first] = vals[(*ai).second];
      }
      cplex.getValues(vals, actionTimeVars);
      std::map<int, std::string> actionTimeVals;
      for (std::map<std::string, int>::const_iterator ai = actionTime.begin();
           ai != actionTime.end(); ai++) {
        actionTimeVals[vals[(*ai).second]] = (*ai).first;
      }
      // Iterate through the sorted action time values and print each action if it is used
      os << "INIT -> ";
      for (std::map<int, std::string>::const_iterator ai = actionTimeVals.begin();
           ai != actionTimeVals.end(); ai++) {
        if (actionVals["AU" + (*ai).second.substr(2)] == 1) {
          os << (*ai).second.substr(3) << " -> ";
        }
      }
      os << "GOAL" << std::endl;
      }

    env.end();
    return objectiveValue;
  }
  catch (IloException& e) {
    std::cerr << "CPLEX Concert exception caught: " << e << std::endl;
  }
  catch (...) {
    std::cerr << "Unknown ILP exception caught" << std::endl;
  }
  env.end();
  return -1;
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
  for (std::map<std::string, const IlpAction*>::const_iterator ai =
             p.actions_.begin();
       ai != p.actions_.end(); ai++) {
    os << (*ai).first << std::endl;
    (*ai).second->print(os, "  ");
  }
  return os;
}