(define (problem simple-parallel-grounded-bulldozer)
  (:domain parallel-grounded-bulldozer)
  (:init (at-person-a) (at-bulldozer-b) (mobile-person))
  (:goal (and (at-bulldozer-a) (at-person-b) (has-apple) (has-banana))))