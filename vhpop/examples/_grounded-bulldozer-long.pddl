(define (problem long-grounded-bulldozer)
  (:domain grounded-bulldozer-long)
  (:init (at-person-a) (at-bulldozer-d) (mobile-person))
  (:goal (and (at-bulldozer-a) (at-person-d))))