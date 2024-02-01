#include "ilp.h"

#include <iostream>
#include <set>
#include <string>

#include <ilcplex/ilocplex.h>

/* TODO: try to get rid of this warning workaround */
[[maybe_unused]] void example_ILP () {
  IloEnv env;
  try {
    IloModel model(env);  
    IloNumVarArray x(env);
    IloRangeArray c(env);
  
    IloEnv env = model.getEnv();  
  
    x.add(IloNumVar(env, 0, 1, IloNumVar::Bool, "x1"));
    x.add(IloNumVar(env, 0, 1, IloNumVar::Bool, "x2"));
    x.add(IloNumVar(env, 0, 1, IloNumVar::Bool, "x3"));  
  
    model.add(IloMinimize(env, x[0] + 2 * x[1] + 10 * x[2]));  
  
    c.add(x[0] + x[1] + x[2] >= 1);
    c.add(x[0] + 3*x[1] + x[2] >= 3);  
  
    model.add(c);

    IloCplex cplex(model);

    // Optimize the problem and obtain solution.
    if ( !cplex.solve() ) {
       env.error() << "Failed to optimize LP" << std::endl;
       throw(-1);
    }

    IloNumArray vals(env);
    env.out() << "Solution status = " << cplex.getStatus() << std::endl;
    env.out() << "Solution value  = " << cplex.getObjValue() << std::endl;
    cplex.getValues(vals, x);
    env.out() << "Values        = " << vals << std::endl;
  }
  catch (IloException& e) {
    std::cerr << "CPLEX Concert exception caught: " << e << std::endl;
  }
  catch (...) {
    std::cerr << "Unknown ILP exception caught" << std::endl;
  }
  env.end();
}

size_t IlpAction::next_id = 0;

IlpAction::IlpAction(const std::string& name)
    : id_(next_id++),
      name_(name) { }

IlpAction::~IlpAction() { }

void IlpAction::add_condition(const std::string condition) {
  conditions_.insert(condition);
}

void IlpAction::add_pos_effect(const std::string effect) {
  effects_.insert(effect);
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
  // Print the effect atoms
  os << prefix << "Add Effects:";
  for (std::set<std::string>::const_iterator ai = effects_.begin();
       ai != effects_.end(); ai++) {
    os << ' ' << *ai;
  }
  os << std::endl;
}

IlpProblem::IlpProblem(const std::string& name)
    : name_(name) { }

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
