# ILP-based delete relaxation heuristics for POCL Planning

## The Report
The report folder has each iteration of the report, starting with the notes files which were used to jot down ideas and potentially useful references. It also contains the poster, and other materials generated for the course.

## The Heuristic
cd into the vhpop folder. You can then run vhpop. Here is an example command to run the ILP heuristic:
```
./vhpop -h ILP -f LCFR -l 10000 -f MW -l unlimited ./examples/_grounded-bulldozer-domain.pddl ./examples/_grounded-bulldozer-simple.pddl
```
The counting constraint relaxation is also available with the heuristic name ILPC. Both will be default print their solutions for each node, but this can be turned off in heuristics.cc by changing the verbosity in their function calls to 0.\
Most of my code can be found in the ilp.h and ilp.cc files, which were fully coded by me. I also added code to heuristics.h and heuristics.cc, as well as plans.h and plans.cc.
