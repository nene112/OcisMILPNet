#ifndef INCLUDE_OCIS_SCIP
#define INCLUDE_OCIS_SCIP
#include "scip/scip.h"
#include "scip/scipdefplugins.h"
#include "scip/scip_sol.h"       // 包含解相关函数

#include "statistics.h"
#include "FileInteraction.h"
#include "edges_utils.h"
#include "ocis_MILP_scip_raii_wrapper.h"

class OCIS_scip {

public:
  string dirpath = "../../data/ws/";
  string mesh_path;

  struct water_allocation_param {
    string dirpath;
    std::vector<string> turnouts_names;
    std::vector<double> q_vec;
    std::vector<double> s_vec;
    std::vector<double> e_vec;
    std::vector<double> stake_vec;
    std::vector<double> delaytime_vec;
    double inflow_max;
    double inflow_start_hr;
    double time_inflow_min_max_delta;

    time_t start_time_t;

    double T;
    string schedule_json_output_path;
    std::map<string, std::vector<string>> summary_dynamic_border;

    double start_time_weight =0.9;//越大越偏向delay
    std::vector<string> bc_Flow;

    Ocis_edges edges_utils;
    json hd_result;

    double sim_dx = 50;
    string sim_inpath = "./";
    string sim_outpath = "./";
  };
  struct MILP_result {
    string dirpath;
    std::vector<string> turnouts_names;
    //std::vector<double> q_vec;
    //std::vector<double> s_vec;
    //std::vector<double> e_vec;
    //std::vector<double> stake_vec;
    //std::vector<double> delaytime_vec;
    double inflow_max;
    double inflow_start_hr;
    double time_inflow_min_max_delta;

    time_t start_time_t;

    double T;
    string schedule_json_output_path;
    std::map<string, std::vector<string>> summary_dynamic_border;

    double start_time_weight =0.9;//越大越偏向delay
    std::vector<string> bc_Flow;

    Ocis_edges edges_utils;
    json hd_result;

    int find_optimal_solution_label = 0;
    std::map<string, std::map<string, std::vector<double>>> solution;

  };



  struct ScheduleScheme {
    int s = 0;
    int e = 0;
    int duration=0;
    double q = 0;
  };


  struct MILP_param {

    string dirpath = "./";
    string input_path = "./";
    Ocis_edges topo_dicts;
    int mode_leakage_inversion = -1;
    int workTime_L = 3600 * 0;//一日时间步中的工作日时间
    int workTime_R = 3600 * 24;//一日时间步中的工作日时间

    int num_pools;
    int num_pools_decision;
    int num_vars;
    int num_vars_decision;
    int time_vars = 24;
    std::vector<double> maxFlow;
    std::vector<double> minFlow;
    std::vector<std::vector<double>> massMatrix;
    int dt = 3600;
    int T = 24;
    std::vector<double> As;
    std::map<string, double> hTarget;
    std::map<string, double> Stage_ic;
    std::map<string, double> Flow_ic;
    std::map<string, double> Stage_target;
    string outputpath;
    std::vector<string> obj;
    std::map<string, int> obj_name_nindex;
    std::vector<string> poolName;

    string time_start;
    time_t time_start_t;
    string rolling_time_start;
    time_t rolling_time_start_t;
    // Work-time window ("HH:MM", e.g. "08:00" ~ "18:00"). When both are
    // configured and the input start_time falls outside the window, the
    // start_time is snapped to the work-time start of the same day.
    string work_time_start;
    string work_time_end;

    std::map<string, std::vector<ScheduleScheme>> gates_ScheduleScheme;
    std::map<string, std::vector<int>> UDCheck;//预期配水时间步骤
    std::map<int, std::vector<int>> UDCheck_id_map;//预期配水时间步骤

    double hr_dt = 3600;
    double daily_dt = 86400;

    std::map<string, double> pool_hMax;
    std::map<string, double> pool_hMin;
    std::map<string, double> pool_Width;
    std::map<string, double> pool_Length;
    std::map<string, double> pool_m;
    std::map<string, double> stake;
    std::map<string, double> demandFlow;
    std::map<string, double> bc_Stage;
    std::map<string, double> delay_time;
    std::map<string, double> bc_Flow;
    std::map<string, double> controlGatesNames;
    std::vector< double> Delta_Stage_weight;
    std::vector< double> Stage_target_weight;
    std::vector< double> actions_weights;
    std::vector< double> Delta_actions_weights;
    std::vector< double> actionsGap_weights;
    std::vector< double> WGap_weights;
    std::vector< double> gates_min_flow;
    std::vector< double> gates_max_flow;
    std::vector< double> gates_max_W;
    std::vector< double> gates_max_duration;
    std::vector< double> gates_stake;
    process_T boundary_flow;
    string boundary_flow_string;
    process_T boundary_stage;
    process_T stage_error;
    process_T boundary_stage2;


    string boundary_stage_string;
    int boundary_stage_cons_label = -1;
    int W_based_optimalAllocation = -1;
    int ref_flow_label = -1;
    int MultiDays_delaytime = -1;
    int MILP_Inversion_label = -1;


    string optimal_obj = "optimal";

    double x_weight = 1.1;       //最小化x(Q)的权重
    double QGap_weight = 1.1;       //最小化Q与目标值的偏差
    double dQ_weight = 1.;       //最小化Q与目标值的偏差
    double flow_weight = -1;
    double stage_weight = -1;
    double fabs_dx_weight = 10;  // 流量变幅 权重
    double fabs_dy_weight = 10;  // 水位变幅 权重
    double fabs_boundary_dx_weight = 100;//流量边界条件权重
    double fabs_boundary_dy_weight = 100;
    double z_weight = 100;

    double free_turnouts_Label = 1;
    double fixed_designFlow = -1;
    int dec_c_label = -1;
    int dec_f_label = -1;
    int hard_f_label = -1;
    int fixed_y = -1;
    int flood_control_label = 0;
    string demand_source = "series";
    // Measured upstream inflow volume over the planning horizon [m3], preserved
    // before ActionDemand mutates boundary_flow under demand_source=duration.
    double inflow_volume_m3 = -1;
    // Cap delivered volume at max_supply_demand_ratio * W_input per turnout.
    // Adjustable before optimize (OCIS_dataConfig / CalculationParams); default 1.0.
    double max_supply_demand_ratio = 1.0;
    // Minimum delivered flow of an opened demand gate, as a fraction of the
    // requested demand flow (0.5 = at least 50%). A gate that cannot reach
    // this threshold is kept closed and the available water is reallocated to
    // higher-priority gates (grain-to-cash ratio up, irrigation progress down).
    // Adjustable via CalculationParams.min_open_delivery_ratio; default 0.5.
    double min_open_delivery_ratio = 0.5;
    // Hard limit on the period-to-period |dQ| change of reach/regulating
    // flows (m3/s), applied to the source release and branch-head through-flow
    // in both master and SP. Prevents spikes and end-of-horizon flow dumps
    // caused by steep observed supply ramps / cumulative through-flow targets.
    // 0 = disabled. Adjustable via CalculationParams.reach_ramp_max.
    double reach_ramp_max = 8.0;
    // Maximum number of period-to-period flow changes allowed for each type-2
    // demand gate in the readable LBBD scheduler. Negative means no hard cap.
    int max_action_changes = -1;
    // Minimum requested number of flow changes. It is normalized to remain
    // strictly below max_action_changes; negative means disabled.
    int min_action_changes = -1;
    std::map<string, int> gate_min_action_changes;
    std::map<string, double> gate_delta_q_max_ratio;
    // If positive, export gate actions as this many staircase segments and let
    // the scheduler choose change points automatically.
    int step_breakpoints = -1;
    // Minimum number of consecutive periods a delivery block must stay open
    // (min-uptime), avoiding fragmented 2-3 step gate flows. 1 = no limit.
    int min_delivery_steps = 1;
    // Default flow-step cap for all turnout gates (type == 2): maximum number
    // of flow steps allowed, enforced as <= 2 * max_flow_steps period-to-
    // period |dQ| transitions in the detailed SP. Per-gate obs.max_flow_steps
    // takes precedence; -1 disables the type-wide default.
    int turnout_max_flow_steps = -1;
    // Turnout-flow operating mode for readable Exact LBBD:
    // 0 = variable flow while open; 1 = one strict constant flow setting over
    // the complete contiguous open interval. Default 1 preserves the current
    // operating rule. Configure via CalculationParams.turnout_constant_flow_mode.
    int turnout_constant_flow_mode = 1;
    // Optional primal heuristic for Exact LBBD. It never supplies a bound or
    // termination status; it only creates warm starts and harvests cuts via
    // the same exact hydraulic SP/Farkas generator. Disabled by default.
    int fast_master_enable = 0;
    // Heuristic-only execution mode: run Fast Master + hydraulic SP and skip
    // Exact Master entirely. Such a result is never marked exact/optimal.
    int fast_master_only = 0;
    int fast_master_num_increments = 20;
    int fast_master_candidates_per_exact_solve = 5;
    // Experimental: inject Farkas cuts harvested from heuristic candidates.
    // Off by default because many early cuts may delay primal discovery.
    int fast_master_harvest_cuts = 0;
    double fast_master_initial_increment_ratio = 0.05;
    double fast_master_min_increment_ratio = 0.0025;
    int fast_master_max_iterations = 100;
    int fast_master_max_repair_iterations = 20;
    double fast_master_time_limit_seconds = 120.0;
    int fast_master_modes_per_task = 10;
    int fast_master_mode_expansion_count = 5;
    double fast_master_service_weight = 1.0;
    double fast_master_congestion_weight = 1.0;
    double fast_master_hydraulic_feedback_weight = 1.0;
    double fast_master_default_kappa = 1.0;
    int fast_master_use_nonlinear_service_benefit = 1;
    int fast_master_use_previous_solution = 1;
    int fast_master_use_dual_congestion = 0;
    int fast_master_use_farkas_feedback = 1;
    int fast_master_adaptive_increment = 1;
    // 0 = off (default): zone flow upper bound is the gate capacity.
    // 1 = on: cap each zone's optimization flow at the maximum demand flow
    // (max of the demand profile / waterDemand q), i.e. never deliver above
    // the requested flow.
    int demand_flow_upper_label = 0;
    // Allocation priority direction: 0 = upstream reaches first,
    // 1 = balanced (no spatial bias), 2 = downstream reaches first.
    // Implemented by scaling each zone's service priority by its topological
    // position (travel delay from the root reach).
    int allocation_priority_label = 0;
    // Enable the scheduling-layer Bellman threshold policy. The policy first
    // evaluates the normal LBBD baseline, computes staged Bellman allocation
    // targets, and validates every target through the existing hydraulic LBBD.
    int lbbd_threshold_label = 0;
    // Minimum aggregate satisfaction retained for the low-priority group.
    double lbbd_threshold_min_low_satisfaction = 0.20;
    // Bellman discount alpha relative to the per-demand arrival rate.
    double lbbd_threshold_discount_rate = 0.10;
    // Demand/resource discretization used by the exact Bellman recursion.
    int lbbd_threshold_resource_units = 36;
    // Number of adjacent marginal-reward classes used by Bellman (1..4).
    int lbbd_threshold_class_count = 4;
    // Linear marginal-reward reduction from early to later demand units.
    double lbbd_threshold_marginal_reward_decay = 0.50;
    // Priority attributes (irr_progress, grain_to_cash_crop_ratio) keyed by
    // gate/station name, loaded from input/object_priority_overrides.json.
    // The readable LBBD scheduler builds zone priority from these values
    // (higher grain-to-cash ratio and lower irrigation progress rank higher).
    std::map<string, std::pair<double, double>> priority_attrs;
    // Output time step (seconds) for action.csv. When > 0 the series is
    // densified to this resolution over [time_start, time_start + T*3600]
    // even though the optimization uses dt = MILP_dt. 0 = use dt as before.
    int output_action_dt = 0;
    // Optional management-unit aggregation. When the units file exists and
    // aggregation_level > 0, readable LBBD optimizes only that hierarchy level
    // and disaggregates the result back to leaf units.
    int aggregation_level = 2;
    string aggregation_units_path = "input/encode_units.csv";
    // Consecutive flows within this tolerance [m3/s] are treated as unchanged
    // when max_action_changes is active.
    double flow_change_tolerance = 1.0e-4;
    // Wall-clock limits for the readable LBBD master solves, in seconds.
    // A non-positive value means "no time limit"; rely on limits/gap instead.
    double master_time_limit = 30.0;
    double duration_master_time_limit = 300.0;
    // SCIP parallel settings. Defaults deliberately force >=2 threads so
    // parallel branch-and-bound is active when the instance has branching work.
    int scip_max_threads = 4;
    int scip_min_threads = 4;
    int scip_parallel_mode = 0;
    double scip_gap = 1.0e-6;
    int scip_print_statistics = 0;


    json dataConfig;
  };
  MILP_param input_params;
  MILP_result result;

  void get_value(MILP_result& result, string var_name, string name,int j,std::vector<double>& vec);


  MILP_param MILP_read_input(string dirpath,string edges_path,string inputjson_path);
  int MILP_ocis_solver_test(MILP_param& input_param) ;
  void MILP_write_output(MILP_param& input_param,MILP_result& result,string dirpath,string filename);
  void MILP_write_output(MILP_param& input_param,MILP_result& result,string dirpath);
  void MILP_write_action(MILP_param& input_param,MILP_result& result,string dirpath);
  
  nlohmann::ordered_json MILP_write_schedule_MultiDays(MILP_param& input_param,MILP_result& result,string dirpath);         //只写schedule_MultiDays.json,被MILP_write_output_MultiDays()调用
  void MILP_write_output_MultiDays(MILP_param& input_param,MILP_result& result,string dirpath);                             // schedule_MultiDays.json 和 action.csv都写
  void MILP_write_RMGateSchedule(MILP_param& input_param,MILP_result& result,string dirpath,string filename);               //兼容老版本 RMGateSchedule.json   
  void MILP_write_csv(MILP_param& input_param,MILP_result& result,string dirpath,string var_name);
  void MILP_write_csv_pools(MILP_param& input_param,MILP_result& result,string dirpath,string var_name);
  void MILP_write_csv_pools(MILP_param& input_param,MILP_result& result,string dirpath,string var_name,string suffix);
  void MILP_write_schedule(MILP_param& input_param,MILP_result& result,string dirpath);

  void Split_mesh();

  std::map<string, std::vector<double>> get_max_action(std::map<string, std::vector<double>>& data);

  nlohmann::json MILP_get_schedule_json(MILP_param& input_param,MILP_result& result,string dirpath);
  nlohmann::ordered_json get_gates_schedule_scheme_by_pool(MILP_param& input_param, MILP_result& result, Ocis_edges::Point* g,Ocis_edges::Point* g_source,int pool_id);
  nlohmann::ordered_json get_gates_schedule_scheme_by_pool_dynamic(MILP_param& input_param, MILP_result& result, Ocis_edges::Point* g,Ocis_edges::Point* g_source,int pool_id);
  nlohmann::ordered_json get_gates_schedule_scheme_by_pool_MultiDays(
    MILP_param& input_param,
    MILP_result& result,
    Ocis_edges::Point* g,
    Ocis_edges::Point* g_source,
    int pool_id,
    h_csv& tidy_data
  );
  void get_recursion_accumulated_STime(double& STime,MILP_param& input_param, MILP_result& result, int i,int j,int pool_id);
  void get_recursion_accumulated_stake(double& STime,MILP_param& input_param,  int i,int pool_id);
  void batch_get_recursion_accumulated_stake(MILP_param& input_param);
  
  void update_h(
    MILP_param& input_param,
    std::map<string, std::map<string, std::vector<double>>>& solution
  );
  void Summary_dyunamic_border(
    MILP_param& input_param,
    std::map<string, std::map<string, std::vector<double>>>& solution
  );
  void add_sum_t_cons(SCIPModel& model, SCIPModel::Var2D& var, SCIPModel::Var1D& var_sum,
    int row_num, int col_num, string cons_name);
  void add_sum_t_cons(SCIPModel& model, double a, SCIPModel::Var2D& var, SCIPModel::Var1D& var_sum,
    int row_num, int col_num, string cons_name);
  void add_sum_t_cons(SCIPModel& model, std::vector<double> max_vec, SCIPModel::Var2D& var, SCIPModel::Var1D& var_sum,
    int row_num, int col_num, string cons_name);

  MILP_result MILP_ocis_solver(MILP_param& input_param);
  MILP_result MILP_ocis_solver_leakageInversion(MILP_param& input_param);
  MILP_result MILP_ocis_solver_waterAllocation(MILP_param& input_param);
  MILP_result MILP_ocis_solver_LP_PathwaysPlanning(MILP_param& input_param);
  MILP_result MILP_ocis_solver_LP_PathwaysPlanning_hybrid(MILP_param& input_param);
  MILP_result MILP_ocis_solver_LP_PathwaysPlanning_sparsity(MILP_param& input_param);
  MILP_result MILP_ocis_solver_LP_PathwaysPlanning_statusCheck(MILP_param& input_param);
  MILP_result MILP_ocis_solver_LP_PathwaysPlanning_Allrules(MILP_param& input_param);
  MILP_result MILP_ocis_solver_LP_PathwaysPlanning_Allrules_Inversion(MILP_param& input_param);
  MILP_result MILP_ocis_solver_LP_PathwaysPlanning_Inversion(MILP_param& input_param);
  MILP_result MILP_ocis_solver_LP_PathwaysPlanning_Inversion1(MILP_param& input_param);
  MILP_result MILP_ocis_solver_LP_PathwaysPlanning_Hierarchical(MILP_param& input_param);
  MILP_result MILP_ocis_solver_LP_PathwaysPlanning_L2(MILP_param& input_param);
  MILP_result MILP_ocis_solver_longTerm(MILP_param& input_param);
  MILP_result MILP_ocis_solver_flood(MILP_param& input_param);
  MILP_result MILP_ocis_solver_hardcons(MILP_param& input_param);
  MILP_result MILP_ocis_solver_sve(MILP_param& input_param);
  std::map<string, double> MILP_ocis_solver_step(MILP_param& params, std::map<string, double> state_demand, std::map<string, double> state_stage, std::map<string, double> state_action);

  void  MutualCoherence(const std::vector<std::vector<double>>& matrix);

  void set_UDCheck(MILP_param& input_param);
  double get_today_UQ(MILP_param& input_param, int day_nindex, int gates_vec_id);
  double get_UQ(MILP_param& input_param, int day_nindex, int gates_vec_id);
  double get_Step_STimeTarget(MILP_param& input_param, int day_nindex, int gates_vec_id);
  double get_Step_ETimeTarget(MILP_param& input_param, int day_nindex, int gates_vec_id);

  double get_Step_STimeTarget_g(MILP_param& input_param, int day_nindex, Ocis_edges::Point* g);
  double get_Step_ETimeTarget_g(MILP_param& input_param, int day_nindex, Ocis_edges::Point* g);

  double get_Step_obs_STime(MILP_param& input_param, int day_nindex, Ocis_edges::Point* g);


  MILP_result MILP_ocis_scheduler(MILP_param& input_param) ;
  MILP_result MILP_ocis_scheduler_longTerm(MILP_param& input_param) ;
  MILP_result MILP_ocis_scheduler_dynamic(MILP_param& input_param) ;
  MILP_result MILP_ocis_scheduler_dynamic_MultiDays(MILP_param& input_param) ;
  MILP_result MILP_ocis_scheduler_dynamic_MultiDays_BD(MILP_param& input_param) ;
  MILP_result MILP_ocis_scheduler_dynamic_MultiDays_Tree(MILP_param& input_param) ;
  MILP_result MILP_ocis_scheduler_dynamic_MultiDays_Tree_dyd(MILP_param& input_param) ;
  MILP_result MILP_ocis_scheduler_dynamic_MultiDays_Tree_dyd_lbbd(MILP_param& input_param) ;
  MILP_result MILP_ocis_scheduler_dynamic_MultiDays_Tree_dyd_lbbd_core(MILP_param& input_param) ;
  MILP_result MILP_ocis_scheduler_dynamic_MultiDays_consider_y(MILP_param& input_param) ;
  MILP_result MILP_Hierarchical_levels(MILP_param& input_param) ;
  MILP_result MILP_ocis_scheduler_dynamic_MultiDays_consider_y_boundaryFlow(MILP_param& input_param) ;
  void add_min_Gap_cons(SCIPModel& model,double a ,SCIP_VAR* var,SCIP_VAR* var_gap,MILP_param& input_param,int gates_nindex,int time_nindex,double target_value,string cons_name);
  void add_min_Gap_cons(SCIPModel& model,SCIP_VAR* var,SCIP_VAR* var_gap,MILP_param& input_param,int gates_nindex,int time_nindex,double target_value,string cons_name);
  void add_min_Gap_cons(SCIPModel& model,SCIP_VAR* base_var,SCIP_VAR* var,SCIP_VAR* var_gap,MILP_param& input_param,int gates_nindex,int time_nindex,double target_value,string cons_name);
  void add_min_Gap_cons(SCIPModel& model,SCIP_VAR* var1,SCIP_VAR* var2,SCIP_VAR* var_gap,MILP_param& input_param,int gates_nindex,int time_nindex,string cons_name);
  void add_min_Gap_with_CheckLabel_cons(SCIPModel& model,SCIP_VAR* var1,SCIP_VAR* var2,SCIP_VAR* var_gap,SCIP_VAR* check_label,MILP_param& input_param,
    int gates_nindex,int time_nindex,string cons_name);

  void balance_check(MILP_param& input_param);

  std::vector<string> calculation_kpis(string h_csv_path,MILP_param& input_param);

  void add_min_Gap_with_CheckLabel_cons(
    SCIPModel& model,
    SCIP_VAR* var1,
    double  target_value,
    SCIP_VAR* var_gap,
    SCIP_VAR* check_label,
    MILP_param& input_param,
    int gates_nindex,
    int time_nindex,
    string cons_name
  );

  void add_status_check_cons(SCIPModel& model,SCIP_VAR* var,SCIP_VAR* var_gap,int gates_nindex,int time_nindex,string cons_name);
  void add_status_check_cons_threshold(SCIPModel& model,double threthold,SCIP_VAR* var,SCIP_VAR* var_gap,int gates_nindex,int time_nindex,string cons_name);
  void add_status_check_inc_cons(SCIPModel& model,SCIP_VAR* var,SCIP_VAR* var_gap,int gates_nindex,int time_nindex,string cons_name);
  void add_status_check_dec_cons(SCIPModel& model,SCIP_VAR* var,SCIP_VAR* var_gap,int gates_nindex,int time_nindex,string cons_name);

  void assignment(SCIPModel& model,SCIP_VAR* x,double value,string var_label) ;

  void add_logic_and_cons(
    SCIPModel& model,
    SCIP_VAR* condition1,
    SCIP_VAR* condition2,
    double c,
    SCIP_VAR* and_label,
    int gates_nindex,
    int time_nindex,
    string cons_name
  );

  void add_delta_variable_cons(SCIPModel& model, SCIP_VAR* var_pre,SCIP_VAR* var_next, SCIP_VAR* var_delta, int gates_nindex, int time_nindex, string cons_name);
  void add_fabs_cons(
    SCIPModel& model,
    SCIP_VAR* var,
    SCIP_VAR* var_fabs,
    int row,
    int col,
    string cons_name
  );
  void add_greaterOrEqual_cons(
    SCIPModel& model,
    double var_coef,
    SCIP_VAR* var,
    SCIP_VAR* var_mid,
    int row,
    int col,
    string cons_name
  );

  void add_greaterOrEqual_cons(
    SCIPModel& model,
    SCIP_VAR* var,
    SCIP_VAR* var_mid,
    int row,
    int col,
    string cons_name
  );

  void add_Equal_cons(
    SCIPModel& model,
    double var_coef,
    SCIP_VAR* var,
    double var_mid_coef,
    SCIP_VAR* var_mid,
    int row,
    int col,
    string cons_name
  );
  void add_Equal_cons(
    SCIPModel& model,
    double var_coef,
    SCIP_VAR* var,
    double var_mid_coef,
    SCIP_VAR* var_mid,
    double epsilon,
    int row,
    int col,
    string cons_name
  );
  void add_Equal_1_cons(
    SCIPModel& model,
    SCIP_VAR* var,
    SCIP_VAR* var_mid,
    int row,
    int col,
    string cons_name
  );

  void add_Equal_cons(
    SCIPModel& model,
    double var_coef,
    SCIP_VAR* var,
    double value,
    int row,
    int col,
    string cons_name
  );
  void add_Equal_cons(
    SCIPModel& model,
    double var_coef,
    SCIP_VAR* var,
    double value,
    double epsilon,
    int row,
    int col,
    string cons_name
  );

  void add_Equal_cons(
    SCIPModel& model,
    double var_coef,
    SCIP_VAR* var,
    double value,
    double epsilon_l,
    double epsilon_r,
    int row,
    int col,
    string cons_name
  );



  void add_integration_cons(
    SCIPModel& model,
    MILP_param& input_param,
    double var_coef,
    SCIPModel::Var2D& Q,
    SCIPModel::Var1D& W,
    string type,
    string cons_name
  );

  void add_McCormickEnvelope_cons(
    SCIPModel& model,
    MILP_param& input_param,
    SCIP_VAR* Q,              //x
    SCIP_VAR* duration,       //y
    SCIP_VAR* W,              //W
    int i,
    int j,
    string cons_name
  );

  void add_Gates_hydraulic_cons(
    SCIPModel& model,
    MILP_param& input_param,
    SCIPModel::Var2D& Q,
    SCIPModel::Var2D& e,
    SCIPModel::Var2D& hu,
    string cons_name
  );

  void add_FlowBalance_solutionCheck(
    SCIPModel& model,
    MILP_param& input_param,
    SCIPModel::Var2D& Q,
    SCIPModel::Var2D& y,
    bool consider_y,
    string cons_name
  );
  void add_FlowBalance_internalModel_Inversion_cons(
    SCIPModel& model,
    MILP_param& input_param,
    SCIPModel::Var2D& Q,
    SCIPModel::Var2D& y,
    bool consider_y,
    string cons_name
  );

  void add_FlowBalance_internalModel_residual_cons(
    SCIPModel& model,
    MILP_param& input_param,
    SCIPModel::Var2D& Q,
    SCIPModel::Var2D& y,
    bool consider_y,
    string cons_name
  );

  void add_FlowBalance_internalModel_cons(
    SCIPModel& model,
    MILP_param& input_param,
    SCIPModel::Var2D& Q,
    SCIPModel::Var2D& y,
    bool consider_y,
    string cons_name
  );
  void add_FlowBalance_internalModel_delay_cons(
    SCIPModel& model,
    MILP_param& input_param,
    SCIPModel::Var2D& Q,
    SCIPModel::Var2D& y,
    bool consider_y,
    string cons_name
  );

  void add_FlowBalance_internalModels_inverseflow_cons(
    SCIPModel& model,
    MILP_param& input_param,
    SCIPModel::Var2D& Q,
    SCIPModel::Var2D& y,
    SCIPModel::Var2D& adjacency_dy,
    SCIPModel::Var2D& adjacency_dy_Check,
    bool consider_y,
    string cons_name
  );

  void add_BiggerFlow_internalModel_cons(
    SCIPModel& model,
    MILP_param& input_param,
    SCIPModel::Var2D& Q,
    SCIPModel::Var2D& y,
    bool consider_y,
    string cons_name
  );

  void add_BiggerFlow_oneSource_internalModel_cons(
    SCIPModel& model,
    MILP_param& input_param,
    SCIPModel::Var2D& Q,
    SCIPModel::Var2D& y,
    bool consider_y,
    string cons_name
  );

  void add_BiggerFlow_dynamic_internalModel_cons(
    SCIPModel& model,
    MILP_param& input_param,
    SCIPModel::Var2D& Q,
    string cons_name
  );
  void add_EqualFlow_dynamic_internalModel_cons(
    SCIPModel& model,
    MILP_param& input_param,
    SCIPModel::Var2D& Q,
    string cons_name
  );
  void add_EqualFlow_dynamic_internalModel_cons(
    SCIPModel& model,
    MILP_param& input_param,
    SCIPModel::Var2D& Q,
    double row,
    double col,
    string cons_name
  );


  void add_BiggerFlow_internalModel_cons(
    SCIPModel& model,
    MILP_param& input_param,
    SCIPModel::Var2D& Q,
    bool consider_y,
    string cons_name
  );

  void add_path_to_top_node_STime_cons(
    SCIPModel& model,
    MILP_param& input_param,
    Ocis_edges::Point* g,
    SCIPModel::Var2D& STime,
    double tau_ratio,
    SCIPModel::Var2D& tau,
    SCIPModel::Var2D& DLTime,
    int pool_i,
    int time_j,
    string cons_name
  );

  void add_pre_STime(
    SCIPModel& model,
    MILP_param& input_param,
    Ocis_edges::Point* g,
    SCIPModel::Var2D& STime,
    SCIPModel::Var2D& tau,
    SCIPModel::Var2D& DLTime,
    int pool_i,
    int time_j,
    string cons_name
  );

  void add_greaterOrEqual_cons(
    SCIPModel& model,
    double value,
    SCIP_VAR* var,
    int row,
    int col,
    string cons_name);

  void add_lessOrEqual_cons(
    SCIPModel& model,
    double value,
    SCIP_VAR* var,
    int row,
    int col,
    string cons_name);

  void add_var_add_var_Equal_var_cons(
    SCIPModel& model,
    SCIP_VAR* var1,
    SCIP_VAR* var2,
    SCIP_VAR* sum_var,
    int row,
    int col,
    string cons_name
  );
  void add_var_add_var_Greater_var_cons(
    SCIPModel& model,
    SCIP_VAR* var1,
    SCIP_VAR* var2,
    SCIP_VAR* sum_var,
    int row,
    int col,
    string cons_name
  );
  void add_var_add_var_Greater_var_cons(
    SCIPModel& model,
    SCIP_VAR* var1,
    SCIP_VAR* var2,
    double constant,
    SCIP_VAR* sum_var,
    int row,
    int col,
    string cons_name
  );

  void add_var1_plus_b_Equal_varr_cons(
    SCIPModel& model,
    SCIP_VAR* var1,
    double b,
    SCIP_VAR* varr,
    int row,
    int col,
    string cons_name
  );

  void add_var_add_var_Equal_var_cons(
    SCIPModel& model,
    SCIP_VAR* var1,
    SCIP_VAR* var2,
    double constant,
    SCIP_VAR* sum_var,
    int row,
    int col,
    string cons_name
  );


  void add_linear_cdot_Equal_var_cons(
    SCIPModel& model,
    double a,
    SCIP_VAR* var,
    SCIP_VAR* righthand_var,
    int row,
    int col,
    string cons_name
  );


  void add_calculate_V_cons(
    SCIPModel& model,
    MILP_param& input_param,
    SCIPModel::Var2D& Q,
    SCIPModel::Var2D& y,
    SCIPModel::Var2D& V,
    SCIPModel::Var2D& Q_segments_label,
    SCIPModel::Var2D& y_segments_label,
    int row,
    int col,
    string cons_name);

  void add_statuscheck_segs_cons(
    SCIPModel& model,
    MILP_param& input_param,
    SCIPModel::Var2D& var,
    SCIPModel::Var3D& y_segments_label,
    SCIPModel::Var2D& y_segments_label_nindex,
    std::vector<double> x,
    int segment_num,
    string cons_name
  );


  void add_calculate_reciprocal_pools_cons(
    SCIPModel& model,
    MILP_param& input_param,
    SCIP_VAR* var,
    SCIP_VAR* var_reciprocal,
    SCIPModel::Var3D& y_segments_label,
    SCIP_VAR* y_segments_label_nindex,
    std::vector<double> x,
    std::vector<std::vector<double> > ab,
    int row,
    int col,
    int segment_num,
    string cons_name
  );
  void add_calculate_reciprocal_pools_cons(
    SCIPModel& model,
    MILP_param& input_param,
    SCIP_VAR* var,
    SCIP_VAR* var_reciprocal,
    double var_reciprocal_coef,
    SCIPModel::Var3D& y_segments_label,
    SCIP_VAR* y_segments_label_nindex,
    std::vector<double> x,
    std::vector<std::vector<double> > ab,
    int row,
    int col,
    int segment_num,
    string cons_name
  
  );

  void add_bilinear_operation_pools_2D_cons(
    SCIPModel& model,
    MILP_param& input_param,
    SCIP_VAR* x,
    SCIP_VAR* y,
    SCIP_VAR* z,
    double z_coef,
    SCIPModel::Var3D& x_segments_label,
    SCIPModel::Var3D& y_segments_label,
    SCIPModel::Var3D& xy_segments_label,
    SCIP_VAR* x_segments_label_nindex,
    SCIP_VAR* y_segments_label_nindex,
    SCIPModel::Var3D& xy_segments_label_nindex,
    Ocis_edges::Piece_wise2D& piece_wise_surface,
    int row,
    int col,
    int segment_num_x,
    int segment_num_y,
    string cons_name
  );



  void add_piece_wise_1D_check_cons(
    SCIPModel& model,
    SCIP_VAR* var,
    SCIP_VAR* y_segments_label,
    double xL,
    double xU,
    int row,
    int col,
    int s,
    string cons_name
  
  );


  void add_piece_wise_1D_func_cons(
    SCIPModel& model,
    SCIP_VAR* var,
    SCIP_VAR* var_func,
    SCIP_VAR* y_segments_label,
    std::vector<double> ab,
    int row,
    int col,
    int s,
    string cons_name
  
  );


  void add_piece_wise_1D_func_cons(
    SCIPModel& model,
    SCIP_VAR* var,
    SCIP_VAR* var_func,
    double var_reciprocal_coef,
    SCIP_VAR* y_segments_label,
    std::vector<double> ab,
    int row,
    int col,
    int s,
    string cons_name
  
  );

  void add_piece_wise_2D_func_cons(
    SCIPModel& model,
    SCIP_VAR* x,
    SCIP_VAR* y,
    SCIP_VAR* z,
    double z_coef,
    SCIP_VAR* x_segments_label,
    SCIP_VAR* y_segments_label,
    SCIPModel::Var3D& xy_segments_label,
    std::vector<std::vector<std::vector<double>>> abc,
    int row,
    int col,
    int s_x,
    int s_y,
    int s_x_num,
    int s_y_num,

    string cons_name
  );


  void add_sum_label_cons(
    SCIPModel& model,
    SCIPModel::Var3D& y_segments_label,
    SCIP_VAR* y_segments_label_nindex,
    int row,
    int col,
    int segment_num,
    string cons_name
  
  );

  void del_cons_byName(
    SCIPModel& model,
    string cons_name
  );


  std::vector<double> piecewise(double min, double max, double segment_num);
  std::vector<std::vector<double>> piecewise_cdot_2D(std::vector<double>& x,std::vector<double>& y);
  std::vector<std::vector<double>> piecewise_get_V_plus_celerity_reciprocal_2D(std::vector<double>& x,std::vector<double>& y);
  std::vector<double> piece_wise_V_add_celerity_reciprocal_1D(std::vector<double>& x,double c);

  std::vector<std::vector<double>> piecewise_approximation_1D(std::vector<double> segments,std::vector<double> function);

 std::vector<std::vector<std::vector<double>>> piecewise_approximation_2D(
    std::vector<double> x,             
    std::vector<double> y,            
    std::vector<std::vector<double>> z_func 
  );




  std::vector<double> piecewise_get_square(std::vector<double> segments);
  std::vector<double> piecewise_get_celerity(std::vector<double> segments);
  std::vector<double> piecewise_get_reciprocal(std::vector<double> segments);
  std::vector<double> piecewise_get_reciprocal(MILP_param& input,std::vector<double> segments,double coef);
  std::vector<double> piecewise_get_reciprocal(std::vector<double> segments,double coef);

  double get_g2g_Length(Ocis_edges::Point* g,Ocis_edges::Point* gt);
  double get_g2g_Length(Ocis_edges::Point* g,Ocis_edges::Point* gt,double& length,int& g_num);
  process_T get_waterDemand_from_input_json(json obs_json,MILP_param& input);
  void get_waterDemand(json obs_json,MILP_param& input);
  void get_obs(json obs_json,MILP_param& input);
  void get_ReTime(json obs_json,MILP_param& input);


 std::map<string,double>  get_turnouts_mile_stage(MILP_param& input,Ocis_edges::Edges* pool, Ocis_edges::Point* g_source);
 std::map<string,double>  get_source_mile_stage(MILP_param& input,Ocis_edges::Edges* pool, Ocis_edges::Point* g_source);
 std::map<string,double>  get_targets_mile_stage(MILP_param& input,Ocis_edges::Edges* pool, Ocis_edges::Point* g_source);
  
  void set_bcFlow(json bc_json,MILP_param& input);
  void set_bcStage(json bc_json,MILP_param& input);
  void set_soft_bcFlow_cons(json bc_json,MILP_param& input);
  void set_obs_STime(process_T STime,MILP_param& input);
  void set_obs_Q(process_T STime,MILP_param& input);
  void set_W(process_T W,MILP_param& input);

  void set_demand_from_boundary_flow(MILP_param& input);
  void update_demandFlow( MILP_param& input);

  private:

  class GateSolutionPrinter {
  public:
    using Formatter = std::function<void(std::ostream&, SCIP*, int, SCIP_VAR**)>;

    // 注册要显示的字段
    void addField(const std::string& name, Formatter formatter, SCIP_VAR** var_array = nullptr) {
      fields_[name] = { formatter, var_array };
    }

    // 打印解决方案
    void print(SCIP* scip, const std::vector<std::string>& field_order, int num_gates = 10) {
      // 1. 收集所有数据
      std::vector<std::map<std::string, std::string>> rows;

      // 2. 准备数据
      for (int i = 0; i < num_gates; ++i) {
        std::map<std::string, std::string> row;
        for (const auto& field_entry : fields_) {  // 使用 entry 代替结构化绑定
          const std::string& name = field_entry.first;  // 获取键
          const auto& field = field_entry.second;       // 获取值

          std::ostringstream oss;
          field.formatter(oss, scip, i, field.var_array);
          row[name] = oss.str();
        }
        rows.push_back(row);
      }

      // 3. 计算列宽
      std::map<std::string, size_t> col_widths;
      for (const auto& name : field_order) {
        size_t max_len = name.length();
        for (const auto& row : rows) {
          max_len = (std::max)(max_len, row.at(name).length());
        }
        col_widths[name] = max_len + 1; // 加1作为边距
      }

      // 4. 打印表头
      std::cout << "|";
      for (const auto& name : field_order) {
        std::cout << " " << std::setw(col_widths[name]) << std::left << name << " |";
      }
      std::cout << "\n";

      // 打印分隔线
      std::cout << "+";
      for (const auto& name : field_order) {
        std::cout << std::string(col_widths[name] + 2, '-') << "+";
      }
      std::cout << "\n";

      // 5. 打印数据行
      for (const auto& row : rows) {
        std::cout << "|";
        for (const auto& name : field_order) {
          std::cout << " " << std::setw(col_widths[name]) << row.at(name) << " |";
        }
        std::cout << "\n";
      }
    }
    // 新增：输出CSV格式
    void printCSV(SCIP* scip,
      const std::vector<std::string>& field_order,
      const std::string& filename = "",
      char delimiter = ',',
      int num_gates = 10) {
      prepareData(scip, num_gates);
    }
  private:
    struct FieldInfo {
      Formatter formatter;
      SCIP_VAR** var_array; // 变量数组指针
    };

    std::map<std::string, FieldInfo> fields_;

    std::vector<std::map<std::string, std::string>> data_;
    void prepareData(SCIP* scip, int num_gates) {
      data_.clear();
      for (int i = 0; i < num_gates; ++i) {
        std::map<std::string, std::string> row;
        for (const auto& field_pair : fields_) {  // 使用 pair 替代结构化绑定
          const std::string& name = field_pair.first;  // 手动解构 pair
          const auto& field = field_pair.second;

          std::ostringstream oss;
          field.formatter(oss, scip, i, field.var_array);
          row[name] = oss.str();
        }
        data_.push_back(row);
      }
    }
  };


};

#endif //
