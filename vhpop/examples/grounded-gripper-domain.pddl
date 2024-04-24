(define (domain gripper-strips)
(:requirements
    :strips
    :typing
    :negative-preconditions
    :disjunctive-preconditions
    :equality
    :existential-preconditions
    :universal-preconditions
    :conditional-effects
)
(:types
)
(:constants
    rooma - object
    roomb - object
    ball1 - object
    ball2 - object
    left - object
    right - object
)
(:predicates
    (room ?x0 - object)
    (ball ?x0 - object)
    (gripper ?x0 - object)
    (at-robby ?x0 - object)
    (at ?x0 - object ?x1 - object)
    (free ?x0 - object)
    (carry ?x0 - object ?x1 - object)
)
(:action move
    :parameters (?from - object ?to - object)
    :precondition (and (room ?from) (room ?to) (at-robby ?from))
    :effect (and (at-robby ?to) (not (at-robby ?from)))
)

(:action pick
    :parameters (?obj - object ?room - object ?gripper - object)
    :precondition (and (ball ?obj) (room ?room) (gripper ?gripper) (at ?obj ?room) (at-robby ?room) (free ?gripper))
    :effect (and (carry ?obj ?gripper) (not (at ?obj ?room)) (not (free ?gripper)))
)

(:action drop
    :parameters (?obj - object ?room - object ?gripper - object)
    :precondition (and (ball ?obj) (room ?room) (gripper ?gripper) (carry ?obj ?gripper) (at-robby ?room))
    :effect (and (at ?obj ?room) (free ?gripper) (not (carry ?obj ?gripper)))
)

)
