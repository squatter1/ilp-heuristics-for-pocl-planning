export CPPFLAGS='-I/mnt/c/PROGRA~1/IBM/ILOG/CPLEX_Studio_Community2211/cplex/include -I/mnt/c/PROGRA~1/IBM/ILOG/CPLEX_Studio_Community2211/concert/include'
export LDFLAGS='-L/mnt/c/PROGRA~1/IBM/ILOG/CPLEX_Studio_Community2211/cplex/lib/x64_windows_msvc14/stat_mda -L/mnt/c/PROGRA~1/IBM/ILOG/CPLEX_Studio_Community2211/concert/lib/x64_windows_msvc14/stat_mda'
export LIBS='-lilocplex -lconcert'

./configure
make