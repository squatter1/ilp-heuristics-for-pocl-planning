(define (problem simple-grounded-bulldozer)
  (:domain grounded-bulldozer)
  (:init (at-person-a) (at-bulldozer-b) (mobile-person))
  (:goal (and (at-bulldozer-a) (at-person-b))))