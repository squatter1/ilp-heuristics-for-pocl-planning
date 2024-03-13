; Bulldozer domain used in (Nguyen & Kambhampati 2001), simple grounded version by Scott Howsam with parallel elements

(define (domain parallel-grounded-bulldozer)
  (:predicates (at-person-a)
           (at-person-b)
           (at-bulldozer-a)
           (at-bulldozer-b)
	       (mobile-person)
           (mobile-bulldozer)
	       (driving)
           (has-apple)
           (has-banana))
  (:action get-apple
	   :precondition (and (at-person-b))
	   :effect (and (has-apple)))
  (:action get-banana
	   :precondition (and (at-person-b))
	   :effect (and (has-banana)))
  (:action walk-to-a
	   :precondition (and (at-person-b)
			      (mobile-person))
	   :effect (and (at-person-a) (not (at-person-b))))
  (:action walk-to-b
	   :precondition (and (at-person-a)
			      (mobile-person))
	   :effect (and (at-person-b) (not (at-person-a))))
  (:action drive-to-a
	   :precondition (and (at-bulldozer-b)
			      (mobile-bulldozer))
	   :effect (and (at-bulldozer-a) (not (at-bulldozer-b))))
  (:action drive-to-b
	   :precondition (and (at-bulldozer-a)
			      (mobile-bulldozer))
	   :effect (and (at-bulldozer-b) (not (at-bulldozer-a))))
  (:action board-at-a
	   :precondition (and (at-person-a)
			      (at-bulldozer-a))
	   :effect (and (driving)
			(mobile-bulldozer)
			(not (at-person-a))
			(not (mobile-person))))
  (:action board-at-b
	   :precondition (and (at-person-b)
			      (at-bulldozer-b))
	   :effect (and (driving)
			(mobile-bulldozer)
			(not (at-person-b))
			(not (mobile-person))))  
  (:action disembark-at-a
	   :precondition (and (driving)
			      (at-bulldozer-a))
	   :effect (and (at-person-a)
			(mobile-person)
			(not (driving))
			(not (mobile-bulldozer))))
  (:action disembark-at-b
	   :precondition (and (driving)
			      (at-bulldozer-b))
	   :effect (and (at-person-b)
			(mobile-person)
			(not (driving))
			(not (mobile-bulldozer)))))
