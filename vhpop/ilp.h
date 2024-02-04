/* ILP creation and solving functions for ILP based heuristics */

#ifndef ILP_H
#define ILP_H

#include "actions.h"

#include <iostream>
#include <map>
#include <set>
#include <string>

void example_ILP();

/*
 * ILP Action definition.
 */
struct IlpAction {
  // Constructs an ILP action with the given name.
  IlpAction(const std::string& name);

  // Constucts and ILP action from an action.
  IlpAction(const Action& action);

  // Deletes this ILP action.
  ~IlpAction();

  // Returns the name of this ILP action.
  const std::string& name() const { return name_; }

  // Adds a condition for this ILP action.
  void add_condition(const std::string condition);

  // Adds a positive effect to this ILP action.
  void add_pos_effect(const std::string effect);

  // Adds a negative effect to this ILP action.
  void add_neg_effect(const std::string effect);

  // List of ILP action conditions.
  const std::set<std::string>& conditions() const { return conditions_; }

  // List of ILP positive action effects.
  const std::set<std::string>& pos_effects() const { return posEffects_; }

  // List of ILP negative action effects.
  const std::set<std::string>& neg_effects() const { return negEffects_; }

  // Returns the id for this ILP action.
  size_t id() const { return id_; }

  // Prints this ILP action on the given stream.
  void print(std::ostream& os, std::string prefix = "") const;

 private:
  // Next ILP action id.
  static size_t next_id;

  // Unique id for ILP actions.
  size_t id_;
  // Name of this ILP action.
  std::string name_;
  // ILP action conditions.
  std::set<std::string> conditions_;
  // List of ILP positive action effects.
  std::set<std::string> posEffects_; 
  // List of ILP negative action effects.
  std::set<std::string> negEffects_; 
};

/*
 * ILP Problem definition. 
 */
struct IlpProblem {
  /* Constructs an empty ILP problem with the given name. */
  IlpProblem(const std::string& name);

  /* Constructs an ILP problem from a problem. */
  IlpProblem(const Problem& problem);

  /* Deletes an ILP problem. */
  ~IlpProblem();

  /* Returns the name of this ILP problem. */
  const std::string& name() const { return name_; }

  /* ILP problem actions. */
  const std::map<std::string, const IlpAction*>& actions() const {
    return actions_;
  }

  /* Adds an ILP action to this problem. */
  void add_action(const IlpAction* action);

  /* Returns the ILP action with the given name, or NULL if it is
     undefined. */
  const IlpAction* find_action(const std::string& name) const;

  /* Adds a proposition to the proposition set of this problem. */
  void add_prop(std::string prop);

  /* Returns the proposition set of this problem. */
  const std::set<std::string>& props() const { return props_; }

  /* Adds a proposition to the initial state of this problem. */
  void add_init_prop(std::string prop);

  /* Returns the initial propositions of this problem. */
  const std::set<std::string>& init() const { return init_; }

  /* Adds a proposition to the goal state of this problem. */
  void add_goal_prop(std::string prop);

  /* Returns the goal of this problem. */
  const std::set<std::string>& goal() const { return goal_; }

  /* Finds the shortest solution length of this delete-relaxed problem with an ILP */
  const size_t solve(std::ostream& os, short int verbosity = 0) const;

private:
  /* Name of this ILP problem. */
  std::string name_;
  /* Problem actions. */
  std::map<std::string, const IlpAction*> actions_;
  /* Propositions. */
  std::set<std::string> props_;
  /* Initial propositions. */
  std::set<std::string> init_;
  /* Goal propositions. */
  std::set<std::string> goal_;

  friend std::ostream& operator<<(std::ostream& os, const IlpProblem& d);
};

/* Output operator for ILP problems. */
std::ostream& operator<<(std::ostream& os, const IlpProblem& d);

#endif  // ILP_H