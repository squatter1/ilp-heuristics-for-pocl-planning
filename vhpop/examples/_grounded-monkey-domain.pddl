(define (domain monkey-domain)
(:predicates
    (F0) ;; at box bananas
    (F1) ;; at box box
    (F2) ;; at box glass
    (F3) ;; at box knife
    (F4) ;; at box monkey
    (F5) ;; at box p1
    (F6) ;; at box p2
    (F7) ;; at box p3
    (F8) ;; at box p4
    (F9) ;; at box water
    (F10) ;; at box waterfountain
    (F11) ;; at knife p4
    (F12) ;; at monkey bananas
    (F13) ;; at monkey box
    (F14) ;; at monkey glass
    (F15) ;; at monkey knife
    (F16) ;; at monkey monkey
    (F17) ;; at monkey p1
    (F18) ;; at monkey p2
    (F19) ;; at monkey p3
    (F20) ;; at monkey p4
    (F21) ;; at monkey water
    (F22) ;; at monkey waterfountain
    (F23) ;; hasbananas
    (F24) ;; hasknife
    (F25) ;; on-floor
    (F26) ;; onbox bananas
    (F27) ;; onbox box
    (F28) ;; onbox glass
    (F29) ;; onbox knife
    (F30) ;; onbox monkey
    (F31) ;; onbox p1
    (F32) ;; onbox p2
    (F33) ;; onbox p3
    (F34) ;; onbox p4
    (F35) ;; onbox water
    (F36) ;; onbox waterfountain
)
(:action climb_bananas
    :precondition (and (F0) (F12))
    :effect (and (F26) (not (F25)))
)
(:action climb_box
    :precondition (and (F1) (F13))
    :effect (and (F27) (not (F25)))
)
(:action climb_glass
    :precondition (and (F2) (F14))
    :effect (and (F28) (not (F25)))
)
(:action climb_knife
    :precondition (and (F3) (F15))
    :effect (and (F29) (not (F25)))
)
(:action climb_monkey
    :precondition (and (F4) (F16))
    :effect (and (F30) (not (F25)))
)
(:action climb_p1
    :precondition (and (F5) (F17))
    :effect (and (F31) (not (F25)))
)
(:action climb_p2
    :precondition (and (F6) (F18))
    :effect (and (F32) (not (F25)))
)
(:action climb_p3
    :precondition (and (F7) (F19))
    :effect (and (F33) (not (F25)))
)
(:action climb_p4
    :precondition (and (F8) (F20))
    :effect (and (F34) (not (F25)))
)
(:action climb_water
    :precondition (and (F9) (F21))
    :effect (and (F35) (not (F25)))
)
(:action climb_waterfountain
    :precondition (and (F10) (F22))
    :effect (and (F36) (not (F25)))
)
(:action get-knife_p4
    :precondition (and (F11) (F20))
    :effect (and (F24) (not (F11)))
)
(:action go-to_bananas_box
    :precondition (and (F13) (F25))
    :effect (and (F12) (not (F13)))
)
(:action go-to_bananas_glass
    :precondition (and (F14) (F25))
    :effect (and (F12) (not (F14)))
)
(:action go-to_bananas_knife
    :precondition (and (F15) (F25))
    :effect (and (F12) (not (F15)))
)
(:action go-to_bananas_monkey
    :precondition (and (F16) (F25))
    :effect (and (F12) (not (F16)))
)
(:action go-to_bananas_p1
    :precondition (and (F17) (F25))
    :effect (and (F12) (not (F17)))
)
(:action go-to_bananas_p2
    :precondition (and (F18) (F25))
    :effect (and (F12) (not (F18)))
)
(:action go-to_bananas_p3
    :precondition (and (F19) (F25))
    :effect (and (F12) (not (F19)))
)
(:action go-to_bananas_p4
    :precondition (and (F20) (F25))
    :effect (and (F12) (not (F20)))
)
(:action go-to_bananas_water
    :precondition (and (F21) (F25))
    :effect (and (F12) (not (F21)))
)
(:action go-to_bananas_waterfountain
    :precondition (and (F22) (F25))
    :effect (and (F12) (not (F22)))
)
(:action go-to_box_bananas
    :precondition (and (F12) (F25))
    :effect (and (F13) (not (F12)))
)
(:action go-to_box_glass
    :precondition (and (F14) (F25))
    :effect (and (F13) (not (F14)))
)
(:action go-to_box_knife
    :precondition (and (F15) (F25))
    :effect (and (F13) (not (F15)))
)
(:action go-to_box_monkey
    :precondition (and (F16) (F25))
    :effect (and (F13) (not (F16)))
)
(:action go-to_box_p1
    :precondition (and (F17) (F25))
    :effect (and (F13) (not (F17)))
)
(:action go-to_box_p2
    :precondition (and (F18) (F25))
    :effect (and (F13) (not (F18)))
)
(:action go-to_box_p3
    :precondition (and (F19) (F25))
    :effect (and (F13) (not (F19)))
)
(:action go-to_box_p4
    :precondition (and (F20) (F25))
    :effect (and (F13) (not (F20)))
)
(:action go-to_box_water
    :precondition (and (F21) (F25))
    :effect (and (F13) (not (F21)))
)
(:action go-to_box_waterfountain
    :precondition (and (F22) (F25))
    :effect (and (F13) (not (F22)))
)
(:action go-to_glass_bananas
    :precondition (and (F12) (F25))
    :effect (and (F14) (not (F12)))
)
(:action go-to_glass_box
    :precondition (and (F13) (F25))
    :effect (and (F14) (not (F13)))
)
(:action go-to_glass_knife
    :precondition (and (F15) (F25))
    :effect (and (F14) (not (F15)))
)
(:action go-to_glass_monkey
    :precondition (and (F16) (F25))
    :effect (and (F14) (not (F16)))
)
(:action go-to_glass_p1
    :precondition (and (F17) (F25))
    :effect (and (F14) (not (F17)))
)
(:action go-to_glass_p2
    :precondition (and (F18) (F25))
    :effect (and (F14) (not (F18)))
)
(:action go-to_glass_p3
    :precondition (and (F19) (F25))
    :effect (and (F14) (not (F19)))
)
(:action go-to_glass_p4
    :precondition (and (F20) (F25))
    :effect (and (F14) (not (F20)))
)
(:action go-to_glass_water
    :precondition (and (F21) (F25))
    :effect (and (F14) (not (F21)))
)
(:action go-to_glass_waterfountain
    :precondition (and (F22) (F25))
    :effect (and (F14) (not (F22)))
)
(:action go-to_knife_bananas
    :precondition (and (F12) (F25))
    :effect (and (F15) (not (F12)))
)
(:action go-to_knife_box
    :precondition (and (F13) (F25))
    :effect (and (F15) (not (F13)))
)
(:action go-to_knife_glass
    :precondition (and (F14) (F25))
    :effect (and (F15) (not (F14)))
)
(:action go-to_knife_monkey
    :precondition (and (F16) (F25))
    :effect (and (F15) (not (F16)))
)
(:action go-to_knife_p1
    :precondition (and (F17) (F25))
    :effect (and (F15) (not (F17)))
)
(:action go-to_knife_p2
    :precondition (and (F18) (F25))
    :effect (and (F15) (not (F18)))
)
(:action go-to_knife_p3
    :precondition (and (F19) (F25))
    :effect (and (F15) (not (F19)))
)
(:action go-to_knife_p4
    :precondition (and (F20) (F25))
    :effect (and (F15) (not (F20)))
)
(:action go-to_knife_water
    :precondition (and (F21) (F25))
    :effect (and (F15) (not (F21)))
)
(:action go-to_knife_waterfountain
    :precondition (and (F22) (F25))
    :effect (and (F15) (not (F22)))
)
(:action go-to_monkey_bananas
    :precondition (and (F12) (F25))
    :effect (and (F16) (not (F12)))
)
(:action go-to_monkey_box
    :precondition (and (F13) (F25))
    :effect (and (F16) (not (F13)))
)
(:action go-to_monkey_glass
    :precondition (and (F14) (F25))
    :effect (and (F16) (not (F14)))
)
(:action go-to_monkey_knife
    :precondition (and (F15) (F25))
    :effect (and (F16) (not (F15)))
)
(:action go-to_monkey_p1
    :precondition (and (F17) (F25))
    :effect (and (F16) (not (F17)))
)
(:action go-to_monkey_p2
    :precondition (and (F18) (F25))
    :effect (and (F16) (not (F18)))
)
(:action go-to_monkey_p3
    :precondition (and (F19) (F25))
    :effect (and (F16) (not (F19)))
)
(:action go-to_monkey_p4
    :precondition (and (F20) (F25))
    :effect (and (F16) (not (F20)))
)
(:action go-to_monkey_water
    :precondition (and (F21) (F25))
    :effect (and (F16) (not (F21)))
)
(:action go-to_monkey_waterfountain
    :precondition (and (F22) (F25))
    :effect (and (F16) (not (F22)))
)
(:action go-to_p1_bananas
    :precondition (and (F12) (F25))
    :effect (and (F17) (not (F12)))
)
(:action go-to_p1_box
    :precondition (and (F13) (F25))
    :effect (and (F17) (not (F13)))
)
(:action go-to_p1_glass
    :precondition (and (F14) (F25))
    :effect (and (F17) (not (F14)))
)
(:action go-to_p1_knife
    :precondition (and (F15) (F25))
    :effect (and (F17) (not (F15)))
)
(:action go-to_p1_monkey
    :precondition (and (F16) (F25))
    :effect (and (F17) (not (F16)))
)
(:action go-to_p1_p2
    :precondition (and (F18) (F25))
    :effect (and (F17) (not (F18)))
)
(:action go-to_p1_p3
    :precondition (and (F19) (F25))
    :effect (and (F17) (not (F19)))
)
(:action go-to_p1_p4
    :precondition (and (F20) (F25))
    :effect (and (F17) (not (F20)))
)
(:action go-to_p1_water
    :precondition (and (F21) (F25))
    :effect (and (F17) (not (F21)))
)
(:action go-to_p1_waterfountain
    :precondition (and (F22) (F25))
    :effect (and (F17) (not (F22)))
)
(:action go-to_p2_bananas
    :precondition (and (F12) (F25))
    :effect (and (F18) (not (F12)))
)
(:action go-to_p2_box
    :precondition (and (F13) (F25))
    :effect (and (F18) (not (F13)))
)
(:action go-to_p2_glass
    :precondition (and (F14) (F25))
    :effect (and (F18) (not (F14)))
)
(:action go-to_p2_knife
    :precondition (and (F15) (F25))
    :effect (and (F18) (not (F15)))
)
(:action go-to_p2_monkey
    :precondition (and (F16) (F25))
    :effect (and (F18) (not (F16)))
)
(:action go-to_p2_p1
    :precondition (and (F17) (F25))
    :effect (and (F18) (not (F17)))
)
(:action go-to_p2_p3
    :precondition (and (F19) (F25))
    :effect (and (F18) (not (F19)))
)
(:action go-to_p2_p4
    :precondition (and (F20) (F25))
    :effect (and (F18) (not (F20)))
)
(:action go-to_p2_water
    :precondition (and (F21) (F25))
    :effect (and (F18) (not (F21)))
)
(:action go-to_p2_waterfountain
    :precondition (and (F22) (F25))
    :effect (and (F18) (not (F22)))
)
(:action go-to_p3_bananas
    :precondition (and (F12) (F25))
    :effect (and (F19) (not (F12)))
)
(:action go-to_p3_box
    :precondition (and (F13) (F25))
    :effect (and (F19) (not (F13)))
)
(:action go-to_p3_glass
    :precondition (and (F14) (F25))
    :effect (and (F19) (not (F14)))
)
(:action go-to_p3_knife
    :precondition (and (F15) (F25))
    :effect (and (F19) (not (F15)))
)
(:action go-to_p3_monkey
    :precondition (and (F16) (F25))
    :effect (and (F19) (not (F16)))
)
(:action go-to_p3_p1
    :precondition (and (F17) (F25))
    :effect (and (F19) (not (F17)))
)
(:action go-to_p3_p2
    :precondition (and (F18) (F25))
    :effect (and (F19) (not (F18)))
)
(:action go-to_p3_p4
    :precondition (and (F20) (F25))
    :effect (and (F19) (not (F20)))
)
(:action go-to_p3_water
    :precondition (and (F21) (F25))
    :effect (and (F19) (not (F21)))
)
(:action go-to_p3_waterfountain
    :precondition (and (F22) (F25))
    :effect (and (F19) (not (F22)))
)
(:action go-to_p4_bananas
    :precondition (and (F12) (F25))
    :effect (and (F20) (not (F12)))
)
(:action go-to_p4_box
    :precondition (and (F13) (F25))
    :effect (and (F20) (not (F13)))
)
(:action go-to_p4_glass
    :precondition (and (F14) (F25))
    :effect (and (F20) (not (F14)))
)
(:action go-to_p4_knife
    :precondition (and (F15) (F25))
    :effect (and (F20) (not (F15)))
)
(:action go-to_p4_monkey
    :precondition (and (F16) (F25))
    :effect (and (F20) (not (F16)))
)
(:action go-to_p4_p1
    :precondition (and (F17) (F25))
    :effect (and (F20) (not (F17)))
)
(:action go-to_p4_p2
    :precondition (and (F18) (F25))
    :effect (and (F20) (not (F18)))
)
(:action go-to_p4_p3
    :precondition (and (F19) (F25))
    :effect (and (F20) (not (F19)))
)
(:action go-to_p4_water
    :precondition (and (F21) (F25))
    :effect (and (F20) (not (F21)))
)
(:action go-to_p4_waterfountain
    :precondition (and (F22) (F25))
    :effect (and (F20) (not (F22)))
)
(:action go-to_water_bananas
    :precondition (and (F12) (F25))
    :effect (and (F21) (not (F12)))
)
(:action go-to_water_box
    :precondition (and (F13) (F25))
    :effect (and (F21) (not (F13)))
)
(:action go-to_water_glass
    :precondition (and (F14) (F25))
    :effect (and (F21) (not (F14)))
)
(:action go-to_water_knife
    :precondition (and (F15) (F25))
    :effect (and (F21) (not (F15)))
)
(:action go-to_water_monkey
    :precondition (and (F16) (F25))
    :effect (and (F21) (not (F16)))
)
(:action go-to_water_p1
    :precondition (and (F17) (F25))
    :effect (and (F21) (not (F17)))
)
(:action go-to_water_p2
    :precondition (and (F18) (F25))
    :effect (and (F21) (not (F18)))
)
(:action go-to_water_p3
    :precondition (and (F19) (F25))
    :effect (and (F21) (not (F19)))
)
(:action go-to_water_p4
    :precondition (and (F20) (F25))
    :effect (and (F21) (not (F20)))
)
(:action go-to_water_waterfountain
    :precondition (and (F22) (F25))
    :effect (and (F21) (not (F22)))
)
(:action go-to_waterfountain_bananas
    :precondition (and (F12) (F25))
    :effect (and (F22) (not (F12)))
)
(:action go-to_waterfountain_box
    :precondition (and (F13) (F25))
    :effect (and (F22) (not (F13)))
)
(:action go-to_waterfountain_glass
    :precondition (and (F14) (F25))
    :effect (and (F22) (not (F14)))
)
(:action go-to_waterfountain_knife
    :precondition (and (F15) (F25))
    :effect (and (F22) (not (F15)))
)
(:action go-to_waterfountain_monkey
    :precondition (and (F16) (F25))
    :effect (and (F22) (not (F16)))
)
(:action go-to_waterfountain_p1
    :precondition (and (F17) (F25))
    :effect (and (F22) (not (F17)))
)
(:action go-to_waterfountain_p2
    :precondition (and (F18) (F25))
    :effect (and (F22) (not (F18)))
)
(:action go-to_waterfountain_p3
    :precondition (and (F19) (F25))
    :effect (and (F22) (not (F19)))
)
(:action go-to_waterfountain_p4
    :precondition (and (F20) (F25))
    :effect (and (F22) (not (F20)))
)
(:action go-to_waterfountain_water
    :precondition (and (F21) (F25))
    :effect (and (F22) (not (F21)))
)
(:action grab-bananas_p3
    :precondition (and (F24) (F33))
    :effect (and (F23))
)
(:action push-box_bananas_box
    :precondition (and (F1) (F13) (F25))
    :effect (and (F0) (F12) (not (F1)) (not (F13)))
)
(:action push-box_bananas_glass
    :precondition (and (F2) (F14) (F25))
    :effect (and (F0) (F12) (not (F2)) (not (F14)))
)
(:action push-box_bananas_knife
    :precondition (and (F3) (F15) (F25))
    :effect (and (F0) (F12) (not (F3)) (not (F15)))
)
(:action push-box_bananas_monkey
    :precondition (and (F4) (F16) (F25))
    :effect (and (F0) (F12) (not (F4)) (not (F16)))
)
(:action push-box_bananas_p1
    :precondition (and (F5) (F17) (F25))
    :effect (and (F0) (F12) (not (F5)) (not (F17)))
)
(:action push-box_bananas_p2
    :precondition (and (F6) (F18) (F25))
    :effect (and (F0) (F12) (not (F6)) (not (F18)))
)
(:action push-box_bananas_p3
    :precondition (and (F7) (F19) (F25))
    :effect (and (F0) (F12) (not (F7)) (not (F19)))
)
(:action push-box_bananas_p4
    :precondition (and (F8) (F20) (F25))
    :effect (and (F0) (F12) (not (F8)) (not (F20)))
)
(:action push-box_bananas_water
    :precondition (and (F9) (F21) (F25))
    :effect (and (F0) (F12) (not (F9)) (not (F21)))
)
(:action push-box_bananas_waterfountain
    :precondition (and (F10) (F22) (F25))
    :effect (and (F0) (F12) (not (F10)) (not (F22)))
)
(:action push-box_box_bananas
    :precondition (and (F0) (F12) (F25))
    :effect (and (F1) (F13) (not (F0)) (not (F12)))
)
(:action push-box_box_glass
    :precondition (and (F2) (F14) (F25))
    :effect (and (F1) (F13) (not (F2)) (not (F14)))
)
(:action push-box_box_knife
    :precondition (and (F3) (F15) (F25))
    :effect (and (F1) (F13) (not (F3)) (not (F15)))
)
(:action push-box_box_monkey
    :precondition (and (F4) (F16) (F25))
    :effect (and (F1) (F13) (not (F4)) (not (F16)))
)
(:action push-box_box_p1
    :precondition (and (F5) (F17) (F25))
    :effect (and (F1) (F13) (not (F5)) (not (F17)))
)
(:action push-box_box_p2
    :precondition (and (F6) (F18) (F25))
    :effect (and (F1) (F13) (not (F6)) (not (F18)))
)
(:action push-box_box_p3
    :precondition (and (F7) (F19) (F25))
    :effect (and (F1) (F13) (not (F7)) (not (F19)))
)
(:action push-box_box_p4
    :precondition (and (F8) (F20) (F25))
    :effect (and (F1) (F13) (not (F8)) (not (F20)))
)
(:action push-box_box_water
    :precondition (and (F9) (F21) (F25))
    :effect (and (F1) (F13) (not (F9)) (not (F21)))
)
(:action push-box_box_waterfountain
    :precondition (and (F10) (F22) (F25))
    :effect (and (F1) (F13) (not (F10)) (not (F22)))
)
(:action push-box_glass_bananas
    :precondition (and (F0) (F12) (F25))
    :effect (and (F2) (F14) (not (F0)) (not (F12)))
)
(:action push-box_glass_box
    :precondition (and (F1) (F13) (F25))
    :effect (and (F2) (F14) (not (F1)) (not (F13)))
)
(:action push-box_glass_knife
    :precondition (and (F3) (F15) (F25))
    :effect (and (F2) (F14) (not (F3)) (not (F15)))
)
(:action push-box_glass_monkey
    :precondition (and (F4) (F16) (F25))
    :effect (and (F2) (F14) (not (F4)) (not (F16)))
)
(:action push-box_glass_p1
    :precondition (and (F5) (F17) (F25))
    :effect (and (F2) (F14) (not (F5)) (not (F17)))
)
(:action push-box_glass_p2
    :precondition (and (F6) (F18) (F25))
    :effect (and (F2) (F14) (not (F6)) (not (F18)))
)
(:action push-box_glass_p3
    :precondition (and (F7) (F19) (F25))
    :effect (and (F2) (F14) (not (F7)) (not (F19)))
)
(:action push-box_glass_p4
    :precondition (and (F8) (F20) (F25))
    :effect (and (F2) (F14) (not (F8)) (not (F20)))
)
(:action push-box_glass_water
    :precondition (and (F9) (F21) (F25))
    :effect (and (F2) (F14) (not (F9)) (not (F21)))
)
(:action push-box_glass_waterfountain
    :precondition (and (F10) (F22) (F25))
    :effect (and (F2) (F14) (not (F10)) (not (F22)))
)
(:action push-box_knife_bananas
    :precondition (and (F0) (F12) (F25))
    :effect (and (F3) (F15) (not (F0)) (not (F12)))
)
(:action push-box_knife_box
    :precondition (and (F1) (F13) (F25))
    :effect (and (F3) (F15) (not (F1)) (not (F13)))
)
(:action push-box_knife_glass
    :precondition (and (F2) (F14) (F25))
    :effect (and (F3) (F15) (not (F2)) (not (F14)))
)
(:action push-box_knife_monkey
    :precondition (and (F4) (F16) (F25))
    :effect (and (F3) (F15) (not (F4)) (not (F16)))
)
(:action push-box_knife_p1
    :precondition (and (F5) (F17) (F25))
    :effect (and (F3) (F15) (not (F5)) (not (F17)))
)
(:action push-box_knife_p2
    :precondition (and (F6) (F18) (F25))
    :effect (and (F3) (F15) (not (F6)) (not (F18)))
)
(:action push-box_knife_p3
    :precondition (and (F7) (F19) (F25))
    :effect (and (F3) (F15) (not (F7)) (not (F19)))
)
(:action push-box_knife_p4
    :precondition (and (F8) (F20) (F25))
    :effect (and (F3) (F15) (not (F8)) (not (F20)))
)
(:action push-box_knife_water
    :precondition (and (F9) (F21) (F25))
    :effect (and (F3) (F15) (not (F9)) (not (F21)))
)
(:action push-box_knife_waterfountain
    :precondition (and (F10) (F22) (F25))
    :effect (and (F3) (F15) (not (F10)) (not (F22)))
)
(:action push-box_monkey_bananas
    :precondition (and (F0) (F12) (F25))
    :effect (and (F4) (F16) (not (F0)) (not (F12)))
)
(:action push-box_monkey_box
    :precondition (and (F1) (F13) (F25))
    :effect (and (F4) (F16) (not (F1)) (not (F13)))
)
(:action push-box_monkey_glass
    :precondition (and (F2) (F14) (F25))
    :effect (and (F4) (F16) (not (F2)) (not (F14)))
)
(:action push-box_monkey_knife
    :precondition (and (F3) (F15) (F25))
    :effect (and (F4) (F16) (not (F3)) (not (F15)))
)
(:action push-box_monkey_p1
    :precondition (and (F5) (F17) (F25))
    :effect (and (F4) (F16) (not (F5)) (not (F17)))
)
(:action push-box_monkey_p2
    :precondition (and (F6) (F18) (F25))
    :effect (and (F4) (F16) (not (F6)) (not (F18)))
)
(:action push-box_monkey_p3
    :precondition (and (F7) (F19) (F25))
    :effect (and (F4) (F16) (not (F7)) (not (F19)))
)
(:action push-box_monkey_p4
    :precondition (and (F8) (F20) (F25))
    :effect (and (F4) (F16) (not (F8)) (not (F20)))
)
(:action push-box_monkey_water
    :precondition (and (F9) (F21) (F25))
    :effect (and (F4) (F16) (not (F9)) (not (F21)))
)
(:action push-box_monkey_waterfountain
    :precondition (and (F10) (F22) (F25))
    :effect (and (F4) (F16) (not (F10)) (not (F22)))
)
(:action push-box_p1_bananas
    :precondition (and (F0) (F12) (F25))
    :effect (and (F5) (F17) (not (F0)) (not (F12)))
)
(:action push-box_p1_box
    :precondition (and (F1) (F13) (F25))
    :effect (and (F5) (F17) (not (F1)) (not (F13)))
)
(:action push-box_p1_glass
    :precondition (and (F2) (F14) (F25))
    :effect (and (F5) (F17) (not (F2)) (not (F14)))
)
(:action push-box_p1_knife
    :precondition (and (F3) (F15) (F25))
    :effect (and (F5) (F17) (not (F3)) (not (F15)))
)
(:action push-box_p1_monkey
    :precondition (and (F4) (F16) (F25))
    :effect (and (F5) (F17) (not (F4)) (not (F16)))
)
(:action push-box_p1_p2
    :precondition (and (F6) (F18) (F25))
    :effect (and (F5) (F17) (not (F6)) (not (F18)))
)
(:action push-box_p1_p3
    :precondition (and (F7) (F19) (F25))
    :effect (and (F5) (F17) (not (F7)) (not (F19)))
)
(:action push-box_p1_p4
    :precondition (and (F8) (F20) (F25))
    :effect (and (F5) (F17) (not (F8)) (not (F20)))
)
(:action push-box_p1_water
    :precondition (and (F9) (F21) (F25))
    :effect (and (F5) (F17) (not (F9)) (not (F21)))
)
(:action push-box_p1_waterfountain
    :precondition (and (F10) (F22) (F25))
    :effect (and (F5) (F17) (not (F10)) (not (F22)))
)
(:action push-box_p2_bananas
    :precondition (and (F0) (F12) (F25))
    :effect (and (F6) (F18) (not (F0)) (not (F12)))
)
(:action push-box_p2_box
    :precondition (and (F1) (F13) (F25))
    :effect (and (F6) (F18) (not (F1)) (not (F13)))
)
(:action push-box_p2_glass
    :precondition (and (F2) (F14) (F25))
    :effect (and (F6) (F18) (not (F2)) (not (F14)))
)
(:action push-box_p2_knife
    :precondition (and (F3) (F15) (F25))
    :effect (and (F6) (F18) (not (F3)) (not (F15)))
)
(:action push-box_p2_monkey
    :precondition (and (F4) (F16) (F25))
    :effect (and (F6) (F18) (not (F4)) (not (F16)))
)
(:action push-box_p2_p1
    :precondition (and (F5) (F17) (F25))
    :effect (and (F6) (F18) (not (F5)) (not (F17)))
)
(:action push-box_p2_p3
    :precondition (and (F7) (F19) (F25))
    :effect (and (F6) (F18) (not (F7)) (not (F19)))
)
(:action push-box_p2_p4
    :precondition (and (F8) (F20) (F25))
    :effect (and (F6) (F18) (not (F8)) (not (F20)))
)
(:action push-box_p2_water
    :precondition (and (F9) (F21) (F25))
    :effect (and (F6) (F18) (not (F9)) (not (F21)))
)
(:action push-box_p2_waterfountain
    :precondition (and (F10) (F22) (F25))
    :effect (and (F6) (F18) (not (F10)) (not (F22)))
)
(:action push-box_p3_bananas
    :precondition (and (F0) (F12) (F25))
    :effect (and (F7) (F19) (not (F0)) (not (F12)))
)
(:action push-box_p3_box
    :precondition (and (F1) (F13) (F25))
    :effect (and (F7) (F19) (not (F1)) (not (F13)))
)
(:action push-box_p3_glass
    :precondition (and (F2) (F14) (F25))
    :effect (and (F7) (F19) (not (F2)) (not (F14)))
)
(:action push-box_p3_knife
    :precondition (and (F3) (F15) (F25))
    :effect (and (F7) (F19) (not (F3)) (not (F15)))
)
(:action push-box_p3_monkey
    :precondition (and (F4) (F16) (F25))
    :effect (and (F7) (F19) (not (F4)) (not (F16)))
)
(:action push-box_p3_p1
    :precondition (and (F5) (F17) (F25))
    :effect (and (F7) (F19) (not (F5)) (not (F17)))
)
(:action push-box_p3_p2
    :precondition (and (F6) (F18) (F25))
    :effect (and (F7) (F19) (not (F6)) (not (F18)))
)
(:action push-box_p3_p4
    :precondition (and (F8) (F20) (F25))
    :effect (and (F7) (F19) (not (F8)) (not (F20)))
)
(:action push-box_p3_water
    :precondition (and (F9) (F21) (F25))
    :effect (and (F7) (F19) (not (F9)) (not (F21)))
)
(:action push-box_p3_waterfountain
    :precondition (and (F10) (F22) (F25))
    :effect (and (F7) (F19) (not (F10)) (not (F22)))
)
(:action push-box_p4_bananas
    :precondition (and (F0) (F12) (F25))
    :effect (and (F8) (F20) (not (F0)) (not (F12)))
)
(:action push-box_p4_box
    :precondition (and (F1) (F13) (F25))
    :effect (and (F8) (F20) (not (F1)) (not (F13)))
)
(:action push-box_p4_glass
    :precondition (and (F2) (F14) (F25))
    :effect (and (F8) (F20) (not (F2)) (not (F14)))
)
(:action push-box_p4_knife
    :precondition (and (F3) (F15) (F25))
    :effect (and (F8) (F20) (not (F3)) (not (F15)))
)
(:action push-box_p4_monkey
    :precondition (and (F4) (F16) (F25))
    :effect (and (F8) (F20) (not (F4)) (not (F16)))
)
(:action push-box_p4_p1
    :precondition (and (F5) (F17) (F25))
    :effect (and (F8) (F20) (not (F5)) (not (F17)))
)
(:action push-box_p4_p2
    :precondition (and (F6) (F18) (F25))
    :effect (and (F8) (F20) (not (F6)) (not (F18)))
)
(:action push-box_p4_p3
    :precondition (and (F7) (F19) (F25))
    :effect (and (F8) (F20) (not (F7)) (not (F19)))
)
(:action push-box_p4_water
    :precondition (and (F9) (F21) (F25))
    :effect (and (F8) (F20) (not (F9)) (not (F21)))
)
(:action push-box_p4_waterfountain
    :precondition (and (F10) (F22) (F25))
    :effect (and (F8) (F20) (not (F10)) (not (F22)))
)
(:action push-box_water_bananas
    :precondition (and (F0) (F12) (F25))
    :effect (and (F9) (F21) (not (F0)) (not (F12)))
)
(:action push-box_water_box
    :precondition (and (F1) (F13) (F25))
    :effect (and (F9) (F21) (not (F1)) (not (F13)))
)
(:action push-box_water_glass
    :precondition (and (F2) (F14) (F25))
    :effect (and (F9) (F21) (not (F2)) (not (F14)))
)
(:action push-box_water_knife
    :precondition (and (F3) (F15) (F25))
    :effect (and (F9) (F21) (not (F3)) (not (F15)))
)
(:action push-box_water_monkey
    :precondition (and (F4) (F16) (F25))
    :effect (and (F9) (F21) (not (F4)) (not (F16)))
)
(:action push-box_water_p1
    :precondition (and (F5) (F17) (F25))
    :effect (and (F9) (F21) (not (F5)) (not (F17)))
)
(:action push-box_water_p2
    :precondition (and (F6) (F18) (F25))
    :effect (and (F9) (F21) (not (F6)) (not (F18)))
)
(:action push-box_water_p3
    :precondition (and (F7) (F19) (F25))
    :effect (and (F9) (F21) (not (F7)) (not (F19)))
)
(:action push-box_water_p4
    :precondition (and (F8) (F20) (F25))
    :effect (and (F9) (F21) (not (F8)) (not (F20)))
)
(:action push-box_water_waterfountain
    :precondition (and (F10) (F22) (F25))
    :effect (and (F9) (F21) (not (F10)) (not (F22)))
)
(:action push-box_waterfountain_bananas
    :precondition (and (F0) (F12) (F25))
    :effect (and (F10) (F22) (not (F0)) (not (F12)))
)
(:action push-box_waterfountain_box
    :precondition (and (F1) (F13) (F25))
    :effect (and (F10) (F22) (not (F1)) (not (F13)))
)
(:action push-box_waterfountain_glass
    :precondition (and (F2) (F14) (F25))
    :effect (and (F10) (F22) (not (F2)) (not (F14)))
)
(:action push-box_waterfountain_knife
    :precondition (and (F3) (F15) (F25))
    :effect (and (F10) (F22) (not (F3)) (not (F15)))
)
(:action push-box_waterfountain_monkey
    :precondition (and (F4) (F16) (F25))
    :effect (and (F10) (F22) (not (F4)) (not (F16)))
)
(:action push-box_waterfountain_p1
    :precondition (and (F5) (F17) (F25))
    :effect (and (F10) (F22) (not (F5)) (not (F17)))
)
(:action push-box_waterfountain_p2
    :precondition (and (F6) (F18) (F25))
    :effect (and (F10) (F22) (not (F6)) (not (F18)))
)
(:action push-box_waterfountain_p3
    :precondition (and (F7) (F19) (F25))
    :effect (and (F10) (F22) (not (F7)) (not (F19)))
)
(:action push-box_waterfountain_p4
    :precondition (and (F8) (F20) (F25))
    :effect (and (F10) (F22) (not (F8)) (not (F20)))
)
(:action push-box_waterfountain_water
    :precondition (and (F9) (F21) (F25))
    :effect (and (F10) (F22) (not (F9)) (not (F21)))
)
)
