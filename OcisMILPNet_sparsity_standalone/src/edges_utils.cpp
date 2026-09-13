#include "edges_utils.h"


int Ocis_edges::find_Edges_vec_id(string& source) {  


  for (int i = 0; i < pools.size(); i++) {
    for (int p = 0; p < pools[i].source.size(); p++) {
      if (pools[i].source[p].name == source) {
        return i;
      }
    }
  }
  return -1;
}
int Ocis_edges::find_gates_vec_id(string& source) {  

  for (int i = 0; i < gates.size(); i++) {
    if (gates[i].name == source) {
      return i;
    }
  }
  return -1;
}










int  Ocis_edges::get_col_by_label(h_csv& edges,string name) {

  int col = -1;
  for (int i = 0; i < edges.size(); i++) {
    if (i == 0) {
      for (int j = 0; j < edges[i].size(); j++) {
        if (edges[i][j] == name) {
          col = j;
          break;
        }
      }
    }
    else
    {
      break;
    }
  }
  return col;

  }


void Ocis_edges::get_label_col_map(h_csv& edges,std::map<string, int>& label_col_map) {

  int col = 0;
  for (int j = 0; j < edges[0].size(); j++) {
    string label = edges[0][j];
    label_col_map[label] = j;
  }
}


void Ocis_edges::set_obs(json input_json) {
  if (input_json.contains("obs")) {
    json obs_json = input_json["obs"];
    int max_obs_STime = 0;
    int max_obs_STime_gates_vec_id = 0;
    
    for (int i = 0; i < gates.size(); i++) {
    string g_name = gates[i].name;

      Ocis_edges::Point* g = get_gate_byName(g_name);
      if (obs_json.contains((g_name))) {
        double flow = -1;//
        if (obs_json[(g_name)].contains("flow")) {
          flow = obs_json[(g_name)]["flow"].get<double>();
        }
        double h = -1;
        double h2 = -1;
        if (obs_json[(g_name)].contains("h")) {
          h = obs_json[(g_name)]["h"].get<double>();
        }
        if (obs_json[(g_name)].contains("h2")) {
          h2 = obs_json[(g_name)]["h2"].get<double>();
        }
        if (flow < 0) {
          flow = 0;
        }
        if (flow > g->maxFlow) {
          flow = g->maxFlow;
        }

        g->obs_flow = flow;
        g->obs_h = h;
        g->obs_h2 = h2;

        if (obs_json[(g_name)].contains("obs_STime")) {
          g->obs_STime = obs_json[(g_name)]["obs_STime"].get<string>();
          if (g->obs_STime != "") {

          
          int obs_STime = StringToTime_t(g->obs_STime);
          gates_obs_STime[g_name] = obs_STime;
          
          if (obs_STime > max_obs_STime && (g->type == 4 || g->type == 2)) {
            max_obs_STime = obs_STime;
            max_obs_STime_gates_vec_id = i;
          }}
        }

        if (obs_json[(g_name)].contains("fixed")) {
          g->fixed = obs_json[(g_name)]["fixed"].get<int>();
        }
        if (obs_json[(g_name)].contains("fixed_W")) {
          g->fixed_W = obs_json[(g_name)]["fixed_W"].get<int>();
        }
        if (obs_json[(g_name)].contains("delta_Q_max")) {
          g->delta_Q_max = obs_json[(g_name)]["delta_Q_max"].get<int>();
        }
        if (obs_json[(g_name)].contains("end_flow")) {
          g->end_flow = obs_json[(g_name)]["end_flow"].get<int>();
        }
        if (obs_json[(g_name)].contains("spillway_gate_label")) {
          g->spillway_gate_label = obs_json[(g_name)]["spillway_gate_label"].get<int>();
        }

        if (obs_json[(g_name)].contains("irr_progress")) {
          double value = obs_json[(g_name)]["irr_progress"].get<double>();
          g->irr_progress = value;
        }
        if (obs_json[(g_name)].contains("grain_to_cash_crop_ratio")) {
          double value = obs_json[(g_name)]["grain_to_cash_crop_ratio"].get<double>();
          g->grain_to_cash_crop_ratio = value;
        }
        if (obs_json[(g_name)].contains("actions_weight")) {
          double value = obs_json[(g_name)]["actions_weight"].get<double>();
          g->actions_weight = value;
        }

        if (obs_json[(g_name)].contains("hydraulic_label")) {
          int label = obs_json[(g_name)]["hydraulic_label"].get<int>();
          if (label == 1) {
            g->hydraulic_label = label;

            if (obs_json[(g_name)].contains("net_B")) {
              double net_B = obs_json[(g_name)]["net_B"].get<double>();
              g->net_B = net_B;

            }
            if (obs_json[(g_name)].contains("hydraulic_mu_ef")) {
              double hydraulic_mu_ef = obs_json[(g_name)]["hydraulic_mu_ef"].get<double>();
              g->hydraulic_mu_ef = hydraulic_mu_ef;

            }
            if (obs_json[(g_name)].contains("hydraulic_mu_es")) {
              double hydraulic_mu_es = obs_json[(g_name)]["hydraulic_mu_es"].get<double>();
              g->hydraulic_mu_es = hydraulic_mu_es;

            }
            if (obs_json[(g_name)].contains("hydraulic_mu_cf")) {
              double hydraulic_mu_cf = obs_json[(g_name)]["hydraulic_mu_cf"].get<double>();
              g->hydraulic_mu_cf = hydraulic_mu_cf;

            }
            if (obs_json[(g_name)].contains("hydraulic_mu_cs")) {
              double hydraulic_mu_cs = obs_json[(g_name)]["hydraulic_mu_cs"].get<double>();
              g->hydraulic_mu_cs = hydraulic_mu_cs;
            }
            if (obs_json[(g_name)].contains("Eopen_num")) {
              double Eopen_num = obs_json[(g_name)]["Eopen_num"].get<int>();
              g->Eopen_num = Eopen_num;
            }

          }
        }

        if (obs_json[(g_name)].contains("BC_STAGE_DOWN_LABEL")) {
          double value = obs_json[(g_name)]["BC_STAGE_DOWN_LABEL"].get<int>();
          g->BC_STAGE_DOWN_LABEL = value;
        }

      }
    }
    if (max_obs_STime_gates_vec_id < gates.size()) {
      gates[max_obs_STime_gates_vec_id].is_max_obs_STime_Label = 1;
    }
  }

  }


void Ocis_edges::read_Graph(string& edges_path,string& input_json_path)
{
    h_csv edges = read_h_csv(edges_path, ',');
    get_gates(edges);
    json input_json = ReadJson(input_json_path);

    set_obs(input_json);

}

void Ocis_edges::read_inputJson(Ocis_edges& Graph, string& inputJson_path)
{
  json input_json = ReadJson(inputJson_path);
  if (input_json.contains("obs")) {
    nlohmann::json obs_json = input_json["obs"];

    int max_obs_STime = 0;
    int max_obs_STime_gates_vec_id = 0;
    
    for (int i = 0; i < Graph.gates.size(); i++) {
      string g_name = Graph.gates[i].name;
      if (isValidUTF8(g_name)) {
        g_name = U2G(g_name);
      }
      Ocis_edges::Point* g = Graph.get_gate_byName(g_name);
      if (obs_json.contains(G2U(g_name))) {


        if (obs_json[G2U(g_name)].contains("flow")) {
          double flow = obs_json[G2U(g_name)]["flow"];
          g->obs_flow = flow;
        }
        if (obs_json[G2U(g_name)].contains("h")) {
          double h = obs_json[G2U(g_name)]["h"];
          g->obs_h = h;
        }

        if (obs_json[G2U(g_name)].contains("fixed")) {
          g->fixed = obs_json[G2U(g_name)]["fixed"].get<int>();

        }

        if (obs_json[G2U(g_name)].contains("irr_progress")) {
          double value = obs_json[G2U(g_name)]["irr_progress"].get<double>();
          g->irr_progress = value;
        }
        if (obs_json[G2U(g_name)].contains("grain_to_cash_crop_ratio")) {
          double value = obs_json[G2U(g_name)]["grain_to_cash_crop_ratio"].get<double>();
          g->grain_to_cash_crop_ratio = value;
        }

        if (obs_json[G2U(g_name)].contains("obs_STime")) {
          g->obs_STime = obs_json[G2U(g_name)]["obs_STime"];

          int obs_STime = StringToTime_t(g->obs_STime);
          Graph.gates_obs_STime[g_name] = obs_STime;


          
          if (obs_STime > max_obs_STime && (g->type == 4 || g->type == 2)) {
            max_obs_STime = obs_STime;
            max_obs_STime_gates_vec_id = i;
          }

        }
      }
    }
    Graph.gates[max_obs_STime_gates_vec_id].is_max_obs_STime_Label = 1;

  }

  if (input_json.contains("waterDemand")) {
    nlohmann::json obs_json = input_json["waterDemand"];
    for (int i = 0; i < Graph.gates.size(); i++) {
      string g_name = Graph.gates[i].name;
      string g_name_utf8 = g_name;
      if (isValidUTF8(g_name_utf8)) {
        g_name = U2G(g_name);
      }
      else
      {
        g_name_utf8 = G2U(g_name);
      }
      Ocis_edges::Point* g = Graph.get_gate_byName(g_name);
      if (obs_json.contains(g_name_utf8)) {
        for (int i = 0; i < obs_json[g_name_utf8].size(); i++) {
          string s = obs_json[g_name_utf8][i]["s"];
          string e = obs_json[g_name_utf8][i]["e"];
          double q = obs_json[g_name_utf8][i]["q"];

          g->duration_input = StringToTime_t(e) - StringToTime_t(s);
          g->demandFlow = q;
          g->W_input = g->demandFlow*g->duration_input;
          g->STime_input = StringToTime_t(s);
          g->ETime_input = StringToTime_t(e);

        }
      }
    }
  }

  if (input_json.contains("CalculationParams")) {
    if (input_json["CalculationParams"].contains("start_time")) {
      start_time = input_json["CalculationParams"]["start_time"];
    }
    if (input_json["CalculationParams"].contains("T")) {
      T = input_json["CalculationParams"]["T"];
    }
    if (input_json["CalculationParams"].contains("fixed_inflow")) {
      params["fixed_inflow"] = input_json["CalculationParams"]["fixed_inflow"];
    }

  }

}

  
void Ocis_edges::inverse_sweep_solver(std::map<string, double>& check_structure_flow) {
  for (auto iter = vec_graph_id.rbegin(); iter != vec_graph_id.rend(); iter++) {
    cout << iter->first << ", " << iter->second << endl;

    int i = iter->second;

    cout << "[pool]-" << i << endl;;
    Ocis_edges::Edges* p = &pools[i];

    string name_source = pools[i].source[0].name;
    Ocis_edges::Point* g_source = get_gate_byName(name_source);
    cout << "[turnouts]";

    double sum_outflow = 0;
    for (int e = 0; e < pools[i].turnouts.size(); e++) {
      string name = pools[i].turnouts[e].name;
      Point* g = get_gate_byName(name);
      if (g->isDecisionVariable) {
        if (g->obs_flow > 0) {
          g->Flow_solved = g->obs_flow;
        }
        if (g->demandFlow > 0) {
          g->Flow_solved = g->demandFlow;
        }
        if (g->Flow_solved < 0) {
          g->Flow_solved = 0;
        }
        sum_outflow += g->Flow_solved;
      }
      else
      {
        g->Flow_solved = 0;
      }
      cout << "; " << g->name << ", " << g->Flow_solved<<",("<<g->obs_flow<<")"<<",("<<g->demandFlow<<")";
    }
    cout << endl;
    cout << "[turnouts_sum]"<<sum_outflow;
    cout << endl;
    cout << "[targets]";

    for (int e = 0; e < pools[i].targets.size(); e++) {
      string name = pools[i].targets[e].name;
      Point* g = get_gate_byName(name);
      //if (g->isDecisionVariable) {
      if (g->Flow_solved < 0) {
        if (g->obs_flow >= 0) {
          g->Flow_solved = g->obs_flow;
        }
        if (g->demandFlow >= 0) {
          g->Flow_solved = g->demandFlow;
        }
      }

      if (g->Flow_solved < 0) {
        g->Flow_solved = 0;
      }

      sum_outflow += g->Flow_solved;
      cout <<"; " << g->name << ", " << g->Flow_solved<<",("<<g->obs_flow<<")"<<"("<<g->demandFlow<<")";
    }

    cout << endl;
    cout << "[source]" ;

    double seepage = sum_outflow * (1 - p->utilization_ratio);
    p->seepage_solved = seepage;
    double sum_outflow_include_seepage = sum_outflow + seepage;

    g_source->Flow_solved = sum_outflow_include_seepage;
    cout << g_source->name << ", " << g_source->Flow_solved<<",("<<g_source->obs_flow<<")";
    cout << endl;

    check_structure_flow[g_source->name] = g_source->Flow_solved;
  }


}


  
void Ocis_edges::direct_sweep_STime_solver(std::map<string, double>& gates_STime) {
  Point* g_water_source = &gates[0];
  g_water_source->STime_solved = StringToTime_t(start_time);
  for (auto iter = vec_graph_id.begin(); iter != vec_graph_id.end(); iter++) {
    int i = iter->second;
    string name_source = pools[i].source[0].name;
    Ocis_edges::Edges* pool = &pools[i];
    Ocis_edges::Point* g_s= get_gate_byName(name_source);

    double c = sqrt(9.8 * pool->pool_h_ic);
    double V = g_s->Flow_solved / pool->As;

    for (int e = 0; e < pools[i].turnouts.size(); e++) {
      string name = pools[i].turnouts[e].name;
      Point* g = get_gate_byName(name);

      double tau = g->GeodesicDistance_in_pool / (V+c);
      g->tau_solved = tau;
      g->STime_solved = g_s->STime_solved+g_s->regulationTime*60 + g->tau_solved;
      std::cout << g->name<<", "<<FormatTime(g->STime_solved) << endl;;


      gates_STime[g->name] = g->STime_solved;
    }

    for (int e = 0; e < pools[i].targets.size(); e++) {
      string name = pools[i].targets[e].name;
      Point* g = get_gate_byName(name);
      double tau = g->GeodesicDistance_in_pool / (V+c);
      g->tau_solved = tau;
      g->STime_solved = g_s->STime_solved+g_s->regulationTime*60 + g->tau_solved;
      std::cout << g->name<<", "<<FormatTime(g->STime_solved) << endl;;
      gates_STime[g->name] = g->STime_solved;
    }

  }



}

  
void Ocis_edges::direct_sweep_turnouts_solver(std::map<string, double>& turnouts_flow) {
    double decrease_flow = 0;
    string name_watersource = gates[0].name;
    Ocis_edges::Point* g_ws = get_gate_byName(name_watersource);


    string fixed_inflow = params["fixed_inflow"];
    if (g_ws->type == 0 && fixed_inflow == "true") {
      decrease_flow = g_ws->obs_flow - g_ws->Flow_solved;
      g_ws->Flow_solved = g_ws->obs_flow;
    }


  for (auto iter = vec_graph_id.begin(); iter != vec_graph_id.end(); iter++) {
    int i = iter->second;
    string name_source = pools[i].source[0].name;
    string name_end = pools[i].targets[0].name;
    Ocis_edges::Edges* pool = &pools[i];
    Ocis_edges::Point* g_s = get_gate_byName(name_source);
    Ocis_edges::Point* g_e = get_gate_byName(name_end);


    
    double sum_outflow = pool->get_sum_outflow(this) + pool->seepage_solved;
    if (decrease_flow < 0) {
      
      for (int i = 0; i < pool->turnouts.size(); i++) {
        string name = pool->turnouts[i].name;
        Ocis_edges::Point* g = get_gate_byName(name);
        if (g->waterDemand_ratio != -1) {
          g->Flow_solved = g->Flow_solved + decrease_flow * g->waterDemand_ratio;
          if (g->Flow_solved < 0) {
            g->Flow_solved = 0;
          }

        }

      }

    }
    else
    {
      

      double availableFlow = g_s->Flow_solved - pool->seepage_solved - g_e->Flow_solved;
      double turnoutsFlow = pool->set_turnouts_flow(this, availableFlow);


    }

    
    
    //double sum_outflow_solved = get_outflow_solved(this);


  }


}


  
void Ocis_edges::direct_sweep_check_structure_solver(
  std::map<string, double>& check_structure_flow,
  std::map<string, double>& check_structure_flow_new
) {

  for (auto iter = vec_graph_id.begin(); iter != vec_graph_id.end(); iter++) {
    int i = iter->second;
    string name_source = pools[i].source[0].name;
    string name_end = pools[i].targets[0].name;
    Ocis_edges::Edges* pool = &pools[i];
    Ocis_edges::Point* g_s = get_gate_byName(name_source);
    Ocis_edges::Point* g_e = get_gate_byName(name_end);

    string fixed_inflow = params["fixed_inflow"];
    if (g_s->type == 0 && fixed_inflow == "true") {
      g_s->Flow_solved = g_s->obs_flow;
      check_structure_flow_new[g_s->name] = g_s->obs_flow;
    }

    
    double sum_outflow = pool->get_sum_outflow(this) + pool->seepage_solved;
    if (g_s->Flow_solved < sum_outflow) {
      
    double decrease_flow = 0;
      decrease_flow = sum_outflow - g_s->Flow_solved;


      for (int e = 0; e < pools[i].targets.size(); e++) {
        string name = pools[i].targets[e].name;
        Point* g = get_gate_byName(name);

        g->Flow_solved = g->Flow_solved - decrease_flow;
        if (g->Flow_solved < 0) {
          g->Flow_solved = 0;
        }

        check_structure_flow_new[g_s->name] = check_structure_flow_new[g_s->name] - decrease_flow;
        if (check_structure_flow_new[g_s->name] < 0) {
          check_structure_flow_new[g_s->name] = 0;
        }

      }

    }
    else
    {

      
      if (i == 9) {
        int a = 0;
      }

      if (pool->seepage_solved <= 0) {
        pool->seepage_solved = g_s->Flow_solved * (1 - pool->utilization_ratio);
      }
      else
      {
        int a = 0;
      }


      sum_outflow = pool->get_sum_outflow(this) + pool->seepage_solved;

      double availableFlow = g_s->Flow_solved -sum_outflow;
      //double turnoutsFlow = pool->set_turnouts_flow(this, availableFlow);
      //increase_flow = increase_flow - turnoutsFlow;

      for (int e = 0; e < pools[i].targets.size(); e++) {
        string name = pools[i].targets[e].name;
        Point* g = get_gate_byName(name);

        g->Flow_solved = g->Flow_solved + availableFlow;
        if (g->Flow_solved < 0) {
          g->Flow_solved = 0;
        }

        check_structure_flow_new[g_s->name] = check_structure_flow_new[g_s->name] + availableFlow;
        if (check_structure_flow_new[g_s->name] < 0) {
          check_structure_flow_new[g_s->name] = 0;
        }

      }

    }



  }


}



void Ocis_edges::direct_sweep_leakage_solver() {
  for (auto iter = vec_graph_id.begin(); iter != vec_graph_id.end(); iter++) {
    int i = iter->second;
    string name_source = pools[i].source[0].name;
    string name_end = pools[i].targets[0].name;
    Ocis_edges::Edges* pool = &pools[i];
    for (int i = 0; i < pool->turnouts.size(); i++) {
      Ocis_edges::Point* g = get_gate_byName(pool->turnouts[i].name);

      if (g->type == 6) {
        g->Flow_solved = pool->seepage_solved;
      }


    }

  }
  for (int i = 0; i < gates.size(); i++) {
    Point* g = &gates[i];
      if (g->Flow_solved < 0.0001) {
        g->Flow_solved = 0;
      }

  }

}


void Ocis_edges::rule_based_pre_solver(Ocis_edges& Graph) {
  ///*
  
  
  
  
  
  
  
  
  
  
  std::map<double, int> kpi_vec_id;
  
  double sum_demandFlow = 0;
  double sum_design_flow = 0;
  for (int i = 0; i < gates.size(); i++) {
    Point* g = &gates[i];
    double kpi_GD = g->GeodesicDistance_to_source / max_geodesic_distance;
    double WeightedSum = g->irr_progress - g->grain_to_cash_crop_ratio - kpi_GD - (g->obs_flow != -1);
    cout << g->name << ", "<<g->demandFlow << ", " << WeightedSum << endl;
    

    if (g->type == 2) {
      kpi_vec_id[WeightedSum] = i;
      g->WeightedSum = WeightedSum;

      double flow = 0;

      
      
      if (g->obs_flow >=0) {
        flow = g->obs_flow;
        g->minFlow = flow;
        g->maxFlow = flow;
      }
      if (g->demandFlow >=0) {
        flow = g->demandFlow;
        g->minFlow = 0.8 * flow;
        g->maxFlow = 1.2 * flow;
      }

      if (flow >= 0) {
        g->isDecisionVariable = 1;
        sum_demandFlow+= flow;
      }
      else
      {
        sum_design_flow += g->maxFlow;

      }



    }
  }


  Point* g_water_source = &gates[0];
  if (sum_demandFlow < 0.8 * g_water_source->obs_flow) {
    
    double demand_source_ratio = sum_demandFlow / g_water_source->obs_flow;
    double free_water_flow = g_water_source->obs_flow - sum_demandFlow;

    double sum_waterAllocation_check = 0;
    for (int i = 0; i < gates.size(); i++) {
      Point* g = &gates[i];
      if (g->isDecisionVariable != 1&&g->type==2) {
        g->isDecisionVariable = 1;
        if (sum_design_flow <= 0) {
          continue;
        }
        double dFlow_ratio = g->maxFlow / sum_design_flow;

        if (isnan(dFlow_ratio) || isinf(dFlow_ratio)) {
          int a = 0;
        }

        double dFlow = dFlow_ratio * free_water_flow;
        g->demandFlow = dFlow;
        sum_waterAllocation_check += dFlow;
      }
    }
    cout << "water allocation check: "<<sum_waterAllocation_check << endl;
    cout << "water allocation check: "<<sum_waterAllocation_check << endl;

  }
  else if (sum_demandFlow > 1.2 * g_water_source->obs_flow)
  {
    
    double sum_allocation = 0;
    for (auto iter = kpi_vec_id.begin(); iter != kpi_vec_id.end(); iter++) {
      int i = iter->second;
      Point* g = &gates[i];
      if (g->type == 2) {
        double flow = g->obs_flow;
        if (g->demandFlow != -1) {
          flow = g->demandFlow;
        }
        if (flow > 0) {
          double sum_allocation_d = sum_allocation;
          sum_allocation += flow;
          g->isDecisionVariable = 1;

          if (sum_allocation_d<g_water_source->obs_flow && sum_allocation>g_water_source->obs_flow) {
            break;
          }
        }
      }
    }
  }
  else
  {
    
    for (int i = 0; i < gates.size(); i++) {
      Point* g = &gates[i];
      g->isDecisionVariable = 1;
    }

  }




  std::map<string, double> check_structure_flow;
  
  inverse_sweep_solver(check_structure_flow);

  get_designFlow_ratio_pool(this);
  
  std::map<string, double> gates_STime;
  direct_sweep_STime_solver(gates_STime);

  
  std::map<string, double> turnouts_flow;
  direct_sweep_turnouts_solver(turnouts_flow);

  
  std::map<string, double> check_structure_flow_new;
  direct_sweep_check_structure_solver(check_structure_flow,check_structure_flow_new);

  direct_sweep_leakage_solver();



  std::cout << "=== rule_based_pre_solver ===\n";
}


double Ocis_edges::get_outflow_solved(Ocis_edges* Graph) {
 
  double sum_outflow_solved = 0;
  
  for (int i = 0; i < Graph->gates.size(); i++) {
    Ocis_edges::Point* g = &Graph->gates[i];
    if (g->type == 2 && g->Flow_solved != -1) {
      sum_outflow_solved += g->Flow_solved;
    }

  }

  if (sum_outflow_solved <= 0) {
    return 0;
  }

  for (int i = 0; i < Graph->gates.size(); i++) {
    Ocis_edges::Point* g = &Graph->gates[i];
    if (g->type == 2 && g->Flow_solved != -1) {
      //g->waterDemand_ratio = g->Flow_solved / sum_outflow_solved;
      
      cout << g->name << ", " << g->waterDemand_ratio << endl;
    }

  }
  return sum_outflow_solved;

}

void Ocis_edges::get_designFlow_ratio_pool(Ocis_edges* Graph) {
 
    double sum_waterDemand = 0;

  for (auto iter = vec_graph_id.begin(); iter != vec_graph_id.end(); iter++) {
    int i = iter->second;
    string name_source = pools[i].source[0].name;
    string name_end = pools[i].targets[0].name;
    Ocis_edges::Edges* pool = &pools[i];

    double sum_maxFlow = 0;

    for (int i = 0; i < pool->turnouts.size(); i++) {
      Ocis_edges::Point* g = get_gate_byName(pool->turnouts[i].name);

      if (g->Flow_solved != -1) {
        sum_waterDemand += g->Flow_solved;
      }

      if (g->type == 2 && g->obs_flow != -1) {
        sum_maxFlow += g->maxFlow;
      }

    }


    for (int i = 0; i < pool->turnouts.size(); i++) {
      Ocis_edges::Point* g = get_gate_byName(pool->turnouts[i].name);
      if (g->type == 2 && g->maxFlow != -1 && sum_maxFlow > 0) {
        g->designFlow_ratio_pool = g->maxFlow / sum_maxFlow;
        cout << g->name << ", " << g->designFlow_ratio_pool << endl;
      }

    }
  }




  for (auto iter = vec_graph_id.begin(); iter != vec_graph_id.end(); iter++) {
    int i = iter->second;
    string name_source = pools[i].source[0].name;
    string name_end = pools[i].targets[0].name;
    Ocis_edges::Edges* pool = &pools[i];
    for (int i = 0; i < pool->turnouts.size(); i++) {
      Ocis_edges::Point* g = get_gate_byName(pool->turnouts[i].name);
      if (g->Flow_solved != -1 && g->Flow_solved != 0&& sum_waterDemand > 0) {
        g->waterDemand_ratio = g->Flow_solved / sum_waterDemand;
      }
    }
  }

}



void Ocis_edges::write_Graph_solution(Ocis_edges& Graph) {

  nlohmann::ordered_json result;
  for (int i = 0; i < Graph.gates.size(); i++) {
    Ocis_edges::Point* g = &Graph.gates[i];
    nlohmann::ordered_json temp_result;

    if (g->STime_solved == -1) {
      g->STime_solved = StringToTime_t(start_time);
    }

    temp_result["q"] = g->Flow_solved;
    temp_result["s"] = FormatTime(g->STime_solved);
    temp_result["s_delaytime"] = FormatTime(g->STime_solved);
    temp_result["e"] = FormatTime(g->STime_solved + g->duration_input);
    temp_result["stake"] = g->stake;
    result[G2U(g->name)].push_back(temp_result);
  }
  string output_file_name = params["output_dir_path"] + "schedule_MultiDays.json";
  std::ofstream out(output_file_name, std::ios::binary);
  out << std::fixed << std::setprecision(2);
  out << result.dump(4);


}


  void Ocis_edges::get_gates(h_csv& edges) {
    std::cout << "=== get_gates ===\n";
    std::map<string, int> label_col_map;
    get_label_col_map(edges,label_col_map);
    h_csv edges_bak = edges;
    // delete repeat obj
    std::vector<string> obj_names_str;
    for (int i = 0; i < edges.size(); i++) {

        if (edges[i][1] != "-1" != find_vec(obj_names_str, edges[i][1])) {
          obj_names_str.push_back(edges[i][1]);
        }
        else
        {
          cout << "\r[Warning]: erase the data at row " << i << ", repeat obj = " << edges[i][1];
          edges.erase(edges.begin() + i);
          i = i - 1;
        }
    }


    int col = -1;
    for (int i = 0; i < edges.size(); i++) {
      if (i == 0) {
        for (int j = 0; j < edges[i].size(); j++) {
          if (edges[i][j] == "ConnectionType") {
            col = j;
            break;
          }
        }
      }
      //cout << edges[i][0] << "," << edges[i][1] << "," << edges[i][2] << endl;


      
      for (int j = 0; j < edges[i].size(); j++) {
        if (j == col && edges[i][j] == "direct") {
          if (edges[i][1] == "-1") {
            edges[i][j] = "indirect";
            edges[i][7] = edges[i-1][7];
            edges[i][8] = edges[i-1][8];
            edges[i][9] = edges[i-1][9];
            edges[i][10] = edges[i-1][10];
            edges[i][11] = edges[i-1][11];
            edges[i][12] = edges[i-1][12];


            if(edges[i].size()>22)
              edges[i][22] = edges[i - 1][22];
          }

        }
      }

      
      for (int j = 0; j < edges[i].size(); j++) {
        if (j == col && edges[i][j] == "indirect") {
          Edges pool_tp;
          pool_tp.id = pools.size();
          check_elements_is_null(edges, i, 0);
          pool_tp.name = (edges[i][0] + "-" + edges[i][1]);
          pool_tp.canal_name = (edges[i][5]);
          //source 
          Point gate_tp;
          gate_tp.id = gates.size();
          gate_tp.name = (edges[i][0]);
          gate_tp.maxFlow = stod(edges[i][16]);
          gate_tp.minFlow = stod(edges[i][17]);
          gate_tp.Flow_ic = stod(edges[i][18]);

          gate_tp.stake = stod(edges[i][4]);

          if (edges[i].size() > 5) {
            check_elements_is_null(edges, i, 5);
            string canalname_tp = edges[i][5];
            string canalname_utf8_tp = canalname_tp;
            gate_tp.canal_name = canalname_tp;
          }


          if (edges[i].size() > 23) {
            check_elements_is_null(edges, i, 23);
            gate_tp.mile_stage = stod(edges[i][23]);
          }
          if (edges[i][15] != "") {
            gate_tp.type = stod(edges[i][15]);

            if (gate_tp.type != 4 && gate_tp.type != 0) {
              gate_tp.type = 4;
              cout << gate_tp.name << ": type error, should be 4(checkstructure), fixed here." << endl;
            }
          }
          else
          {
            gate_tp.type = 2;
          }
          if (find_gates_vec_id(gate_tp.name) == -1) {
            gates.push_back(gate_tp);
          }

          //target
          if (edges[i][1] != "-1") {
            Point gate_tp_target;
            gate_tp_target.id = gates.size();
            check_elements_is_null(edges, i, 1);
            gate_tp_target.name = (edges[i][1]);

            for (int k = 0; k < edges.size(); k++) {

              string name = edges[k][0];
              string name_uft8 = name;
              if (name == gate_tp_target.name) {
                if (edges[k].size() > 5) {
                  check_elements_is_null(edges, k, 5);
                  string canalname_tp = edges[k][5];
                  string canalname_utf8_tp = canalname_tp;
                  gate_tp_target.canal_name = canalname_tp;
                }

                gate_tp_target.maxFlow = stod(edges[k][16]);
                gate_tp_target.minFlow = stod(edges[k][17]);
                gate_tp_target.Flow_ic = stod(edges[k][18]);

                gate_tp_target.stake = stod(edges[k][4]);

                if (edges[k].size() > 23) {
                  check_elements_is_null(edges, k, 23);
                  gate_tp_target.mile_stage = stod(edges[k][23]);
                }
                if (edges[i][15] != "") {
                  gate_tp_target.type = stod(edges[i][15]);

                  if (gate_tp_target.type != 4 ) {
                    gate_tp_target.type = 4;
                    cout << gate_tp_target.name << ": type error, should be 4(checkstructure), fixed here." << endl;
                  }

                }
                break;
              }
            }

            if (find_gates_vec_id(gate_tp_target.name) == -1) {
              gates.push_back(gate_tp_target);
            }
            pool_tp.targets.push_back(gate_tp_target);

          }


          pool_tp.source.push_back(gate_tp);



          if (get_col_by_label(edges, "pool_slope") != -1) {
            int col_int = get_col_by_label(edges, "pool_slope");
            check_elements_is_null(edges, i, col_int);
            pool_tp.pool_slope = stod(edges[i][col_int]);
          }
          else
          {
            pool_tp.pool_slope = 0.0001;
          }
          if (get_col_by_label(edges, "pool_manning") != -1) {
            int col_int = get_col_by_label(edges, "pool_manning");
            check_elements_is_null(edges, i, col_int);
            if (edges[i].size() > col_int) {
              pool_tp.pool_manning = stod(edges[i][col_int]);
            }
          }
          else
          {
            pool_tp.pool_manning = 0.0151;
          }
          if (get_col_by_label(edges, "zb") != -1) {
            int col_int = get_col_by_label(edges, "zb");
            check_elements_is_null(edges, i, col_int);
            if (edges[i].size() > col_int) {
              pool_tp.zb = stod(edges[i][col_int]);
            }
          }
          if (get_col_by_label(edges, "utilization_ratio") != -1) {
            int col_int = get_col_by_label(edges, "utilization_ratio");
            check_elements_is_null(edges, i, col_int);

            double utilization_ratio = 0.995;
            if (edges[i].size() > col_int) {
              utilization_ratio = stod(edges[i][col_int]);
            }
            pool_tp.utilization_ratio =utilization_ratio ;
          }
          if (get_col_by_label(edges, "delaytime") != -1) {
            int col_int = get_col_by_label(edges, "delaytime");
            check_elements_is_null(edges, i, col_int);
            if (edges[i].size() > col_int) {
              pool_tp.delaytime = stod(edges[i][col_int])*60;
            }
          }

          if (edges[i][1] != "-1") {
            pool_tp.pool_Width = stod(edges[i][8]);
            pool_tp.pool_Length = stod(edges[i][9]);
            pool_tp.pool_m = stod(edges[i][10]);
            pool_tp.pool_hTarget = stod(edges[i][11]);
            pool_tp.pool_h_ic = stod(edges[i][12]);
            pool_tp.max_h = stod(edges[i][13]);
            pool_tp.min_h = stod(edges[i][14]);

            if (pool_tp.min_h >= pool_tp.max_h) {
              pool_tp.max_h = 5;
              pool_tp.min_h = 4.9;
            }

            pool_tp.As = (1.0 / 1.0) * pool_tp.pool_Length * (pool_tp.pool_Width + 2 * pool_tp.pool_m * pool_tp.pool_h_ic);
            pool_tp.Ac = 1. * (2 * pool_tp.pool_Width + 2 * pool_tp.pool_m * pool_tp.pool_h_ic) * pool_tp.pool_h_ic;

            //std::cout <<pool_tp.As<<", " << pool_tp.Ac << std::endl;
            pools.push_back(pool_tp);
          }
          //check_structures_name.push_back(edges[i][0]);
        }
        if (j == col && edges[i][j] == "direct") {
          Point gate_tp;
          gate_tp.id =gates.size();
          string name = edges[i][1];
          gate_tp.name = (name);
          gate_tp.maxFlow = stod(edges[i][16]);

          if (edges[i][17] == "") {
            gate_tp.minFlow = 0;
          }
          else
          {
            gate_tp.minFlow = stod(edges[i][17]);
          }

          gate_tp.Flow_ic = stod(edges[i][18]);
          gate_tp.stake = stod(edges[i][4]);
          if (edges[i].size() > 5) {
            check_elements_is_null(edges, i, 5);
            string canalname = edges[i][5];
            string canalname_utf8 = canalname;
              canalname_utf8 = (edges[i][5]);
            gate_tp.canal_name =canalname;
          }

          gate_tp.type = stod(edges[i][15]);
          if (edges[i].size() > 23){
            check_elements_is_null(edges, i, 23);
            gate_tp.mile_stage = stod(edges[i][23]);
          }

          if (edges[i].size()>get_col_by_label(edges, "level1") &&get_col_by_label(edges, "level1")>-1) {
            int col_int = get_col_by_label(edges, "level1");
            check_elements_is_null(edges, i, col_int);
            gate_tp.level1 = (edges[i][col_int]);
          }

          string source;
            source = (edges[i][0]);
          int e_id = find_Edges_vec_id(source);

          if (e_id == -1) {
            continue;
          }

          if (gate_tp.edges_id == -1 && e_id != -1) {
            gate_tp.edges_id = e_id;
          }


          if (gate_tp.name == "-1") {
              gate_tp.name = (edges[i][0]);
            if (!pools[e_id].is_in_targets(gate_tp.name)) {
              pools[e_id].targets.push_back(gate_tp);
            }
            continue;
          }
          if (find_gates_vec_id(gate_tp.name) == -1) {
            gates.push_back(gate_tp);
          }
          else
          { 
            int checkstructure_id = find_gates_vec_id(gate_tp.name);

            gates[checkstructure_id].junction_stake = gate_tp.stake;

          }


          pools[e_id].turnouts.push_back(gate_tp);

          string target;
          string target_utf8;
            target = (edges[i][2]);
          if (!pools[e_id].is_in_targets(target)) {
            int target_id = find_gates_vec_id(target);
            if (target_id != -1) {
              pools[e_id].targets.push_back(gates[target_id]);
            }
          }
          }
        }
      }
      for (int i = 0; i < pools.size(); i++) {
        pools[i].id = i;
      }
      for (int i = 0; i < gates.size(); i++) {
        gates[i].id = i;

        if (gates[i].mile_stage < 0) {
          gates[i].mile_stage = gates[i].stake;
        }

      }


    massMatrix.resize(pools.size());
    for (int i = 0; i < massMatrix.size(); i++) {
      massMatrix[i].resize(gates.size());

      for (int s = 0; s < pools[i].source.size(); s++) {
        string name = pools[i].source[s].name;
        Point* g_tp = get_gate_byName(name);
        pools[i].source[s].id = g_tp->id;

        if (g_tp->type == 4 || g_tp->type == 0) {
          massMatrix[i][g_tp->id] = 1;
        }
        g_tp->isCheckStructure = 1;
      }

      for (int s = 0; s < pools[i].turnouts.size(); s++) {
        string name = pools[i].turnouts[s].name;
        Point* g_tp = get_gate_byName(name);
        pools[i].turnouts[s].id = g_tp->id;
        if (g_tp->type == 4 || g_tp->type == 2|| g_tp->type == 6|| g_tp->type == 3|| g_tp->type == 7) {
          massMatrix[i][g_tp->id] = -1;
        }
        if (g_tp->type == 1|| g_tp->type == 5) {
          massMatrix[i][g_tp->id] = 1;
        }
      }

      for (int s = 0; s < pools[i].targets.size(); s++) {
        string name = pools[i].targets[s].name;
        Point* g_tp = get_gate_byName(name);
        pools[i].targets[s].id = g_tp->id;
        massMatrix[i][g_tp->id] = -1;
        g_tp->isCheckStructure = 1;

      }

    }


    for (int i = 0; i < gates.size(); i++) {
      gates_stakes[gates[i].name] = gates[i].stake;
      //cout << gates[i].Flow_ic << endl;
    }


    if (pools.size() > 1) {
      
      int pools_graph_id = 0;
      Graph_Traversal_Algorithms_BFS(&pools[0],pools_graph_id);
    }

    
    max_geodesic_distance = 0;

    Ocis_edges::Point* water_source_g;
    for (int i = 0; i < pools.size(); i++) {
        Ocis_edges::Edges* p = &pools[i];
        p->pool_h = p->pool_h_ic;

      
      vec_graph_id[pools[i].graph_id] = pools[i].id;

      string source_name ;
      for (int s = 0; s < pools[i].source.size(); s++) {
        source_name = pools[i].source[s].name;
      }
      Ocis_edges::Point* s_g =get_gate_byName(source_name);
      /// <summary>
      
      /// </summary>
      /// <param name="edges"></param>
      s_g->isDecisionVariable = true;
      if (s_g->type == 0) {
        //s_g->isDecisionVariable = true;
        water_source_g =s_g;
        for (int e = 0; e < pools[i].targets.size(); e++) {
          int end_id = -1;
          end_id = pools[i].targets[e].id;
          Ocis_edges::Point* e_g = &gates[end_id];
          //e_g->isDecisionVariable = true;

          std::vector<int > path_to_top_node;
          get_recursion_path_pool_id(path_to_top_node, i);
          e_g->path_to_top_node = path_to_top_node;
        
        }
        for (int e = 0; e < pools[i].turnouts.size(); e++) {
          string end_name = pools[i].turnouts[e].name;
          Ocis_edges::Point* e_g = get_gate_byName(end_name);

          std::vector<int > path_to_top_node;
          get_recursion_path_pool_id(path_to_top_node, i);
          e_g->path_to_top_node = path_to_top_node;

        }

      }
      else if (s_g->type != 0) {

        string name_source = pools[i].source[0].name;
        Ocis_edges::Point* g_source = get_gate_byName(name_source);
        for (int e = 0; e < pools[i].targets.size(); e++) {
          string end_name = pools[i].targets[e].name;
          Ocis_edges::Point* e_g = get_gate_byName(end_name);
          //e_g->isDecisionVariable = true;
          if (water_source_g->canal_name == e_g->canal_name) {
          }
          else
          {
            //e_g->stake = s_g->stake + e_g->mile_stage;
            if (e_g->stake > max_geodesic_distance) {
              max_geodesic_distance = e_g->stake;
            }
          }
          std::vector<int > path_to_top_node;
          get_recursion_path_pool_id(path_to_top_node, i);
          e_g->path_to_top_node = path_to_top_node;
        }
        for (int e = 0; e < pools[i].turnouts.size(); e++) {
          string end_name = pools[i].turnouts[e].name;
          Ocis_edges::Point* e_g = get_gate_byName(end_name);
          if (water_source_g->canal_name == e_g->canal_name) {
          }
          else
          {
            if (e_g->stake > max_geodesic_distance) {
              max_geodesic_distance = e_g->stake;
            }

          }
        
          std::vector<int > path_to_top_node;
          get_recursion_path_pool_id(path_to_top_node, i);
          e_g->path_to_top_node = path_to_top_node;

        }
      }
    }
   
    for (auto iter = vec_graph_id.begin(); iter != vec_graph_id.end(); iter++) {
      int i = iter->second;
        string name_source = pools[i].source[0].name;
        Ocis_edges::Point* g_source = get_gate_byName(name_source);
        for (int e = 0; e < pools[i].targets.size(); e++) {
          string end_name = pools[i].targets[e].name;
          Ocis_edges::Point* e_g = get_gate_byName(end_name);
          if (g_source == nullptr) {
            e_g->GeodesicDistance_to_source = e_g->stake;
          }
          else
          {
            double Length = get_g2g_Length(g_source, e_g);
            e_g->GeodesicDistance_to_source = g_source->GeodesicDistance_to_source + Length;
            e_g->GeodesicDistance_in_pool =  Length;
          }
        }
        for (int e = 0; e < pools[i].turnouts.size(); e++) {
          string end_name = pools[i].turnouts[e].name;
          Ocis_edges::Point* e_g = get_gate_byName(end_name);
          if (g_source == nullptr) {
            e_g->GeodesicDistance_to_source = e_g->stake;
          }
          else
          {
            if (g_source->id == 3) {
              int a = 0;
            }
            double Length = get_g2g_Length(g_source, e_g);
            e_g->GeodesicDistance_to_source = g_source->GeodesicDistance_to_source + Length;
            e_g->GeodesicDistance_in_pool =  Length;
          }
        }

    }
    max_geodesic_distance = 0;
    for (int i = 0; i < gates.size(); i++) {
      Point* g = &gates[i];
      if (g->GeodesicDistance_to_source > max_geodesic_distance) {
        max_geodesic_distance = g->GeodesicDistance_to_source;
      }
    }
    cout << "\nmax_geodesic_distance: " << max_geodesic_distance << endl;
    for (int i = 0; i < edges_bak.size(); i++) {
      for (int j = 0; j < gates.size(); j++) {
        if (gates[j].name == edges_bak[i][0] && edges_bak[i][label_col_map["ConnectionType"]] == "indirect") {
          gates[j].maxFlow = stod(edges_bak[i][label_col_map["maxFlow"]]);
          break;
        }
        else if (gates[j].name == edges_bak[i][0] && edges_bak[i][1] == "-1" && edges_bak[i][label_col_map["ConnectionType"]] == "direct") {
          gates[j].maxFlow = stod(edges_bak[i][label_col_map["maxFlow"]]);
          break;
        }
        else if (gates[j].name == edges_bak[i][1] && gates[j].type != 4 && edges_bak[i][label_col_map["ConnectionType"]] == "direct") {
          gates[j].maxFlow = stod(edges_bak[i][label_col_map["maxFlow"]]);
          break;
        }
      }
    }
    std::cout << "=== get_gates ===\n";
  }
  double Ocis_edges::get_g2g_Length(Ocis_edges::Point* g, Ocis_edges::Point* gt) {
    double Length = 100;
    if (g->canal_name == gt->canal_name) {
        Length = gt->stake - g->stake;
    }
    else
    { 
      //if (gt->mile_stage != -1) {
      //  Length = gt->mile_stage;
      //}
      //else
      //{
      //  Length = gt->stake ;
      //}
      if(gt->stake>Length)
        Length = gt->stake ;
    }
    return fabs(Length);
  }
 

  void Ocis_edges::get_recursion_path_pool_id(std::vector<int>& path_pool_id,  int pool_id) {


    
    string name_source = pools[pool_id].source[0].name;
    string name_utf8_source = G2U(name_source);

    path_pool_id.push_back(pool_id);
    
    Edges* e_pre = get_pool_byEndName(name_source);
    if (e_pre != nullptr)
      get_recursion_path_pool_id(path_pool_id,  e_pre->id);

  }

  void Ocis_edges::write_sim_pools_volume(string outputpath) {

    process_T sim_pools_volume;
    for (int i = 0; i < pools.size(); i++) {
      Ocis_edges::Edges* p = &pools[i];
      sim_pools_volume[p->name] = p->sim_pools_volume;
    }

    segments_Write_process_T(outputpath, sim_pools_volume, "sim_pools_volume.csv");
  }
  void Ocis_edges::write_sim_pools_volume(string outputpath,string suffix) {

    process_T sim_pools_volume;
    for (int i = 0; i < pools.size(); i++) {
      Ocis_edges::Edges* p = &pools[i];
      sim_pools_volume[p->name] = p->sim_pools_volume;
    }

    segments_Write_process_T(outputpath, sim_pools_volume, "sim_pools_volume_"+suffix+".csv");
  }


  void Ocis_edges::calc_delaytime(double inflow_max,Point* g,Edges* e,int inflow_start) {

    double rTime = -1;
    double using_stake = -1;
    for (int i = 0; i < e->source.size(); i++) {
      if (gates[e->source[i].id].regulationTime != -1) {
        if (rTime == -1) {
          rTime = 0;
        }
        if (using_stake == -1) {
          using_stake = 0;
        }

        rTime += gates[e->source[i].id].regulationTime;
        using_stake += gates[e->source[i].id].mile_stage;
      }
    }


    double edge_stage = 0;
    if (using_stake != -1) {
      edge_stage = g->stake - using_stake / e->source.size();
    }
    if (rTime != -1) {
      double rTime_hr = (rTime / e->source.size()-inflow_start)/3600;
      g->delaytime = rTime_hr + edge_stage / (inflow_max / ((e->pool_Width + e->pool_m * e->pool_h_ic) * e->pool_h_ic)) / 3600;
      g->regulationTime = rTime + g->delaytime * 3600;
    }
    else
    {
      g->delaytime = g->stake / (inflow_max / ((e->pool_Width +e->pool_m* e->pool_h_ic)*e->pool_h_ic))/3600;
    }

  }

  void Ocis_edges::calc_edge_delaytime(double inflow_max,Point* g,Edges* e,int inflow_start) {

    double rTime = -1;
    double using_stake = -1;
    for (int i = 0; i < e->source.size(); i++) {
      if (gates[e->source[i].id].regulationTime != -1) {
        if (rTime == -1) {
          rTime = 0;
        }
        if (using_stake == -1) {
          using_stake = 0;
        }

        rTime += gates[e->source[i].id].regulationTime;
        using_stake += gates[e->source[i].id].mile_stage;
      }
    }


    double edge_stage = 0;
    if (using_stake != -1) {
      edge_stage = g->mile_stage - using_stake / e->source.size();
    }
    g->delaytime = edge_stage / (inflow_max / ((e->pool_Width + e->pool_m * e->pool_h_ic) * e->pool_h_ic)) / 3600;

    if (g->delaytime < 0) {
      int a = 0;
    }


  }

  std::vector<double> Ocis_edges::Edges::get_turnouts_stakes() {
    std::vector<double >  stake;
    for (int i = 0; i < turnouts.size(); i++) {
      stake.push_back(turnouts[i].stake);
    }
    return stake;
  }





  std::vector<double> Ocis_edges::Edges::get_turnouts_edge_stage() {

    double source_mile_stage = 0;
    for (int i = 0; i < source.size(); i++) {
      source_mile_stage=source[i].mile_stage;
    }
    std::vector<double >  stake;
    for (int i = 0; i < turnouts.size(); i++) {
      double edge_milestage = turnouts[i].mile_stage-source_mile_stage;
      if (edge_milestage < 0) {
      edge_milestage = turnouts[i].mile_stage;
      }
      stake.push_back(edge_milestage);
    }
    return stake;
  }


  std::vector<string> Ocis_edges::Edges::get_turnouts_names() {
    std::vector<string >  names;
    for (int i = 0; i < turnouts.size(); i++) {
      names.push_back(turnouts[i].name);
    }
    return names;
  }
  std::vector<string> Ocis_edges::get_check_structures_name() {
    std::vector<string >  names;
    for (int i = 0; i < pools.size(); i++) {
      for (int j = 0; j < pools[i].source.size(); j++) {
        names.push_back(pools[i].source[j].name);
      }
    }
    return names;
  }


  string Ocis_edges::get_inflow_name() {
    return gates[0].name;
  }

  void Ocis_edges::calc_regulationTime() {
    std::vector<string >  names;
    for (int i = 0; i < pools.size(); i++) {

      double v0 = 0;
      for (int j = 0; j < pools[i].source.size(); j++) {
        v0 += pools[i].source[j].obs_flow/(pools[i].pool_h_ic*pools[i].pool_Width);
      }
      for (int j = 0; j < pools[i].turnouts.size(); j++) {
        pools[i].turnouts[j].regulationTime = pools[i].pool_Length / v0;
      }
      for (int j = 0; j < pools[i].targets.size(); j++) {
        pools[i].targets[j].regulationTime = pools[i].pool_Length / v0;
      }
    }
  }

  Ocis_edges::Point* Ocis_edges::get_gate_byName(string name) {

    for (int i = 0; i < gates.size(); i++) {
      if (gates[i].name == name) {
        return &gates[i];
      }
    }

    return nullptr;
  }

  Ocis_edges::Point* Ocis_edges::get_gate_byID(int id) {

    for (int i = 0; i < gates.size(); i++) {
      if (gates[i].id == id) {
        return &gates[i];
      }
    }

    return nullptr;
  }


  Ocis_edges::Edges* Ocis_edges::get_pool_byName(string name) {

    for (int i = 0; i < pools.size(); i++) {
      if (pools[i].name == name) {
        return &pools[i];
      }
    }

    return nullptr;
  }


  Ocis_edges::Edges* Ocis_edges::get_pool_byTurnoutsName(string name) {
    for (int i = 0; i < pools.size(); i++) {
      for (int j = 0; j < pools[i].turnouts.size(); j++) {
        if (pools[i].turnouts[j].name == name) {
          return &pools[i];
        }
      }
    }

    return nullptr;

  }

  Ocis_edges::Edges* Ocis_edges::get_pool_bySourceName(string name) {

    for (int i = 0; i < pools.size(); i++) {
      for (int j = 0; j < pools[i].source.size(); j++) {
        if (pools[i].source[j].name == name) {
          return &pools[i];
        }
      }
    }

    return nullptr;
  }

  Ocis_edges::Edges* Ocis_edges::get_pool_byEndName(string name) {

    for (int i = 0; i < pools.size(); i++) {
      for (int j = 0; j < pools[i].targets.size(); j++) {
        if (pools[i].targets[j].name == name) {
          return &pools[i];
        }
      }
      for (int j = 0; j < pools[i].turnouts.size(); j++) {
        if (pools[i].turnouts[j].name == name) {
          return &pools[i];
        }
      }

    }

    return nullptr;
  }



  int Ocis_edges::Graph_Traversal_Algorithms_BFS(Edges* e,int& recursion_id) {

    if (e->graph_id == -1) {
      e->graph_id = recursion_id;
        recursion_id++;
    }

    for (int t = 0; t < e->turnouts.size(); t++) {
      string name = e->turnouts[t].name;
      Edges* e = get_pool_bySourceName(name);
      if (e != NULL&&e->graph_id==-1) {
        e->graph_id = recursion_id;
        recursion_id++;
        Graph_Traversal_Algorithms_BFS(e,recursion_id);
        //std::cout << "process" << std::endl;
      }
    }
    for (int t = 0; t < e->targets.size(); t++) {
      string name = e->targets[t].name;
      Edges* e = get_pool_bySourceName(name);
      if (e != NULL&&e->graph_id==-1) {
        e->graph_id = recursion_id;
        recursion_id++;
        Graph_Traversal_Algorithms_BFS(e,recursion_id);

        //std::cout << "process" << std::endl;
      }

    }
    return 1;
  }








  std::map<string, double> Ocis_edges::get_gates_distance() {
    std::map<string, double> name_dist;
    for (int i = 0; i < gates.size(); i++) {
      name_dist[gates[i].name] = gates[i].stake;
    }
    return name_dist;
  }



  std::map<string, std::vector<string > > Ocis_edges::get_check_structures_name_turnouts() {
    std::map<string, std::vector<string > > check_structures_name_turnouts;
    for (int i = 0; i < pools.size(); i++) {

      std::vector<string >  check_names;
      for (int j = 0; j < pools[i].turnouts.size(); j++) {
        for (int k = 0; k < pools[i].source.size(); k++) {

          check_structures_name_turnouts[pools[i].source[k].name].push_back(pools[i].turnouts[j].name);
        }

      }
    }
    return check_structures_name_turnouts;
  }

  bool Ocis_edges::Edges::is_in_turnouts(string& obj) {
    for (int i = 0; i < turnouts.size(); i++) {
      if (turnouts[i].name == obj) {
        return true;
      }
    }
  }

  bool Ocis_edges::Edges::is_in_targets(string& obj) {
    for (int i = 0; i < targets.size(); i++) {
      if (targets[i].name == obj) {
        return true;
      }
    }

    return false;
  }


  double Ocis_edges::Edges::get_sum_outflow(Ocis_edges* edges) {
    
    double sum_outflow = 0;

    for (int i = 0; i < turnouts.size(); i++) {
      string name = turnouts[i].name;
      Ocis_edges::Point* g = edges->get_gate_byName(name);
      sum_outflow += g->Flow_solved;

    }
    
    for (int i = 0; i < targets.size(); i++) {
      string name = targets[i].name;
      Ocis_edges::Point* g = edges->get_gate_byName(name);
      sum_outflow += g->Flow_solved;

    }
    if (sum_outflow < 0) {
      sum_outflow = 0;
    }

    return sum_outflow;
  }
  double Ocis_edges::Edges::set_turnouts_flow(Ocis_edges* edges,double availableFlow) {
    
    double sum_outflow = 0;

    for (int i = 0; i < turnouts.size(); i++) {
      string name = turnouts[i].name;
      Ocis_edges::Point* g = edges->get_gate_byName(name);

      if (g->type == 2&&g->obs_flow!=-1) {
        g->Flow_solved = availableFlow * g->designFlow_ratio_pool;

        if (g->Flow_solved > g->demandFlow) {
          g->Flow_solved = g->demandFlow;
        }

        sum_outflow += g->Flow_solved;
      }

    }
    
    if (sum_outflow < 0) {
      sum_outflow = 0;
    }

    return sum_outflow;
  }


