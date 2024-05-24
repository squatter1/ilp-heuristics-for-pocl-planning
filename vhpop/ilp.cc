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
    // Check if the condition is an atom
    const Atom* atom = dynamic_cast<const Atom*>(&condition_formula);
    if (atom) {
      add_condition(PredicateTable::name((*atom).predicate()));
    } else {
      condition_formula.print(std::cout, 0, Bindings::EMPTY);
      std::cout << "Not a conjunction ERROR" << std::endl;
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
        std::cout << "Not an atom ERROR" << std::endl;
        continue; // Not an atom
      }
    }
  } else {
    // Check if the goal is an atom
    const Atom* atom = dynamic_cast<const Atom*>(&goal_formula);
    if (atom) {
      add_goal_prop(PredicateTable::name((*atom).predicate()));
    } else {
      std::cout << "Not a conjunction ERROR" << std::endl;
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

IlpProblem::IlpProblem(const Problem& problem, std::map<std::string, Predicate> new_predicates, std::map<std::string, const ActionSchema *> new_actions)
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
        std::cout << "Not an atom ERROR" << std::endl;
        continue; // Not an atom
      }
    }
  } else {
    // Check if the goal is an atom
    const Atom* atom = dynamic_cast<const Atom*>(&goal_formula);
    if (atom) {
      add_goal_prop(PredicateTable::name((*atom).predicate()));
    } else {
      std::cout << "Not a conjunction ERROR" << std::endl;
      return; // Not a conjunction
    }
  }

  // Add the propositions
  for (std::map<std::string, Predicate>::const_iterator ai =
           new_predicates.begin();
       ai != new_predicates.end(); ai++) {
    add_prop((*ai).first);
  }

  // Add the actions
  for (std::map<std::string, const ActionSchema*>::const_iterator ai =
             new_actions.begin();
         ai != new_actions.end(); ai++) {
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
        model.add(actionTimeVars[actionTime[("AT-" + (*ai).first).c_str()]]
               >= propTimeVars[propTime[("PT-" + (*ci)).c_str()]]);
      }
    }

    // Constraint set 6: If a is the first achiever of p, then a must precede p
    for (std::map<std::string, IlpAction*>::const_iterator ai =
             actions_.begin();
         ai != actions_.end(); ai++) {
      for (std::set<std::string>::const_iterator ei = (*ai).second->pos_effects().begin();
           ei != (*ai).second->pos_effects().end(); ei++) {
        const size_t M = numActions + 1;
        model.add(actionTimeVars[actionTime[("AT-" + (*ai).first).c_str()]] + 1 
               <= propTimeVars[propTime[("PT-" + (*ei)).c_str()]] 
                + M * (1 - addEffectVars[addEffect[("AE-" + (*ai).first + "->" + (*ei)).c_str()]]));
      }
    }

    IloCplex cplex(model);
    //if (verbosity < 2) cplex.setOut(env.getNullStream());

    // Optimize the problem and obtain solution.
    if ( !cplex.solve() ) {
      env.error() << "Failed to optimize LP" << std::endl;
      throw(-1);
    }

    IloNumArray vals(env);
    const size_t objectiveValue = static_cast<size_t>(cplex.getObjValue());
    if (verbosity >= 3) {
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
        actionVals[(*ai).first.substr(3)] = std::round(vals[(*ai).second]);
      }
      cplex.getValues(vals, actionTimeVars);
      std::map<std::string, int> actionTimeVals;
      for (std::map<std::string, int>::const_iterator ai = actionTime.begin();
           ai != actionTime.end(); ai++) {
        actionTimeVals[(*ai).first.substr(3)] = std::round(vals[(*ai).second]);
      }
      // Add (action, time) pairs to sorted_actions
      std::vector<std::pair<std::string, int>> sorted_actions;
      for (std::map<std::string, int>::const_iterator ai = actionVals.begin();
           ai != actionVals.end(); ai++) {
        if ((*ai).second == 1) {
          sorted_actions.push_back(std::make_pair((*ai).first, actionTimeVals[(*ai).first]));
        }
      }
      // Sort the actions by time
      std::sort(sorted_actions.begin(), sorted_actions.end(), [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
        return a.second < b.second;
      });
      // Print the sorted actions
      os << "INIT";
      int last_time = -1;
      for (const auto& action : sorted_actions) {
        if (action.second == last_time) {
          os << " AND " << action.first;
        } else {
          os << " -> " << action.first;
        }
        last_time = action.second;
      }
      os << " -> GOAL" << std::endl;
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

IlpNode::IlpNode(const Problem& problem, const Plan& plan, std::map<std::string, Predicate> new_predicates, std::map<std::string, const ActionSchema *> new_actions)
    : id_(plan.serial_no()) {
  problem_ = new IlpProblem(problem, new_predicates, new_actions);
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

const size_t IlpNode::solve(std::ostream& os, short int verbosity, bool lp_relax, size_t lb, size_t ub, size_t seconds) const {
  IloEnv env;
  try {
    IloModel model(env);  

    // Props are now 2D, first dimension gets to the prop, second gets to the instance of that prop (due to deletions)
    std::map<std::string, int> prop;
    std::map<std::string, int> propTime;
    std::map<std::string, int> action;
    std::map<std::string, int> actionTime;
    std::map<std::string, int> stepTime;
    std::map<std::string, int> addEffect;

    IloArray<IloNumVarArray> propVars(env);
    IloArray<IloNumVarArray> propTimeVars(env);
    IloNumVarArray stepTimeVars(env);
    IloArray<IloNumVarArray> actionVars(env);
    IloArray<IloNumVarArray> actionTimeVars(env);
    // Add effect vars take into account duplicate actions, but don't take into account duplicate props (otherwise they would be 3D)
    IloArray<IloNumVarArray> addEffectVars(env); 
  
    IloEnv env = model.getEnv();  

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
    
    size_t upperBound = ub + plan_->steps().size();
    size_t num_actions = problem_->actions().size();
    size_t num_props = problem_->props().size();
    // Use upper bound of $min(|A|,|p|)+|del_P|+|P|$ is none is provided
    if (upperBound > std::min(num_actions, num_props) + planDelEffects + num_props) {
      upperBound = std::min(num_actions, num_props) + planDelEffects + num_props;
      ub = upperBound - plan_->steps().size();
    }
    if (verbosity >= 2) {
      os << "Upper bound: " << upperBound << std::endl;
    }  

    // Create the prop and propTime variables
    for (std::set<std::string>::const_iterator ai = problem_->props().begin();
         ai != problem_->props().end(); ai++) {
      if (lp_relax) {
        IloNumVarArray propVar(env, propInstances[(*ai)], 0, 1, IloNumVar::Float);
        propVars.add(propVar);
      } else {
        IloNumVarArray propVar(env, propInstances[(*ai)], 0, 1, IloNumVar::Bool);
        propVars.add(propVar);
      }
      prop[("PA-" + (*ai)).c_str()] = propVars.getSize() - 1;
      if (lp_relax) {
        IloNumVarArray propTimeVar(env, propInstances[(*ai)], 0, upperBound, IloNumVar::Float);
        propTimeVars.add(propTimeVar);
      } else {
        IloNumVarArray propTimeVar(env, propInstances[(*ai)], 0, upperBound, IloNumVar::Int);
        propTimeVars.add(propTimeVar);
      }
      propTime[("PT-" + (*ai)).c_str()] = propTimeVars.getSize() - 1;
    }

    // Create stepTime variables, as well as associated addEffect variables
    for (std::map<size_t, IlpAction*>::const_iterator ai =
             plan_->steps().begin();
         ai != plan_->steps().end(); ai++) {
      if (lp_relax) {
        IloNumVar stepTimeVar(env, 0, upperBound, IloNumVar::Float);
        stepTimeVars.add(stepTimeVar);
      } else {
        IloNumVar stepTimeVar(env, 0, upperBound, IloNumVar::Int);
        stepTimeVars.add(stepTimeVar);
      }
      stepTime[("ST-" + (*ai).second->name()).c_str()] = stepTimeVars.getSize() - 1;
      for (std::set<std::string>::const_iterator ei = (*ai).second->pos_effects().begin();
           ei != (*ai).second->pos_effects().end(); ei++) {
        // Only one instance of each plan step, so only one add effect variable
        if (lp_relax) {
          IloNumVarArray addEffectVar(env, 1, 0, 1, IloNumVar::Float);
          addEffectVars.add(addEffectVar);
        } else {
          IloNumVarArray addEffectVar(env, 1, 0, 1, IloNumVar::Bool);
          addEffectVars.add(addEffectVar);
        }
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
      if (lp_relax) {
        IloNumVarArray actionVar(env, actionInstances, 0, 1, IloNumVar::Float);
        actionVars.add(actionVar);
      } else {
        IloNumVarArray actionVar(env, actionInstances, 0, 1, IloNumVar::Bool);
        actionVars.add(actionVar);
      }
      action[("AU-" + (*ai).first).c_str()] = actionVars.getSize() - 1;
      if (lp_relax) {
        IloNumVarArray actionTimeVar(env, actionInstances, 0, upperBound, IloNumVar::Float);
        actionTimeVars.add(actionTimeVar);
      } else {
        IloNumVarArray actionTimeVar(env, actionInstances, 0, upperBound, IloNumVar::Int);
        actionTimeVars.add(actionTimeVar);
      }
      actionTime[("AT-" + (*ai).first).c_str()] = actionTimeVars.getSize() - 1;
      for (std::set<std::string>::const_iterator ei = (*ai).second->pos_effects().begin();
           ei != (*ai).second->pos_effects().end(); ei++) {
        if (lp_relax) {
          IloNumVarArray addEffectVar(env, actionInstances, 0, 1, IloNumVar::Float);
          addEffectVars.add(addEffectVar);
        } else {
          IloNumVarArray addEffectVar(env, actionInstances, 0, 1, IloNumVar::Bool);
          addEffectVars.add(addEffectVar);
        }
        addEffect[("AE-" + (*ai).first + "->" + (*ei)).c_str()] = addEffectVars.getSize() - 1;
      }
    } 
  
    // Objective is to minimise the number of actions used (sum of action usage action variables)
    IloExpr actionUsage(env);
    for (int i = 0; i < actionVars.getSize(); i++) {
      actionUsage += IloSum(actionVars[i]);
    }
    model.add(IloMinimize(env, actionUsage));

    // Set the minimum number of actions to be used
    model.add(actionUsage >= static_cast<IloNum>(lb));

    // Set the maximum number of actions to be used
    model.add(actionUsage <= static_cast<IloNum>(ub));

    // Constraint set 1: For each goal proposition, ensure that there is at least one instance achieved after all of its deletions.
    for (std::set<std::string>::const_iterator ai = problem_->goal().begin();
         ai != problem_->goal().end(); ai++) {
      IloOr goalAchieved(env);
      // For at least one instance of the prop, it must be equal to 1 and all of its deletions must be prior to it
      for (int i = 0; i < propVars[prop[("PA-" + (*ai)).c_str()]].getSize(); i++) {
        IloAnd instanceAchieved(env);
        IloConstraint instanceUsed = propVars[prop[("PA-" + (*ai)).c_str()]][i] == 1;
        instanceAchieved.add(instanceUsed);
        // For each step, if it deletes the prop, it must be before the instance
        for (std::map<size_t, IlpAction*>::const_iterator si =
                 plan_->steps().begin();
             si != plan_->steps().end(); si++) {
          if ((*si).second->neg_effects().count(*ai) > 0) {
            IloConstraint deletionBeforeInstance = stepTimeVars[stepTime[("ST-" + (*si).second->name()).c_str()]] - propTimeVars[propTime[("PT-" + (*ai)).c_str()]][i] + 1 <= 0;
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
             problem_->actions().begin();
         ai != problem_->actions().end(); ai++) {
      for (std::set<std::string>::const_iterator ei = (*ai).second->pos_effects().begin();
           ei != (*ai).second->pos_effects().end(); ei++) {
        for (int i = 0; i < actionVars[action[("AU-" + (*ai).first).c_str()]].getSize(); i++) {
          IloConstraint c3 = actionVars[action[("AU-" + (*ai).first).c_str()]][i] >= addEffectVars[addEffect[("AE-" + (*ai).first + "->" + (*ei)).c_str()]][i];
          model.add(c3);
          c3.setName(("C3-A_" + (*ai).first + "-E_" + (*ei)).c_str());
        }
      }
    }

    // Constraint set 4 not required

    // Constraint set 5: For each precondition, ensure one instance is achieved beforehand with no deletions in-between the prop time and action time
    for (std::map<std::string, IlpAction*>::const_iterator ai =
             problem_->actions().begin();
         ai != problem_->actions().end(); ai++) {
      for (std::set<std::string>::const_iterator ci = (*ai).second->conditions().begin();
           ci != (*ai).second->conditions().end(); ci++) {
        for (int i = 0; i < actionVars[action[("AU-" + (*ai).first).c_str()]].getSize(); i++) {
          IloOr preconditionSatisfied(env);
          // Either this action instance isn't used
          IloConstraint actionNotUsed = actionVars[action[("AU-" + (*ai).first).c_str()]][i] == 0;
          preconditionSatisfied.add(actionNotUsed);
          // Or there is an instance of the precondition that is satisfied before the action without being deleted
          for (int j = 0; j < propVars[prop[("PA-" + (*ci)).c_str()]].getSize(); j++) {
            IloAnd preconditionSatisfiedInstance(env);
            // Precondition must be used
            IloConstraint preconditionUsed = propVars[prop[("PA-" + (*ci)).c_str()]][j] == 1;
            preconditionSatisfiedInstance.add(preconditionUsed);
            // The precondition must precede the action
            IloConstraint preconditionBeforeAction = propTimeVars[propTime[("PT-" + (*ci)).c_str()]][j] - actionTimeVars[actionTime[("AT-" + (*ai).first).c_str()]][i] <= 0;
            preconditionSatisfiedInstance.add(preconditionBeforeAction);
            // For each step which deletes the precondition, it must either be before the prop or after the action
            for (std::map<size_t, IlpAction*>::const_iterator si =
                     plan_->steps().begin();
                 si != plan_->steps().end(); si++) {
              if ((*si).second->neg_effects().count(*ci) > 0) {
                IloOr noDeletion(env);
                // First case: the step is before the prop
                IloConstraint deletionBeforeProp = stepTimeVars[stepTime[("ST-" + (*si).second->name()).c_str()]] - propTimeVars[propTime[("PT-" + (*ci)).c_str()]][j] + 1 <= 0;
                noDeletion.add(deletionBeforeProp);
                // Second case: the step is after the action
                IloConstraint deletionAfterAction = stepTimeVars[stepTime[("ST-" + (*si).second->name()).c_str()]] - actionTimeVars[actionTime[("AT-" + (*ai).first).c_str()]][i] >= 1;
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
             plan_->steps().begin();
         ai != plan_->steps().end(); ai++) {
      for (std::set<std::string>::const_iterator ci = (*ai).second->conditions().begin();
           ci != (*ai).second->conditions().end(); ci++) {
        IloOr preconditionSatisfied(env);
        // There must be an instance of the precondition that is satisfied before the step without being deleted
        for (int j = 0; j < propVars[prop[("PA-" + (*ci)).c_str()]].getSize(); j++) {
          IloAnd preconditionSatisfiedInstance(env);
          // Precondition must be used
          IloConstraint preconditionUsed = propVars[prop[("PA-" + (*ci)).c_str()]][j] == 1;
          preconditionSatisfiedInstance.add(preconditionUsed);
          // The precondition must precede the step
          IloConstraint preconditionBeforeStep = propTimeVars[propTime[("PT-" + (*ci)).c_str()]][j] - stepTimeVars[stepTime[("ST-" + (*ai).second->name()).c_str()]] <= 0;
          preconditionSatisfiedInstance.add(preconditionBeforeStep);
          // For each step which deletes the precondition, it must either be before the prop or after the step
          for (std::map<size_t, IlpAction*>::const_iterator si =
                   plan_->steps().begin();
               si != plan_->steps().end(); si++) {
            // If this is the same step, skip it
            if ((*si).second->name() == (*ai).second->name()) {
              continue;
            }
            if ((*si).second->neg_effects().count(*ci) > 0) {
              IloOr noDeletion(env);
              // First case: the step is before the prop
              IloConstraint deletionBeforeProp = stepTimeVars[stepTime[("ST-" + (*si).second->name()).c_str()]] - propTimeVars[propTime[("PT-" + (*ci)).c_str()]][j] + 1 <= 0;
              noDeletion.add(deletionBeforeProp);
              // Second case: the step is after the step
              IloConstraint deletionAfterStep = stepTimeVars[stepTime[("ST-" + (*si).second->name()).c_str()]] - stepTimeVars[stepTime[("ST-" + (*ai).second->name()).c_str()]] >= 1;
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
    for (std::set<std::string>::const_iterator pi = problem_->props().begin();
         pi != problem_->props().end(); pi++) {
      for (int i = 0; i < propVars[prop[("PA-" + (*pi)).c_str()]].getSize(); i++) {
        // Set init prop instances to time 0
        if (problem_->init().count(*pi) > 0 && i == 0) {
          model.add(propVars[prop[("PA-" + (*pi)).c_str()]][i] == 1);
          model.add(propTimeVars[propTime[("PT-" + (*pi)).c_str()]][i] == 0);
          continue;
        }
        IloOr propAchieved(env);
        // Either the instance is false, or it is true and there is an action that achieves it
        IloConstraint propFalse = propVars[prop[("PA-" + (*pi)).c_str()]][i] == 0;
        propAchieved.add(propFalse);
        // For at least one achiever of the prop, it must be equal to 1 and one step before the instance
        for (std::map<std::string, IlpAction*>::const_iterator ai =
                 problem_->actions().begin();
             ai != problem_->actions().end(); ai++) {
          if ((*ai).second->pos_effects().count(*pi) > 0) {
            for (int j = 0; j < actionVars[action[("AU-" + (*ai).first).c_str()]].getSize(); j++) {
              IloAnd instanceAchieved(env);
              // Action effect = 1
              IloConstraint actionEffect = addEffectVars[addEffect[("AE-" + (*ai).first + "->" + (*pi)).c_str()]][j] == 1;
              instanceAchieved.add(actionEffect);
              // Prop time - Action time = 1
              IloConstraint propAchievedBeforeAction = propTimeVars[propTime[("PT-" + (*pi)).c_str()]][i] - actionTimeVars[actionTime[("AT-" + (*ai).first).c_str()]][j] - 1 == 0;
              instanceAchieved.add(propAchievedBeforeAction);
              propAchieved.add(instanceAchieved);
            }
          }
        }
        // Also check for steps that achieve the proposition
        for (std::map<size_t, IlpAction*>::const_iterator ai =
                 plan_->steps().begin();
             ai != plan_->steps().end(); ai++) {
          if ((*ai).second->pos_effects().count(*pi) > 0) {
            IloAnd instanceAchieved(env);
            // Action effect = 1
            IloConstraint actionEffect = addEffectVars[addEffect[("SE-" + (*ai).second->name() + "->" + (*pi)).c_str()]][0] == 1;
            instanceAchieved.add(actionEffect);
            // Prop time - Step time = 1
            IloConstraint propAchievedBeforeStep = propTimeVars[propTime[("PT-" + (*pi)).c_str()]][i] - stepTimeVars[stepTime[("ST-" + (*ai).second->name()).c_str()]] - 1 == 0;
            instanceAchieved.add(propAchievedBeforeStep);
            propAchieved.add(instanceAchieved);
          }
        }
        model.add(propAchieved);
      }
    }


    // Constraint set 7: Step ordering constraints
    for (std::map<size_t, size_t>::const_iterator oi = plan_->orderings().begin();
         oi != plan_->orderings().end(); oi++) {
      IloConstraint c7 = stepTimeVars[stepTime[("ST-" + plan_->steps().at((*oi).first)->name()).c_str()]] 
                       - stepTimeVars[stepTime[("ST-" + plan_->steps().at((*oi).second)->name()).c_str()]] + 1 <= 0;
      model.add(c7);
      c7.setName(("C7-S_" + plan_->steps().at((*oi).first)->name() + "-S_" + plan_->steps().at((*oi).second)->name()).c_str());
    }

    // Constraint set 8: Step interference
    for (std::map<size_t, IlpAction*>::const_iterator ai =
             plan_->steps().begin();
         ai != plan_->steps().end(); ai++) {
      for (std::map<size_t, IlpAction*>::const_iterator bi =
               plan_->steps().begin();
           bi != plan_->steps().end(); bi++) {
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
              IloConstraint c9 = (stepTimeVars[stepTime[("ST-" + (*ai).second->name()).c_str()]] - stepTimeVars[stepTime[("ST-" + (*bi).second->name()).c_str()]] != 0);
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
             plan_->steps().begin();
         ai != plan_->steps().end(); ai++) {
      for (std::map<size_t, IlpAction*>::const_iterator bi =
               plan_->steps().begin();
           bi != plan_->steps().end(); bi++) {
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
              IloConstraint c10 = (stepTimeVars[stepTime[("ST-" + (*ai).second->name()).c_str()]] - stepTimeVars[stepTime[("ST-" + (*bi).second->name()).c_str()]] != 0);
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


    IloCplex cplex(model);
    if (verbosity < 2) cplex.setOut(env.getNullStream());

    // Set the lower limit of objective function parameter CPX_PARAM_OBJLLIM to lb
    cplex.setParam(IloCplex::Param::MIP::Limits::LowerObjStop, lb);
    // Set time limit
    cplex.setParam(IloCplex::Param::TimeLimit, seconds);

    // Optimize the problem and obtain solution.
    cplex.solve();

    if (cplex.getStatus() == IloAlgorithm::Infeasible) {
        env.end();
        return -1;
    }

    IloNumArray vals(env);
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
             plan_->steps().begin();
         ai != plan_->steps().end(); ai++) {
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
  return -2;
}


const size_t IlpNode::counting_solve(std::ostream& os, short int verbosity, bool lp_relax, size_t lb, size_t ub, size_t seconds) const {
  IloEnv env;
  try {
    IloModel model(env);  

    // Props are now 2D, first dimension gets to the prop, second gets to the instance of that prop (due to deletions)
    std::map<std::string, int> prop;
    std::map<std::string, int> propTime;
    std::map<std::string, int> action;
    std::map<std::string, int> actionTime;
    std::map<std::string, int> stepTime;
    std::map<std::string, int> addEffect;

    IloArray<IloNumVarArray> propVars(env);
    IloArray<IloNumVarArray> propTimeVars(env);
    IloNumVarArray stepTimeVars(env);
    IloArray<IloNumVarArray> actionVars(env);
    IloArray<IloNumVarArray> actionTimeVars(env);
    // Add effect vars take into account duplicate actions, but don't take into account duplicate props (otherwise they would be 3D)
    IloArray<IloNumVarArray> addEffectVars(env); 
  
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

    size_t upperBound = ub + plan_->steps().size();
    size_t num_actions = problem_->actions().size();
    size_t num_props = problem_->props().size();
    // Use upper bound of $min(|A|,|p|)+|del_P|+|P|$ is none is provided
    if (upperBound > std::min(num_actions, num_props) + planDelEffects + num_props) {
      upperBound = std::min(num_actions, num_props) + planDelEffects + num_props;
      ub = upperBound - plan_->steps().size();
    }
    
    const size_t M = upperBound + 2; // + 2 to avoid issues where we are comparing times with a difference of 1
    if (verbosity >= 2) {
      os << "Upper bound: " << upperBound << std::endl;
    }

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

    // Create stepTime variables, as well as associated addEffect variables
    for (std::map<size_t, IlpAction*>::const_iterator ai =
             plan_->steps().begin();
         ai != plan_->steps().end(); ai++) {
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
    model.add(IloMinimize(env, actionUsage));

    // Set the minimum number of actions to be used
    model.add(actionUsage >= static_cast<IloNum>(lb));

    // Set the maximum number of actions to be used
    model.add(actionUsage <= static_cast<IloNum>(ub));

    // Constraint set 1: The goals must be achieved
    for (std::set<std::string>::const_iterator ai = problem_->goal().begin();
         ai != problem_->goal().end(); ai++) {
      // Get the number of steps which have this prop as a neg effect
      size_t deletions = 0;
      for (std::map<size_t, IlpAction*>::const_iterator si =
               plan_->steps().begin();
           si != plan_->steps().end(); si++) {
        if ((*si).second->neg_effects().count(*ai) > 0) {
          deletions++;
        }
      }
      IloConstraint c1 = IloSum(propVars[prop[("PA-" + (*ai)).c_str()]]) - deletions >= 1;
      model.add(c1);
      c1.setName(("C1-" + (*ai)).c_str());
    }

    // Constraint set 2: Actions require their preconditions
    for (std::map<std::string, IlpAction*>::const_iterator ai =
             problem_->actions().begin();
         ai != problem_->actions().end(); ai++) {
      for (std::set<std::string>::const_iterator ci = (*ai).second->conditions().begin();
           ci != (*ai).second->conditions().end(); ci++) {
        // For each instance of the action, sum of propVars for each precondition must be greater than or equal to the actionVar
        for (int i = 0; i < actionVars[action[("AU-" + (*ai).first).c_str()]].getSize(); i++) {
          IloConstraint c2 = IloSum(propVars[prop[("PA-" + (*ci)).c_str()]]) >= actionVars[action[("AU-" + (*ai).first).c_str()]][i];
          model.add(c2);
          c2.setName(("C2-A_" + (*ai).first + "-P_" + (*ci)).c_str());
        }
      }
    }
    for (std::map<size_t, IlpAction*>::const_iterator ai =
             plan_->steps().begin();
         ai != plan_->steps().end(); ai++) {
      for (std::set<std::string>::const_iterator ci = (*ai).second->conditions().begin();
           ci != (*ai).second->conditions().end(); ci++) {
        IloConstraint c2 = IloSum(propVars[prop[("PA-" + (*ci)).c_str()]]) >= 1;
        model.add(c2);
        c2.setName(("C2-S_" + (*ai).second->name() + "-P_" + (*ci)).c_str());
      }
    }

    // Constraint set 3: An action can be the first achiever only if it is used
    for (std::map<std::string, IlpAction*>::const_iterator ai =
             problem_->actions().begin();
         ai != problem_->actions().end(); ai++) {
      for (std::set<std::string>::const_iterator ei = (*ai).second->pos_effects().begin();
           ei != (*ai).second->pos_effects().end(); ei++) {
        for (int i = 0; i < actionVars[action[("AU-" + (*ai).first).c_str()]].getSize(); i++) {
          IloConstraint c3 = actionVars[action[("AU-" + (*ai).first).c_str()]][i] >= addEffectVars[addEffect[("AE-" + (*ai).first + "->" + (*ei)).c_str()]][i];
          model.add(c3);
          c3.setName(("C3-A_" + (*ai).first + "-E_" + (*ei)).c_str());
        }
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
          propAddEffects.add(addEffectVars[addEffect[("SE-" + (*ai).second->name() + "->" + (*pi)).c_str()]][0]);
        }
      }
      IloConstraint c4 = IloSum(propVars[prop[("PA-" + (*pi)).c_str()]]) <= problem_->init().count(*pi) + IloSum(propAddEffects);
      model.add(c4);
      c4.setName(("C4-" + (*pi)).c_str());
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
            model.add(instanceSatisfied == (propTimeVars[propTime[("PT-" + (*ci)).c_str()]][j] 
                                          - actionTimeVars[actionTime[("AT-" + (*ai).first).c_str()]][i] 
                                          + M * (1 - propVars[prop[("PA-" + (*ci)).c_str()]][j]) <= 0));
            preconditionSatisfied.add(instanceSatisfied);
          }
          // Get the number of steps which delete the precondition that occur before the action
          IloNumVarArray preconditionDeleted(env);
          for (std::map<size_t, IlpAction*>::const_iterator si =
                   plan_->steps().begin();
               si != plan_->steps().end(); si++) {
            if ((*si).second->neg_effects().count(*ci) > 0) {
              IloNumVar instanceDeleted(env, 0, 1, IloNumVar::Bool);
              model.add(instanceDeleted == (stepTimeVars[stepTime[("ST-" + (*si).second->name()).c_str()]] 
                                          - actionTimeVars[actionTime[("AT-" + (*ai).first).c_str()]][i] < 0));
              preconditionDeleted.add(instanceDeleted);
            }
          }
          // The precondition must be satisfied more than it is deleted
          IloConstraint c5 = IloSum(preconditionSatisfied) - IloSum(preconditionDeleted) + M * (1 - actionVars[action[("AU-" + (*ai).first).c_str()]][i]) >= 1;
          model.add(c5);
          c5.setName(("C5-A_" + (*ai).first + "-P_" + (*ci)).c_str());
        }
      }
    }
    
    for (std::map<size_t, IlpAction*>::const_iterator ai =
             plan_->steps().begin();
         ai != plan_->steps().end(); ai++) {
      for (std::set<std::string>::const_iterator ci = (*ai).second->conditions().begin();
           ci != (*ai).second->conditions().end(); ci++) {
        // Get the number of instances of the precondition that are satisfied before the action
        IloNumVarArray preconditionSatisfied(env);
        for (int j = 0; j < propVars[prop[("PA-" + (*ci)).c_str()]].getSize(); j++) {
          IloNumVar instanceSatisfied(env, 0, 1, IloNumVar::Bool);
          model.add(instanceSatisfied == (propTimeVars[propTime[("PT-" + (*ci)).c_str()]][j] 
                                        - stepTimeVars[stepTime[("ST-" + (*ai).second->name()).c_str()]] 
                                        + M * (1 - propVars[prop[("PA-" + (*ci)).c_str()]][j]) <= 0));
          preconditionSatisfied.add(instanceSatisfied);
        }
        // Get the number of steps which delete the precondition that occur before the action
        IloNumVarArray preconditionDeleted(env);
        for (std::map<size_t, IlpAction*>::const_iterator si =
                 plan_->steps().begin();
             si != plan_->steps().end(); si++) {
          // If this is the same step as the action, then skip it
          if ((*si).second->name() == (*ai).second->name()) {
            continue;
          }
          if ((*si).second->neg_effects().count(*ci) > 0) {
            IloNumVar instanceDeleted(env, 0, 1, IloNumVar::Bool);
            model.add(instanceDeleted == (stepTimeVars[stepTime[("ST-" + (*si).second->name()).c_str()]] 
                                        - stepTimeVars[stepTime[("ST-" + (*ai).second->name()).c_str()]] <= 0));
            preconditionDeleted.add(instanceDeleted);
          }
        }
        // The precondition must be satisfied more than it is deleted
        model.add(IloSum(preconditionSatisfied) - IloSum(preconditionDeleted) > 0);
      }
    }

    // Constraint set 6: each non init instance of p must be associated with an achiever of p
    for (std::set<std::string>::const_iterator pi = 
             problem_->props().begin();
          pi != problem_->props().end(); pi++) {
      for (int i = 0; i < propVars[prop[("PA-" + (*pi)).c_str()]].getSize(); i++) {
        // Set init prop instances to time 0
        if (problem_->init().count(*pi) > 0 && i == 0) {
          model.add(propVars[prop[("PA-" + (*pi)).c_str()]][i] == 1);
          model.add(propTimeVars[propTime[("PT-" + (*pi)).c_str()]][i] == 0);
          continue;
        }
        // Sum the number of actions which achieve this prop
        IloNumVarArray propAchieved(env);
        for (std::map<std::string, IlpAction*>::const_iterator ai =
                 problem_->actions().begin();
             ai != problem_->actions().end(); ai++) {
          if ((*ai).second->pos_effects().count(*pi) > 0) {
            for (int j = 0; j < actionVars[action[("AU-" + (*ai).first).c_str()]].getSize(); j++) {
              IloNumVar instanceAchieved(env, 0, 1, IloNumVar::Bool);
              model.add(instanceAchieved == (propTimeVars[propTime[("PT-" + (*pi)).c_str()]][i]
                                          - actionTimeVars[actionTime[("AT-" + (*ai).first).c_str()]][j]  
                                          - M * (1 - addEffectVars[addEffect[("AE-" + (*ai).first + "->" + (*pi)).c_str()]][j]) > 0));
              propAchieved.add(instanceAchieved);
            }
          }
        }
        for (std::map<size_t, IlpAction*>::const_iterator ai =
                 plan_->steps().begin();
             ai != plan_->steps().end(); ai++) {
          if ((*ai).second->pos_effects().count(*pi) > 0) {
            IloNumVar instanceAchieved(env, 0, 1, IloNumVar::Bool);
            model.add(instanceAchieved == (propTimeVars[propTime[("PT-" + (*pi)).c_str()]][i]
                                        - stepTimeVars[stepTime[("ST-" + (*ai).second->name()).c_str()]]
                                        - M * (1 - addEffectVars[addEffect[("SE-" + (*ai).second->name() + "->" + (*pi)).c_str()]][0]) > 0));
            propAchieved.add(instanceAchieved);
          }
        }
        // Sum the number of instances of the prop that are satisfied before or at the same time as this instance
        IloNumVarArray propSatisfied(env);
        for (int j = 0; j < propVars[prop[("PA-" + (*pi)).c_str()]].getSize(); j++) {
          // If j=0 and init prop, then continue, or if j=i then continue
          if ((j == 0 && problem_->init().count(*pi) > 0) || j == i) {
            continue;
          }
          // Else, instanceSatisifed is 1 if the prop is satisfied before or at the same time as this instance
          IloNumVar instanceSatisfied(env, 0, 1, IloNumVar::Bool);
          model.add(instanceSatisfied == (propTimeVars[propTime[("PT-" + (*pi)).c_str()]][j] 
                                      - propTimeVars[propTime[("PT-" + (*pi)).c_str()]][i] <= 0));
          propSatisfied.add(instanceSatisfied);
        }
        // Prop must be not satisfied, or have more achievers than satisfied instances
        IloConstraint propHasAchiever = IloSum(propAchieved) - IloSum(propSatisfied) + M * (1 - propVars[prop[("PA-" + (*pi)).c_str()]][i]) > 0;
        model.add(propHasAchiever);
        propHasAchiever.setName(("C6-P_" + (*pi) + "-" + std::to_string(i)).c_str());
      }
    }

    // Constraint set 7: Step ordering constraints
    for (std::map<size_t, size_t>::const_iterator oi = plan_->orderings().begin();
         oi != plan_->orderings().end(); oi++) {
      IloConstraint c7 = stepTimeVars[stepTime[("ST-" + plan_->steps().at((*oi).first)->name()).c_str()]] 
                       - stepTimeVars[stepTime[("ST-" + plan_->steps().at((*oi).second)->name()).c_str()]] < 0;
      model.add(c7);
      c7.setName(("C7-S_" + plan_->steps().at((*oi).first)->name() + "-S_" + plan_->steps().at((*oi).second)->name()).c_str());
    }

    // Constraint set 8: Step interference
    for (std::map<size_t, IlpAction*>::const_iterator ai =
             plan_->steps().begin();
         ai != plan_->steps().end(); ai++) {
      for (std::map<size_t, IlpAction*>::const_iterator bi =
               plan_->steps().begin();
           bi != plan_->steps().end(); bi++) {
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
              IloConstraint c9 = (stepTimeVars[stepTime[("ST-" + (*ai).second->name()).c_str()]] - stepTimeVars[stepTime[("ST-" + (*bi).second->name()).c_str()]] != 0);
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

    IloCplex cplex(model);
    if (verbosity < 2) cplex.setOut(env.getNullStream());

    // Set the lower limit of objective function parameter CPX_PARAM_OBJLLIM to lb
    cplex.setParam(IloCplex::Param::MIP::Limits::LowerObjStop, lb);
    // Set time limit
    cplex.setParam(IloCplex::Param::TimeLimit, seconds);

    // Optimize the problem and obtain solution.
    cplex.solve();

    if (cplex.getStatus() == IloAlgorithm::Infeasible) {
        env.end();
        return -1;
    }

    IloNumArray vals(env);
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
             plan_->steps().begin();
         ai != plan_->steps().end(); ai++) {
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

    env.end();
    // Return the total number of actions used (including steps)
    return objectiveValue;
  }
  catch (IloException& e) {
    std::cerr << "CPLEX Concert exception caught: " << e << std::endl;
  }
  catch (...) {
    std::cerr << "Unknown ILP exception caught" << std::endl;
  }
  env.end();
  return 0;
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