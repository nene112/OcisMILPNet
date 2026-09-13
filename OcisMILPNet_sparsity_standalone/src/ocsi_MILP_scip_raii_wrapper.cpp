

#include "ocis_MILP_scip_raii_wrapper.h"
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <cstdlib>
#pragma comment(linker, "/STACK:8000000")  
SCIPModel::SCIPModel() {
  std::cout << "=== SCIP INIT START ===" << std::endl;
  SCIP_CALL_ABORT(SCIPcreate(&scip_));
  SCIP_CALL_ABORT(SCIPincludeDefaultPlugins(scip_));
  SCIP_CALL_ABORT(SCIPcreateProbBasic(scip_, "SCIP_RAII_Model"));
  
  SCIP_CALL_ABORT(SCIPsetIntParam(scip_, "display/verblevel", 1)); 



  const char* thread_env = std::getenv("OCIS_SCIP_THREADS");
  int nthreads = thread_env && *thread_env ? std::atoi(thread_env) : 4;
  const char* mode_env = std::getenv("OCIS_SCIP_PARALLEL_MODE");
  const int parallel_mode = mode_env && *mode_env ? std::atoi(mode_env) : 0;
  if (nthreads < 1) {
    nthreads = 1;
  }
  SCIPsetIntParam(scip_, "parallel/maxnthreads", nthreads);
  SCIPsetIntParam(scip_, "parallel/minnthreads", nthreads);
  SCIPsetIntParam(scip_, "parallel/mode", parallel_mode);
  std::cout << "[SCIP parallel] maxnthreads=" << nthreads
            << " minnthreads=" << nthreads
            << " mode=" << parallel_mode << std::endl;
  
  
//   SCIP_CALL_ABORT(SCIPsetNumericalParam(scip_, "numerics/feastol", 1e-5));
   //SCIPsetRealParam(scip_, "numerics/feastol", 1e-03);
  std::cout << "SCIPcreate() done" << std::endl;


}



SCIP_RETCODE SCIPModel::initialize() {
  
   /* include event handler for best solution found */
  SCIP_CALL(SCIPincludeEventHdlrBestsol(scip_));

  initialized_ = true;
  return SCIP_OKAY;
}


SCIP_RETCODE SCIPModel::SCIPincludeEventHdlrBestsol(SCIP* scip)
{
  SCIP_EVENTHDLRDATA* eventhdlrdata;
  SCIP_EVENTHDLR* eventhdlr;

  eventhdlrdata = NULL;

  
  SolutionData* solData = NULL;// (SolutionData*)malloc(sizeof(SolutionData));
  SCIP_CALL(SCIPallocBlockMemory(scip_, &solData));  

  solData->solutionCount = 0;
  solData->maxSolutions = 1;      
  solData->stopOnFirst = TRUE;    


  
  SCIPincludeEventhdlrBasic(scip_, &eventhdlr,
    "MyEventCallback",           
    "在找到解时停止求解",        
    eventExecFoundSolution,         
    eventhdlrdata);   

  
  SCIPeventhdlrSetData(eventhdlr, (SCIP_EVENTHDLRDATA*)solData);
  printf("解决方案监控器已注册\n");

  
  SCIPsetEventhdlrInit(scip, eventhdlr, eventInitSolutionMonitor);
  SCIPsetEventhdlrExit(scip, eventhdlr, eventExitSolutionMonitor);
  printf("已捕获多个事件类型用于测试\n");
  return SCIP_OKAY;

}




SCIPModel::~SCIPModel() {
  
  for (auto var : vars_) {
    SCIPreleaseVar(scip_, &var);
  }
  vars_.clear();

  
  SCIP_CALL_ABORT(SCIPfree(&scip_));
}

SCIP_VAR* SCIPModel::addVariable(const std::string& name, double lb, double ub, double obj_coef, VarType type) {
  SCIP_VAR* var = nullptr;
  SCIP_VARTYPE vartype = SCIP_VARTYPE_CONTINUOUS;
  if (type == VarType::INTEGER) vartype = SCIP_VARTYPE_INTEGER;
  else if (type == VarType::BINARY) vartype = SCIP_VARTYPE_BINARY;
  //if (vartype == SCIP_VARTYPE_INTEGER) {
  //  ub = 1;
  //}

  if (lb > ub) {
    int a = 0;
  }


  if (isinf(obj_coef)) {
    obj_coef = 1;
  }
  SCIP_CALL_ABORT(SCIPcreateVarBasic(scip_, &var, name.c_str(), lb, ub, obj_coef, vartype));
  SCIP_CALL_ABORT(SCIPaddVar(scip_, var));
  vars_.push_back(var);
  return var;
}


SCIPModel::Var1D SCIPModel::addVariables1D(int n, const std::string& prefix,
  double lb, double ub, double obj_coef, VarType type) {
  ensurePrefixUnique(prefix);
  Var1D var1d(n);  

  for (int i = 0; i < n; ++i) {
    addVariable(generateVarName(prefix, i), lb, ub, obj_coef, type);
    var1d.set(i, vars_.back());
  }
  return var1d;
}

SCIPModel::Var1D SCIPModel::addVariables1D(int n, const std::string& prefix,
  double lb, double ub, std::vector<double> obj_coef, VarType type) {
  ensurePrefixUnique(prefix);
  Var1D var1d(n);  

  for (int i = 0; i < n; ++i) {
    addVariable(generateVarName(prefix, i), lb, ub, obj_coef[i], type);
    var1d.set(i, vars_.back());
  }
  return var1d;

}


SCIPModel::Var1D SCIPModel::addVariables1D(int n, const std::string& prefix,
  std::vector<double> lb_vec, std::vector<double>  ub_vec, double obj_coef, VarType type) {
  ensurePrefixUnique(prefix);
  Var1D var1d(n);  

  for (int i = 0; i < n; ++i) {

    double lb = lb_vec[i];
    double ub = ub_vec[i];

    addVariable(generateVarName(prefix, i), lb, ub, obj_coef, type);
    var1d.set(i, vars_.back());
  }
  return var1d;
}





SCIPModel::Var2D SCIPModel::addVariables2D(int rows, int cols, const std::string& prefix, double lb, double ub, double obj_coef, VarType type) {
  ensurePrefixUnique(prefix);
  Var2D var2d(rows, cols);
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      SCIP_VAR* var = addVariable(generateVarName(prefix, i, j), lb, ub, obj_coef, type);
      var2d.set(i, j, var);
    }
  }
  return var2d;
}

SCIPModel::Var3D SCIPModel::addVariables3D(int rows, int cols, int znum, const std::string& prefix,
  double lb, double ub, double obj_coef, VarType type) {
  ensurePrefixUnique(prefix);
  Var3D var3d(rows, cols, znum);
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      for (int k = 0; k < znum; ++k) {
        SCIP_VAR* var = addVariable(generateVarName(prefix, i, j, k), lb, ub, obj_coef, type);
        var3d.set(i, j, k, var);
      }
    }
  }
  return var3d;

}






SCIPModel::Var2D SCIPModel::addVariables2D(int rows, int cols, const std::string& prefix, double lb, double ub, std::vector<double> obj_coef_vec, VarType type) {
  ensurePrefixUnique(prefix);
  Var2D var2d(rows, cols);
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      double coef = 1;
      if (obj_coef_vec.size() > i) {
        coef = obj_coef_vec[i];
      }

      SCIP_VAR* var = addVariable(generateVarName(prefix, i, j), lb, ub, coef, type);
      var2d.set(i, j, var);
    }
  }
  return var2d;
}
SCIPModel::Var2D SCIPModel::addVariables2D(int rows, int cols, const std::string& prefix,
  double lb, double ub, std::map<int, std::vector<int>> obj_coef_vec, int min_max, VarType type) {
  ensurePrefixUnique(prefix);
  Var2D var2d(rows, cols);
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      double coef = 1;
      if (obj_coef_vec.size() > i) {
        coef = min_max * obj_coef_vec[i][j];
      }

      SCIP_VAR* var = addVariable(generateVarName(prefix, i, j), lb, ub, coef, type);
      var2d.set(i, j, var);
    }
  }
  return var2d;
}

SCIPModel::Var2D SCIPModel::addVariables2D(int rows, int cols, const std::string& prefix,
  double lb, double ub, std::vector< std::vector<double>> obj_coef_vec, VarType type) {
  ensurePrefixUnique(prefix);
  Var2D var2d(rows, cols);
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      double coef = 1;
      if (obj_coef_vec.size() > i) {
        coef = obj_coef_vec[i][j];
      }

      SCIP_VAR* var = addVariable(generateVarName(prefix, i, j), lb, ub, coef, type);
      var2d.set(i, j, var);
    }
  }
  return var2d;
}


SCIPModel::Var2D SCIPModel::addVariables2D(int rows, int cols, const std::string& prefix, double lb, std::vector<double>  ub, std::vector<double> obj_coef_vec, VarType type) {
  ensurePrefixUnique(prefix);
  Var2D var2d(rows, cols);
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      double coef = 1;
      if (obj_coef_vec.size() > i) {
        coef = obj_coef_vec[i];
      }
      double u_bound = 100;
      if (ub.size() > i) {
        u_bound = ub[i];
      }


      SCIP_VAR* var = addVariable(generateVarName(prefix, i, j), lb, u_bound, coef, type);
      var2d.set(i, j, var);
    }
  }
  return var2d;
}


SCIPModel::Var2D SCIPModel::addVariables2D(int rows, int cols, const std::string& prefix, double lb, std::vector<double>  ub, double coef, VarType type) {
  ensurePrefixUnique(prefix);
  Var2D var2d(rows, cols);
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      double u_bound = 100;
      if (ub.size() > i) {
        u_bound = ub[i];
      }


      SCIP_VAR* var = addVariable(generateVarName(prefix, i, j), lb, u_bound, coef, type);
      var2d.set(i, j, var);
    }
  }
  return var2d;
}
SCIPModel::Var2D SCIPModel::addVariables2D(
  int rows, int cols,
  const std::string& prefix,
  std::vector<double>& lb,
  std::vector<double>& ub,
  double coef, VarType type
) {
  ensurePrefixUnique(prefix);
  Var2D var2d(rows, cols);
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      double u_bound = 100;
      double u_bound_low = 0;
      if (ub.size() > i) {
        u_bound = ub[i];
      }
      if (lb.size() > i) {
        u_bound_low = lb[i];
      }



      SCIP_VAR* var = addVariable(generateVarName(prefix, i, j), u_bound_low, u_bound, coef, type);
      var2d.set(i, j, var);
    }
  }
  return var2d;
}
SCIPModel::Var2D SCIPModel::addVariables2D(
  int rows, int cols,
  const std::string& prefix,
  std::vector<double>& lb,
  std::vector<double>& ub,
  std::vector<double>& coef,
  VarType type
) {
  ensurePrefixUnique(prefix);
  Var2D var2d(rows, cols);
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      double u_bound = 100;
      double u_bound_low = 0;
      double coef_value = 0;
      if (ub.size() > i) {
        u_bound = ub[i];
      }
      if (lb.size() > i) {
        u_bound_low = lb[i];
      }
      if (coef.size() > i) {
        coef_value = coef[i];
      }
      SCIP_VAR* var = addVariable(generateVarName(prefix, i, j), u_bound_low, u_bound, coef_value, type);
      var2d.set(i, j, var);
    }
  }
  return var2d;
}


SCIP_CONS* SCIPModel::addLinearConstraint(
  const std::vector<SCIP_VAR*>& vars,
  const std::vector<double>& coeffs,
  double lhs, double rhs,
  const std::string& name)
{
  if (vars.size() != coeffs.size()) {
    throw std::invalid_argument("Variables and coefficients size mismatch");
  }

  SCIP_CONS* cons = nullptr;
  SCIP_CALL_ABORT(SCIPcreateConsBasicLinear(
    scip_, &cons, name.c_str(), 0, nullptr, nullptr, lhs, rhs));

  for (size_t i = 0; i < vars.size(); ++i) {
    SCIP_CALL_ABORT(SCIPaddCoefLinear(scip_, cons, vars[i], coeffs[i]));
  }

  SCIP_CALL_ABORT(SCIPaddCons(scip_, cons));
  SCIP_CONS* added_cons = cons;
  SCIP_CALL_ABORT(SCIPreleaseCons(scip_, &cons));
  return added_cons;
}


SCIP_CONS* SCIPModel::addEqualityConstraint(
  const std::vector<SCIP_VAR*>& vars,
  const std::vector<double>& coeffs,
  double rhs,
  const std::string& name)
{
  
  return addLinearConstraint(vars, coeffs, rhs, rhs, name);
}

SCIP_RETCODE SCIPModel::solve() {
  SCIP_CALL_ABORT(SCIPsetObjsense(scip_, SCIP_OBJSENSE_MINIMIZE));
  auto start = std::chrono::high_resolution_clock::now();

  SCIP_RETCODE retcode = SCIPsolve(scip_);

  
  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  
  std::cout << "**** Solve Time Consume : " << duration.count() << " millisecond ****" << std::endl;

  return retcode;
}
SCIP_RETCODE SCIPModel::solve_max() {
  SCIP_CALL_ABORT(SCIPsetObjsense(scip_, SCIP_OBJSENSE_MAXIMIZE));
  auto start = std::chrono::high_resolution_clock::now();

  SCIP_RETCODE retcode = SCIPsolve(scip_);

  
  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  
  std::cout << "Solve Time Consume : " << duration.count() << " millisecond" << std::endl;

  return retcode;
}

double SCIPModel::getSolution(SCIP_VAR* var) const {
  //   return SCIPgetSolVal(scip_, nullptr, var);

  SCIP_SOL* sol = SCIPgetBestSol(scip_);
  if (sol == nullptr) {
    
    //throw std::runtime_error("No solution available");
    return 0;
  }
  return SCIPgetSolVal(scip_, sol, var);
}

void SCIPModel::printSolution() const {
  SCIP_SOL* sol = SCIPgetBestSol(scip_);
  if (sol) {
    SCIP_CALL_ABORT(SCIPprintSol(scip_, sol, nullptr, FALSE));
  }
}


void SCIPModel::ensurePrefixUnique(const std::string& prefix) {
  
  
}

std::string SCIPModel::generateVarName(const std::string& prefix, int index1, int index2, int index3) const {
  std::ostringstream oss;
  oss << prefix;

  if (index1 >= 0) {
    oss << "_" << index1;
    if (index2 >= 0) {
      oss << "_" << index2;

      if (index3 >= 0) {
        oss << "_" << index3;
      }

    }
  }

  return oss.str();
}
