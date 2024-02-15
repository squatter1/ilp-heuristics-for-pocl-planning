#include "ilp.h"
#include "actions.h"
#include "formulas.h"
#include "problems.h"
#include "plans.h"

#include <iostream>
#include <set>
#include <string>
#include <algorithm>

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
    for (std::map<std::string, IlpAction*>::const_iterator ai =
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
    for (std::map<std::string, IlpAction*>::const_iterator ai =
             actions_.begin();
         ai != actions_.end(); ai++) {
      for (std::set<std::string>::const_iterator ci = (*ai).second->conditions().begin();
           ci != (*ai).second->conditions().end(); ci++) {
        model.add(propVars[prop[("PA-" + (*ci)).c_str()]] >= actionVars[action[("AU-" + (*ai).first).c_str()]]);
      }
    }

    // Constraint set 3: An action can be the first achiever only if it is used
    for (std::map<std::string, IlpAction*>::const_iterator ai =
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
      for (std::map<std::string, IlpAction*>::const_iterator ai =
               actions_.begin();
           ai != actions_.end(); ai++) {
        if ((*ai).second->pos_effects().count(*pi) > 0) {
          propAddEffects.add(addEffectVars[addEffect[("AE-" + (*ai).first + "->" + (*pi)).c_str()]]);
        }
      }
      model.add(propVars[prop[("PA-" + (*pi)).c_str()]] == init_.count(*pi) + IloSum(propAddEffects));
    }

    // Constraint set 5: Actions must be preceded by the satisfaction of their preconditions
    for (std::map<std::string, IlpAction*>::const_iterator ai =
             actions_.begin();
         ai != actions_.end(); ai++) {
      for (std::set<std::string>::const_iterator ci = (*ai).second->conditions().begin();
           ci != (*ai).second->conditions().end(); ci++) {
        model.add(actionTimeVars[actionTime[("AT-" + (*ai).first).c_str()]] >= propTimeVars[propTime[("PT-" + (*ci)).c_str()]]);
      }
    }

    // Constraint set 6: If a is the first achiever of p, then a must precede p
    for (std::map<std::string, IlpAction*>::const_iterator ai =
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
  links_[from] = std::make_pair(to, condition);
}

void IlpPlan::clear_causal_links() {
  links_.clear();
}

void IlpPlan::add_ordering(const size_t before, const size_t after) {
  orderings_[before] = after;
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
  for (std::map<size_t, std::pair<size_t, std::string>>::const_iterator ai =
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
  for (std::map<size_t, size_t>::const_iterator ai =
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

IlpNode::IlpNode(IlpProblem* problem, IlpPlan* plan)
    : id_(plan->serial_no()) {
  problem_ = problem;
  plan_ = plan;
  remove_causal_links();
}

IlpNode::~IlpNode() { 
  // Delete the plan and problem
  delete problem_;
  delete plan_;
}

const size_t IlpNode::solve(std::ostream& os, short int verbosity) const {
  IloEnv env;
  try {
    IloModel model(env);  

    // Props are now 2D, first dimension gets to the prop, second gets to the instance of that prop (due to deletions)
    std::map<std::string, int> prop;
    std::map<std::string, int> propTime;
    std::map<std::string, int> action;
    std::map<std::string, int> actionTime;
    std::map<std::string, int> step;
    std::map<std::string, int> stepTime;
    std::map<std::string, int> addEffect;

    IloArray<IloNumVarArray> propVars(env);
    IloArray<IloNumVarArray> propTimeVars(env);
    IloNumVarArray stepVars(env);
    IloNumVarArray stepTimeVars(env);
    IloArray<IloNumVarArray> actionVars(env);
    IloArray<IloNumVarArray> actionTimeVars(env);
    IloArray<IloNumVarArray> addEffectVars(env); // Add effect vars do take into account duplicate actions, but don't take into account duplicate props
  
    IloEnv env = model.getEnv();  

    // Get the set of goal and init propositions from the problem, and setminus them
    std::set<std::string> goal = problem_->goal();
    std::set<std::string> init = problem_->init();
    std::set<std::string> goalMinusInit;
    std::set_difference(init.begin(), init.end(), goal.begin(), goal.end(), std::inserter(goalMinusInit, goalMinusInit.begin()));

    // Set prop instances to 1 for each prop initially
    std::map<std::string, int> propInstances;
    for (std::set<std::string>::const_iterator ai = problem_->props().begin();
         ai != problem_->props().end(); ai++) {
      propInstances[(*ai)] = 1;
    }

    // Calculate the total number of delete effects in the plan, and hence also the number of prop instances
    int planDelEffects = 0;
    for (std::map<size_t, IlpAction*>::const_iterator ai =
             plan_->steps().begin();
         ai != plan_->steps().end(); ai++) {
      planDelEffects += (*ai).second->neg_effects().size();
      // For each delete effect, add a new instance of the prop
      for (std::set<std::string>::const_iterator ei = (*ai).second->neg_effects().begin();
           ei != (*ai).second->neg_effects().end(); ei++) {
        propInstances[(*ei)]++;
      }
    }

    // Upper bound of $|G\setminus I|+|del_P|+|P|$
    int upperBound = goalMinusInit.size() + planDelEffects + plan_->steps().size();
    const size_t M = upperBound + 1;

    // Create the prop and propTime variables
    for (std::set<std::string>::const_iterator ai = problem_->props().begin();
         ai != problem_->props().end(); ai++) {
      IloNumVarArray propVar(env, propInstances[(*ai)], 0, 1, IloNumVar::Bool);
      propVars.add(propVar);
      prop[("PA-" + (*ai)).c_str()] = propVars.getSize() - 1;
      IloNumVarArray propTimeVar(env, propInstances[(*ai)], 0, upperBound, IloNumVar::Int);
      propTimeVars.add(propTimeVar);
      propTime[("PT-" + (*ai)).c_str()] = propTimeVars.getSize() - 1;
    }

    // Create the step and stepTime variables, as well as associated addEffect variables
    for (std::map<size_t, IlpAction*>::const_iterator ai =
             plan_->steps().begin();
         ai != plan_->steps().end(); ai++) {
      IloNumVar stepVar(env, 0, 1, IloNumVar::Bool);
      stepVars.add(stepVar);
      step[("SU-" + (*ai).second->name()).c_str()] = stepVars.getSize() - 1;
      IloNumVar stepTimeVar(env, 0, upperBound, IloNumVar::Int);
      stepTimeVars.add(stepTimeVar);
      stepTime[("ST-" + (*ai).second->name()).c_str()] = stepTimeVars.getSize() - 1;
      for (std::set<std::string>::const_iterator ei = (*ai).second->pos_effects().begin();
           ei != (*ai).second->pos_effects().end(); ei++) {
        // Only one instance of each plan step, so only one add effect variable
        IloNumVarArray addEffectVar(env, 1, 0, 1, IloNumVar::Bool);
        addEffectVars.add(addEffectVar);
        addEffect[("SE-" + (*ai).second->name() + "->" + (*ei)).c_str()] = addEffectVars.getSize() - 1;
      }
    }

    // Create the action and actionTime variables, as well as associated addEffect variables
    for (std::map<std::string, IlpAction*>::const_iterator ai =
             problem_->actions().begin();
         ai != problem_->actions().end(); ai++) {
      // Determine the number of instances of this action required (1 + the number of times an add effect of this action is deleted)
      int actionInstances = 1;
      for (std::set<std::string>::const_iterator ei = (*ai).second->pos_effects().begin();
           ei != (*ai).second->pos_effects().end(); ei++) {
        actionInstances += propInstances[(*ei)] - 1;
      }
      IloNumVarArray actionVar(env, actionInstances, 0, 1, IloNumVar::Bool);
      actionVars.add(actionVar);
      action[("AU-" + (*ai).first).c_str()] = actionVars.getSize() - 1;
      IloNumVarArray actionTimeVar(env, actionInstances, 0, upperBound, IloNumVar::Int);
      actionTimeVars.add(actionTimeVar);
      actionTime[("AT-" + (*ai).first).c_str()] = actionTimeVars.getSize() - 1;
      for (std::set<std::string>::const_iterator ei = (*ai).second->pos_effects().begin();
           ei != (*ai).second->pos_effects().end(); ei++) {
        IloNumVarArray addEffectVar(env, actionInstances, 0, 1, IloNumVar::Bool);
        addEffectVars.add(addEffectVar);
        addEffect[("AE-" + (*ai).first + "->" + (*ei)).c_str()] = addEffectVars.getSize() - 1;
      }
    } 
  
    // Objective is to minimise the number of actions used (sum of action usage action variables)
    IloExpr actionUsage(env);
    for (int i = 0; i < actionVars.getSize(); i++) {
      actionUsage += IloSum(actionVars[i]);
    }
    // TODO delete
    IloExpr stepUsage(env);
    for (int i = 0; i < stepVars.getSize(); i++) {
      stepUsage += stepVars[i];
    }
    IloExpr stepTimeUsage(env);
    for (int i = 0; i < stepTimeVars.getSize(); i++) {
      stepTimeUsage += stepTimeVars[i];
    }
    model.add(IloMinimize(env, actionUsage + stepUsage + stepTimeUsage));

    // Constraint set 0: all step usage variables must be 1
    for (std::map<size_t, IlpAction*>::const_iterator ai =
             plan_->steps().begin();
         ai != plan_->steps().end(); ai++) {
      model.add(stepVars[step[("SU-" + (*ai).second->name()).c_str()]] == 1);
    }

    // Constraint set 1: The goals must be achieved
    for (std::set<std::string>::const_iterator ai = problem_->goal().begin();
         ai != problem_->goal().end(); ai++) {
      IloOr goalAchieved(env);
      // For each instance of this goal prop, add the propVar to the goalAchieved expression
      for (int i = 0; i < propVars[prop[("PA-" + (*ai)).c_str()]].getSize(); i++) {
        IloAnd instanceAchieved(env);
        instanceAchieved.add(propVars[prop[("PA-" + (*ai)).c_str()]][i] == 1);
        // For each step that deletes this prop, the time of the prop must be greater than the time of the step
        for (std::map<size_t, IlpAction*>::const_iterator si =
                 plan_->steps().begin();
             si != plan_->steps().end(); si++) {
          if ((*si).second->neg_effects().count(*ai) > 0) {
            instanceAchieved.add(propTimeVars[propTime[("PT-" + (*ai)).c_str()]][i] - stepTimeVars[stepTime[("ST-" + (*si).second->name()).c_str()]] > 0);
          }
        }
        goalAchieved.add(instanceAchieved);
      }
      model.add(goalAchieved);
    }

    // Constraint set 2: Actions require their preconditions
    for (std::map<std::string, IlpAction*>::const_iterator ai =
             problem_->actions().begin();
         ai != problem_->actions().end(); ai++) {
      for (std::set<std::string>::const_iterator ci = (*ai).second->conditions().begin();
           ci != (*ai).second->conditions().end(); ci++) {
        // For each instance of the action, sum of propVars for each precondition must be greater than or equal to the actionVar
        for (int i = 0; i < actionVars[action[("AU-" + (*ai).first).c_str()]].getSize(); i++) {
          model.add(IloSum(propVars[prop[("PA-" + (*ci)).c_str()]]) >= actionVars[action[("AU-" + (*ai).first).c_str()]][i]);
        }
      }
    }
    for (std::map<size_t, IlpAction*>::const_iterator ai =
             plan_->steps().begin();
         ai != plan_->steps().end(); ai++) {
      for (std::set<std::string>::const_iterator ci = (*ai).second->conditions().begin();
           ci != (*ai).second->conditions().end(); ci++) {
        model.add(IloSum(propVars[prop[("PA-" + (*ci)).c_str()]]) >= stepVars[step[("SU-" + (*ai).second->name()).c_str()]]);
      }
    }

    // Constraint set 3: An action can be the first achiever only if it is used
    for (std::map<std::string, IlpAction*>::const_iterator ai =
             problem_->actions().begin();
         ai != problem_->actions().end(); ai++) {
      for (std::set<std::string>::const_iterator ei = (*ai).second->pos_effects().begin();
           ei != (*ai).second->pos_effects().end(); ei++) {
        for (int i = 0; i < actionVars[action[("AU-" + (*ai).first).c_str()]].getSize(); i++) {
          model.add(actionVars[action[("AU-" + (*ai).first).c_str()]][i] >= addEffectVars[addEffect[("AE-" + (*ai).first + "->" + (*ei)).c_str()]][i]);
        }
      }
    }
    for (std::map<size_t, IlpAction*>::const_iterator ai =
             plan_->steps().begin();
         ai != plan_->steps().end(); ai++) {
      for (std::set<std::string>::const_iterator ei = (*ai).second->pos_effects().begin();
           ei != (*ai).second->pos_effects().end(); ei++) {
        model.add(stepVars[step[("SU-" + (*ai).second->name()).c_str()]] >= addEffectVars[addEffect[("SE-" + (*ai).second->name() + "->" + (*ei)).c_str()]][0]);
      }
    }

    // Constraint set 4: If a proposition is achieved, it must be true in the initial state or be the effect of some action
    for (std::set<std::string>::const_iterator pi = problem_->props().begin();
         pi != problem_->props().end(); pi++) {
      // Create an expression for the sum of add effects
      IloNumVarArray propAddEffects(env);
      for (std::map<std::string, IlpAction*>::const_iterator ai =
               problem_->actions().begin();
           ai != problem_->actions().end(); ai++) {
        if ((*ai).second->pos_effects().count(*pi) > 0) {
          for (int i = 0; i < actionVars[action[("AU-" + (*ai).first).c_str()]].getSize(); i++) {
            propAddEffects.add(addEffectVars[addEffect[("AE-" + (*ai).first + "->" + (*pi)).c_str()]][i]);
          }
        }
      }
      for (std::map<size_t, IlpAction*>::const_iterator ai =
               plan_->steps().begin();
           ai != plan_->steps().end(); ai++) {
        if ((*ai).second->pos_effects().count(*pi) > 0) {
          propAddEffects.add(addEffectVars[addEffect[("SE-" + (*ai).second->name() + "->" + (*pi)).c_str()]]);
        }
      }
      IloNumVarArray propSatisfied(env);
      for (int i = 0; i < propVars[prop[("PA-" + (*pi)).c_str()]].getSize(); i++) {
        propSatisfied.add(propVars[prop[("PA-" + (*pi)).c_str()]][i]);
      }
      model.add(IloSum(propSatisfied) == problem_->init().count(*pi) + IloSum(propAddEffects));
    }

    // Constraint set 5: Actions must be preceded by the satisfaction of their preconditions
    for (std::map<std::string, IlpAction*>::const_iterator ai =
             problem_->actions().begin();
         ai != problem_->actions().end(); ai++) {
      for (std::set<std::string>::const_iterator ci = (*ai).second->conditions().begin();
           ci != (*ai).second->conditions().end(); ci++) {
        for (int i = 0; i < actionVars[action[("AU-" + (*ai).first).c_str()]].getSize(); i++) {
          // Get the number of instances of the precondition that are satisfied before the action
          IloNumVarArray preconditionSatisfied(env);
          for (int j = 0; j < propVars[prop[("PA-" + (*ci)).c_str()]].getSize(); j++) {
            IloNumVar instanceSatisfied(env, 0, 1, IloNumVar::Bool);
            model.add(instanceSatisfied == (propTimeVars[propTime[("PT-" + (*ci)).c_str()]][j] - actionTimeVars[actionTime[("AT-" + (*ai).first).c_str()]][i] + M * (1 - propVars[prop[("PA-" + (*ci)).c_str()]][j]) <= 0));
            preconditionSatisfied.add(instanceSatisfied);
          }
          // Get the number of steps which delete the precondition that occur before the action
          IloNumVarArray preconditionDeleted(env);
          for (std::map<size_t, IlpAction*>::const_iterator si =
                   plan_->steps().begin();
               si != plan_->steps().end(); si++) {
            // If this is the same step as the action, then skip it
            if ((*si).second->name() == (*ai).first) {
              continue;
            }
            if ((*si).second->neg_effects().count(*ci) > 0) {
              IloNumVar instanceDeleted(env, 0, 1, IloNumVar::Bool);
              model.add(instanceDeleted == (stepTimeVars[stepTime[("ST-" + (*si).second->name()).c_str()]] - actionTimeVars[actionTime[("AT-" + (*ai).first).c_str()]][i] <= 0));
              preconditionDeleted.add(instanceDeleted);
            }
          }
          // The precondition must be satisfied more than it is deleted
          model.add(IloSum(preconditionSatisfied) - IloSum(preconditionDeleted) > 0);
        }
      }
    }
    std::map<std::pair<std::string, std::string>, int> preconditionSatisfiedMap;
    std::map<std::pair<std::string, std::string>, int> preconditionDeletedMap;
    IloArray<IloNumVarArray> preconditionSatisfiedVars(env);
    IloArray<IloNumVarArray> preconditionDeletedVars(env);
    for (std::map<size_t, IlpAction*>::const_iterator ai =
             plan_->steps().begin();
         ai != plan_->steps().end(); ai++) {
      for (std::set<std::string>::const_iterator ci = (*ai).second->conditions().begin();
           ci != (*ai).second->conditions().end(); ci++) {
        // Get the number of instances of the precondition that are satisfied before the action
        IloNumVarArray preconditionSatisfied(env);
        for (int j = 0; j < propVars[prop[("PA-" + (*ci)).c_str()]].getSize(); j++) {
          IloNumVar instanceSatisfied(env, 0, 1, IloNumVar::Bool);
          model.add(instanceSatisfied == (propTimeVars[propTime[("PT-" + (*ci)).c_str()]][j] - stepTimeVars[stepTime[("ST-" + (*ai).second->name()).c_str()]] + M * (1 - propVars[prop[("PA-" + (*ci)).c_str()]][j]) <= 0));
          preconditionSatisfied.add(instanceSatisfied);
        }
        preconditionSatisfiedVars.add(preconditionSatisfied);
        preconditionSatisfiedMap[std::make_pair((*ai).second->name(), (*ci))] = preconditionSatisfiedVars.getSize() - 1;
        // Get the number of steps which delete the precondition that occur before the action
        IloNumVarArray preconditionDeleted(env);
        for (std::map<size_t, IlpAction*>::const_iterator si =
                 plan_->steps().begin();
             si != plan_->steps().end(); si++) {
          if ((*si).second->neg_effects().count(*ci) > 0) {
            IloNumVar instanceDeleted(env, 0, 1, IloNumVar::Bool);
            model.add(instanceDeleted == (stepTimeVars[stepTime[("ST-" + (*si).second->name()).c_str()]] - stepTimeVars[stepTime[("ST-" + (*ai).second->name()).c_str()]] < 0));
            preconditionDeleted.add(instanceDeleted);
          }
        }
        preconditionDeletedVars.add(preconditionDeleted);
        preconditionDeletedMap[std::make_pair((*ai).second->name(), (*ci))] = preconditionDeletedVars.getSize() - 1;
        // The precondition must be satisfied more than it is deleted
        model.add(IloSum(preconditionSatisfied) - IloSum(preconditionDeleted) > 0);
      }
    }

    // Constraint set 6: If a is the first achiever of p, then a must precede p
    for (std::map<std::string, IlpAction*>::const_iterator ai =
             problem_->actions().begin();
         ai != problem_->actions().end(); ai++) {
      for (std::set<std::string>::const_iterator ei = (*ai).second->pos_effects().begin();
           ei != (*ai).second->pos_effects().end(); ei++) {
        for (int i = 0; i < actionVars[action[("AU-" + (*ai).first).c_str()]].getSize(); i++) {
          IloOr firstAchiever(env);
          for (int j = 0; j < propVars[prop[("PA-" + (*ei)).c_str()]].getSize(); j++) {
            firstAchiever.add(actionTimeVars[actionTime[("AT-" + (*ai).first).c_str()]][i] + 1 <=
                  propTimeVars[propTime[("PT-" + (*ei)).c_str()]][j] + M * (1 - addEffectVars[addEffect[("AE-" + (*ai).first + "->" + (*ei)).c_str()]][i]));
          }
          model.add(firstAchiever);
        }
      }
    }
    for (std::map<size_t, IlpAction*>::const_iterator ai =
             plan_->steps().begin();
         ai != plan_->steps().end(); ai++) {
      for (std::set<std::string>::const_iterator ei = (*ai).second->pos_effects().begin();
           ei != (*ai).second->pos_effects().end(); ei++) {
        IloOr firstAchiever(env);
        for (int j = 0; j < propVars[prop[("PA-" + (*ei)).c_str()]].getSize(); j++) {
          os << "ST-" << (*ai).second->name() << " + 1 <= " << "PT-" << (*ei) << "[" << j << "] - IF SE-" << (*ai).second->name() << "->" << (*ei) << std::endl;
          firstAchiever.add(stepTimeVars[stepTime[("ST-" + (*ai).second->name()).c_str()]] + 1 <=
                propTimeVars[propTime[("PT-" + (*ei)).c_str()]][j] + M * (1 - addEffectVars[addEffect[("SE-" + (*ai).second->name() + "->" + (*ei)).c_str()]][0]));
        }
        model.add(firstAchiever);
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
      // Iterate through both dimensions of the propVars array
      for (std::map<std::string, int>::const_iterator ai = prop.begin();
           ai != prop.end(); ai++) {
        cplex.getValues(vals, propVars[(*ai).second]);
        for (int i = 0; i < propVars[(*ai).second].getSize(); i++) {
          os << "Prop " << (*ai).first << "[" << i << "] = " << vals[i] << std::endl;
        }
      }
      // Iterate through both dimensions of the propTimeVars array
      for (std::map<std::string, int>::const_iterator ai = propTime.begin();
           ai != propTime.end(); ai++) {
        cplex.getValues(vals, propTimeVars[(*ai).second]);
        for (int i = 0; i < propTimeVars[(*ai).second].getSize(); i++) {
          os << "Prop Time " << (*ai).first << "[" << i << "] = " << vals[i] << std::endl;
        }
      }
      // Iterate through both dimensions of the actionVars array
      for (std::map<std::string, int>::const_iterator ai = action.begin();
           ai != action.end(); ai++) {
        cplex.getValues(vals, actionVars[(*ai).second]);
        for (int i = 0; i < actionVars[(*ai).second].getSize(); i++) {
          os << "Action " << (*ai).first << "[" << i << "] = " << vals[i] << std::endl;
        }
      }
      // Iterate through both dimensions of the actionTimeVars array
      for (std::map<std::string, int>::const_iterator ai = actionTime.begin();
           ai != actionTime.end(); ai++) {
        cplex.getValues(vals, actionTimeVars[(*ai).second]);
        for (int i = 0; i < actionTimeVars[(*ai).second].getSize(); i++) {
          os << "Action Time " << (*ai).first << "[" << i << "] = " << vals[i] << std::endl;
        }
      }
      // Iterate through both dimensions of the addEffectVars array
      for (std::map<std::string, int>::const_iterator ai = addEffect.begin();
           ai != addEffect.end(); ai++) {
        cplex.getValues(vals, addEffectVars[(*ai).second]);
        for (int i = 0; i < addEffectVars[(*ai).second].getSize(); i++) {
          os << "Add Effect " << (*ai).first << "[" << i << "] = " << vals[i] << std::endl;
        }
      }
      // Iterate through the single dimension of the stepVars array
      cplex.getValues(vals, stepVars);
      for (std::map<std::string, int>::const_iterator ai = step.begin();
           ai != step.end(); ai++) {
        os << "Step " << (*ai).first << " = " << vals[(*ai).second] << std::endl;
      }
      // Iterate through the single dimension of the stepTimeVars array
      cplex.getValues(vals, stepTimeVars);
      for (std::map<std::string, int>::const_iterator ai = stepTime.begin();
           ai != stepTime.end(); ai++) {
        os << "Step Time " << (*ai).first << " = " << vals[(*ai).second] << std::endl;
      }
      // Precondition satisfied vars
      for (std::map<size_t, IlpAction*>::const_iterator ai =
             plan_->steps().begin();
         ai != plan_->steps().end(); ai++) {
        for (std::set<std::string>::const_iterator ci = (*ai).second->conditions().begin();
             ci != (*ai).second->conditions().end(); ci++) {
          cplex.getValues(vals, preconditionSatisfiedVars[preconditionSatisfiedMap[std::make_pair((*ai).second->name(), (*ci))]]);
          for (int i = 0; i < preconditionSatisfiedVars[preconditionSatisfiedMap[std::make_pair((*ai).second->name(), (*ci))]].getSize(); i++) {
            os << "Precondition Satisfied " << (*ai).second->name() << "->" << (*ci) << "[" << i << "] = " << vals[i] << std::endl;
          }
        }
      }
      // Precondition deleted vars
      for (std::map<size_t, IlpAction*>::const_iterator ai =
             plan_->steps().begin();
         ai != plan_->steps().end(); ai++) {
        for (std::set<std::string>::const_iterator ci = (*ai).second->conditions().begin();
             ci != (*ai).second->conditions().end(); ci++) {
          cplex.getValues(vals, preconditionDeletedVars[preconditionDeletedMap[std::make_pair((*ai).second->name(), (*ci))]]);
          for (int i = 0; i < preconditionDeletedVars[preconditionDeletedMap[std::make_pair((*ai).second->name(), (*ci))]].getSize(); i++) {
            os << "Precondition Deleted " << (*ai).second->name() << "->" << (*ci) << "[" << i << "] = " << vals[i] << std::endl;
          }
        }
      }
    }
    //if (verbosity >= 1) {
    //  cplex.getValues(vals, actionVars);
    //  std::map<std::string, int> actionVals;
    //  for (std::map<std::string, int>::const_iterator ai = action.begin();
    //       ai != action.end(); ai++) {
    //    actionVals[(*ai).first] = vals[(*ai).second];
    //  }
    //  cplex.getValues(vals, actionTimeVars);
    //  std::map<int, std::string> actionTimeVals;
    //  for (std::map<std::string, int>::const_iterator ai = actionTime.begin();
    //       ai != actionTime.end(); ai++) {
    //    actionTimeVals[vals[(*ai).second]] = (*ai).first;
    //  }
    //  // Iterate through the sorted action time values and print each action if it is used
    //  os << "INIT -> ";
    //  for (std::map<int, std::string>::const_iterator ai = actionTimeVals.begin();
    //       ai != actionTimeVals.end(); ai++) {
    //    if (actionVals["AU" + (*ai).second.substr(2)] == 1) {
    //      os << (*ai).second.substr(3) << " -> ";
    //    }
    //  }
    //  os << "GOAL" << std::endl;
    //}

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

void IlpNode::remove_causal_links() {
  // Iterate through the causal links
  for (std::map<size_t, std::pair<size_t, std::string>>::const_iterator li =
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
    // For each action in plan, if the actions neg effects contains the condition, add link_prop to the preconditions
    for (std::map<size_t, IlpAction*>::const_iterator ai =
             plan_->steps().begin();
         ai != plan_->steps().end(); ai++) {
      // Skip if this is the to action
      std::cout << "Checking action " << (*ai).second->name() << " Lid=" << (*li).second.first << " Aid=" << (*ai).first << std::endl;
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