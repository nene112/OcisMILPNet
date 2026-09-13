#pragma once
#ifndef INCLUDE_EDGES_UTILS
#define INCLUDE_EDGES_UTILS

#include "FileInteraction.h"

class Ocis_edges {

public:

  std::map<string, string> params;

  class Edges;
  class Point;

  string start_time;
  int T = 24;

  class Piece_wise1D {
  public:
    std::vector<double> x;
    std::vector<double>  z_func;
    std::vector<std::vector<double>> ab_1D;
  };

  class Piece_wise2D {
  public:
    std::vector<double> x;
    std::vector<double> y;
    std::vector<std::vector<double>>  z_func;
    std::vector<std::vector<std::vector<double>>> abc_2D;
  };

  process_T result_h;


  class Point
  {
  public:
    int id;
    int sim_vec_id=-1;
    int sim_nindex=-1;
    string name;
    string canal_name;
    std::vector<string> levels;
    string level1;
    double maxFlow;
    double minFlow;
    double Flow_ic=-1;
    double WeightedSum=-1;
    double waterDemand_ratio=-1;
    double designFlow_ratio_pool=-1;




    double Flow_solved=-1;
    int STime_solved=-1;
    int ETime_solved=-1;
    int RTime_solved=-1;
    int tau_solved=-1;

    double stake;//
    double junction_stake=-1;//
    double mile_stage=-1;
    double accumulated_stake = 0;
    double accumulated_tau_max = 0;
    double edge_stage;
    int type=-1;
    int edges_id=-1;
    int pre_edges_id=-1;

    int massCoef = 0;

    int regulationTime=10;
    double delaytime=-1;                  
    double obs_flow=-1;
    double obs_h=-1;
    double obs_h2=-1;
    double irr_progress = 1.;
    double grain_to_cash_crop_ratio = 1.;
    double actions_weight = 0;
    string obs_STime="-1";
    int fixed = 0;
    int fixed_y = 0;
    int fixed_W = 0;
    int STime=-1;
    double STime_ratio=-1;
    int ETime=-1;
    int delta_Q_max = -1;
    int end_flow = -1;
    int spillway_gate_label = -1;
    int regulation_structure_label = -1;

    //BD
    int status_MP = 1;

// hydraulic function
    int pump_label = -1;
    std::map<double, double> Q_DH;


    int weir_label = -1;
    int hydraulic_label = 0;
    double net_B = 15.;
    double hydraulic_mu_ef = 0.6;
    double hydraulic_mu_es = 0.6;
    double hydraulic_mu_cf = 0.3;
    double hydraulic_mu_cs = 0.8;
    int Eopen_num = 1;

    double weir_flow_length = 1;
    double weir_height = 2;
    double weir_cross_width = 2;


    int BC_STAGE_UP_LABEL = 0;
    int BC_STAGE_DOWN_LABEL = 0;

    double e_0 = 0.;
    double hu_0 = 0.;
    double hd_0 = 0.;
    double f_0 = 0.;
    double pf_pe = 0.;
    double pf_phu = 0.;
    double pf_phd = 0.;


    int duration_input=-1;
    double demandFlow = -1;
    double target_h = -1;
    double W_task_gap = 0.2;
    int max_flow_steps = -1;

    int STime_input=-1;
    int ETime_input=-1;
    int W_input=-1;
    double W_shortage_ratio=0;
    int W_setLabel=-1;

    int isDecisionVariable = false;
    int isCheckStructure = false;

    int is_max_obs_STime_Label = -1;
    int bc_Flow_label = -1;
    int soft_Flow_cons_label = -1;

    double GeodesicDistance_to_source = 0;
    double GeodesicDistance_in_pool = 0;

    std::vector<int> path_to_top_node;

    Piece_wise2D piece_wise_Q_cdot_h;


    //solution
    std::vector<double> Q_sol_vec;
    double Q_sol_first = -1;
    double Q_sol_average = -1;
    double W_sol = -1;
    double stage_sol = -1;


    // residual
    double LTI_g = 0;
    std::vector<double> g;

  };

  class Edges
  {
  public:
    int id;
    int sim_vec_id = -1;
    std::vector<int> sim_nindex_vec;
    std::map<time_t, double> sim_pools_volume;
    double sim_last_t_volume = 0;

    string name;
    string canal_name;

    int graph_id=-1;
    std::vector<int>  graph_pre;
    std::vector<int>  graph_next;
    double As;
    double Ac;
    double pool_Length;
    double pool_Width;
    double pool_m;
    double zb=0;
    double pool_hTarget;
    double pool_h_ic;
    double pool_h_threthold = 2;
    double irrigation_process=-1;
    double waterlevel_threshold=-1;
    double pool_c;
    double max_h;
    double min_h;
    double pool_slope;
    double pool_manning;
    double delaytime=-1;
    int isDecisionVariable = false;

    double leakage_A = 1.;
    double leakage_m = 0.3;
    double leakage_Q = 0.;

    double sum_turnouts_Q = 0;
    double sum_turnouts_W = 0;

    double pool_h;

    std::vector<Point>  source;
    std::vector<Point>  turnouts;
    std::vector<Point>  targets;

  double utilization_ratio = 0.995;
    std::vector<double> get_turnouts_stakes();
    std::vector<double> get_turnouts_edge_stage();
    std::vector<string> get_turnouts_names();
    bool is_in_turnouts(string& obj);
    bool is_in_targets(string& obj);


    int bc_Stage_label = -1;

   std::vector<double> pool_segments_hh;
   std::vector<double> h_squre_vec;
   std::vector<std::vector<double> > pool_segments_ax_b_1D_hh;

   std::vector<double> pool_segments_h_reciprocal;
   std::vector<std::vector<double> > pool_segments_ax_b_1D_h_reciprocal;

    Piece_wise1D piece_wise_celerity;
    Piece_wise1D piece_wise_celerity_reciprocal;
    Piece_wise1D piece_wise_V_add_celerity_reciprocal;
    Piece_wise1D piece_wise_tau;
    Piece_wise2D piece_wise_V_plus_celerity_reciprocal;



    double seepage_solved = -1;
    double get_sum_outflow(Ocis_edges* edges);
    double set_turnouts_flow(Ocis_edges* edges,double availableFlow);
  };

  std::vector< std::vector< double>> massMatrix;
  std::vector<Point> gates;
  std::vector<Edges> pools;
  double max_geodesic_distance = 0;
  std::map<string,int> gates_stakes;
  std::map<string,int> gates_obs_STime;
  std::map<double, double> Q_cdot_h;;

  std::map<string, int> label_col_map;
  std::map<int, int> vec_graph_id;
  process_T boundary_flow;

  void set_obs(json input_json);
  void read_Graph( string& edges_path,string& input_json_path);
  void read_inputJson(Ocis_edges& Graph, string& inputJson_path);
  void rule_based_pre_solver(Ocis_edges& Graph);
  void write_Graph_solution(Ocis_edges& Graph);

  
  void inverse_sweep_solver(std::map<string, double>& check_structure_flow);
  
  void direct_sweep_STime_solver(std::map<string, double>& gates_STime);
  
  void direct_sweep_turnouts_solver(std::map<string, double>& turnouts_flow);
  
  void direct_sweep_check_structure_solver(std::map<string, double>& check_structure_flow,std::map<string, double>& check_structure_flow_new);

  
  void direct_sweep_leakage_solver();

  void get_designFlow_ratio_pool(Ocis_edges* Graph);
  double get_outflow_solved(Ocis_edges* Graph);


  double get_g2g_Length(Ocis_edges::Point* g, Ocis_edges::Point* gt) ;

  void get_gates(h_csv& edges);
  void get_label_col_map(h_csv& edges,std::map<string, int>& label_col_map);
  int get_col_by_label(h_csv& edges,string name);

  std::vector<string> get_check_structures_name();
 std::map<string, std::vector<string > >  get_check_structures_name_turnouts();
  int find_Edges_vec_id(string& source);
  int find_gates_vec_id(string& source);

  Edges* get_pool_byName(string name);
  Edges* get_pool_bySourceName(string name);
  Edges* get_pool_byTurnoutsName(string name);
  Edges* get_pool_byEndName(string name);
  Point* get_gate_byName(string name);
  Point* get_gate_byID(int id);
  void calc_regulationTime();
  string get_inflow_name();
  void calc_delaytime(double inflow, Point* g, Edges* e, int inflow_start);
  void calc_edge_delaytime(double inflow, Point* g, Edges* e, int inflow_start);


  int Graph_Traversal_Algorithms_BFS(Edges* e, int& recursion_id);
  std::map<string, double> get_gates_distance();

  void get_recursion_path_pool_id(std::vector<int>& path_pool_id, int pool_id);

  void write_sim_pools_volume(string outputpath);
  void write_sim_pools_volume(string outputpath,string suffix);
};

#endif



    //for (int i = 0; i < pools.size(); i++) {
    //  pools[i].graph_id = i;

    //  for (int t = 0; t < pools[i].turnouts.size(); t++) {
    //    string name = pools[i].turnouts[t].name;
    //    Edges* e = get_pool_bySourceName(name);
    //    if (e != NULL) {

    //      std::cout << "process" << std::endl;
    //    }


    //  }
    //  for (int t = 0; t < pools[i].targets.size(); t++) {
    //    string name = pools[i].targets[t].name;
    //    Edges* e = get_pool_bySourceName(name);
    //    if (e != NULL) {

    //      std::cout << "process" << std::endl;
    //    }

    //  }
    //}

