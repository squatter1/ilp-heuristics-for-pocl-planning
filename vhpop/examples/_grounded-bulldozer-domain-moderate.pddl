; Bulldozer domain used in (Nguyen & Kambhampati 2001), simple grounded version by Scott Howsam

(define (domain grounded-bulldozer-moderate)
  (:predicates (at-person-a)
           (at-person-b)
           (at-person-c)
           (at-person-d)
           (at-bulldozer-a)
           (at-bulldozer-b)
           (at-bulldozer-c)
           (at-bulldozer-d)
	       (mobile-person)
           (mobile-bulldozer)
  )
  (:action walk-a-to-b
	   :precondition (and (at-person-a)
			      (mobile-person))
	   :effect (and (at-person-b) (not (at-person-a))))
  (:action walk-b-to-a
	   :precondition (and (at-person-b)
			      (mobile-person))
	   :effect (and (at-person-a) (not (at-person-b))))
  (:action walk-b-to-c
	   :precondition (and (at-person-b)
			      (mobile-person))
	   :effect (and (at-person-c) (not (at-person-b))))
  (:action walk-c-to-b
	   :precondition (and (at-person-c)
			      (mobile-person))
	   :effect (and (at-person-b) (not (at-person-c))))
  (:action walk-c-to-d
	   :precondition (and (at-person-c)
			      (mobile-person))
	   :effect (and (at-person-d) (not (at-person-c))))
  (:action walk-d-to-c
	   :precondition (and (at-person-d)
			      (mobile-person))
	   :effect (and (at-person-c) (not (at-person-d))))
  (:action walk-d-to-a
	   :precondition (and (at-person-d)
			      (mobile-person))
	   :effect (and (at-person-a) (not (at-person-d))))
  (:action walk-a-to-d
	   :precondition (and (at-person-a)
			      (mobile-person))
	   :effect (and (at-person-d) (not (at-person-a))))
  (:action drive-a-to-b
	   :precondition (and (at-bulldozer-a)
			      (mobile-bulldozer))
	   :effect (and (at-bulldozer-b) (not (at-bulldozer-a))))
  (:action drive-b-to-a
	   :precondition (and (at-bulldozer-b)
			      (mobile-bulldozer))
	   :effect (and (at-bulldozer-a) (not (at-bulldozer-b))))
  (:action drive-b-to-c
	   :precondition (and (at-bulldozer-b)
			      (mobile-bulldozer))
	   :effect (and (at-bulldozer-c) (not (at-bulldozer-b))))
  (:action drive-c-to-b
	   :precondition (and (at-bulldozer-c)
			      (mobile-bulldozer))
	   :effect (and (at-bulldozer-b) (not (at-bulldozer-c))))
  (:action drive-c-to-d
	   :precondition (and (at-bulldozer-c)
			      (mobile-bulldozer))
	   :effect (and (at-bulldozer-d) (not (at-bulldozer-c))))
  (:action drive-d-to-c
	   :precondition (and (at-bulldozer-d)
			      (mobile-bulldozer))
	   :effect (and (at-bulldozer-c) (not (at-bulldozer-d))))
  (:action drive-d-to-a
	   :precondition (and (at-bulldozer-d)
			      (mobile-bulldozer))
	   :effect (and (at-bulldozer-a) (not (at-bulldozer-d))))
  (:action drive-a-to-d
	   :precondition (and (at-bulldozer-a)
			      (mobile-bulldozer))
	   :effect (and (at-bulldozer-d) (not (at-bulldozer-a))))
  (:action board-at-a
	   :precondition (and (at-person-a)
			      (at-bulldozer-a))
	   :effect (and (mobile-bulldozer)
			(not (at-person-a))
			(not (mobile-person))))
  (:action board-at-b
	   :precondition (and (at-person-b)
			      (at-bulldozer-b))
	   :effect (and (mobile-bulldozer)
			(not (at-person-b))
			(not (mobile-person))))  
  (:action board-at-c
	   :precondition (and (at-person-c)
			      (at-bulldozer-c))
	   :effect (and (mobile-bulldozer)
			(not (at-person-c))
			(not (mobile-person))))  
  (:action board-at-d
	   :precondition (and (at-person-d)
			      (at-bulldozer-d))
	   :effect (and (mobile-bulldozer)
			(not (at-person-d))
			(not (mobile-person))))  
  (:action disembark-at-a
	   :precondition (and (mobile-bulldozer)
			      (at-bulldozer-a))
	   :effect (and (at-person-a)
			(mobile-person)
			(not (mobile-bulldozer))))
  (:action disembark-at-b
	   :precondition (and (mobile-bulldozer)
			      (at-bulldozer-b))
	   :effect (and (at-person-b)
			(mobile-person)
			(not (mobile-bulldozer))))
  (:action disembark-at-c
	   :precondition (and (mobile-bulldozer)
			      (at-bulldozer-c))
	   :effect (and (at-person-c)
			(mobile-person)
			(not (mobile-bulldozer))))
  (:action disembark-at-d
	   :precondition (and (mobile-bulldozer)
			      (at-bulldozer-d))
	   :effect (and (at-person-d)
			(mobile-person)
			(not (mobile-bulldozer))))
)
