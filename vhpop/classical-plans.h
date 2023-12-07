/* SCOTT HOWSAM */
// Copyright (C) 2002--2005 Carnegie Mellon University
// Copyright (C) 2019 Google Inc
//
// This file is part of VHPOP.
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
// Partial plans, and their components.

#ifndef CLASSICAL_PLANS_H
#define CLASSICAL_PLANS_H

// TODO delete the inclusions and structs that aren't needed
#include "chain.h"
#include "flaws.h"
#include "orderings.h"

struct Parameters;
struct BindingList;
struct Literal;
struct Atom;
struct Negation;
struct Effect;
struct EffectList;
struct Action;
struct Problem;
struct Bindings;
struct ActionEffectMap;
struct FlawSelectionOrder;

/* ====================================================================== */
/* Step */

/*
 * Plan step.
 */
struct Step {
  /* Constructs a step instantiated from an action. */
  Step(size_t id, const Action& action)
    : id_(id), action_(&action) {}

  /* Constructs a step. */
  Step(const Step& s)
    : id_(s.id_), action_(s.action_) {}

  /* Returns the step id. */
  size_t id() const { return id_; }

  /* Returns the action that this step is instantiated from. */
  const Action& action() const { return *action_; }

private:
  /* Step id. */
  size_t id_;
  /* Action that this step is instantiated from. */
  const Action* action_;
};


/* ====================================================================== */
/* Classical Plan */

/*
 * Classical Plan.
 */
struct ClassicalPlan {
  /* Id of goal step. */
  static const size_t GOAL_ID;

  /* Returns classical plan for given plan. */
  static const ClassicalPlan* pop_to_classical(const Plan& plan);

  /* Deletes this plan. */
  ~ClassicalPlan();

  /* Returns the steps of this plan. */
  const Chain<Step>* steps() const { return steps_; }

  /* Returns the number of unique steps in this plan. */
  size_t num_steps() const { return num_steps_; }

  /* Returns the open conditions of this plan. */
  const Chain<OpenCondition>* open_conds() const { return open_conds_; }

  /* Checks if this plan is complete. */
  bool complete() const;

  /* Returns the primary rank of this plan, where a lower rank
     signifies a better plan. */
  float primary_rank() const;

  /* Returns the serial number of this plan. */
  size_t serial_no() const;

private:

  /* Chain of steps. */
  const Chain<Step>* steps_;
  /* Number of unique steps in plan. */
  size_t num_steps_;
  /* Rank of this plan. */
  mutable std::vector<float> rank_;
  /* Plan id (serial number). */
  mutable size_t id_;

  /* Constructs a classical plan. */
  ClassicalPlan(const Chain<Step>* steps, size_t num_steps,
       const ClassicalPlan* parent);

  friend bool operator<(const ClassicalPlan& p1, const ClassicalPlan& p2);
  friend std::ostream& operator<<(std::ostream& os, const ClassicalPlan& p);
};

/* Less than operator for plans. */
bool operator<(const ClassicalPlan& p1, const ClassicalPlan& p2);

/* Output operator for plans. */
std::ostream& operator<<(std::ostream& os, const ClassicalPlan& p);


#endif /* CLASSICAL_PLANS_H */
