#include <iostream>

#include <ilcplex/ilocplex.h>

/* TODO: try to get rid of this warning workaround */
[[maybe_unused]] void example_ILP () {
  IloEnv env;
  try {
    IloModel model(env);  
    IloNumVarArray x(env);
    IloRangeArray c(env);
  
    IloEnv env = model.getEnv();  
  
    x.add(IloNumVar(env, 0, 1, IloNumVar::Bool, "x1"));
    x.add(IloNumVar(env, 0, 1, IloNumVar::Bool, "x2"));
    x.add(IloNumVar(env, 0, 1, IloNumVar::Bool, "x3"));  
  
    model.add(IloMinimize(env, x[0] + 2 * x[1] + 10 * x[2]));  
  
    c.add(x[0] + x[1] + x[2] >= 1);
    c.add(x[0] + 3*x[1] + x[2] >= 3);  
  
    model.add(c);

    IloCplex cplex(model);

    // Optimize the problem and obtain solution.
    if ( !cplex.solve() ) {
       env.error() << "Failed to optimize LP" << std::endl;
       throw(-1);
    }

    IloNumArray vals(env);
    env.out() << "Solution status = " << cplex.getStatus() << std::endl;
    env.out() << "Solution value  = " << cplex.getObjValue() << std::endl;
    cplex.getValues(vals, x);
    env.out() << "Values        = " << vals << std::endl;
  }
  catch (IloException& e) {
    std::cerr << "CPLEX Concert exception caught: " << e << std::endl;
  }
  catch (...) {
    std::cerr << "Unknown ILP exception caught" << std::endl;
  }
  env.end();
}