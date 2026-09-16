# ILP-based delete relaxation heuristics for POCL Planning

## AI-Usage Statement
This repository used NO LLM assistance for its original development, and MINIMAL LLM assistance for repository cleanup and publication.

## Repository Context
This repository is forked from vhpop https://github.com/hlsyounes/vhpop and moved over from an original repository on ANU gitlab. As part of my COMP3770 independent research course, I developed and tested a novel forms of partial order causal link planning heuristic within vhpop, which is what this repository holds. Unlike many of my other ANU work, as this was entirely independant work, it is not bound by ANUs copyright licences which usually prevent the public release of work.

### My additions
Most of my code can be found in the ilp.h and ilp.cc files, which were fully coded by me. I also added code to heuristics.h and heuristics.cc (all sections relating to the ILP and ILPC heuristics), as well as plans.h and plans.cc (only minor details changed).

## The Report
The full final project report, and single A0 size poster, are in the root directory, and should be referred to as a reference for the context behind this repository.  

The report folder has each iteration of the report, starting with the notes files which were used to jot down ideas and potentially useful references. It also contains other materials generated for the course.

## Building with CPLEX
The ILP and ILPC heuristics call IBM ILOG CPLEX through its C++ Concert API, so CPLEX must be installed before VHPOP will link. The project was developed against CPLEX Studio 22.1.1 on x86-64 Linux; adjust the paths below if your installation differs.

Prerequisites: a C++17 compiler, GNU autotools (autoconf, automake, libtool), bison and flex, and CPLEX Studio (a free academic licence is available from IBM).

The build scripts are not tracked, so regenerate them first, then pass the CPLEX include and library locations to `make`:
```
cd vhpop
autoreconf -i
./configure
make \
  CPPFLAGS='-I/opt/ibm/ILOG/CPLEX_Studio2211/cplex/include -I/opt/ibm/ILOG/CPLEX_Studio2211/concert/include' \
  LDFLAGS='-L/opt/ibm/ILOG/CPLEX_Studio2211/cplex/lib/x86-64_linux/static_pic -L/opt/ibm/ILOG/CPLEX_Studio2211/concert/lib/x86-64_linux/static_pic' \
  LIBS='-lilocplex -lconcert -lcplex -lm -lpthread'
```
`CPLEX_Studio2211` is the default install directory name for version 22.1.1; the `static_pic` folder holds the static Concert and CPLEX libraries that the heuristics link against. A successful build produces the `vhpop` executable in the same folder.

## Running VHPOP with these Heuristics
cd into the vhpop folder. You can then run vhpop. Here is an example command to run the ILP heuristic:
```
./vhpop -h ILP -f LCFR -l 10000 -f MW -l unlimited ./examples/_grounded-bulldozer-domain.pddl ./examples/_grounded-bulldozer-simple.pddl
```
For further details on how to run vhpop, refer to the vhpop readme.\
The counting constraint relaxation is also available with the heuristic name ILPC. By default neither heuristic prints anything while searching. To see the ILP solution found for each search node, pass VHPOP's verbosity option, for example `-v` or `-v1`. Level `-v2` additionally shows the ILP model being built and the CPLEX solver log, and `-v3` dumps the full constraint set; these higher levels also enable VHPOP's own diagnostic output.
