(define (domain gripper-strips)
(:predicates
    (F0) ;; at ball1 rooma
    (F1) ;; at ball1 roomb
    (F2) ;; at ball2 rooma
    (F3) ;; at ball2 roomb
    (F4) ;; at-robby rooma
    (F5) ;; at-robby roomb
    (F6) ;; carry ball1 left
    (F7) ;; carry ball1 right
    (F8) ;; carry ball2 left
    (F9) ;; carry ball2 right
    (F10) ;; free left
    (F11) ;; free right
)
(:action drop_ball1_rooma_left
    :precondition (and (F4) (F6))
    :effect (and (F0) (F10) (not (F6)))
)
(:action drop_ball1_rooma_right
    :precondition (and (F4) (F7))
    :effect (and (F0) (F11) (not (F7)))
)
(:action drop_ball1_roomb_left
    :precondition (and (F5) (F6))
    :effect (and (F1) (F10) (not (F6)))
)
(:action drop_ball1_roomb_right
    :precondition (and (F5) (F7))
    :effect (and (F1) (F11) (not (F7)))
)
(:action drop_ball2_rooma_left
    :precondition (and (F4) (F8))
    :effect (and (F2) (F10) (not (F8)))
)
(:action drop_ball2_rooma_right
    :precondition (and (F4) (F9))
    :effect (and (F2) (F11) (not (F9)))
)
(:action drop_ball2_roomb_left
    :precondition (and (F5) (F8))
    :effect (and (F3) (F10) (not (F8)))
)
(:action drop_ball2_roomb_right
    :precondition (and (F5) (F9))
    :effect (and (F3) (F11) (not (F9)))
)
(:action move_rooma_roomb
    :precondition (and (F4))
    :effect (and (F5) (not (F4)))
)
(:action move_roomb_rooma
    :precondition (and (F5))
    :effect (and (F4) (not (F5)))
)
(:action pick_ball1_rooma_left
    :precondition (and (F0) (F4) (F10))
    :effect (and (F6) (not (F0)) (not (F10)))
)
(:action pick_ball1_rooma_right
    :precondition (and (F0) (F4) (F11))
    :effect (and (F7) (not (F0)) (not (F11)))
)
(:action pick_ball1_roomb_left
    :precondition (and (F1) (F5) (F10))
    :effect (and (F6) (not (F1)) (not (F10)))
)
(:action pick_ball1_roomb_right
    :precondition (and (F1) (F5) (F11))
    :effect (and (F7) (not (F1)) (not (F11)))
)
(:action pick_ball2_rooma_left
    :precondition (and (F2) (F4) (F10))
    :effect (and (F8) (not (F2)) (not (F10)))
)
(:action pick_ball2_rooma_right
    :precondition (and (F2) (F4) (F11))
    :effect (and (F9) (not (F2)) (not (F11)))
)
(:action pick_ball2_roomb_left
    :precondition (and (F3) (F5) (F10))
    :effect (and (F8) (not (F3)) (not (F10)))
)
(:action pick_ball2_roomb_right
    :precondition (and (F3) (F5) (F11))
    :effect (and (F9) (not (F3)) (not (F11)))
)
)
