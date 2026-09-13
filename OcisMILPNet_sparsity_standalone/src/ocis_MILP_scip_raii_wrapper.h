#pragma once
#include <memory>
#include <vector>
#include <string>
#include <stdexcept>  
#include <scip/scip.h>
#include <scip/scipdefplugins.h>
#include "scip/scipshell.h"
#include "scip/scip_conflict.h"

#include <unordered_map>
#include <map>



typedef struct {
  int solutionCount;      
  int maxSolutions;       
  SCIP_Bool stopOnFirst;  
} SolutionData;



static void executeCustomLogic(SCIP* scip, SCIP_SOL* sol)
{
  
  printf("   执行自定义逻辑...\n");

  // SCIP_VAR** vars = SCIPgetVars(scip);
  // int nvars = SCIPgetNVars(scip);
  // 
  
  // {
  //     SCIP_Real val = SCIPgetSolVal(scip, sol, vars[i]);
  
  // }
}



static SCIP_DECL_EVENTEXEC(eventExecFoundSolution)
{

  SolutionData* solData = (SolutionData*)SCIPeventhdlrGetData(eventhdlr);

  if (solData == NULL)
    return SCIP_OKAY;

  solData->solutionCount++;

  SCIP_SOL* bestSol = SCIPgetBestSol(scip);
  SCIP_Real objVal = SCIPgetSolOrigObj(scip, bestSol);

  printf("找到第 %d 个可行解！目标值: %f\n",
    solData->solutionCount, objVal);
  printf("   当前节点: %ld, 求解时间: %.2f 秒\n",
    SCIPgetNNodes(scip), SCIPgetSolvingTime(scip));

  
  executeCustomLogic(scip, bestSol);

  
  if (solData->stopOnFirst && solData->solutionCount >= 1)
  {
    printf("找到第一个可行解，停止求解\n");
    //  SCIPinterruptSolve(scip);
              //solData->shouldStop = true;

              
    
    SCIPsetRealParam(scip, "limits/time", SCIPgetSolvingTime(scip));
  }
  
  else if (solData->maxSolutions > 0 && solData->solutionCount >= solData->maxSolutions)
  {
    printf("已达到最大解数量 (%d)，停止求解\n", solData->maxSolutions);
    SCIPinterruptSolve(scip);
  }

  return SCIP_OKAY;

}


static SCIP_DECL_EVENTINIT(eventInitSolutionMonitor)
{
  
  SolutionData* solData = (SolutionData*)malloc(sizeof(SolutionData));
  solData->solutionCount = 0;
  solData->maxSolutions = 1;      
  solData->stopOnFirst = TRUE;    
  
  SCIP_CALL(SCIPcatchEvent(scip, SCIP_EVENTTYPE_BESTSOLFOUND, eventhdlr, NULL, NULL));

  // SCIPsetEventhdlrData(scip, eventhdlr, solData);
  printf("解决方案监控器已初始化\n");

  return SCIP_OKAY;
}


static SCIP_DECL_EVENTEXIT(eventExitSolutionMonitor)
{
  SolutionData* solData = (SolutionData*)SCIPeventhdlrGetData(eventhdlr);

  if (solData != NULL)
  {
    printf("求解统计: 共找到 %d 个可行解\n", solData->solutionCount);
    //free(solData);
  //  SCIPsetEventhdlrData(scip, eventhdlr, NULL);
  }
  
  SCIP_CALL(SCIPdropEvent(scip, SCIP_EVENTTYPE_BESTSOLFOUND, eventhdlr, NULL, -1));
  printf("解决方案监控器已退出\n");
  return SCIP_OKAY;
}


class SCIPModel {
public:
  enum class VarType { CONTINUOUS, INTEGER, BINARY };
  std::vector<double> dualValues; 

  
  class Var1D {
  private:
    std::vector<SCIP_VAR*> vars_;
  public:
    
    Var1D() = default;  
    explicit Var1D(size_t n) : vars_(n, nullptr) {}  

    
    Var1D(const Var1D&) = delete;
    Var1D& operator=(const Var1D&) = delete;

    
    Var1D(Var1D&&) = default;
    Var1D& operator=(Var1D&&) = default;

    
    SCIP_VAR* operator()(size_t i) const {
      if (i >= vars_.size()) throw std::out_of_range("Var1D index out of range");
      return vars_[i];
    }

    void set(size_t i, SCIP_VAR* var) {
      if (i >= vars_.size()) throw std::out_of_range("Var1D index out of range");
      vars_[i] = var;
    }

    size_t size() const noexcept { return vars_.size(); }
  };

  
  class Var2D {
  private:
    std::vector<std::vector<SCIP_VAR*>> vars_;
  public:
    Var2D() = default;  
    Var2D(int rows, int cols) : vars_(rows, std::vector<SCIP_VAR*>(cols, nullptr)) {}

    void set(int i, int j, SCIP_VAR* var) {
      if (i >= 0 && i < vars_.size() && j >= 0 && j < vars_[i].size()) {
        vars_[i][j] = var;
      }
    }
    SCIP_VAR* operator()(int i, int j) const {

      if (i > vars_.size()) {
        int a = 0;
        printf("index in 1D out of var2D range!");
      }
      else if (j > vars_[i].size())
      {
        int a = 0;
        printf("index in 2D out of var2D range!");

      }

      return vars_.at(i).at(j);  
    }
  };

  class Var3D {
  private:
    std::vector<std::vector<std::vector<SCIP_VAR*>>> vars_;
  public:
    Var3D() = default;  
  
    Var3D(int rows, int cols, int znum)
      : vars_(rows, std::vector<std::vector<SCIP_VAR*>>(
        cols, std::vector<SCIP_VAR*>(znum, nullptr))) {}

    void set(int i, int j, int k, SCIP_VAR* var) {
      if (i >= 0 && i < vars_.size() &&
        j >= 0 && j < vars_[i].size() &&
        k >= 0 && k < vars_[i][j].size()) {
        vars_[i][j][k] = var;
      }
      else {
        throw std::out_of_range("Var3D::set: Index out of range");
      }
    }
    SCIP_VAR* operator()(int i, int j, int k) const {
      return vars_.at(i).at(j).at(k);  
    }

    
    size_t rows() const { return vars_.size(); }
    size_t cols() const { return (vars_.empty() ? 0 : vars_[0].size()); }
    size_t znum() const { return (vars_.empty() || vars_[0].empty() ? 0 : vars_[0][0].size()); }

  };


  SCIPModel();
  ~SCIPModel();

  SCIPModel(const SCIPModel&) = delete;
  SCIPModel& operator=(const SCIPModel&) = delete;

  SCIP_VAR* addVariable(const std::string& name, double lb, double ub, double obj_coef, VarType type);

  
  Var1D addVariables1D(int n, const std::string& prefix,
    double lb, double ub, double obj_coef, VarType type);
  Var1D addVariables1D(int n, const std::string& prefix,
    double lb, double ub, std::vector<double> obj_coef, VarType type);

  Var1D addVariables1D(int n, const std::string& prefix,
    std::vector<double> lb_vec, std::vector<double>  ub_vec, double obj_coef, VarType type);

  
  Var2D addVariables2D(int rows, int cols, const std::string& prefix,
    double lb, double ub, double obj_coef, VarType type);
  Var3D addVariables3D(int rows, int cols, int znum, const std::string& prefix,
    double lb, double ub, double obj_coef, VarType type);

  
  
  Var2D addVariables2D(int rows, int cols, const std::string& prefix,
    double lb, double ub, std::vector<double> obj_coef_vec, VarType type);
  
  Var2D addVariables2D(int rows, int cols, const std::string& prefix,
    double lb, std::vector<double> ub, std::vector<double> obj_coef_vec, VarType type);
  
  Var2D addVariables2D(int rows, int cols, const std::string& prefix,
    double lb, std::vector<double> ub, double obj_coef, VarType type);

  
  Var2D addVariables2D(int rows, int cols, const std::string& prefix,
    std::vector<double>& lb, std::vector<double>& ub, double obj_coef, VarType type);
  
  Var2D addVariables2D(int rows, int cols, const std::string& prefix,
    std::vector<double>& lb, std::vector<double>& ub,
    std::vector<double>& obj_coef, VarType type);

  
  Var2D addVariables2D(int rows, int cols, const std::string& prefix,
    double lb, double ub, std::map<int, std::vector<int>> obj_coef_vec, int min_max, VarType type);

  Var2D addVariables2D(int rows, int cols, const std::string& prefix,
    double lb, double ub, std::vector< std::vector<double>> obj_coef_vec, VarType type);




  SCIP_CONS* addLinearConstraint(const std::vector<SCIP_VAR*>& vars,
    const std::vector<double>& coeffs,
    double lhs, double rhs,
    const std::string& name);
  SCIP_CONS* addEqualityConstraint(
    const std::vector<SCIP_VAR*>& vars,
    const std::vector<double>& coeffs,
    double rhs,
    const std::string& name);

  SCIP_RETCODE solve();
  SCIP_RETCODE solve_max();
  double getSolution(SCIP_VAR* var) const;
  void printSolution() const;

  SCIP* getSCIP() { return scip_; }
  const std::vector<SCIP_VAR*>& getVars() const { return vars_; }

  
  void countVariableTypes(int& ncontinuous, int& ninteger) const {
    ncontinuous = 0;
    ninteger = 0;
    for (SCIP_VAR* var : vars_) {
      if (var) { 
        SCIP_VARTYPE vartype = SCIPvarGetType(var);
        if (vartype == SCIP_VARTYPE_CONTINUOUS) {
          ncontinuous++;
        }
        else if (vartype == SCIP_VARTYPE_INTEGER || vartype == SCIP_VARTYPE_BINARY) {
          ninteger++;
        }
      }
    }
  }


  std::vector<std::vector<double>> getConstraintMatrixFromLP(SCIP* scip) {

    std::vector<std::vector<double>> matrix;

    if (scip == nullptr) {
      return matrix;
    }

    
    SCIP_CONS** conss = SCIPgetOrigConss(scip);
    int nconss = SCIPgetNOrigConss(scip);

    
    SCIP_VAR** vars = SCIPgetOrigVars(scip);
    int nvars = SCIPgetNOrigVars(scip);


    if (nvars == 0 || nconss == 0) {
      return matrix;
    }

    
    std::unordered_map<SCIP_VAR*, int> varToIndex;
    for (int j = 0; j < nvars; ++j) {
      varToIndex[vars[j]] = j;
    }

    
    matrix.resize(nconss, std::vector<double>(nvars, 0.0));

    
    for (int i = 0; i < nconss; ++i) {
      SCIP_CONS* cons = conss[i];

      
      const char* consname = SCIPconsGetName(cons);

      
          
      int nconsvars = 0;
      SCIP_Bool success = FALSE;  

      
      SCIPgetConsNVars(scip, cons, &nconsvars, &success);

      if (success && nconsvars > 0) {
        
        SCIP_VAR** consvars = nullptr;
        SCIP_Real* conscoefs = nullptr;

        
        SCIP_CALL_ABORT(SCIPallocBlockMemoryArray(scip, &consvars, nconsvars));
        SCIP_CALL_ABORT(SCIPallocBlockMemoryArray(scip, &conscoefs, nconsvars));

        
        SCIPgetConsVars(scip, cons, consvars, nconsvars, &success);
        if (!success) {
          SCIPfreeBlockMemoryArray(scip, &conscoefs, nconsvars);
          SCIPfreeBlockMemoryArray(scip, &consvars, nconsvars);
          continue;
        }

        SCIPgetConsVals(scip, cons, conscoefs, nconsvars, &success);
        if (!success) {
          SCIPfreeBlockMemoryArray(scip, &conscoefs, nconsvars);
          SCIPfreeBlockMemoryArray(scip, &consvars, nconsvars);
          continue;
        }

        
        for (int k = 0; k < nconsvars; ++k) {
          SCIP_VAR* var = consvars[k];
          double coef = conscoefs[k];

          auto it = varToIndex.find(var);
          if (it != varToIndex.end()) {
            int colIdx = it->second;
            matrix[i][colIdx] = coef;
          }
          else {
          }
        }

        
        SCIPfreeBlockMemoryArray(scip, &conscoefs, nconsvars);
        SCIPfreeBlockMemoryArray(scip, &consvars, nconsvars);
      }
    }

    return matrix;
  }

  SCIP_RETCODE initialize();
  void resetModel(SCIP* scip) {

    
    
    
    
    
    

     /* Free all solution-related data */
    (SCIPfreeSolve(scip, TRUE));

    /* Reset all parameters to their default values */
    (SCIPfreeTransform(scip));


  }

  void resetModelPreserveSolution(SCIP* scip) {
    
    SCIPfreeSolve(scip, FALSE);  

    
    SCIPfreeTransform(scip);

    
    SCIP_SOL* bestsol_before = SCIPgetBestSol(scip);

    if (bestsol_before == NULL) {
      
      SCIPfreeSolve(scip, TRUE);
      SCIPfreeTransform(scip);
      printf("没有解可保留，完全重置\n");
      return;
    }

    
    SCIP_VAR** vars = SCIPgetVars(scip);
    int nvars = SCIPgetNVars(scip);      


    
    std::vector<double> saved_values(nvars);
    for (int i = 0; i < nvars; ++i) {
      saved_values[i] = SCIPgetSolVal(scip, bestsol_before, vars[i]);
    }

    
    SCIP_Real old_obj = SCIPgetSolOrigObj(scip, bestsol_before);

    
    SCIPfreeSolve(scip, FALSE);  
    SCIPfreeTransform(scip);

    
    SCIP_SOL* retained_sol = SCIPgetBestSol(scip);

    if (retained_sol != NULL) {
      
      printf("解被自动保留！目标值: %g\n",
        SCIPgetSolOrigObj(scip, retained_sol));

      
      bool values_match = true;
      for (int i = 0; i < nvars && values_match; ++i) {
        double retained_val = SCIPgetSolVal(scip, retained_sol, vars[i]);
        if (fabs(retained_val - saved_values[i]) > 1e-6) {
          values_match = false;
        }
      }

      if (!values_match) {
        printf("警告：保留的解值不匹配原始解\n");
      }
    }
    else {
      
      printf("解未被自动保留，正在手动添加...\n");

      
      SCIP_SOL* newsol;
      (SCIPcreateSol(scip, &newsol, NULL));

      
      for (int i = 0; i < nvars; ++i) {

        if (saved_values[i] < 0.1) {
          saved_values[i] = 0.1;
        }

        (SCIPsetSolVal(scip, newsol, vars[i], saved_values[i]));
      }

      
      unsigned int stored;  
      (SCIPaddSol(scip, newsol, &stored));

      if (stored) {
        printf("手动添加成功！目标值: %g\n", old_obj);
      }
      else {
        printf("手动添加失败！\n");
      }

      (SCIPfreeSol(scip, &newsol));
    }

    
    //SCIPfreeBufferArray(scip, &vars);


  }


  /** includes event handler for best solution found */
  SCIP_RETCODE SCIPincludeEventHdlrBestsol(
    SCIP* scip                /**< SCIP data structure */
  );
  
  bool isInitialized() const {
    return initialized_;
  }


private:
  SCIP* scip_;
  std::vector<SCIP_VAR*> vars_;  
  std::unordered_map<std::string, Var2D> var2d_map_; 
  bool initialized_;

  void ensurePrefixUnique(const std::string& prefix);
  std::string generateVarName(const std::string& prefix, int index1 = -1, int index2 = -1, int index3 = -1) const;
  //std::string generateVarName(const std::string& prefix, int index1 = -1, int index2 = -1, int index3 = -1) const;

};

