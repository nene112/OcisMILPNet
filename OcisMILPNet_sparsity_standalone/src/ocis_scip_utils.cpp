#include "ocis_scip_utils.h"
#include <cmath>

// Standalone-retained implementation set.

template<typename ResultType>
void ResultJsonToAction(const json& result_json, ResultType& result, int dt, int T);

OCIS_scip::MILP_param  OCIS_scip::MILP_read_input(string dirpath,string edges_path,string inputjson_path){
     std::cout << "=== MILP_read_input ===\n";
    MILP_param params;
    params.dirpath = dirpath;
    string inputpath = dirpath + "input/";
    h_csv edges = read_h_csv(dirpath + edges_path, ',');
    Ocis_edges edges_utils;
    edges_utils.get_gates(edges);
    params.topo_dicts = edges_utils;

    params.num_pools = params.topo_dicts.pools.size();
    params.num_vars = params.topo_dicts.gates.size();
    params.outputpath = dirpath;

    json input_json = ReadJson(dirpath + inputjson_path);
    if (input_json.contains("CalculationParams")) {
      if (input_json["CalculationParams"].contains("start_time")) {
        params.time_start = input_json["CalculationParams"]["start_time"];
        params.time_start_t = StringToTime_t(params.time_start);
        params.rolling_time_start = input_json["CalculationParams"]["start_time"];
        params.rolling_time_start_t = StringToTime_t(params.rolling_time_start);
      }
      if (input_json["CalculationParams"].contains("work_time_start")) {
        params.work_time_start =
            input_json["CalculationParams"]["work_time_start"].get<string>();
      }
      if (input_json["CalculationParams"].contains("work_time_end")) {
        params.work_time_end =
            input_json["CalculationParams"]["work_time_end"].get<string>();
      }
      // Snap an out-of-window start_time to the work-time start of the same
      // day. Example: work_time_start=08:00, work_time_end=18:00 and
      // start_time=2025-06-13 05:00 -> start_time becomes 2025-06-13 08:00.
      if (!params.work_time_start.empty() && !params.work_time_end.empty()) {
        auto parse_hhmm = [](const string& s, int& minutes) -> bool {
          const size_t colon = s.find(':');
          if (colon == string::npos) return false;
          const int h = atoi(s.substr(0, colon).c_str());
          const int m = atoi(s.substr(colon + 1).c_str());
          if (h < 0 || h > 23 || m < 0 || m > 59) return false;
          minutes = h * 60 + m;
          return true;
        };
        int work_start_min = 0;
        int work_end_min = 0;
        if (parse_hhmm(params.work_time_start, work_start_min) &&
            parse_hhmm(params.work_time_end, work_end_min) &&
            work_end_min > work_start_min) {
          struct tm tmv;
#if defined(_WIN32)
          localtime_s(&tmv, &params.time_start_t);
#else
          localtime_r(&params.time_start_t, &tmv);
#endif
          const int cur_min = tmv.tm_hour * 60 + tmv.tm_min;
          if (cur_min < work_start_min || cur_min >= work_end_min) {
            tmv.tm_hour = work_start_min / 60;
            tmv.tm_min = work_start_min % 60;
            tmv.tm_sec = 0;
            const time_t snapped = mktime(&tmv);
            params.time_start = FormatTime(snapped);
            params.time_start_t = snapped;
            params.rolling_time_start = FormatTime(snapped);
            params.rolling_time_start_t = snapped;
            std::cout << "[WorkTime] start_time outside work window "
                      << params.work_time_start << "~" << params.work_time_end
                      << "; snapped to " << params.time_start << std::endl;
          }
        }
      }
      if (input_json["CalculationParams"].contains("MILP_dt")) {
        params.dt = input_json["CalculationParams"]["MILP_dt"];
      }
      if (input_json["CalculationParams"].contains("T")) {
        params.T = input_json["CalculationParams"]["T"];

        if (params.T * 3600 < params.dt) {
          params.T = params.dt / 3600;
        }

        params.time_vars = params.T*3600 / params.dt;
      }

        if (input_json["CalculationParams"].contains("fixed_designFlow")) {
          int value = input_json["CalculationParams"]["fixed_designFlow"].get<int>();
          params.fixed_designFlow = value;
        }


      if (input_json["CalculationParams"].contains("flow_weight")) {
        params.flow_weight = input_json["CalculationParams"]["flow_weight"];
      }
      if (input_json["CalculationParams"].contains("stage_weight")) {
        params.stage_weight = input_json["CalculationParams"]["stage_weight"];
      }
      if (input_json["CalculationParams"].contains("x_weight")) {
        params.x_weight = input_json["CalculationParams"]["x_weight"];
      }
      if (input_json["CalculationParams"].contains("QGap_weight")) {
        params.QGap_weight = input_json["CalculationParams"]["QGap_weight"];
      }

      if (input_json["CalculationParams"].contains("z_weight")) {
        params.z_weight = input_json["CalculationParams"]["z_weight"];
      }
      if (input_json["CalculationParams"].contains("fabs_boundary_dy_weight")) {
        params.fabs_boundary_dy_weight = input_json["CalculationParams"]["fabs_boundary_dy_weight"];
      }
      if (input_json["CalculationParams"].contains("fabs_dx_weight")) {
        params.fabs_dx_weight = input_json["CalculationParams"]["fabs_dx_weight"];
      }
      if (input_json["CalculationParams"].contains("fabs_dy_weight")) {
        params.fabs_dy_weight = input_json["CalculationParams"]["fabs_dy_weight"];
      }
      if (input_json["CalculationParams"].contains("boundary_stage_cons_label")) {
        params.boundary_stage_cons_label = input_json["CalculationParams"]["boundary_stage_cons_label"];
      }
      if (input_json["CalculationParams"].contains("workTime_L")) {
        params.workTime_L = input_json["CalculationParams"]["workTime_L"].get<int>()*3600;
      }
      if (input_json["CalculationParams"].contains("workTime_R")) {
        params.workTime_R = input_json["CalculationParams"]["workTime_R"].get<int>()*3600;
      }

      if (input_json["CalculationParams"].contains("ref_flow_label")) {
        params.ref_flow_label = input_json["CalculationParams"]["ref_flow_label"].get<int>();
      }

      if (input_json["CalculationParams"].contains("W_based_optimalAllocation")) {
        params.W_based_optimalAllocation = input_json["CalculationParams"]["W_based_optimalAllocation"].get<int>();
      }
      if (input_json["CalculationParams"].contains("MultiDays_delaytime")) {
        params.MultiDays_delaytime = input_json["CalculationParams"]["MultiDays_delaytime"].get<int>();
        if (input_json["CalculationParams"].contains("max_supply_demand_ratio")) {
          params.max_supply_demand_ratio = input_json["CalculationParams"]["max_supply_demand_ratio"].get<double>();
        }
      }
      if (input_json["CalculationParams"].contains("max_action_changes")) {
        params.max_action_changes = input_json["CalculationParams"]["max_action_changes"].get<int>();
      }
      if (input_json["CalculationParams"].contains("step_breakpoints")) {
        params.step_breakpoints = input_json["CalculationParams"]["step_breakpoints"].get<int>();
      }
      if (input_json["CalculationParams"].contains("flow_change_tolerance")) {
        params.flow_change_tolerance = input_json["CalculationParams"]["flow_change_tolerance"].get<double>();
      }
      if (input_json["CalculationParams"].contains("min_delivery_steps")) {
        params.min_delivery_steps =
            input_json["CalculationParams"]["min_delivery_steps"].get<int>();
      }
      if (input_json["CalculationParams"].contains("turnout_max_flow_steps")) {
        params.turnout_max_flow_steps =
            input_json["CalculationParams"]["turnout_max_flow_steps"].get<int>();
      }
      if (input_json["CalculationParams"].contains(
              "turnout_constant_flow_mode")) {
        params.turnout_constant_flow_mode =
            input_json["CalculationParams"]
                ["turnout_constant_flow_mode"].get<int>();
      }
      if (input_json["CalculationParams"].contains("fast_master_enable")) {
        params.fast_master_enable =
            input_json["CalculationParams"]["fast_master_enable"].get<int>();
      }
      if (input_json["CalculationParams"].contains("fast_master_only")) {
        params.fast_master_only =
            input_json["CalculationParams"]["fast_master_only"].get<int>();
      }
      if (input_json["CalculationParams"].contains(
              "fast_master_num_increments")) {
        params.fast_master_num_increments =
            input_json["CalculationParams"]
                ["fast_master_num_increments"].get<int>();
      }
      if (input_json["CalculationParams"].contains(
              "fast_master_candidates_per_exact_solve")) {
        params.fast_master_candidates_per_exact_solve =
            input_json["CalculationParams"]
                ["fast_master_candidates_per_exact_solve"].get<int>();
      }
      if (input_json["CalculationParams"].contains(
              "fast_master_harvest_cuts")) {
        params.fast_master_harvest_cuts =
            input_json["CalculationParams"]
                ["fast_master_harvest_cuts"].get<int>();
      }
      auto read_fast_int = [&](const char* key, int& value) {
        if (input_json["CalculationParams"].contains(key)) {
          value = input_json["CalculationParams"][key].get<int>();
        }
      };
      auto read_fast_double = [&](const char* key, double& value) {
        if (input_json["CalculationParams"].contains(key)) {
          value = input_json["CalculationParams"][key].get<double>();
        }
      };
      read_fast_double("fast_master_initial_increment_ratio",
                       params.fast_master_initial_increment_ratio);
      read_fast_double("fast_master_min_increment_ratio",
                       params.fast_master_min_increment_ratio);
      read_fast_int("fast_master_max_iterations",
                    params.fast_master_max_iterations);
      read_fast_int("fast_master_max_repair_iterations",
                    params.fast_master_max_repair_iterations);
      read_fast_double("fast_master_time_limit_seconds",
                       params.fast_master_time_limit_seconds);
      read_fast_int("fast_master_modes_per_task",
                    params.fast_master_modes_per_task);
      read_fast_int("fast_master_mode_expansion_count",
                    params.fast_master_mode_expansion_count);
      read_fast_double("fast_master_service_weight",
                       params.fast_master_service_weight);
      read_fast_double("fast_master_congestion_weight",
                       params.fast_master_congestion_weight);
      read_fast_double("fast_master_hydraulic_feedback_weight",
                       params.fast_master_hydraulic_feedback_weight);
      read_fast_double("fast_master_default_kappa",
                       params.fast_master_default_kappa);
      read_fast_int("fast_master_use_nonlinear_service_benefit",
                    params.fast_master_use_nonlinear_service_benefit);
      read_fast_int("fast_master_use_previous_solution",
                    params.fast_master_use_previous_solution);
      read_fast_int("fast_master_use_dual_congestion",
                    params.fast_master_use_dual_congestion);
      read_fast_int("fast_master_use_farkas_feedback",
                    params.fast_master_use_farkas_feedback);
      read_fast_int("fast_master_adaptive_increment",
                    params.fast_master_adaptive_increment);
      if (input_json["CalculationParams"].contains("min_open_delivery_ratio")) {
        params.min_open_delivery_ratio =
            input_json["CalculationParams"]["min_open_delivery_ratio"].get<double>();
      }
      if (input_json["CalculationParams"].contains("reach_ramp_max")) {
        params.reach_ramp_max =
            input_json["CalculationParams"]["reach_ramp_max"].get<double>();
      }
      if (input_json["CalculationParams"].contains("demand_flow_upper_label")) {
        params.demand_flow_upper_label =
            input_json["CalculationParams"]["demand_flow_upper_label"].get<int>();
      }
      if (input_json["CalculationParams"].contains("allocation_priority_label")) {
        params.allocation_priority_label =
            input_json["CalculationParams"]["allocation_priority_label"].get<int>();
      }
      if (input_json["CalculationParams"].contains("output_action_dt")) {            
        params.output_action_dt =
            input_json["CalculationParams"]["output_action_dt"].get<int>();
      }
      if (input_json["CalculationParams"].contains("aggregation_level")) {
        params.aggregation_level =
            input_json["CalculationParams"]["aggregation_level"].get<int>();
      }
      if (input_json["CalculationParams"].contains("aggregation_units_path")) {
        params.aggregation_units_path =
            input_json["CalculationParams"]["aggregation_units_path"].get<string>();
      }
      if (input_json["CalculationParams"].contains("master_time_limit")) {
        params.master_time_limit = input_json["CalculationParams"]["master_time_limit"].get<double>();
      }
      if (input_json["CalculationParams"].contains("duration_master_time_limit")) {
        params.duration_master_time_limit = input_json["CalculationParams"]["duration_master_time_limit"].get<double>(); 
      }
      if (input_json["CalculationParams"].contains("scip_max_threads")) {
        params.scip_max_threads = input_json["CalculationParams"]["scip_max_threads"].get<int>(); 
      }
      if (input_json["CalculationParams"].contains("scip_min_threads")) {
        params.scip_min_threads = input_json["CalculationParams"]["scip_min_threads"].get<int>();
      }
      if (input_json["CalculationParams"].contains("scip_parallel_mode")) {
        params.scip_parallel_mode = input_json["CalculationParams"]["scip_parallel_mode"].get<int>();
      }
      if (input_json["CalculationParams"].contains("scip_gap")) {
        params.scip_gap = input_json["CalculationParams"]["scip_gap"].get<double>();
      }
      if (input_json["CalculationParams"].contains("scip_print_statistics")) {
        params.scip_print_statistics = input_json["CalculationParams"]["scip_print_statistics"].get<int>(); 
      }

      if (input_json["CalculationParams"].contains("MILP_Inversion_label")) {
        params.MILP_Inversion_label = input_json["CalculationParams"]["MILP_Inversion_label"].get<int>();
      }
      if (input_json["CalculationParams"].contains("optimal_obj")) {
        params.optimal_obj = input_json["CalculationParams"]["optimal_obj"].get<string>();
      }
      if (input_json["CalculationParams"].contains("fixed_y")) {
        params.fixed_y = input_json["CalculationParams"]["fixed_y"].get<int>();
      }
      if (input_json["CalculationParams"].contains("flood_control_label")) {
        params.flood_control_label = input_json["CalculationParams"]["flood_control_label"].get<int>();
      }
      if (input_json["CalculationParams"].contains("demand_source")) { // "duration" or "series"
        params.demand_source = input_json["CalculationParams"]["demand_source"].get<string>();
        if (input_json["CalculationParams"].contains("max_supply_demand_ratio")) {
          params.max_supply_demand_ratio = input_json["CalculationParams"]["max_supply_demand_ratio"].get<double>(); 
        }
      }
      if (input_json["CalculationParams"].contains("max_action_changes")) {
        params.max_action_changes = input_json["CalculationParams"]["max_action_changes"].get<int>(); 
      }
      if (input_json["CalculationParams"].contains("min_action_changes")) {
        params.min_action_changes = input_json["CalculationParams"]["min_action_changes"].get<int>();
      }
      if (input_json["CalculationParams"].contains("step_breakpoints")) {
        params.step_breakpoints = input_json["CalculationParams"]["step_breakpoints"].get<int>();
      }
      if (input_json["CalculationParams"].contains("flow_change_tolerance")) {
        params.flow_change_tolerance = input_json["CalculationParams"]["flow_change_tolerance"].get<double>();
      }

      const int configured_min_action_changes = params.min_action_changes;
      params.min_action_changes = params.max_action_changes < 0
          ? -1
          : std::min(std::max(-1, params.min_action_changes),
                     params.max_action_changes - 1);
      if (params.min_action_changes != configured_min_action_changes) {
        std::cout << "[CalculationParams] min_action_changes corrected from "
                  << configured_min_action_changes << " to "
                  << params.min_action_changes << std::endl;
      }

    }
    if (input_json.contains("Stage_target_weight")) {
      std::map<string, double> Stage_target_weight = input_json["Stage_target_weight"].get<std::map<string, double> >();


      params.Stage_target_weight.resize(edges_utils.pools.size());
      for (auto& stw : Stage_target_weight) {

        std::string pool_name = (stw.first);
        Ocis_edges::Edges* e = edges_utils.get_pool_byName(pool_name);
        if (e == nullptr) {
          params.Stage_target_weight[e->id] = 10;
        }
        else
        {
          params.Stage_target_weight[e->id] = stw.second;
        }
      }
    }
    // Per-object priority attributes (irr_progress / grain-to-cash-crop ratio)
    // from input/object_priority_overrides.json, if present. These feed the
    // allocation priority in the readable LBBD scheduler. The attributes are
    // copied onto representative gates by get_waterDemand for station demand
    // objects, and re-applied after get_obs so explicit overrides win.
    auto apply_priority_overrides = [&]() {
      for (auto& g : params.topo_dicts.gates) {
        auto it = params.priority_attrs.find(g.name);
        if (it == params.priority_attrs.end()) {
          string u = g.name;
          string gbk = g.name;
          if (isValidUTF8(g.name)) {
            gbk = U2G(g.name);
          } else {
            u = G2U(g.name);
          }
          it = params.priority_attrs.find(u);
          if (it == params.priority_attrs.end()) {
            it = params.priority_attrs.find(gbk);
          }
        }
        if (it != params.priority_attrs.end()) {
          g.irr_progress = it->second.first;
          g.grain_to_cash_crop_ratio = it->second.second;
        }
      }
    };
    {
      const string overrides_path = inputpath + "object_priority_overrides.json";
      if (isExist(overrides_path)) {
        json overrides_json = ReadJson(overrides_path);
        for (auto& item : overrides_json.items()) {
          double irr = 1.0;
          double grain = 1.0;
          if (item.value().contains("irr_progress")) {
            irr = item.value()["irr_progress"].get<double>();
          }
          if (item.value().contains("grain_to_cash_crop_ratio")) {
            grain = item.value()["grain_to_cash_crop_ratio"].get<double>();
          }
          params.priority_attrs[item.key()] = {irr, grain};
        }
        apply_priority_overrides();
        std::cout << "[PriorityAttrs] loaded "
                  << params.priority_attrs.size()
                  << " overrides from " << overrides_path << std::endl;
      }
    }

    if (input_json.contains("waterDemand")) {
      get_waterDemand(input_json["waterDemand"], params);
      process_T Q_series_from_input_json =
        get_waterDemand_from_input_json(input_json["waterDemand"], params);
        // Keep the legacy per-gate series and merge the station-representative
        // demand Q series added by get_waterDemand (rep gates), so LBBD zone
        // demand profiles (and the min-open-flow ratio) cover those gates too.
        for (auto& kv : params.boundary_flow) {
          if (!Q_series_from_input_json.count(kv.first)) {
            Q_series_from_input_json[kv.first] = kv.second;
          }
        }
        params.boundary_flow = Q_series_from_input_json;
        segments_Write_process_T(inputpath, params.boundary_flow, "waterdemand.csv");
    }

    if (input_json.contains("obs")) {
      get_obs(input_json["obs"], params);
    }
    // Explicit overrides take precedence over obs attributes.
    apply_priority_overrides();

    if (input_json.contains("ReTime")) {
      get_ReTime(input_json["ReTime"], params);
    }


    if (input_json.contains("bc_Flow")) {
      set_bcFlow(input_json["bc_Flow"], params);
    }
    if (input_json.contains("soft_Flow_cons_label")) {
      set_soft_bcFlow_cons(input_json["soft_Flow_cons_label"], params);
    }

    if (input_json.contains("bc_Stage")) {
      set_bcStage(input_json["bc_Stage"], params);
    }

    //derive_type2_demand_from_boundary_flow_if_needed(*this, params);


    params.gates_max_flow.resize(params.topo_dicts.gates.size());
    params.gates_min_flow.resize(params.topo_dicts.gates.size());
    params.gates_stake.resize(params.topo_dicts.gates.size());
    for (int i = 0; i < params.topo_dicts.gates.size(); i++) {
      params.gates_max_flow[i] = params.topo_dicts.gates[i].maxFlow;
      params.gates_min_flow[i] =  params.topo_dicts.gates[i].minFlow;
      params.gates_stake[i] = params.topo_dicts.gates[i].stake;
    }


    return params;
  }

void OCIS_scip::MILP_write_csv(MILP_param& input_param, MILP_result& result, string dirpath,string var_name) {

     std::vector<string> gates_names;
     gates_names.push_back("tm");
     for (int i = 0; i < input_param.num_vars; i++) {
       string name_utf8 = (input_param.topo_dicts.gates[i].name);
       gates_names.push_back(name_utf8);

     }


     process_T action_td;
     time_t rolling_time_t = StringToTime_t(input_param.time_start);
     h_csv action = create_h_csv(input_param.time_vars, input_param.num_vars + 1);
     for (int i = 0; i < input_param.time_vars; i++) {
       time_t rolling_time_ttemp = rolling_time_t + i * input_param.dt;
       action[i][0] = FormatTime(rolling_time_ttemp);
       for (int j = 0; j < input_param.num_vars; j++) {
         string name = input_param.topo_dicts.gates[j].name;
         string name_utf8 = name;
         name_utf8 = (name);

         if (i < result.solution[var_name][name].size()) {
           action[i][j + 1] = to_string(fabs(result.solution[var_name][name][i]));
           action_td[name_utf8][(rolling_time_ttemp)] = fabs(result.solution[var_name][name][i]);

         }

       }
     }
     action.emplace(action.begin(), gates_names);  
     write_h_csv(dirpath+var_name+".csv", action, ',');

     write_tidyData(dirpath + var_name + "_td.csv", action_td);
  }

void OCIS_scip::MILP_write_csv_pools(MILP_param& input_param, MILP_result& result, string dirpath,string var_name) {
////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// 
    std::map<string, string> config_map;
    string config_path = input_param.input_path + "config.txt";
    h_csv config_csv;
    if (isExist(config_path)) {
      config_csv = read_h_csv(config_path,',');
      for (auto& cc : config_csv) {
        if(cc.size()>=2){
          config_map[cc[1]] = cc[0];
        }
      }

    }


     std::vector<string> gates_names;
     std::vector<string> encode_names;
     gates_names.push_back("tm");
     encode_names.push_back("tm");
     auto iterQ_begin = result.solution[var_name].begin();
     for (auto& iterQ = iterQ_begin; iterQ != result.solution[var_name].end(); iterQ++) {
       string name = iterQ->first;
       string name_utf8 = (name);
       string name_encode;
       if (config_map.contains(name) ) {
         name_encode = config_map[name];
       }
       if ( config_map.contains(name_utf8)) {
         name_encode = config_map[name_utf8];
       }
       if (config_map.contains(name) && !config_map.contains(name_utf8)) {
         config_map[name_utf8]=config_map[name];
       }
       if (config_map.contains(name_utf8) && !config_map.contains(name)) {
         config_map[name]=config_map[name_utf8];
       }
       if (!config_map.contains(name_utf8) && !config_map.contains(name)) {
         name_encode = name_utf8;
       }



       gates_names.push_back(name_utf8);
       encode_names.push_back(name_encode);

     }

     process_obj_T obj_action;
     process_T action_td;
     time_t rolling_time_t = StringToTime_t(input_param.time_start);
     if (result.solution.find(var_name) != result.solution.end()) {
       h_csv action = create_h_csv(input_param.time_vars, result.solution[var_name].size() + 1);
       for (int i = 0; i < input_param.time_vars; i++) {
         time_t rolling_time_ttemp = rolling_time_t + (i) * input_param.dt;
         action[i][0] = FormatTime(rolling_time_ttemp);
         //for (int j = 0; j < input_param.num_vars; j++) {
         auto iterQ_begin = result.solution[var_name].begin();
         int j = 0;
         for (auto& iterQ = iterQ_begin; iterQ != result.solution[var_name].end(); iterQ++) {
           string name = iterQ->first;
           string name_utf8 = name;
           name_utf8 = (name);

           if (i < result.solution[var_name][name].size()) {
             action[i][j + 1] = to_string(fabs(result.solution[var_name][name][i]));
             action_td[name_utf8][(rolling_time_ttemp)] = fabs(result.solution[var_name][name][i]);
           }
           j++;
         }
       }
       h_csv action_encode = action;
       process_T action_encode_td;

       for (auto& at : action_td) {
         string name = at.first;
         string name_encode;
         if (config_map.contains(name)) {
           name_encode = config_map[name];
         }
         else
         {
           name_encode = name;
         }
         action_encode_td[name_encode] = at.second;
       }

       action.emplace(action.begin(), gates_names);  
       action_encode.emplace(action_encode.begin(), encode_names);  

       write_h_csv(dirpath + var_name+".csv", action, ',');
       write_h_csv(dirpath + var_name + "_encode.csv", action_encode,',');
       write_tidyData(dirpath + var_name+ "_td.csv", action_td);
       write_tidyData(dirpath + var_name+ "_encode_td.csv", action_encode_td);
     }
  }

void OCIS_scip::get_value(MILP_result& result,string var_name,string name,int j,std::vector<double>& vec) {
      if (result.solution.find(var_name) != result.solution.end() && result.solution[var_name].find(name) != result.solution[var_name].end()&&result.solution[var_name][name].size()>j) {
        double STime = result.solution[var_name][name][j];
        vec.push_back(STime);
      }

  }

nlohmann::ordered_json OCIS_scip::get_gates_schedule_scheme_by_pool_MultiDays(
    MILP_param& input_param,
    MILP_result& result,
    Ocis_edges::Point* g,
    Ocis_edges::Point* g_source,
    int pool_id,
    h_csv& tidy_data
    ) {
    time_t start_of_the_day = GetStartOfDay(input_param.time_start_t);
    time_t end_of_the_day = GetEndOfDay(input_param.time_start_t);
    time_t end_of_the_simulation = GetEndOfDay(start_of_the_day + input_param.time_vars * input_param.dt);
    time_t start_of_the_regulation = (input_param.time_start_t);
    string time_str = input_param.time_start;

    nlohmann::ordered_json  result_json = nlohmann::json::array();;
    nlohmann::ordered_json  sub_result_json;
    string name_source = g_source->name;
    string name_utf8_source = (name_source);

    string name = g->name;
    string name_utf8 = (name);

    std::vector<double> Q_vec;
    std::vector<double> STime_vec;
    std::vector<double> ETime_vec;
    std::vector<double> RTime_vec;
    std::vector<double> duration_vec;

    std::vector<double> UQ_vec;
    std::vector<double> USTime_vec;
    std::vector<double> UETime_vec;

    tidy_data.resize(input_param.time_vars);
    for (int j = 0; j < input_param.time_vars; j++) {
      get_value(result, "Q", name, j, Q_vec);
      get_value(result, "UQ", name, j, UQ_vec);
      get_value(result, "STime", name, j, STime_vec);
      get_value(result, "ETime", name, j, ETime_vec);
      get_value(result, "RTime", name, j, RTime_vec);
      get_value(result, "USTime", name, j, USTime_vec);
      get_value(result, "UETime", name, j, UETime_vec);
      get_value(result, "duration", name, j, duration_vec);

    }

    
    
    
    const bool single_snapshot = (STime_vec.size() == 1);
    int snapshot_i = 0;
    if (single_snapshot) {
      snapshot_i = input_param.dt > 0
          ? static_cast<int>(std::llround(STime_vec[0] / input_param.dt))
          : 0;
      if (snapshot_i <= 0) {
        for (int j = 0; j < static_cast<int>(Q_vec.size()); ++j) {
          if (Q_vec[j] > 1.0e-8) {
            snapshot_i = j;
            break;
          }
        }
      }
      snapshot_i = std::max(
          0, std::min(snapshot_i, input_param.time_vars - 1));
    }

    // LBBD exports one STime/ETime value and a full gate-flow series. Build
    // schedule blocks directly from that series: Q already includes hydraulic
    // travel delay, so adding RTime again would double-count it. Absolute
    // timestamps are anchored at time_start_t, not at midnight.
    if (single_snapshot) {
      double lag_s = RTime_vec.empty() ? 0.0 : RTime_vec[0];
      auto lag_it = result.solution.find("lag_s");
      if (lag_it != result.solution.end()) {
        auto gate_it = lag_it->second.find(name);
        if (gate_it != lag_it->second.end() && !gate_it->second.empty()) {
          lag_s = gate_it->second[0];
        }
      }
      int open_rank = -1;
      auto rank_it = result.solution.find("open_rank");
      if (rank_it != result.solution.end()) {
        auto gate_it = rank_it->second.find(name);
        if (gate_it != rank_it->second.end() && !gate_it->second.empty()) {
          open_rank = static_cast<int>(gate_it->second[0]);
        }
      }

      double schedule_shift_s = 0.0;
      auto open_it = result.solution.find("STime_cont");
      if (open_it != result.solution.end()) {
        auto gate_it = open_it->second.find(name);
        if (gate_it != open_it->second.end() && !gate_it->second.empty()) {
          int first_positive = 0;
          while (first_positive < static_cast<int>(Q_vec.size()) &&
                 Q_vec[first_positive] <= 1.0e-8) {
            ++first_positive;
          }
          schedule_shift_s = std::max(
              0.0, gate_it->second[0] -
                       static_cast<double>(first_positive * input_param.dt));
        }
      }

      const int n = static_cast<int>(Q_vec.size());
      for (int begin = 0; begin < n;) {
        if (Q_vec[begin] <= 1.0e-8) {
          ++begin;
          continue;
        }
        int end = begin + 1;
        while (end < n && Q_vec[end] > 1.0e-8 &&
               std::fabs(Q_vec[end] - Q_vec[begin]) <= 1.0e-9) {
          ++end;
        }

        const time_t input_s_t = input_param.time_start_t +
            static_cast<time_t>(begin) * input_param.dt;
        const time_t input_e_t = input_param.time_start_t +
            static_cast<time_t>(end) * input_param.dt;
        const time_t s_t = input_s_t +
            static_cast<time_t>(std::llround(schedule_shift_s));
        const time_t e_t = input_e_t +
            static_cast<time_t>(std::llround(schedule_shift_s));
        const double q = Q_vec[begin];
        const double q_input = begin < static_cast<int>(UQ_vec.size())
            ? UQ_vec[begin] : q;

        nlohmann::ordered_json block;
        block["q"] = q;
        block["q_input"] = q_input;
        block["s"] = FormatTime(s_t);
        block["s_input"] = FormatTime(input_s_t);
        block["s_delaytime"] = FormatTime(s_t);
        block["e"] = FormatTime(e_t);
        block["e_input"] = FormatTime(input_e_t);
        block["type_suffix"] = g->type;
        block["decision_type_suffix"] = g->isDecisionVariable;
        block["obs_flow"] = g->obs_flow;
        block["lag_s"] = lag_s;
        block["open_rank"] = open_rank;
        block["W"] = q * static_cast<double>(e_t - s_t);
        block["W_input"] = q_input * static_cast<double>(e_t - s_t);
        block["q_maxFlow"] = g->maxFlow;
        result_json.push_back(block);

        tidy_data[begin] = {
            name_utf8, to_string(q), FormatTime(s_t),
            FormatTime(s_t), FormatTime(e_t) };
        begin = end;
      }
      return result_json;
    }

    for (int i = 0; i < input_param.time_vars; i++) {

      if (single_snapshot) {
        if (i != snapshot_i) continue;
      } else if (STime_vec.size() <= i) {
        continue;
      }
      
      
      const int vi = single_snapshot ? 0 : i;

      if (STime_vec[vi] > i * input_param.dt + input_param.workTime_R) {
        STime_vec[vi] = 0;
      }

      string s_str = FormatTime(start_of_the_day +i * input_param.dt + RTime_vec[vi]);
      if (USTime_vec[vi] < 0) {
        USTime_vec[vi] = 0;
      }
      if (start_of_the_day + i * input_param.dt  > g->ETime_input&&g->ETime_input>0) {
        continue;
        s_str = "";
      }

      // For the LBBD single-block snapshot the input times (USTime/UETime)
      // already carry the absolute period offset; adding i*dt again would push
      // a late-opening gate's s_input a whole block further out.
      const time_t input_base = single_snapshot
          ? 0 : static_cast<time_t>(i) * input_param.dt;
      string s_input_str = FormatTime(start_of_the_day + input_base + USTime_vec[vi]);
      string s_delaytime_str = FormatTime(start_of_the_day + i * input_param.dt + RTime_vec[vi]);



      if (ETime_vec[vi] > end_of_the_day) {
        ETime_vec[vi] = end_of_the_day - start_of_the_day;
      }

      if (Q_vec[i] > 0) {
        ETime_vec[vi] = g->W_input / Q_vec[i];
      }
      else
      {
        ETime_vec[vi] = input_param.T * 3600;
      }
      if (start_of_the_day + ETime_vec[vi] > start_of_the_day + input_param.T * 3600 ||
        start_of_the_day + ETime_vec[vi] < start_of_the_day + input_param.time_start_t
        ) {
        ETime_vec[vi] = input_param.T * 3600;
      }

      string e_str = FormatTime(start_of_the_day + ETime_vec[vi]);
      if (UETime_vec[vi] < 0) {
        UETime_vec[vi] = 0;
      }

      string e_input_str = FormatTime(start_of_the_day + input_base + UETime_vec[vi]);
      if (start_of_the_day + i * input_param.dt + UETime_vec[vi] == start_of_the_day + i * input_param.dt + input_param.workTime_R) {
        //e_input_str = "";
      }
      if (e_input_str == FormatTime(start_of_the_day + i * input_param.dt)) {
        e_input_str = FormatTime(start_of_the_day + 1 * input_param.dt);
      }
      if (e_input_str != ""&&StringToTime_t(s_str) > StringToTime_t(e_input_str)) {
        s_str =FormatTime(start_of_the_day + i * input_param.dt);
      }


      string end_of_the_day_str = FormatTime(end_of_the_day );
      if (Q_vec[i] < 0.001) {
        Q_vec[i] = 0;
      }

      
      
      
      {
        auto it_s = result.solution.find("STime_cont");
        auto it_e = result.solution.find("ETime_cont");
        if (it_s != result.solution.end()) {
          auto it_g = it_s->second.find(name);
          if (it_g != it_s->second.end() && !it_g->second.empty()) {
            s_delaytime_str = FormatTime(
                start_of_the_day +
                static_cast<time_t>(it_g->second[0]));
            s_str = s_delaytime_str;
          }
        }
        if (it_e != result.solution.end()) {
          auto it_g = it_e->second.find(name);
          if (it_g != it_e->second.end() && !it_g->second.empty()) {
            e_str = FormatTime(
                start_of_the_day +
                static_cast<time_t>(it_g->second[0]));
          }
        }
      }

    // Post-process schedule start/end times into the work window
    // (work_time_start ~ work_time_end, default 08:00~18:00): snap the
    // time-of-day to the nearest work boundary so the summary never reports a
    // delivery starting/ending outside work hours.
    int ws_min = 8 * 60, we_min = 18 * 60;
    {
      int h = 0, m = 0;
      if (sscanf(input_param.work_time_start.c_str(), "%d:%d", &h, &m) == 2 &&
          h >= 0 && h <= 23 && m >= 0 && m <= 59) {
        ws_min = h * 60 + m;
      }
      if (sscanf(input_param.work_time_end.c_str(), "%d:%d", &h, &m) == 2 &&
          h >= 0 && h <= 23 && m >= 0 && m <= 59) {
        we_min = h * 60 + m;
      }
      if (we_min <= ws_min) {
        ws_min = 8 * 60;
        we_min = 18 * 60;
      }
    }
    auto snap_sched_time = [&](const string& ts) -> string {
      time_t t = StringToTime_t(ts);
      if (t <= 0) return ts;
      struct tm tmv;
#if defined(_WIN32)
      localtime_s(&tmv, &t);
#else
      localtime_r(&t, &tmv);
#endif
      const int tod_min = tmv.tm_hour * 60 + tmv.tm_min;
      if (tod_min < ws_min) {
        tmv.tm_hour = ws_min / 60;
        tmv.tm_min = ws_min % 60;
      } else if (tod_min > we_min) {
        tmv.tm_hour = we_min / 60;
        tmv.tm_min = we_min % 60;
      }
      tmv.tm_sec = 0;
      return FormatTime(mktime(&tmv));
    };
    s_delaytime_str = snap_sched_time(s_delaytime_str);
    s_input_str = snap_sched_time(s_input_str);
    e_str = snap_sched_time(e_str);
    e_input_str = snap_sched_time(e_input_str);
    if (StringToTime_t(e_str) <= StringToTime_t(s_delaytime_str)) {
      time_t s_t = StringToTime_t(s_delaytime_str);
      struct tm stm;
#if defined(_WIN32)
      localtime_s(&stm, &s_t);
#else
      localtime_r(&s_t, &stm);
#endif
      stm.tm_hour = we_min / 60;
      stm.tm_min = we_min % 60;
      stm.tm_sec = 0;
      stm.tm_mday += 1;
      e_str = FormatTime(mktime(&stm));
    }

    
    double rounded = std::round(Q_vec[i] * 1000.0) / 1000.0;
      sub_result_json["q"] = rounded;
      sub_result_json["q_input"] = UQ_vec[i];
      sub_result_json["s"] = s_delaytime_str;
      sub_result_json["s_input"] = s_input_str;
      sub_result_json["s_delaytime"] = s_delaytime_str;
      sub_result_json["e"] = e_str;
      sub_result_json["e_input"] = e_input_str;
      sub_result_json["type_suffix"] = g->type;
      sub_result_json["decision_type_suffix"] = g->isDecisionVariable;
      sub_result_json["obs_flow"] = g->obs_flow;
      sub_result_json["lag_s"] = "-1";
      sub_result_json["open_rank"] = "-1";
      {
        auto it_lag = result.solution.find("lag_s");
        if (it_lag != result.solution.end()) {
          auto it_g = it_lag->second.find(name);
          if (it_g != it_lag->second.end() && !it_g->second.empty()) {
            sub_result_json["lag_s"] = it_g->second[0];
          }
        }
        auto it_rank = result.solution.find("open_rank");
        if (it_rank != result.solution.end()) {
          auto it_g = it_rank->second.find(name);
          if (it_g != it_rank->second.end() && !it_g->second.empty()) {
            sub_result_json["open_rank"] =
                static_cast<int>(it_g->second[0]);
          }
        }
      }

      int duration_solve = StringToTime_t(e_str) - StringToTime_t(s_str);
      double W_solve = rounded * duration_solve;
      sub_result_json["W"] = W_solve;
 
      int duration_input = StringToTime_t(e_input_str) - StringToTime_t(s_input_str);
      double W_input = UQ_vec[i] * duration_input;
      sub_result_json["W_input"] = W_input;



      tidy_data[i].push_back(name_utf8);
      tidy_data[i].push_back(to_string(Q_vec[i]));
      tidy_data[i].push_back(s_str);
      tidy_data[i].push_back(s_delaytime_str);
      tidy_data[i].push_back(e_str);


      if (fabs(ETime_vec[vi] - STime_vec[vi])<10) {
        //sub_result_json["e"] = "-";
        //sub_result_json["e_input"] = "-";
      }

      //if (!g->isCheckStructure&&(!g->isDecisionVariable && UQ_vec[i] == -1)) {
      //  sub_result_json["q"] = g->obs_flow;
      //  sub_result_json["q_input"] = g->obs_flow;
      //  sub_result_json["s"] = "-";
      //  sub_result_json["s_input"] = "-";
      //  //sub_result_json["s_delaytime"] = "-";
      //  sub_result_json["e"] = "-";
      //  sub_result_json["e_input"] = "-";
      //}
      if (g->isCheckStructure && UQ_vec[i] == -1) {
        //sub_result_json["q"] = g->obs_flow;
        sub_result_json["q_input"] = g->obs_flow;
        //sub_result_json["s"] = "-";
        //sub_result_json["s_input"] = "-";
        //sub_result_json["s_delaytime"] = "-";
        //sub_result_json["e"] = "-";
        //sub_result_json["e_input"] = "-";
      }
      sub_result_json["q_maxFlow"] = g->maxFlow;
      result_json.push_back(sub_result_json);
    }


    return result_json;


  }

nlohmann::ordered_json OCIS_scip::MILP_write_schedule_MultiDays(MILP_param& input_param, MILP_result& result, string dirpath) {
 

    h_csv tidy_data_of_schedule;
    std::vector<string> tidy_label = { "obj","q","s","s_delaytime","e"};
    tidy_data_of_schedule.push_back(tidy_label);
  
    nlohmann::ordered_json  result_json;

    for (int i = 0; i < input_param.num_pools; i++) {
      string name = input_param.topo_dicts.pools[i].source[0].name;
      string name_utf8 = (name);
      Ocis_edges::Point* g = input_param.topo_dicts.get_gate_byName(name);


      for (int j = 0; j < input_param.topo_dicts.pools[i].source.size(); j++) {
        string name_t = input_param.topo_dicts.pools[i].source[j].name;
        string name_utf8_t = (name_t);
        Ocis_edges::Point* g_t = input_param.topo_dicts.get_gate_byName(name_t);

        h_csv tidy_data;
        nlohmann::ordered_json  result_json_temp = get_gates_schedule_scheme_by_pool_MultiDays(input_param, result, g_t,g,i,tidy_data);

        if (!result_json_temp.is_null() || !result_json_temp.empty()) {
          result_json[name_utf8_t] = result_json_temp;
        }


        tidy_data_of_schedule.insert(tidy_data_of_schedule.end(), std::begin(tidy_data), std::end(tidy_data));
        //tidy_data_of_schedule.push_back(tidy_data);
      }


      for (int j = 0; j < input_param.topo_dicts.pools[i].turnouts.size(); j++) {
        string name_t = input_param.topo_dicts.pools[i].turnouts[j].name;
        string name_utf8_t = (name_t);
        Ocis_edges::Point* g_t = input_param.topo_dicts.get_gate_byName(name_t);
        if (g_t->isDecisionVariable) {
          int a = 0;
        }

        h_csv tidy_data;
        nlohmann::ordered_json  result_json_temp = get_gates_schedule_scheme_by_pool_MultiDays(input_param, result, g_t,g,i,tidy_data);
        if (!result_json_temp.is_null() || !result_json_temp.empty()) {
          result_json[name_utf8_t] = result_json_temp;
        }
        tidy_data_of_schedule.insert(tidy_data_of_schedule.end(), std::begin(tidy_data), std::end(tidy_data));
      }

      for (int j = 0; j < input_param.topo_dicts.pools[i].targets.size(); j++) {
        string name_t = input_param.topo_dicts.pools[i].targets[j].name;
        string name_utf8_t = (name_t);
        Ocis_edges::Point* g_t = input_param.topo_dicts.get_gate_byName(name_t);
        Ocis_edges::Edges* e_next = input_param.topo_dicts.get_pool_bySourceName(name_t);
        if (e_next == nullptr) {

          if (g_t->isDecisionVariable) {
            int a = 0;
          }

          h_csv tidy_data;
          nlohmann::ordered_json  result_json_temp = get_gates_schedule_scheme_by_pool_MultiDays(input_param, result, g_t, g, i, tidy_data);
          if (!result_json_temp.is_null() || !result_json_temp.empty()) {
            result_json[name_utf8_t] = result_json_temp;
          }
          tidy_data_of_schedule.insert(tidy_data_of_schedule.end(), std::begin(tidy_data), std::end(tidy_data));
        }
      }



    }
    string output_file_name = input_param.outputpath + "schedule_MultiDays.json";
    std::ofstream out(output_file_name, std::ios::binary);
    out << std::fixed << std::setprecision(2);
    out << result_json.dump(4);

    string output_tidy_data_of_schedule = input_param.outputpath  + "tidy_data_of_schedule.csv";
    write_h_csv(output_tidy_data_of_schedule, tidy_data_of_schedule, ',');

    output_tidy_data_of_schedule = input_param.outputpath + "duration_td.csv";
    write_h_csv(output_tidy_data_of_schedule, tidy_data_of_schedule, ',');


    return result_json;

  }

double action_value_at(
      const OCIS_scip::MILP_result& result,
      const Ocis_edges::Point* gate,
      const string& name,
      double t_out,
      int source_dt,
      int output_dt,
      double horizon_s) {
    (void)gate;
    auto itq = result.solution.find("Q");
    if (itq == result.solution.end()) itq = result.solution.find("action");
    if (itq != result.solution.end()) {
      auto itg = itq->second.find(name);
      if (itg != itq->second.end()) {
        const auto& qvec = itg->second;
        double period_seconds = std::max(1, source_dt);
        auto meta = result.solution.find("LBBD_meta");
        if (meta != result.solution.end()) {
          auto value = meta->second.find("period_seconds");
          if (value != meta->second.end() && !value->second.empty() &&
              value->second[0] > 0.0) {
            period_seconds = value->second[0];
          }
        }
        // A constant-flow LBBD turnout is one physical opening block. Direct
        // overlap averaging would create two artificial partial-flow levels
        // at its boundaries (four reported changes instead of open + close).
        // Spread the same volume uniformly over the complete output bins that
        // cover the block, preserving both volume and the two-change process.
        auto turnout = result.solution.find("LBBD_turnout_Q");
        bool constant_turnout = turnout != result.solution.end() &&
            turnout->second.find(name) != turnout->second.end();
        int first_positive = -1;
        int last_positive = -1;
        double open_q = -1.0;
        double source_volume = 0.0;
        for (int pi = 0; pi < static_cast<int>(qvec.size()); ++pi) {
          const double value = fabs(qvec[pi]);
          source_volume += value * period_seconds;
          if (value <= 1.0e-10) continue;
          if (first_positive < 0) {
            first_positive = pi;
            open_q = value;
          } else if (fabs(value - open_q) >
                     1.0e-7 * std::max(1.0, open_q)) {
            constant_turnout = false;
          }
          last_positive = pi;
        }
        if (constant_turnout && first_positive >= 0) {
          const double bin = static_cast<double>(std::max(1, output_dt));
          const double block_start = std::floor(
              first_positive * period_seconds / bin) * bin;
          const double block_end = std::min(
              horizon_s, std::ceil((last_positive + 1) * period_seconds / bin) *
                  bin);
          if (block_end > block_start) {
            return t_out >= block_start && t_out < block_end
                ? source_volume / (block_end - block_start) : 0.0;
          }
        }
        const double interval_end = std::min(
            horizon_s, t_out + static_cast<double>(std::max(1, output_dt)));
        if (interval_end <= t_out) return 0.0;
        const int first = std::max(
            0, static_cast<int>(std::floor(t_out / period_seconds)));
        const int last = std::min(
            static_cast<int>(qvec.size()),
            static_cast<int>(std::ceil(interval_end / period_seconds)));
        double volume = 0.0;
        for (int pi = first; pi < last; ++pi) {
          const double overlap = std::max(
              0.0, std::min(interval_end, (pi + 1) * period_seconds) -
                  std::max(t_out, pi * period_seconds));
          volume += fabs(qvec[pi]) * overlap;
        }
        return volume / (interval_end - t_out);
      }
    }
    return 0.0;
  }

void OCIS_scip::MILP_write_output(MILP_param& input_param,MILP_result& result,string dirpath,string filename){
     std::cout << "=== MILP_write_output ===\n";

     const bool action_output_requested =
         filename.find("action") != string::npos;

     mkdir_h(dirpath + "output/");
     dirpath = dirpath + "output/";

     //MILP_write_schedule(input_param, result, dirpath);
     
     
     std::vector<string> gates_names;
     gates_names.push_back("tm");
     //for (int i = 0; i < input_param.num_vars; i++) {
        auto iterQ_begin = result.solution["Q"].begin();
        for (auto& iterQ = iterQ_begin; iterQ != result.solution["Q"].end(); iterQ++) {
           string name = iterQ->first;
       string name_utf8 = isValidUTF8(name) ? name : G2U(name);
       gates_names.push_back(name_utf8);

     }

     json action_j;
     process_obj_T obj_action;
     process_T action_td;
     time_t rolling_time_t = StringToTime_t(input_param.time_start);
     if (result.solution.find("Q") != result.solution.end()) {
       // action.csv output time step: densify to output_action_dt when > 0,
       // covering [time_start, time_start + T*3600]. Before a gate opens:
       // obs_flow or 0; while open: scheduled flow; after close: 0.
       const int out_dt = input_param.output_action_dt > 0
           ? input_param.output_action_dt : input_param.dt;
       const time_t horizon_s = static_cast<time_t>(input_param.T) * 3600;
       const int out_steps = static_cast<int>(
           (static_cast<long long>(horizon_s) + out_dt - 1) / out_dt);
       h_csv action = create_h_csv(
           out_steps, result.solution["Q"].size() + 1);
       for (int i = 0; i < out_steps; i++) {
        const time_t t_out = std::min(
            static_cast<time_t>(i) * out_dt, horizon_s);
        time_t rolling_time_ttemp = rolling_time_t + t_out;
        action[i][0] = FormatTime(rolling_time_ttemp);
         //for (int j = 0; j < input_param.num_vars; j++) {
        auto iterQ_begin = result.solution["Q"].begin();
        int j = 0;
        for (auto& iterQ = iterQ_begin; iterQ != result.solution["Q"].end(); iterQ++) {
           string name = iterQ->first;
           string name_utf8 = isValidUTF8(name) ? name : G2U(name);

           Ocis_edges::Point* gate =
               input_param.topo_dicts.get_gate_byName(name);
           const double value = action_value_at(
               result, gate, name,
               static_cast<double>(t_out), input_param.dt, out_dt,
               static_cast<double>(horizon_s));
           action[i][j + 1] = to_string(value);
           action_td[name_utf8][rolling_time_ttemp] = fabs(value);
           action_j[name_utf8][FormatTime(rolling_time_ttemp)] =
               to_string(fabs(value));
           j++;
         }
       }
       action.emplace(action.begin(), gates_names);  


       if (action_output_requested) {
         mkdir_h(dirpath + "action/");
         write_h_csv(input_param.outputpath + "action.csv", action, ',');
         write_h_csv(dirpath + "Q.csv", action, ',');
         write_h_csv(dirpath + filename, action, ',');
         write_h_csv(input_param.input_path + "action.csv", action, ',');
         write_h_csv(input_param.outputpath + "channel_Gates_Stastic_Q_action.csv", action, ',');
         write_tidyData(dirpath + "action/action_td.csv", action_td);
         write_tidyData(dirpath + "Q_td.csv", action_td);
         write_tidyData(input_param.input_path + "action_td.csv", action_td);
       }
       if (filename == "waterallocation_td.csv") {
         write_tidyData(dirpath + "waterallocation_td.csv", action_td);
         write_tidyData(input_param.input_path + "waterallocation_td.csv", action_td);
       }
       if (filename == "hdcm_waterallocation_td.csv") {
         write_tidyData(dirpath + "hdcm_waterallocation_td.csv", action_td);
         write_tidyData(input_param.input_path + "hdcm_waterallocation_td.csv", action_td);
       }
       WriteJson(action_j, input_param.outputpath, "action.json");


      nlohmann::json new_action_j = nlohmann::json::object();

      for (const auto& [code, process] : action_j.items())
      {
          // 
          std::string target_name = mapper.GetCode(code);

          if (target_name.empty())
          {
              std::cout << "未找到匹配关系: " << code << std::endl;
              continue;
          }

          std::cout << code
                    << " -> "
                    << target_name
                    << std::endl;

          
          new_action_j[target_name] = process;
      }


       WriteJson(new_action_j, input_param.outputpath, "sj-RMGateSchedule.txt.json");

      // if (input_param.dataConfig.contains("selected_model")) {
      //   string model = input_param.dataConfig["selected_model"];
      //   if (input_param.dataConfig[model].contains("case_name") && input_param.dataConfig[model].contains("output_action_name_suffix")) {
      //   string casename = input_param.dataConfig[model]["case_name"];
      //   string output_action_name = input_param.dataConfig[model]["output_action_name_suffix"];
      //   string output_action_path = casename +  output_action_name;
      //   WriteJson(action_j, input_param.outputpath, output_action_path);
 
      //   }
      //}


       //for (int j = 0; j < input_param.num_pools; j++) {

       //  Ocis_edges::Edges* p = &input_param.topo_dicts.pools[j];

       //  string poolname = input_param.topo_dicts.pools[j].name;
       //  string poolname_utf8 = poolname;
       //  if (!isValidUTF8(poolname)) {
       //    poolname_utf8 = (poolname);
       //  }

       //  rolling_time_t = StringToTime_t(input_param.time_start);
       //  for (int i = 0; i < input_param.time_vars; i++) {
       //    rolling_time_t = rolling_time_t + input_param.dt;


       //    for (int k = 0; k < p->source.size(); k++) {
       //      string name = p->source[k].name;
       //      string name_utf8 = name;
       //      if (!isValidUTF8(name)) {
       //        name_utf8 = (name);
       //      }
       //      if (result.solution["Q"][name].empty()) {
       //        continue;
       //      }

       //      if (i < result.solution["Q"][name].size()) {
       //        obj_action[poolname][name][(rolling_time_t)] = fabs(result.solution["Q"][name][i]);
       //      }
       //    }
       //    for (int k = 0; k < p->turnouts.size(); k++) {
       //      string name = p->turnouts[k].name;
       //      string name_utf8 = name;
       //      if (!isValidUTF8(name)) {
       //        name_utf8 = (name);
       //      }
       //      if (result.solution["Q"][name].empty()) {
       //        continue;
       //      }


       //      if (i < result.solution["Q"][name].size()) {
       //        obj_action[poolname][name][(rolling_time_t)] = fabs(result.solution["Q"][name][i]);
       //      }
       //    }
       //    for (int k = 0; k < p->targets.size(); k++) {
       //      string name = p->targets[k].name;
       //      string name_utf8 = name;
       //      if (!isValidUTF8(name)) {
       //        name_utf8 = (name);
       //      }
       //      if (result.solution["Q"][name].empty()) {
       //        continue;
       //      }


       //      if (i < result.solution["Q"][name].size()) {
       //        obj_action[poolname][name][(rolling_time_t)] = fabs(result.solution["Q"][name][i]);
       //      }
       //    }
       //  }
       //}
     }


     for (auto iter = obj_action.begin(); iter != obj_action.end(); iter++) {
       string name = iter->first;
       string name_utf8 = name;
       if (!isPossibleGBK(name)) {
         name_utf8 = (name);
       }
       else
       {
         name = (name);
       }
       
       Ocis_edges::Edges* p = input_param.topo_dicts.get_pool_byName(name);
       segments_Write_process_T(dirpath, iter->second, "/Q_"+ to_string(p->id) +"_" + name_utf8 +   ".csv");
     }




     //if (result.solution.find("Q") != result.solution.end()) {
     //  if (!action_output_requested)
     //    MILP_write_csv(input_param, result, dirpath, "Q");
     //  MILP_write_csv(input_param, result, dirpath, "e");
     //}
     //if (result.solution.find("W") != result.solution.end()) {
     //  MILP_write_csv(input_param, result, dirpath, "W");
     //}
     //if (result.solution.find("W_completed_ratio") != result.solution.end()) {
     //  MILP_write_csv(input_param, result, dirpath, "W_completed_ratio");
     //}

     //if (result.solution.find("DCheck") != result.solution.end()) {
     //  MILP_write_csv(input_param, result, dirpath, "DCheck");
     //}
     //if (result.solution.find("SCheck") != result.solution.end()) {
     //  MILP_write_csv(input_param, result, dirpath, "SCheck");
     //}
     //if (result.solution.find("CCheck") != result.solution.end()) {
     //  MILP_write_csv(input_param, result, dirpath, "CCheck");
     //}
     //if (result.solution.find("STime") != result.solution.end()) {
     //  MILP_write_csv(input_param, result, dirpath, "STime");
     //}
     //if (result.solution.find("USTime") != result.solution.end()) {
     //  MILP_write_csv(input_param, result, dirpath, "USTime");
     //}

     //if (result.solution.find("ETime") != result.solution.end()) {
     //  MILP_write_csv(input_param, result, dirpath, "ETime");
     //}
     //if (result.solution.find("QGap") != result.solution.end()) {
     //  MILP_write_csv(input_param, result, dirpath, "QGap");
     //}
     //if (result.solution.find("DLTime") != result.solution.end()) {
     //  MILP_write_csv(input_param, result, dirpath, "DLTime");
     //}
     //if (result.solution.find("DLTimeGap") != result.solution.end()) {
     //  MILP_write_csv(input_param, result, dirpath, "DLTimeGap");
     //}

     //if (result.solution.find("STimeGap") != result.solution.end()) {
     //  MILP_write_csv(input_param, result, dirpath, "STimeGap");
     //}
     //if (result.solution.find("RTimeGap") != result.solution.end()) {
     //  MILP_write_csv(input_param, result, dirpath, "RTimeGap");
     //}
     //if (result.solution.find("DeltaCheck") != result.solution.end()) {
     //  MILP_write_csv(input_param, result, dirpath, "DeltaCheck");
     //}
     //if (result.solution.find("tau") != result.solution.end()) {
     //  MILP_write_csv(input_param, result, dirpath, "tau");
     //}
     //if (result.solution.find("RTime") != result.solution.end()) {
     //  MILP_write_csv(input_param, result, dirpath, "RTime");
     //}




     if (result.solution.find("dx") != result.solution.end()) {
       h_csv dx_action = create_h_csv(input_param.time_vars, input_param.num_vars + 1);
       for (int i = 0; i < input_param.time_vars; i++) {
         dx_action[i][0] = FormatTime(rolling_time_t);
         rolling_time_t = rolling_time_t + input_param.dt;
         for (int j = 0; j < input_param.num_vars; j++) {
           dx_action[i][j + 1] = to_string(result.solution["dx"][input_param.topo_dicts.gates[j].name][i]);
         }
       }
       dx_action.emplace(dx_action.begin(), gates_names);  
       write_h_csv(dirpath + "dx_action.csv", dx_action, ',');
     }

     if (result.solution.find("fabs_dx") != result.solution.end()) {
       h_csv fabs_dx_action = create_h_csv(input_param.time_vars, input_param.num_vars + 1);
       for (int i = 0; i < input_param.time_vars; i++) {
         fabs_dx_action[i][0] = FormatTime(rolling_time_t);
         rolling_time_t = rolling_time_t + input_param.dt;
         for (int j = 0; j < input_param.num_vars; j++) {
           fabs_dx_action[i][j + 1] = to_string(result.solution["fabs_dx"][input_param.topo_dicts.gates[j].name][i]);
         }
       }
       fabs_dx_action.emplace(fabs_dx_action.begin(), gates_names);  
       write_h_csv(dirpath + "fabs_dx_action.csv", fabs_dx_action, ',');
     }

	 if (result.solution.find("boundary_dy") != result.solution.end()) {
		 auto it = result.solution.find("boundary_dy");
		 if (it != result.solution.end()) {
			 h_csv action_boundary_y = create_h_csv(input_param.time_vars, input_param.num_vars + 1);
			 for (int i = 0; i < input_param.time_vars; i++) {
				 action_boundary_y[i][0] = FormatTime(rolling_time_t);
				 rolling_time_t = rolling_time_t + input_param.dt;
				 for (int j = 0; j < input_param.num_vars; j++) {
					 action_boundary_y[i][j + 1] = to_string(result.solution["boundary_dy"][input_param.topo_dicts.gates[j].name][i]);
				 }
			 }
			 action_boundary_y.emplace(action_boundary_y.begin(), gates_names);  
			 write_h_csv(dirpath + "action_boundary_dy.csv", action_boundary_y, ',');
		 }

	 }


     if (result.solution.find("sub_dt") != result.solution.end()) {
         process_T sub_dt_map = get_solution(result.solution, "sub_dt");
     }



	 //pools
	 if (result.solution.find("stage") != result.solution.end()) {
	   mkdir_h(dirpath + "stage/");
	   MILP_write_csv_pools(input_param, result, dirpath + "stage/", "stage");
	   MILP_write_csv_pools(input_param, result, input_param.input_path , "stage");
	 }
	 if (result.solution.find("WaterVolume") != result.solution.end()) {
	   MILP_write_csv_pools(input_param, result, dirpath, "WaterVolume");
	 }
	 if (result.solution.find("pool_tau") != result.solution.end()) {
	   MILP_write_csv_pools(input_param, result, dirpath, "pool_tau");
	 }
	 if (result.solution.find("yStatusCheck") != result.solution.end()) {
	   MILP_write_csv_pools(input_param, result, dirpath, "yStatusCheck");
	 }
	 if (result.solution.find("complement_yStatusCheck") != result.solution.end()) {
	   MILP_write_csv_pools(input_param, result, dirpath, "complement_yStatusCheck");
	 }
	 if (result.solution.find("y_segments_label_nindex") != result.solution.end()) {
	   MILP_write_csv_pools(input_param, result, dirpath, "y_segments_label_nindex");
	 }

	 if (result.solution.find("y_reciprocal") != result.solution.end()) {
	   MILP_write_csv_pools(input_param, result, dirpath, "y_reciprocal");
	 }
	 if (result.solution.find("y_segments_label_nindex") != result.solution.end()) {
	   MILP_write_csv_pools(input_param, result, dirpath, "y_segments_label_nindex");
	 }

	 if (result.solution.find("V_segments_label_nindex") != result.solution.end()) {
	   MILP_write_csv_pools(input_param, result, dirpath, "V_segments_label_nindex");
	 }
	 if (result.solution.find("V") != result.solution.end()) {
	   MILP_write_csv_pools(input_param, result, dirpath, "V");
	 }
	 if (result.solution.find("piece_wise_V_add_celerity_reciprocal") != result.solution.end()) {
	   MILP_write_csv_pools(input_param, result, dirpath, "piece_wise_V_add_celerity_reciprocal");
	 }

	 if (result.solution.find("celerity") != result.solution.end()) {
	   MILP_write_csv_pools(input_param, result, dirpath, "celerity");
	 }

	 if (result.solution.find("celerity_reciprocal") != result.solution.end()) {
	   MILP_write_csv_pools(input_param, result, dirpath, "celerity_reciprocal");
	 }

	 for (auto iter = result.solution.begin(); iter != result.solution.end(); iter++) {
	   if (!iter->second.empty()) {
		 string filelabel = iter->first;
		 if (action_output_requested
		     && (filelabel == "action" || filelabel == "Q")) continue;
		 MILP_write_csv_pools(input_param, result, dirpath + "/", filelabel);
	   }

	 }

     json ocis_milp_output_json;
     if(result.solution.find("flood")!=result.solution.end())
       ocis_milp_output_json["flood"] = result.solution["flood"]["all"][0];
     if(result.solution.find("flood")!=result.solution.end())
       ocis_milp_output_json["flood_control"] = result.solution["flood_control"]["all"][0];
     if(result.solution.find("flood")!=result.solution.end())
       ocis_milp_output_json["kpi_Ef"] = result.solution["kpi_Ef"]["all"][0];
     if(result.solution.find("flood")!=result.solution.end())
       ocis_milp_output_json["kpi_dy"] = result.solution["kpi_dy"]["all"][0];
     if(result.solution.find("flood")!=result.solution.end())
       ocis_milp_output_json["kpi_dQ"] = result.solution["kpi_dQ"]["all"][0];
  }

void OCIS_scip::MILP_write_output_MultiDays(MILP_param& input_param,MILP_result& result,string dirpath){
     std::cout << "=== MILP_write_output ===\n";
    
    const double totalSeconds = input_param.T * 3600.0;

    const int stepCount =
        static_cast<int>(
            std::ceil(totalSeconds / input_param.dt));

     mkdir_h(dirpath + "output/");
     dirpath = dirpath + "output/";

     nlohmann::ordered_json result_json = MILP_write_schedule_MultiDays(input_param, result, dirpath);

     if (result.solution.find("action") == result.solution.end() ||
         result.solution["action"].empty()) {
       ResultJsonToAction(
         result_json,
         result,
         input_param.dt,
         input_param.T);
     }

     std::vector<string> gates_names;
     gates_names.push_back("tm");
     for (int i = 0; i < input_param.num_vars; i++) {
       string name_utf8 = (input_param.topo_dicts.gates[i].name);
       gates_names.push_back(name_utf8);

     }

     process_obj_T obj_action;
     process_T action_td;
     time_t rolling_time_t = StringToTime_t(input_param.time_start);
     if (result.solution.find("action") != result.solution.end()) {
       // action.csv output time step: densify to output_action_dt when > 0,
       // covering [time_start, time_start + T*3600]. Before a gate opens:
       // obs_flow or 0; while open: scheduled flow; after close: 0.
       const int out_dt = input_param.output_action_dt > 0
           ? input_param.output_action_dt : input_param.dt;
       const time_t horizon_s = static_cast<time_t>(input_param.T) * 3600;
       const int out_steps = static_cast<int>(
           (static_cast<long long>(horizon_s) + out_dt - 1) / out_dt);
       h_csv action = create_h_csv(out_steps, input_param.num_vars + 1);
       for (int i = 0; i < out_steps; i++) {
         const time_t t_out = std::min(
             static_cast<time_t>(i) * out_dt, horizon_s);
         action[i][0] = FormatTime(rolling_time_t + t_out);
         for (int j = 0; j < input_param.num_vars; j++) {
           string name = input_param.topo_dicts.gates[j].name;
           string name_utf8 = name;
           if (!isValidUTF8(name)) {
             name_utf8 = (name);
           }
           const double value = action_value_at(
               result, &input_param.topo_dicts.gates[j], name,
               static_cast<double>(t_out), input_param.dt, out_dt,
               static_cast<double>(horizon_s));
           action[i][j + 1] = to_string(value);
           action_td[name_utf8][rolling_time_t + t_out] = value;
         }
       }
       action.emplace(action.begin(), gates_names);  
       write_h_csv(dirpath + "action.csv", action, ',');
       write_tidyData(dirpath + "action_td.csv", action_td);
     }

     if (result.solution.find("Q") != result.solution.end()) {
       MILP_write_csv(input_param, result, dirpath, "Q");
     }
     if (result.solution.find("UQ") != result.solution.end()) {
       MILP_write_csv(input_param, result, dirpath, "UQ");
     }
     if (result.solution.find("W") != result.solution.end()) {
       MILP_write_csv(input_param, result, dirpath, "W");
     }
     if (result.solution.find("duration") != result.solution.end()) {
       MILP_write_csv(input_param, result, dirpath, "duration");
     }
     if (result.solution.find("W_input") != result.solution.end()) {
       MILP_write_csv(input_param, result, dirpath, "W_input");
     }


     if (result.solution.find("DCheck") != result.solution.end()) {
       MILP_write_csv(input_param, result, dirpath, "DCheck");
     }
     if (result.solution.find("SCheck") != result.solution.end()) {
       MILP_write_csv(input_param, result, dirpath, "SCheck");
     }
     if (result.solution.find("CCheck") != result.solution.end()) {
       MILP_write_csv(input_param, result, dirpath, "CCheck");
     }
     if (result.solution.find("STime") != result.solution.end()) {
       MILP_write_csv(input_param, result, dirpath, "STime");
     }
     if (result.solution.find("USTime") != result.solution.end()) {
       MILP_write_csv(input_param, result, dirpath, "USTime");
     }

     if (result.solution.find("ETime") != result.solution.end()) {
       MILP_write_csv(input_param, result, dirpath, "ETime");
     }
     if (result.solution.find("QGap") != result.solution.end()) {
       MILP_write_csv(input_param, result, dirpath, "QGap");
     }
     if (result.solution.find("DLTime") != result.solution.end()) {
       MILP_write_csv(input_param, result, dirpath, "DLTime");
     }
     if (result.solution.find("DLTimeGap") != result.solution.end()) {
       MILP_write_csv(input_param, result, dirpath, "DLTimeGap");
     }

     if (result.solution.find("STimeGap") != result.solution.end()) {
       MILP_write_csv(input_param, result, dirpath, "STimeGap");
     }
     if (result.solution.find("RTimeGap") != result.solution.end()) {
       MILP_write_csv(input_param, result, dirpath, "RTimeGap");
     }
     if (result.solution.find("RTime") != result.solution.end()) {
       MILP_write_csv(input_param, result, dirpath, "RTime");
     }

     if (result.solution.find("DeltaCheck") != result.solution.end()) {
       MILP_write_csv(input_param, result, dirpath, "DeltaCheck");
     }
     if (result.solution.find("tau_g") != result.solution.end()) {
       MILP_write_csv(input_param, result, dirpath, "tau_g");
     }

     if (result.solution.find("tau") != result.solution.end()) {
       MILP_write_csv(input_param, result, dirpath, "tau");
     }
     if (result.solution.find("RTime") != result.solution.end()) {
       MILP_write_csv(input_param, result, dirpath, "RTime");
     }




     if (result.solution.find("dx") != result.solution.end()) {
       h_csv dx_action = create_h_csv(stepCount, input_param.num_vars + 1);
       for (int i = 0; i < stepCount; i++) {
         dx_action[i][0] = FormatTime(rolling_time_t);
         rolling_time_t = rolling_time_t + input_param.dt;
         for (int j = 0; j < input_param.num_vars; j++) {
           dx_action[i][j + 1] = to_string(result.solution["dx"][input_param.topo_dicts.gates[j].name][i]);
         }
       }
       dx_action.emplace(dx_action.begin(), gates_names);  
       write_h_csv(dirpath + "dx_action.csv", dx_action, ',');
     }

     if (result.solution.find("fabs_dx") != result.solution.end()) {
       h_csv fabs_dx_action = create_h_csv(stepCount, input_param.num_vars + 1);
       for (int i = 0; i < stepCount; i++) {
         fabs_dx_action[i][0] = FormatTime(rolling_time_t);
         rolling_time_t = rolling_time_t + input_param.dt;
         for (int j = 0; j < input_param.num_vars; j++) {
           fabs_dx_action[i][j + 1] = to_string(result.solution["fabs_dx"][input_param.topo_dicts.gates[j].name][i]);
         }
       }
       fabs_dx_action.emplace(fabs_dx_action.begin(), gates_names);  
       write_h_csv(dirpath + "fabs_dx_action.csv", fabs_dx_action, ',');
     }

     if (result.solution.find("boundary_dy") != result.solution.end()) {
     auto it = result.solution.find("boundary_dy");
     if (it != result.solution.end()) {
       h_csv action_boundary_y = create_h_csv(stepCount, input_param.num_vars + 1);
       for (int i = 0; i < stepCount; i++) {
         action_boundary_y[i][0] = FormatTime(rolling_time_t);
         rolling_time_t = rolling_time_t + input_param.dt;
         for (int j = 0; j < input_param.num_vars; j++) {
           action_boundary_y[i][j + 1] = to_string(result.solution["boundary_dy"][input_param.topo_dicts.gates[j].name][i]);
         }
       }
       action_boundary_y.emplace(action_boundary_y.begin(), gates_names);  
       write_h_csv(dirpath + "action_boundary_dy.csv", action_boundary_y, ',');
     }

     }

     //pools
     if (result.solution.find("y") != result.solution.end()) {
       MILP_write_csv_pools(input_param, result, dirpath, "y");
     }
     if (result.solution.find("y_reciprocal") != result.solution.end()) {
       MILP_write_csv_pools(input_param, result, dirpath, "y_reciprocal");
     }
     if (result.solution.find("y_segments_label_nindex") != result.solution.end()) {
       MILP_write_csv_pools(input_param, result, dirpath, "y_segments_label_nindex");
     }

     if (result.solution.find("V_segments_label_nindex") != result.solution.end()) {
       MILP_write_csv_pools(input_param, result, dirpath, "V_segments_label_nindex");
     }
     if (result.solution.find("V") != result.solution.end()) {
       MILP_write_csv_pools(input_param, result, dirpath, "V");
     }
     if (result.solution.find("piece_wise_V_add_celerity_reciprocal") != result.solution.end()) {
       MILP_write_csv_pools(input_param, result, dirpath, "piece_wise_V_add_celerity_reciprocal");
     }

     if (result.solution.find("celerity") != result.solution.end()) {
       MILP_write_csv_pools(input_param, result, dirpath, "celerity");
     }

     if (result.solution.find("celerity_reciprocal") != result.solution.end()) {
       MILP_write_csv_pools(input_param, result, dirpath, "celerity_reciprocal");
     }
     if (result.solution.find("infiltrationQ") != result.solution.end()) {
       MILP_write_csv_pools(input_param, result, dirpath, "infiltrationQ");
     }



     std::vector<string> pool_names;
     pool_names.push_back("tm");
     for (int i = 0; i < input_param.num_pools; i++) {
       string name = input_param.topo_dicts.pools[i].name;
         if (!isValidUTF8(name)) {
           name = (name);
         }

       pool_names.push_back(name);
     }
     if (result.solution.find("stage") != result.solution.end()) {
       h_csv stage = create_h_csv(stepCount, input_param.num_pools + 1);
       rolling_time_t = StringToTime_t(input_param.time_start);
       for (int i = 0; i < stepCount; i++) {
         stage[i][0] = FormatTime(rolling_time_t);
         rolling_time_t = rolling_time_t + input_param.dt;
         for (int j = 0; j < input_param.num_pools; j++) {
           string name = input_param.topo_dicts.pools[j].name;
           stage[i][j + 1] = to_string(result.solution["stage"][name][i]);
         }
       }
       stage.emplace(stage.begin(), pool_names);  
       write_h_csv(dirpath + "stage.csv", stage, ',');
     }

     json ocis_milp_output_json;
     if(result.solution.find("flood")!=result.solution.end())
       ocis_milp_output_json["flood"] = result.solution["flood"]["all"][0];
     if(result.solution.find("flood")!=result.solution.end())
       ocis_milp_output_json["flood_control"] = result.solution["flood_control"]["all"][0];
     if(result.solution.find("flood")!=result.solution.end())
       ocis_milp_output_json["kpi_Ef"] = result.solution["kpi_Ef"]["all"][0];
     if(result.solution.find("flood")!=result.solution.end())
       ocis_milp_output_json["kpi_dy"] = result.solution["kpi_dy"]["all"][0];
     if(result.solution.find("flood")!=result.solution.end())
       ocis_milp_output_json["kpi_dQ"] = result.solution["kpi_dQ"]["all"][0];

    //string output_file_name = dirpath + "ocis_milp_output.json";
    //std::ofstream out(output_file_name, std::ios::binary);
    //out << std::fixed << std::setprecision(2);
    //out << ocis_milp_output_json.dump(4);


  }

void OCIS_scip::Summary_dyunamic_border(
    MILP_param& input_param,
    std::map<string, std::map<string, std::vector<double>>>& solution
  ) {
 

    std::map<string, std::vector<double>>   action_map = solution["Q"];
    for (int j = 0; j < input_param.topo_dicts.pools.size(); j++) {
      Ocis_edges::Edges* p = &input_param.topo_dicts.pools[j];

      std::vector<double> pool_dynamic_border_summary;
      for (int i = 0; i < p->turnouts.size(); i++) {
        Ocis_edges::Point* g = input_param.topo_dicts.get_gate_byName(p->turnouts[i].name);
        if (i == 0) {
          pool_dynamic_border_summary.resize(action_map[g->name].size());
        }


        for (int t = 0; t < action_map[g->name].size(); t++) {
          double flow = action_map[g->name][t];

          if (t >= pool_dynamic_border_summary.size()) {
            continue;
          }

          if (g->type == 2 || g->type == 1 || g->type == 5) {
            pool_dynamic_border_summary[t] += flow;
          }
          else
          {
            pool_dynamic_border_summary[t] -= flow;
          }
        }
      }

      string dynamic_border_name = "d" + to_string(p->id);
      action_map[dynamic_border_name] = pool_dynamic_border_summary;

    }
  
    solution["Q"] = action_map;
  }

void  OCIS_scip::balance_check(MILP_param& input_param) {

    std::unordered_map<string, double> pool_balance_check;
    std::unordered_map<string, string> pool_flow_balance_check_obs_checkstructure;
    std::unordered_map<string, string> pool_flow_balance_check_obs_turnouts;

    for (int i = 0; i < input_param.num_pools; ++i) {
      Ocis_edges::Edges* p = &input_param.topo_dicts.pools[i];
      double balance = 0;
      string obs_check_structure_str;
      string obs_turnouts_str;
      for (int g = 0; g < input_param.topo_dicts.pools[i].source.size(); ++g) {
        string name = input_param.topo_dicts.pools[i].source[g].name;
        Ocis_edges::Point* g_tp = input_param.topo_dicts.get_gate_byName(name);
        int g_id = g_tp->id;
        balance = balance + g_tp->obs_flow;
        obs_check_structure_str += to_string(fabs(g_tp->obs_flow));
        obs_check_structure_str += ",";
      }
      for (int g = 0; g < input_param.topo_dicts.pools[i].turnouts.size(); ++g) {
        string name = input_param.topo_dicts.pools[i].turnouts[g].name;
        Ocis_edges::Point* g_tp = input_param.topo_dicts.get_gate_byName(name);
        int g_id = g_tp->id;
        balance = balance - g_tp->obs_flow;
        obs_turnouts_str += to_string(fabs(g_tp->obs_flow));
        obs_turnouts_str += ",";
      }
      for (int g = 0; g < input_param.topo_dicts.pools[i].targets.size(); ++g) {
        string name = input_param.topo_dicts.pools[i].targets[g].name;
        Ocis_edges::Point* g_tp = input_param.topo_dicts.get_gate_byName(name);
        int g_id = g_tp->id;
        balance = balance - g_tp->obs_flow;

        obs_check_structure_str += to_string(fabs(g_tp->obs_flow));
      }

      pool_balance_check[p->name] = balance;
      pool_flow_balance_check_obs_checkstructure[p->name] = obs_check_structure_str;
      pool_flow_balance_check_obs_turnouts[p->name] = obs_turnouts_str;

    }

    write_tidyData(input_param.outputpath+"pool_flow_balance_check.csv", pool_balance_check,{"obj","value"});
    write_tidyData(input_param.outputpath+"pool_flow_balance_check_obs_checkstructure.csv", pool_flow_balance_check_obs_checkstructure,{"obj","value"});
    write_tidyData(input_param.outputpath+"pool_flow_balance_check_obs_turnouts.csv", pool_flow_balance_check_obs_turnouts,{"obj","value"});
    
    
    
    json gates_complete_ratio;
    json station_demandQ;
    json station_Q_sol_first;
    json station_Q_sol_average;
    double station_demand_Flow = 0;

    std::map<string,double> map_gates_complete_ratio;
    std::map<string,double> map_station_demandQ;
    std::map<string,double> map_station_Q_sol_first;
    std::map<string,double> map_station_Q_sol_average;

    for (int i = 0; i < input_param.num_vars; i++) {
      Ocis_edges::Point* g = &input_param.topo_dicts.gates[i];
      string level1 = (g->level1);
      string name = (g->name);

      double ratio = g->Q_sol_first/g->demandFlow;


      if (level1 != "" && g->demandFlow >= 0) {

        map_station_demandQ[level1] += g->demandFlow;
        map_station_Q_sol_first[level1] += g->Q_sol_first;
        map_station_Q_sol_average[level1] += g->Q_sol_average;

        gates_complete_ratio[name] = ratio;
        station_demandQ[level1] = g->demandFlow;
        station_Q_sol_first[level1] = g->Q_sol_first;
        station_Q_sol_average[level1] = g->Q_sol_average;
      }

    }
    write_process_strT(map_station_demandQ, input_param.outputpath, "station_demandQ.json");
    write_process_strT(map_station_Q_sol_first, input_param.outputpath, "station_Q_sol_first.json");
    write_process_strT(map_station_Q_sol_average, input_param.outputpath, "station_Q_sol_average.json");

    //WriteJson(gates_complete_ratio, input_param.outputpath, "gates_complete_ratio.json");
    //WriteJson(station_demandQ, input_param.outputpath, "station_demandQ.json");
    //WriteJson(station_Q_sol_first, input_param.outputpath, "station_Q_sol_first.json");
    //WriteJson(station_Q_sol_average, input_param.outputpath, "station_Q_sol_average.json");
    
  }

void OCIS_scip::get_obs(json obs_json,MILP_param& input ) {


    int max_obs_STime = 0;
    int max_obs_STime_gates_vec_id = 0;
    
    for (int i = 0; i < input.topo_dicts.gates.size(); i++) {
      string g_name = input.topo_dicts.gates[i].name;
      Ocis_edges::Point* g = input.topo_dicts.get_gate_byName(g_name);
      if (obs_json.contains((g_name))) {
        double flow = -1;//
        if (obs_json[(g_name)].contains("flow")) {
          flow = obs_json[(g_name)]["flow"].get<double>();

        }
        else if (obs_json[(g_name)].contains("q"))
        {
          flow = obs_json[(g_name)]["q"].get<double>();

        }
        double h = -1;//
        if (obs_json[(g_name)].contains("h")) {
          h = obs_json[(g_name)]["h"].get<double>();
        }

        g->obs_flow = flow;
        g->obs_h = h;

        if (obs_json[(g_name)].contains("obs_STime")) {
          g->obs_STime = obs_json[(g_name)]["obs_STime"].get<string>();
 
          if (g->obs_STime == "") {
            g->obs_STime = input.time_start;
          }

          int obs_STime = StringToTime_t(g->obs_STime);
          input.topo_dicts.gates_obs_STime[g_name] = obs_STime;


          
          if (obs_STime > max_obs_STime && (g->type == 4 || g->type == 2)) {
            max_obs_STime = obs_STime;
            max_obs_STime_gates_vec_id = i;
          }

        }

        if (obs_json[(g_name)].contains("fixed")) {
          g->fixed = obs_json[(g_name)]["fixed"].get<int>();

        }
        if (obs_json[(g_name)].contains("fixed_y")) {
          g->fixed_y = obs_json[(g_name)]["fixed_y"].get<int>();

        }

        if (obs_json[(g_name)].contains("fixed_W")) {
          g->fixed_W = obs_json[(g_name)]["fixed_W"].get<int>();
        }
        if (obs_json[(g_name)].contains("delta_Q_max")) {
          g->delta_Q_max = obs_json[(g_name)]["delta_Q_max"].get<int>();
        }
        if (obs_json[(g_name)].contains("delta_Q_max_ratio")) {
          input.gate_delta_q_max_ratio[g_name] = std::clamp(
              obs_json[(g_name)]["delta_Q_max_ratio"].get<double>(),
              0.0, 1.0);
        }
        if (obs_json[(g_name)].contains("end_flow")) {
          g->end_flow = obs_json[(g_name)]["end_flow"].get<int>();
        }
        if (obs_json[(g_name)].contains("spillway_gate_label")) {
          int label = obs_json[(g_name)]["spillway_gate_label"].get<int>();
          if (label == 1) {
              g->spillway_gate_label = label;

            if (obs_json[(g_name)].contains("spillway_gate_param")) {

            }

          }

        }
        if (obs_json[(g_name)].contains("regulation_structure_label")) {
          int label = obs_json[(g_name)]["regulation_structure_label"].get<int>();
          if (label == 1) {
              g->regulation_structure_label = label;

            if (obs_json[(g_name)].contains("regulation_structure_parameter")) {
              double start_up_h = obs_json[(g_name)]["regulation_structure_parameter"]["start_up_h"].get<double>();
              double stop_down_h = obs_json[(g_name)]["regulation_structure_parameter"]["stop_down_h"].get<double>();

            }

          }

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
          }
        }
        if (obs_json[(g_name)].contains("weir_label")) {

          int label = obs_json[(g_name)]["weir_label"].get<int>();
          if (label == 1) {
              g->weir_label = label;

            if (obs_json[(g_name)].contains("weir_param")) {
              double weir_flow_length = obs_json[(g_name)]["weir_param"]["weir_flow_length"].get<double>();
              double weir_height = obs_json[(g_name)]["weir_param"]["weir_height"].get<double>();
              double weir_cross_width = obs_json[(g_name)]["weir_param"]["weir_cross_width"].get<double>();
              g->weir_flow_length = weir_flow_length;
              g->weir_height = weir_height;
              g->weir_cross_width = weir_cross_width;

            }

          }


        }

        if (obs_json[(g_name)].contains("pump_label")) {
          int label = obs_json[(g_name)]["pump_label"].get<int>();
          if (label == 1&& obs_json[(g_name)].contains("H_V_CURVE")) {
            g->pump_label = label;

            std::vector<std::array<double, 2>> Q_DH = obs_json[(g_name)]["H_V_CURVE"].get<std::vector<std::array<double, 2>>>();
            for (int i = 0; i < Q_DH.size(); i++) {
              g->Q_DH[Q_DH[i][0]] = Q_DH[i][1];
            }

          }

        }

        if (obs_json[(g_name)].contains("target_flow")) {
          double value = obs_json[(g_name)]["target_flow"].get<double>();
          g->demandFlow = value;
        }
        if (obs_json[(g_name)].contains("target_h")) {
          double value = obs_json[(g_name)]["target_h"].get<double>();
          g->target_h = value;
        }

        if (obs_json[(g_name)].contains("W_task_gap")) {
          double value = obs_json[(g_name)]["W_task_gap"].get<double>();
          if (!std::isfinite(value) || value < 0 || value > 1) {
            value = 0.2;
          }

          g->W_task_gap = value;
        }
        if (obs_json[(g_name)].contains("min_action_changes")) {
          int value = std::max(
              -1, obs_json[(g_name)]["min_action_changes"].get<int>());
          const int max_changes = obs_json[(g_name)].contains("max_flow_steps")
              ? obs_json[(g_name)]["max_flow_steps"].get<int>()
              : g->max_flow_steps;
          if (max_changes >= 0 && value >= max_changes) {
            value = max_changes - 1;
            std::cout << "[obs] " << g_name
                      << " min_action_changes corrected to " << value
                      << std::endl;
          }
          input.gate_min_action_changes[g_name] = value;
        }
        if (obs_json[(g_name)].contains("max_flow_steps")) {
          const int value =
              obs_json[(g_name)]["max_flow_steps"].get<int>();
          g->max_flow_steps = value;
          input.topo_dicts.gates[i].max_flow_steps = value;
        }


      }
    }
    input.topo_dicts.gates[max_obs_STime_gates_vec_id].is_max_obs_STime_Label = 1;



  }

void OCIS_scip::get_ReTime(json obs_json,MILP_param& input ) {
    
    for (int i = 0; i < input.topo_dicts.gates.size(); i++) {
      string g_name = input.topo_dicts.gates[i].name;
      if (isValidUTF8(g_name)) {
        g_name = (g_name);
      }
      Ocis_edges::Point* g = input.topo_dicts.get_gate_byName(g_name);
      if (obs_json.contains((g_name))) {
        double ReTime = obs_json[(g_name)].get<double>();
        g->regulationTime = ReTime;
      }
    }
  }

void OCIS_scip::get_waterDemand(json obs_json,MILP_param& input ) {
    // Match waterDemand keys (usually UTF-8) against gate names (may be GBK).
    auto demand_key_for_gate = [&](const string& g_name) -> string {
      if (obs_json.contains(g_name)) return g_name;
      string u = g_name;
      string gbk = g_name;
      if (isValidUTF8(g_name)) {
        gbk = U2G(g_name);
      } else {
        u = G2U(g_name);
      }
      if (obs_json.contains(u)) return u;
      if (obs_json.contains(gbk)) return gbk;
      return string();
    };
    auto names_equal = [&](const string& a, const string& b) -> bool {
      if (a == b) return true;
      string au = isValidUTF8(a) ? a : G2U(a);
      string bu = isValidUTF8(b) ? b : G2U(b);
      if (au == bu) return true;
      string ag = isValidUTF8(a) ? U2G(a) : a;
      string bg = isValidUTF8(b) ? U2G(b) : b;
      return ag == bg;
    };

    int direct_n = 0;
    double direct_W = 0.0;
    std::map<std::string, process_h> demandProcesses;
    for (int i = 0; i < input.topo_dicts.gates.size(); i++) {
      string g_name = input.topo_dicts.gates[i].name;
      Ocis_edges::Point* g = input.topo_dicts.get_gate_byName(g_name);
      string key = demand_key_for_gate(g_name);
      if (key.empty() || g == nullptr) continue;

      double max_q = 0;
      double min_s = input.time_start_t + input.T * 3600;
      double max_e = input.time_start_t;
      double sum_W = 0.0;
      for (int k = 0; k < (int)obs_json[key].size(); k++) {
        string s = obs_json[key][k]["s"].get<string>();
        string e = obs_json[key][k]["e"].get<string>();
        double q = obs_json[key][k]["q"].get<double>();
        ScheduleScheme t_s_s;
        t_s_s.s = StringToTime_t(s);
        t_s_s.e = StringToTime_t(e);
        t_s_s.q = q;
        if (g->type == 2) g->isDecisionVariable = true;
        t_s_s.duration = t_s_s.e - t_s_s.s;
        if (t_s_s.duration < 0) t_s_s.duration = 0;
        input.gates_ScheduleScheme[g_name].push_back(t_s_s);
        if (q > max_q) max_q = q;
        if (StringToTime_t(s) <= min_s) min_s = StringToTime_t(s);
        if (StringToTime_t(e) >= max_e) max_e = StringToTime_t(e);
        if (q > 0) {
          // Keep demand volume as declared (do not shrink by maxFlow).
          if (g->STime_input <= -1) g->STime_input = StringToTime_t(s);
          sum_W += q * (StringToTime_t(e) - StringToTime_t(s));
          g->W_setLabel = 1;

          addDemandProcess(
            demandProcesses,
            g->name,
            t_s_s.q,
            t_s_s.s,
            t_s_s.e,
            input_params.dt
          );
        }
      }
      g->demandFlow = max_q;
      if (g->maxFlow > 0 && g->demandFlow > g->maxFlow) g->demandFlow = g->maxFlow;
      g->duration_input = (max_e) - (min_s);
      g->W_input = static_cast<int>(std::llround(
          sum_W > 0 ? sum_W : g->demandFlow * g->duration_input));
      if (g->W_input < 0) g->W_input = 0;
      if (g->W_input > 0) {
        g->fixed_W = 1;
        direct_n += 1;
        direct_W += g->W_input;
      }
      g->STime_input = (min_s);
      g->ETime_input = (max_e);



    }
    std::cout << "[waterDemand] direct gate matches=" << direct_n
              << " W=" << direct_W << std::endl;
    input_params.boundary_flow = demandProcesses;


    // Station-level keys: distribute volume to type=2 children via level1/stationRank1.
    // Skip gates that already received a direct waterDemand entry (no double count).
    int station_n = 0;
    double station_W = 0.0;
    for (auto& demandFlow_obj : obs_json.items()) {
      string name = demandFlow_obj.key();
      bool is_direct_gate = false;
      for (int i = 0; i < input.topo_dicts.gates.size(); ++i) {
        if (demand_key_for_gate(input.topo_dicts.gates[i].name) == name) {
          is_direct_gate = true;
          break;
        }
      }
      if (is_direct_gate) continue;

      double max_q = 0.0;
      double total_w = 0.0;
      time_t min_s = input.time_start_t + input.T * 3600;
      time_t max_e = input.time_start_t;
      std::vector<double> station_q;
      std::vector<time_t> station_s;
      std::vector<time_t> station_e;
      for (auto& vec_obj : demandFlow_obj.value()) {
        double q = vec_obj["q"].get<double>();
        if (q <= 0) continue;
        time_t s_t = StringToTime_t(vec_obj["s"].get<string>());
        time_t e_t = StringToTime_t(vec_obj["e"].get<string>());
        if (e_t <= s_t) continue;
        if (q > max_q) max_q = q;
        total_w += q * difftime(e_t, s_t);
        if (s_t < min_s) min_s = s_t;
        if (e_t > max_e) max_e = e_t;
        station_q.push_back(q);
        station_s.push_back(s_t);
        station_e.push_back(e_t);
      }
      if (total_w <= 0) continue;

      // One representative gate per station demand (same object as figure/LBBD).
      Ocis_edges::Point* rep = nullptr;
      double best_cap = -1.0;
      int n_cand = 0;
      for (int i = 0; i < input.topo_dicts.gates.size(); ++i) {
        Ocis_edges::Point* g = &input.topo_dicts.gates[i];
        if (g->type != 2) continue;
        if (g->W_setLabel == 1) continue; // already has direct demand
        if (g->name.find("leakage") != string::npos) continue;
        if (!names_equal(g->level1, name) && !names_equal(g->name, name)) continue;
        n_cand += 1;
        double cap = std::max(0.0, g->maxFlow);
        if (cap > best_cap) { best_cap = cap; rep = g; }
      }
      if (rep == nullptr) continue;
      // W_input is an integer legacy field.  Avoid implicit truncation and the
      // default -1 sentinel biasing every station allocation downward by 1 m3.
      if (rep->W_input < 0) rep->W_input = 0;
      rep->W_input += static_cast<int>(std::llround(total_w));
      rep->demandFlow = max_q;
      if (rep->maxFlow > 0 && rep->demandFlow > rep->maxFlow) {
        // keep demandFlow informative; volume stays total_w
      }
      rep->fixed_W = 1;
      rep->W_setLabel = 1;
      rep->isDecisionVariable = true;
      rep->STime_input = min_s;
      rep->ETime_input = max_e;
      rep->duration_input = difftime(max_e, min_s);
      // Carry the station's priority attributes (irr_progress / grain-to-cash
      // ratio) onto the representative gate, so LBBD prioritizes by the
      // demand object (station) rather than by the rep gate's own default
      // attributes.
      {
        auto pa_it = input.priority_attrs.find(name);
        if (pa_it == input.priority_attrs.end()) {
          string u = name;
          string gbk = name;
          if (isValidUTF8(name)) {
            gbk = U2G(name);
          } else {
            u = G2U(name);
          }
          pa_it = input.priority_attrs.find(u);
          if (pa_it == input.priority_attrs.end()) {
            pa_it = input.priority_attrs.find(gbk);
          }
        }
        if (pa_it != input.priority_attrs.end()) {
          rep->irr_progress = pa_it->second.first;
          rep->grain_to_cash_crop_ratio = pa_it->second.second;
        }
      }
      // Expose the station's demand Q series under the rep gate name, so LBBD
      // applies the min-open-flow ratio to the demand object's flow: an opened
      // rep gate then delivers at least 50% of the station demand Q.
      for (size_t k = 0; k < station_q.size(); ++k) {
        addDemandProcess(demandProcesses, rep->name, station_q[k],
                         station_s[k], station_e[k], input_params.dt);
      }
      station_n += 1;
      station_W += total_w;


      std::cout << "[waterDemand] station '" << name << "' -> rep '" << rep->name
                << "' (cand=" << n_cand << "), W=" << total_w << std::endl;
    }
    std::cout << "[waterDemand] station allocations=" << station_n
              << " W=" << station_W << std::endl;

    // Publish the demand Q series (direct gates + station-representative
    // gates) on the MILP_param object that MILP_read_input merges into the
    // solver boundary flow.
    input.boundary_flow = demandProcesses;
  }

process_T OCIS_scip::get_waterDemand_from_input_json(json obs_json,MILP_param& input) {

    auto demand_key_for_gate = [&](const string& g_name) -> string {
      if (obs_json.contains(g_name)) return g_name;
      string u = g_name;
      string gbk = g_name;
      if (isValidUTF8(g_name)) {
        gbk = U2G(g_name);
      } else {
        u = G2U(g_name);
      }
      if (obs_json.contains(u)) return u;
      if (obs_json.contains(gbk)) return gbk;
      return string();
    };

    std::map<std::string, process_h> demandProcesses;
    for (int i = 0; i < input.topo_dicts.gates.size(); i++) {
      string g_name = input.topo_dicts.gates[i].name;
      Ocis_edges::Point* g = input.topo_dicts.get_gate_byName(g_name);
      string key = demand_key_for_gate(g_name);
      if (key.empty() || g == nullptr) continue;

      double max_q = 0;
      double min_s = input.time_start_t ;
      double max_e = input.time_start_t+ (input.T) * 3600;
      double sum_W = 0.0;
      for (int k = 0; k < (int)obs_json[key].size(); k++) {
        string s = obs_json[key][k]["s"].get<string>();
        string e = obs_json[key][k]["e"].get<string>();
        double q = obs_json[key][k]["q"].get<double>();
        ScheduleScheme t_s_s;
        const time_t s_raw = StringToTime_t(s);
        const time_t e_raw = StringToTime_t(e);
        // Business rule: a demand period that crosses midnight but ends before
        // 08:00 of the next day is treated as a one-day demand, so the ending
        // time is not extended by +3600. Under daily optimization periods this
        // lets the demand be completed within a single period at full demand
        // flow, instead of forcing a 25h demand to spill into a second day and
        // halve the delivered Q.
        bool same_day_rule = false;
        {
          struct tm stm;
          struct tm etm;
#if defined(_WIN32)
          localtime_s(&stm, &s_raw);
          localtime_s(&etm, &e_raw);
#else
          localtime_r(&s_raw, &stm);
          localtime_r(&e_raw, &etm);
#endif
          const long s_day =
              stm.tm_year * 10000L + stm.tm_mon * 100L + stm.tm_mday;
          const long e_day =
              etm.tm_year * 10000L + etm.tm_mon * 100L + etm.tm_mday;
          same_day_rule = (e_day > s_day) && (etm.tm_hour < 8);
        }
        t_s_s.s = static_cast<int>(s_raw);
        t_s_s.e = e_raw + (same_day_rule ? 0 : 3600);
        t_s_s.q = q;
        t_s_s.duration = t_s_s.e - t_s_s.s;
        if (t_s_s.duration < 0) t_s_s.duration = 0;
        if (q > max_q) max_q = q;
        if (StringToTime_t(s) <= min_s) {
          t_s_s.s = min_s;
          t_s_s.e = t_s_s.s + t_s_s.duration;

        }
        if (StringToTime_t(e) >= max_e) {
          t_s_s.e = (max_e);
          t_s_s.s = t_s_s.e - t_s_s.duration;
        }

        if (q > 0) {
          addDemandProcess(
            demandProcesses,
            g->name,
            0,
            min_s,
            t_s_s.s,
            input_params.dt
          );

          addDemandProcess(
            demandProcesses,
            g->name,
            t_s_s.q,
            t_s_s.s,
            t_s_s.e,
            input_params.dt
          );
          addDemandProcess(
            demandProcesses,
            g->name,
            0,
            t_s_s.e,
            max_e,
            input_params.dt
          );

        }
      }
    }
     return  demandProcesses;

  }

void OCIS_scip::set_bcFlow(json bc_json, MILP_param& input) {

    std::vector<string> bc_Flow_objs = bc_json.get<std::vector<string>>();


    for (int i = 0; i < input.topo_dicts.gates.size(); i++) {
      string g_name = input.topo_dicts.gates[i].name;
      if (isValidUTF8(g_name)) {
        g_name = (g_name);
      }
      Ocis_edges::Point* g = input.topo_dicts.get_gate_byName(g_name);

      if (find_vec(bc_Flow_objs, (g_name))) {

        g->bc_Flow_label = 1;
      }
    }

  }

void OCIS_scip::set_soft_bcFlow_cons(json bc_json, MILP_param& input) {

    std::vector<string> bc_Flow_objs = bc_json.get<std::vector<string>>();


    for (int i = 0; i < input.topo_dicts.gates.size(); i++) {
      string g_name = input.topo_dicts.gates[i].name;
      if (isValidUTF8(g_name)) {
        g_name = (g_name);
      }
      Ocis_edges::Point* g = input.topo_dicts.get_gate_byName(g_name);

      if (find_vec(bc_Flow_objs, (g_name))) {

        g->soft_Flow_cons_label = 1;
      }
    }

  }

void OCIS_scip::set_obs_STime(process_T STime, MILP_param& input) {

    for (auto iter = STime.begin(); iter != STime.end(); iter++) {
      string name = iter->first;

      Ocis_edges::Point* g = input.topo_dicts.get_gate_byName(name);
      auto stime_value = iter->second;

      time_t initial_stime = input.time_start_t;
      for (auto sub_iter = stime_value.begin(); sub_iter != stime_value.end(); sub_iter++) {
        if (sub_iter->first > initial_stime) {
          initial_stime = sub_iter->first;
        }
      }

      if (g->obs_STime != "-1") {
        if (initial_stime > StringToTime_t(g->obs_STime)) {
          g->obs_STime = FormatTime( initial_stime);
        }
      }
      else
      {
          g->obs_STime =FormatTime( initial_stime);
      }

    }

  }

void OCIS_scip::set_obs_Q(process_T STime, MILP_param& input) {

    for (auto iter = STime.begin(); iter != STime.end(); iter++) {
      string name = iter->first;

      Ocis_edges::Point* g = input.topo_dicts.get_gate_byName(name);
      if (g == nullptr) {
        continue;
      }
      auto stime_value = iter->second;


      
      for (auto sub_iter = stime_value.begin(); sub_iter != stime_value.end(); sub_iter++) {
          g->obs_flow = sub_iter->second;
      }
    }

    for (auto& g : input.topo_dicts.gates) {
      if (g.obs_flow >= 0) {
        continue;
      }
      auto flow_iter = input.boundary_flow.find(g.name);
      if (flow_iter == input.boundary_flow.end()) {
        continue;
      }
      auto prior = flow_iter->second.lower_bound(input.time_start_t);
      if (prior == flow_iter->second.begin()) {
        continue;
      }
      g.obs_flow = std::prev(prior)->second;
      assert(std::prev(prior)->first < input.time_start_t);
    }

  }

void OCIS_scip::set_demand_from_boundary_flow(MILP_param& input)
  {
    if (input.boundary_flow.empty()) {
      cout << "[ActionDemand] boundary_flow is empty, skip deriving demand from action.csv" << endl;
      return;
    }

    time_t horizon_start = input.time_start_t;
    time_t horizon_end = input.time_start_t + input.T * 3600;
    double default_dt = input.daily_dt > 0 ? input.daily_dt : 86400;
    int demand_count = 0;
    int unmatched_count = 0;
    double total_W_demand = 0.0;
    double total_raw_action_W = 0.0;
    std::map<string, double> action_W_by_name;
    std::map<string, double> action_W_by_utf8_name;
    std::map<string, double> target_q_by_gate;
    h_csv mapping_debug;
    mapping_debug.push_back({
      "action_obj",
      "raw_volume_m3",
      "residual_volume_m3",
      "mapping_mode",
      "mapped_gate",
      "mapped_gate_maxflow",
      "allocation_ratio",
      "assigned_volume_m3",
      "first_positive_time",
      "last_positive_time"
    });

    for (auto iter = input.boundary_flow.begin(); iter != input.boundary_flow.end(); iter++) {
      double W_input = 0.0;
      auto& series = iter->second;
      for (auto sub_iter = series.begin(); sub_iter != series.end(); sub_iter++) {
        time_t t_t = sub_iter->first;
        if (t_t < horizon_start || t_t >= horizon_end || sub_iter->second <= 0) {
          continue;
        }
        auto iter_next = sub_iter;
        iter_next++;
        time_t next_t = t_t + static_cast<time_t>(default_dt);
        if (iter_next != series.end()) {
          next_t = iter_next->first;
        }
        if (next_t <= t_t) {
          next_t = t_t + static_cast<time_t>(default_dt);
        }
        if (next_t > horizon_end) {
          next_t = horizon_end;
        }
        double dt = difftime(next_t, t_t);
        if (dt > 0) {
          W_input += sub_iter->second * dt;
        }
      }
      if (W_input > 0) {
        action_W_by_name[iter->first] = W_input;
        action_W_by_utf8_name[(iter->first)] = W_input;
        total_raw_action_W += W_input;
      }
    }

    for (auto iter = input.boundary_flow.begin(); iter != input.boundary_flow.end(); iter++) {
      string name = iter->first;

      std::vector<Ocis_edges::Point*> demand_gates;
      Ocis_edges::Point* exact_gate = input.topo_dicts.get_gate_byName(name);
          if (exact_gate==nullptr){
            continue;
          }
          if (U2G(exact_gate->name) == "东一支") {
            int a = 0;
          }


      auto& series = iter->second;
      double W_input = 0.0;
      double max_action_q = 0.0;
      time_t first_positive_t = 0;
      time_t last_positive_end_t = 0;

      for (auto sub_iter = series.begin(); sub_iter != series.end(); sub_iter++) {
        time_t t_t = sub_iter->first;
        if (t_t < horizon_start || t_t >= horizon_end) {
          continue;
        }

        double q = sub_iter->second;
        if (q < 0) {
          continue;
        }
        if (q > max_action_q) {
          max_action_q = q;
        }

        // Preserve the complete input volume even when one day's equivalent
        // flow exceeds the gate capacity. The per-period Q upper bound will
        // spread that volume over more periods; clipping q here silently
        // deleted part of the user's demand.
        if (exact_gate != nullptr && q > exact_gate->maxFlow) {
          cout << "[ActionDemand] daily equivalent flow exceeds maxFlow for "
               << name << "; preserving volume and extending delivery"
               << endl;
        }



        auto iter_next = sub_iter;
        iter_next++;
        time_t next_t = t_t + static_cast<time_t>(default_dt);
        if (iter_next != series.end()) {
          next_t = iter_next->first;
        }
        if (next_t <= t_t) {
          next_t = t_t + static_cast<time_t>(default_dt);
        }
        if (next_t > horizon_end) {
          next_t = horizon_end;
        }

        double dt = difftime(next_t, t_t);
        if (dt <= 0) {
          continue;
        }

        W_input += q * dt;
        if (q > 1.0e-12) {
          if (first_positive_t == 0) {
            first_positive_t = t_t;
          }
          last_positive_end_t = next_t;
        }
      }

      if (W_input <= 0 || first_positive_t == 0) {
        continue;
      }
      double raw_W_input = W_input;

      if (exact_gate == nullptr) {
        exact_gate = input.topo_dicts.get_gate_byName((name));
      }
      if (exact_gate != nullptr) {


        demand_gates.push_back(exact_gate);
      }
      else {
        for (int i = 0; i < input.topo_dicts.gates.size(); i++) {
          Ocis_edges::Point* child_gate = &input.topo_dicts.gates[i];
          if (child_gate->type == 2 && (child_gate->level1 == name || (child_gate->level1) == name)) {
            demand_gates.push_back(child_gate);
          }
        }
        if (demand_gates.empty()) {
          string name_utf8 = (name);
          string prefix_utf8 = name_utf8;
          string management_suffix = "管理站";
          size_t suffix_pos = prefix_utf8.find(management_suffix);
          if (suffix_pos != string::npos) {
            prefix_utf8 = prefix_utf8.substr(0, suffix_pos);
          }
          if (!prefix_utf8.empty() && prefix_utf8 != name_utf8) {
            for (int i = 0; i < input.topo_dicts.gates.size(); i++) {
              Ocis_edges::Point* child_gate = &input.topo_dicts.gates[i];
              string child_name_utf8 = (child_gate->name);
              string child_level1_utf8 = (child_gate->level1);
              if (child_gate->type == 2 &&
                  (child_name_utf8.find(prefix_utf8) == 0 || child_level1_utf8.find(prefix_utf8) == 0)) {
                demand_gates.push_back(child_gate);
              }
            }
          }
        }
      }

      if (demand_gates.empty()) {
        unmatched_count += 1;
        mapping_debug.push_back({
          (name),
          to_string(raw_W_input),
          to_string(raw_W_input),
          "unmatched",
          "",
          "0",
          "0",
          "0",
          FormatTime(first_positive_t),
          FormatTime(last_positive_end_t)
        });
        continue;
      }

      string mapping_mode = exact_gate != nullptr ? "exact_gate" : "level1_residual";
      if (exact_gate == nullptr) {
        double child_exact_W = 0.0;
        for (auto* demand_gate : demand_gates) {
          auto child_W_iter = action_W_by_name.find(demand_gate->name);
          if (child_W_iter != action_W_by_name.end()) {
            child_exact_W += child_W_iter->second;
            continue;
          }
          auto child_W_utf8_iter = action_W_by_utf8_name.find((demand_gate->name));
          if (child_W_utf8_iter != action_W_by_utf8_name.end()) {
            child_exact_W += child_W_utf8_iter->second;
          }
        }
        if (child_exact_W > 0) {
          W_input -= child_exact_W;
          if (W_input <= 0) {
            mapping_debug.push_back({
              (name),
              to_string(raw_W_input),
              to_string(W_input),
              "level1_residual_zero",
              "",
              "0",
              "0",
              "0",
              FormatTime(first_positive_t),
              FormatTime(last_positive_end_t)
            });
            continue;
          }
        }
      }

      double capacity_sum = 0.0;
      for (auto* demand_gate : demand_gates) {
        if (demand_gate->maxFlow > 0) {
          capacity_sum += demand_gate->maxFlow;
        }
      }
      if (capacity_sum <= 0) {
        capacity_sum = static_cast<double>(demand_gates.size());
      }

      for (auto* g : demand_gates) {
        double capacity_weight = (g->maxFlow > 0 && capacity_sum > 0)
          ? g->maxFlow / capacity_sum
          : 1.0 / demand_gates.size();
        double gate_W_input = W_input * capacity_weight;
        double target_q = g->maxFlow > 0 ? g->maxFlow : max_action_q;
        if (target_q <= 0) {
          target_q = max_action_q;
        }
        if (target_q <= 0 || gate_W_input < 0) {
          continue;
        }

        g->isDecisionVariable = (g->type == 2);
        g->demandFlow = target_q;
        // W_input uses a negative legacy sentinel. Clear it before adding any
        // mapped demand; otherwise every positive input task is understated
        // by exactly 1 m3 and therefore cannot be satisfied exactly.
        if (g->W_input < 0) {
          g->W_input = 0;
        }
        g->W_input += gate_W_input;
        g->duration_input = g->W_input / target_q;
        g->W_setLabel = 1;
        g->fixed_W = 1;
        g->STime_input = first_positive_t;
        g->ETime_input = first_positive_t + static_cast<time_t>(ceil(g->duration_input));
        if (g->ETime_input > horizon_end) {
          g->ETime_input = horizon_end;
        }
        if (g->ETime_input < g->STime_input && last_positive_end_t > 0) {
          g->ETime_input = last_positive_end_t;
        }

        input.gates_ScheduleScheme[g->name].clear();
        ScheduleScheme scheme;
        scheme.s = static_cast<int>(g->STime_input);
        scheme.e = static_cast<int>(g->ETime_input);
        scheme.duration = static_cast<int>(g->duration_input);
        scheme.q = target_q;
        input.gates_ScheduleScheme[g->name].push_back(scheme);

        // Defer boundary_flow mutation until the iteration over boundary_flow is
        // complete; mutating the map while reading it double-counts action demand.
        target_q_by_gate[g->name] = target_q;

        total_W_demand += gate_W_input;
        demand_count += 1;
        mapping_debug.push_back({
          (name),
          to_string(raw_W_input),
          to_string(W_input),
          mapping_mode,
          (g->name),
          to_string(g->maxFlow),
          to_string(capacity_weight),
          to_string(gate_W_input),
          FormatTime(first_positive_t),
          FormatTime(last_positive_end_t)
        });
      }
    }

    // Preserve measured source inflow before duration mode replaces demand
    // boundary series with the derived constant target flow.
    {
      double inflow_volume = 0.0;
      for (int i = 0; i < input.topo_dicts.gates.size(); ++i) {
        Ocis_edges::Point* source_gate = &input.topo_dicts.gates[i];
        if (source_gate->type != 0) {
          continue;
        }
        auto boundary = input.boundary_flow.find(source_gate->name);
        if (boundary == input.boundary_flow.end()) {
          boundary = input.boundary_flow.find(G2U(source_gate->name));
        }
        if (boundary == input.boundary_flow.end()) {
          boundary = input.boundary_flow.find(U2G(source_gate->name));
        }
        if (boundary == input.boundary_flow.end()) {
          continue;
        }
        auto& series = boundary->second;
        for (auto sample = series.begin(); sample != series.end(); ++sample) {
          if (sample->first < horizon_start || sample->first >= horizon_end ||
              sample->second <= 0) {
            continue;
          }
          auto next = sample;
          ++next;
          time_t next_time = next != series.end()
              ? next->first
              : sample->first + static_cast<time_t>(default_dt);
          if (next_time <= sample->first) {
            next_time = sample->first + static_cast<time_t>(default_dt);
          }
          if (next_time > horizon_end) {
            next_time = horizon_end;
          }
          const double duration = difftime(next_time, sample->first);
          if (duration > 0) {
            inflow_volume += sample->second * duration;
          }
        }
      }
      if (inflow_volume > 0) {
        input.inflow_volume_m3 = inflow_volume;
        cout << "[ActionDemand] preserved inflow_volume_m3="
             << inflow_volume << endl;
      }
    }

    if (input.demand_source == "duration") {
    for (auto iter = target_q_by_gate.begin(); iter != target_q_by_gate.end(); iter++) {
      Ocis_edges::Point* target_gate = input.topo_dicts.get_gate_byName(iter->first);
      if (target_gate == nullptr) {
        target_gate = input.topo_dicts.get_gate_byName(G2U(iter->first));
      }
      if (target_gate == nullptr) {
        target_gate = input.topo_dicts.get_gate_byName(U2G(iter->first));
      }
      if (target_gate != nullptr && target_gate->type == 0) {
        continue;
      }
      input.boundary_flow[iter->first].clear();
      input.boundary_flow[iter->first][horizon_start] = iter->second;
      input.boundary_flow[iter->first][horizon_end] = iter->second;
    }


    }
    mkdir_h(input.outputpath + "output/");
    write_h_csv(input.outputpath + "output/action_demand_mapping_debug.csv", mapping_debug, ',');

    cout << "[ActionDemand] derived demand from action.csv: objects=" << demand_count
         << ", unmatched_action_objects=" << unmatched_count
         << ", raw_total_W=" << total_raw_action_W
         << ", assigned_total_W=" << total_W_demand << " m3" << endl;
  }

void OCIS_scip::set_bcStage(json bc_json, MILP_param& input) {

    std::vector<string> bc= bc_json.get<std::vector<string>>();


    for (int i = 0; i < input.topo_dicts.pools.size(); i++) {
      string g_name = input.topo_dicts.pools[i].name;
      if (isValidUTF8(g_name)) {
        g_name = (g_name);
      }
      Ocis_edges::Edges* p = &input.topo_dicts.pools[i];

      if (find_vec(bc, (g_name))) {

        p->bc_Stage_label = 1;
      }
    }

  }

template<typename ResultType>
void ResultJsonToAction(
    const json& result_json,
    ResultType& result,
    int dt,
    int T)
{
      if (dt <= 0)
    {
        throw std::invalid_argument("dt 必须大于 0");
    }

    if (T <= 0.0)
    {
        throw std::invalid_argument("T 必须大于 0");
    }

    
    const double totalSeconds = T * 3600.0;

    const int stepCount =
        static_cast<int>(
            std::ceil(totalSeconds / dt));

    
    std::time_t simulationStart =
        std::numeric_limits<std::time_t>::max();

    bool hasValidRecord = false;

    for (auto gateIt = result_json.begin();
         gateIt != result_json.end();
         ++gateIt)
    {
        const json& records = gateIt.value();

        if (!records.is_array())
        {
            continue;
        }

        for (const auto& record : records)
        {
            if (!record.contains("s") ||
                !record["s"].is_string())
            {
                continue;
            }

            const std::time_t startTime =
                StringToTime_t(
                    record["s"].get<std::string>());

            simulationStart =
                std::min(simulationStart, startTime);

            hasValidRecord = true;
        }
    }

    if (!hasValidRecord)
    {
        throw std::runtime_error(
            "result_json 中没有有效的 s 时间");
    }

    
    result.solution["action"].clear();

    
    for (auto gateIt = result_json.begin();
         gateIt != result_json.end();
         ++gateIt)
    {
        const std::string name = gateIt.key();
        const json& records = gateIt.value();

        if (!records.is_array())
        {
            continue;
        }

        
        std::vector<double>& action =
            result.solution["action"][name];

        action.assign(stepCount, 0.0);

        for (const auto& record : records)
        {
            if (!record.contains("s") ||
                !record.contains("e") ||
                !record.contains("q"))
            {
                continue;
            }

            const std::time_t startTime =
                StringToTime_t(
                    record["s"].get<std::string>());

            const std::time_t endTime =
                StringToTime_t(
                    record["e"].get<std::string>());

            const double q =
                record["q"].get<double>();

            if (endTime <= startTime)
            {
                continue;
            }

            const double startOffset =
                std::difftime(
                    startTime,
                    simulationStart);

            const double endOffset =
                std::difftime(
                    endTime,
                    simulationStart);

            







            int startIndex =
                static_cast<int>(
                    std::floor(startOffset / dt));

            int endIndex =
                static_cast<int>(
                    std::ceil(endOffset / dt));

            startIndex =
                std::max(startIndex, 0);

            endIndex =
                std::min(endIndex, stepCount);


            if (startIndex > 1) {
              int a = 0;
            }

            for (int i = 0;
                 i < stepCount;
                 ++i)
            {
              if (i >= startIndex && i <= endIndex) {
                result.solution["action"][name][i] = q;
              }
            }
        }
    }
}

  std::map<string, std::vector<double>> OCIS_scip::get_max_action(std::map<string, std::vector<double>>& data) {

    std::map<string, std::vector<double>> max_action_map;

    for (auto iter = data.begin(); iter != data.end(); iter++) {
      string name = iter->first;
      if (iter->second.size() > 0) {

        std::map<double, int> value_index_map;
        for (int i = 0; i < iter->second.size();i++) {
          value_index_map[iter->second[i]] = i;
        }


        double value = value_index_map.rbegin()->first;
        max_action_map[name].push_back(value);
      }
    }

    return max_action_map;
  }

 OCIS_scip::MILP_result OCIS_scip::MILP_ocis_solver_LP_PathwaysPlanning_sparsity(OCIS_scip::MILP_param& input_param) {
    OCIS_scip::MILP_result result;
    // A physical gate can occur in edges.csv in more than one role.  Its
    // canonical Point::type is taken from the first occurrence, so relying on
    // that single value can misclassify a type-2 irrigation turnout as a
    // conveyance/check gate.  Preserve the per-edge turnout role instead.
    std::vector<bool> is_irrigation_gate(input_param.num_vars, false);
    for (const auto& pool : input_param.topo_dicts.pools) {
      for (const auto& turnout : pool.turnouts) {
        if (turnout.type == 2 && turnout.id >= 0 &&
            turnout.id < input_param.num_vars) {
          is_irrigation_gate[turnout.id] = true;
        }
      }
    }
    const auto is_auxiliary_gate = [](const Ocis_edges::Point& gate) {
      return gate.type == 1 || gate.type == 6;
    };
    const auto is_delivery_demand_gate = [&](const Ocis_edges::Point& gate) {
      const bool irrigation_delivery =
          gate.id >= 0 && gate.id < input_param.num_vars &&
          is_irrigation_gate[gate.id];
      return !is_auxiliary_gate(gate) && gate.type != 0 &&
             (irrigation_delivery || gate.fixed != 1) &&
             gate.fixed_W == 1 && gate.W_input > 0;
    };
    for (int i = 0; i < input_params.topo_dicts.gates.size(); i++) {

      Ocis_edges::Point* g =
        &input_params.topo_dicts.gates[i];
      if (g->fixed_W == 1) {
        cout << g->W_input << endl;;
      }
    }

    //Encourage larger regulating-gate flow
    input_param.actions_weights.resize(input_param.topo_dicts.gates.size());
    input_param.actionsGap_weights.resize(input_param.topo_dicts.gates.size());
    input_param.WGap_weights.resize(input_param.topo_dicts.gates.size());
    input_param.Delta_actions_weights.resize(input_param.topo_dicts.gates.size());
    const double auxiliary_flow_weight = 1.0e6;
    for (int i = 0; i < input_param.topo_dicts.gates.size(); i++) {
      Ocis_edges::Point* g = &input_param.topo_dicts.gates[i];

      input_param.actions_weights[i] = 10e0 *  (1.0/g->maxFlow);
      input_param.actionsGap_weights[i] = 10e6 * (1.0 / g->maxFlow);
      if (g->type == 0||g->type==4) {
        input_param.actions_weights[i] = 0;
      }
      if (g->type == 2) {

        if (g->fixed_W == 1) {
          input_param.actions_weights[i] = 10e0 * (1.0 / g->maxFlow);
          input_param.WGap_weights[i] =10e0 * 1 / ( g->maxFlow);
        }
        else
        {
          input_param.actions_weights[i] = 10e0 * (1.0 / g->maxFlow);
        }
      }
      if (g->type == 1||g->type==6) {
        // Type 1/6 flows are auxiliary balancing variables. They are never
        // delivery tasks and must be driven to their smallest feasible value.
        input_param.actions_weights[i] =
            auxiliary_flow_weight / std::max(1.0, g->maxFlow);
      }

      input_param.gates_max_flow[i] = g->maxFlow;
      input_param.gates_min_flow[i] = 0;
    }


        // Method 1.1: use the default random engine and distribution
    std::random_device rd;  // Nondeterministic random seed
    std::mt19937 gen(rd()); // Mersenne Twister 19937 engine
    std::uniform_real_distribution<> dis(0.0, 1.0); // Uniform distribution on [0.0, 1.0)
    
    std::vector<double> y_min; y_min.resize(input_param.topo_dicts.pools.size());
    std::vector<double> y_max; y_max.resize(input_param.topo_dicts.pools.size());
    for (int i = 0; i < input_param.topo_dicts.pools.size(); i++) {
      Ocis_edges::Edges* p = &input_param.topo_dicts.pools[i];
      y_min[i] = p->min_h;// p->pool_h_ic - 0.2;
      y_max[i] = p->max_h;

      if (p->irrigation_process == -1) {
        p->irrigation_process = dis(gen);
        p->waterlevel_threshold = dis(gen);
        cout << p->irrigation_process << endl;
        cout << p->waterlevel_threshold << endl;
      }

    }
    h_csv_double demand_state;
    h_csv_double demand_flow;
    demand_state.resize(input_param.time_vars);
    demand_flow.resize(input_param.time_vars);
    for (int j = 0; j < input_param.time_vars; ++j) {
      demand_state[j].resize(input_param.num_vars);
      demand_flow[j].resize(input_param.num_vars);
    }

    for (int j = 0; j < input_param.time_vars; ++j) {
      for (int i = 0; i < input_param.num_vars; ++i) {
        Ocis_edges::Point* g = &input_param.topo_dicts.gates[i];

        Ocis_edges::Edges* p = input_param.topo_dicts.get_pool_byTurnoutsName(g->name);
        string g_name = input_param.topo_dicts.gates[i].name;
        time_t k_t = StringToTime_t(input_param.time_start) + (j * input_param.dt);
        double obs_flow = input_param.topo_dicts.gates[i].obs_flow > 0 ? input_param.topo_dicts.gates[i].obs_flow : 0;
        auto it = input_param.boundary_flow.find(g_name);
        double UQ = -1;
        if (it != input_param.boundary_flow.end()) {
          UQ = GateScheduleLinearInterpolate(it->second, k_t);
        }

        if (UQ > 0) {
          demand_state[j][i] = 1;
          demand_flow[j][i] = UQ;
        }

        if (k_t > g->STime_input && k_t < g->ETime_input) {
          demand_state[j][i] = 1;
          demand_flow[j][i] = g->demandFlow;

        }


      }
    }

    for (auto iter = input_param.boundary_flow.begin(); iter != input_param.boundary_flow.end(); iter++) {

      string name = iter->first;
      Ocis_edges::Point* g = input_param.topo_dicts.get_gate_byName(name);
      Ocis_edges::Edges* pool_next = input_param.topo_dicts.get_pool_bySourceName(name);

      if (g == nullptr) {
        continue;
      }

      double sum_value = 0;
      for (auto sub_iter = iter->second.begin(); sub_iter != iter->second.end(); sub_iter++) {
        if (sub_iter->first >= input_param.time_start_t + input_param.T * 3600) {
          break;
        }
        double value = sub_iter->second;
        if (value < 0) {
          value = 0;
        }
        if (value > g->maxFlow) {
          value = g->maxFlow;
        }
        sum_value += input_param.dt * value;
      }

      if (g != nullptr && (g->type == 2 ||g->type == 4||g->type==-1) && g->fixed != 1) {

        double W_max = g->maxFlow * input_param.dt * input_param.time_vars;
        if (W_max < sum_value) {
          sum_value = W_max;
        }
        //g->W_input = sum_value;
        //g->fixed_W = 1;
      }

    }

    for (int i = 0; i < input_param.num_vars; ++i) {

        Ocis_edges::Point* g = &input_param.topo_dicts.gates[i];

        if (g->type == 2 && g->demandFlow > 0 && g->fixed != 1) {

        g->fixed_W = 1;

      }


    }


    try {
      SCIPModel model;
      auto y = model.addVariables2D(input_param.num_pools, input_param.time_vars, "y", y_min, y_max, 0.0, SCIPModel::VarType::CONTINUOUS);
      auto infiltrationQ = model.addVariables2D(input_param.num_pools, input_param.time_vars, "infiltrationQ", 0, SCIPinfinity(model.getSCIP()), 0, SCIPModel::VarType::CONTINUOUS);
      auto fabs_dy = model.addVariables2D(input_param.num_pools, input_param.time_vars, "fabs_dy", 0, 5, 1., SCIPModel::VarType::CONTINUOUS);
      auto dy = model.addVariables2D(input_param.num_pools, input_param.time_vars, "dy", -1000, 1000, 0.00, SCIPModel::VarType::CONTINUOUS);
      auto yGap = model.addVariables2D(input_param.num_pools, input_param.time_vars, "yGap", 0, 1000, 1, SCIPModel::VarType::CONTINUOUS);

      // ========== 1. Variable definitions ==========
      // 1D continuous variables (3) in [0,10]; use larger weights for small flows
      auto Q = model.addVariables2D(input_param.num_vars, input_param.time_vars, "Q", input_param.gates_min_flow, input_param.gates_max_flow, input_param.actions_weights, SCIPModel::VarType::CONTINUOUS);
      auto fabs_dQ = model.addVariables2D(input_param.num_vars, input_param.time_vars, "fabs_dQ", 0, input_param.gates_max_flow, 1, SCIPModel::VarType::CONTINUOUS);
      auto dQ = model.addVariables2D(input_param.num_vars, input_param.time_vars, "dQ", -1000, 1000, 0, SCIPModel::VarType::CONTINUOUS);
      auto QGap = model.addVariables2D(input_param.num_vars, input_param.time_vars, "QGap", 0, 100, input_param.actionsGap_weights, SCIPModel::VarType::CONTINUOUS);
      auto W = model.addVariables1D(input_param.num_vars, "W", 0, SCIPinfinity(model.getSCIP()), 0, SCIPModel::VarType::CONTINUOUS);
      auto WGap = model.addVariables1D(input_param.num_vars, "WGap", 0, SCIPinfinity(model.getSCIP()),input_param.WGap_weights, SCIPModel::VarType::CONTINUOUS);

      // 2D continuous variables (2x3) in [0,5]

      add_sum_t_cons(model, input_param.dt, Q, W, input_param.num_vars, input_param.time_vars, "W_sum");
      for (int i = 0; i < input_param.num_vars; ++i) {
        Ocis_edges::Point* g = &input_param.topo_dicts.gates[i];
          if (U2G(g->name) == "孤城节制闸") {
            int a = 0;
          }

        const bool is_type2_irrigation_gate =
            g->id >= 0 && g->id < input_param.num_vars &&
            is_irrigation_gate[g->id];
        if (is_type2_irrigation_gate) {
          // Type-2 gates are irrigation deliveries, never surplus-water
          // outlets. Bind every irrigation gate to its supplied volume; an
          // absent/zero input demand therefore means W == 0.  This is based on
          // the edge role rather than the potentially overwritten Point type.
          const double required_irrigation_volume =
              g->fixed_W == 1 && g->W_input > 0 ? g->W_input : 0.0;
          add_Equal_cons(model, 1, W(i), required_irrigation_volume,
                         0.0, 0.0, g->id, 0, "W_irrigation_hard_cons");
        }
        else if (g->fixed_W == 1 && g->W_input >= 0 && g->type != 0 &&
                 !is_auxiliary_gate(*g)) {
          // Every supplied delivery volume is a hard requirement. Auxiliary
          // type-1/type-6 variables are deliberately excluded from demand.
          add_Equal_cons(model, 1, W(i), g->W_input, 0.0, 0.0,
                         g->id, 0, "W_hard_cons");
          if (U2G(g->name) == "东一支") {
            int a = 0;
          }
          //add_min_Gap_cons(model, W(i), WGap(i), input_param, i, 0, g->W_input, "WGap_cons");
        }

        //Initial conditions
        int j = 0;
        if (g->obs_flow >= 0) {
            add_min_Gap_cons(model, Q(i, j), QGap(i, j), input_param, i, j, g->obs_flow, "initialQ_cons");
              //add_Equal_cons(model, 1, Q(g->id, j), g->obs_flow, 0.001 * g->obs_flow, g->id, j, "Q_hard_cons");
        }
        else
        {
          auto it = input_param.boundary_flow.find(g->name);
          double UQ = -1;
          time_t k_t = StringToTime_t(input_param.time_start) + (j * input_param.dt);
          if (it != input_param.boundary_flow.end()) {
            UQ =GateScheduleLinearInterpolate(it->second, k_t);
          }
          g->obs_flow = UQ;
          //add_min_Gap_cons(model, Q(i, j), QGap(i, j), input_param, i, j, g->obs_flow, "initialQ_cons");

        }

        //Terminal-state target
        j = input_param.time_vars-1;
        if (g->end_flow >= 0) {
            //add_Equal_cons(model, 1, Q(g->id, j), g->end_flow, 0.2 * g->end_flow, g->id, j, "endflow_cons");//Forcing upstream and downstream regulating-gate flows may cause leakage-flow constraint violations
        }

      }

      // ========== 2. Add constraints ==========
      for (int j = 0; j < input_param.time_vars; ++j) {
        for (int i = 0; i < input_param.num_vars; ++i) {
          Ocis_edges::Point* g = &input_param.topo_dicts.gates[i];

          Ocis_edges::Edges* p = input_param.topo_dicts.get_pool_byTurnoutsName(g->name);
          string g_name = input_param.topo_dicts.gates[i].name;
          time_t k_t = StringToTime_t(input_param.time_start) + (j * input_param.dt);
          double obs_flow = input_param.topo_dicts.gates[i].obs_flow > 0 ? input_param.topo_dicts.gates[i].obs_flow : 0;
          auto it = input_param.boundary_flow.find(g_name);
          double UQ = -1;
          if (it != input_param.boundary_flow.end()) {
            UQ = GateScheduleLinearInterpolate(it->second, k_t);
          }

          if (g->demandFlow > 0&&input_param.demand_source=="duration") {
            UQ = g->demandFlow;
          }


          if (UQ > g->maxFlow) {
            UQ = g->maxFlow;
          }

          if (g->fixed_W == 1 && g->obs_flow > 0) {
            //add_min_Gap_cons(model, Q(i, j), QGap(i, j), input_param, i, j, g->obs_flow, "maxQ_cons");
          }

          if (UQ >= 0) {
            if (input_param.ref_flow_label == 1) {
              add_min_Gap_cons(model, Q(i, j), QGap(i, j), input_param, i, j, UQ, "QGap_designflow_cons");
            }
            if (g->fixed == 1 ) {
              // Prescribed external/internal inflows must match their
              // supplied processes exactly. Other fixed gates retain their
              // existing task-gap tolerance.
              const bool is_prescribed_inflow =
                  g_name == "黄壁庄" || g_name == "引江口";
              const double fixed_epsilon =
                  is_prescribed_inflow ? 0.0 : g->W_task_gap * UQ;
              add_Equal_cons(model, 1, Q(g->id, j), UQ, fixed_epsilon, g->id, j, "Q_hard_cons");
            }
            //if (j == 0) {
            //  add_Equal_cons(model, 1, Q(g->id, j), UQ,  UQ, g->id, j, "Q_hard_cons");
            //}


          }
          else if(g->obs_flow>=0)
          {
            add_min_Gap_cons(model, Q(i, j), QGap(i, j), input_param, i, j, g->obs_flow, "QGap_designflow_cons");

          }
          else
          {
            //add_min_Gap_cons(model, Q(i, j), QGap(i, j), input_param, i, j, 0, "QGap_designflow_cons");
          }

          //Leakage
          if (g->type == 6) {
              //add_Equal_cons(model, 1, Q(g->id, j), 1, infiltrationQ(p->id, j), 0.0 * g->maxFlow, g->id, j, "gate_infiltrationQ_cons");
          }

          //2.1.3 Minimize flow variation between consecutive time steps
          if (j > 0) {
            add_delta_variable_cons(model, Q(i, j - 1), Q(i, j), dQ(i, j), i, j, "dQ_cons");
            //add_greaterOrEqual_cons(model, 0.2, Q(i, j - 1), fabs_dQ(i, j), i, j, "fabsdQ>20_cons");
          }

          if (g->delta_Q_max > 0&&g->fixed!=1) {
            add_lessOrEqual_cons(model,g->delta_Q_max, fabs_dQ(i, j), i, j, "fabsdQ>20_cons");
          }

          //2.1.4 Absolute-value constraint on flow variation
          add_fabs_cons(model, dQ(i, j), fabs_dQ(i, j), i, j, "fabs_dQ_cons");


        }
      }

      // ********** **********Initial conditions ********************//
      add_FlowBalance_internalModel_cons(model, input_param, Q, y, true, "FlowBalance_internal_model_");
      //add_BiggerFlow_dynamic_internalModel_cons(model, input_param, x, "FlowBalance_dynamic_model_");

      for (int i = 0; i < input_param.num_pools; ++i) {
        string g_name = input_param.topo_dicts.pools[i].source[0].name;  /** Use only the upstream regulating gate of the pool to compute flow and velocity*/
        string g_end_name = input_param.topo_dicts.pools[i].targets[0].name;  /** Use only the upstream regulating gate of the pool to compute flow and velocity*/
        Ocis_edges::Point* g_source = input_param.topo_dicts.get_gate_byName(g_name);
        Ocis_edges::Point* g_end = input_param.topo_dicts.get_gate_byName(g_end_name);
        int g_source_id = g_source->id;
        int g_end_id = g_end->id;

        Ocis_edges::Edges* p = &input_param.topo_dicts.pools[i];


        for (int j = 0; j < input_param.time_vars; ++j) {
          time_t k_t = StringToTime_t(input_param.time_start) + (j * input_param.dt);

          double Uy = GateScheduleLinearInterpolate(input_param.boundary_stage[g_end_name], k_t);
          if (Uy > 0) {
            add_min_Gap_cons(model, y(i, j), yGap(i, j), input_param, i, j, Uy, "yGap_designflow_cons");
            //add_Equal_cons(model, 1, y(i, j), Uy, i, j, "y_hard_cons");                                       //Forcing upstream and downstream regulating-gate flows may cause leakage-flow constraint violations
          }
          else
          {
            add_min_Gap_cons(model, y(i, j), yGap(i, j), input_param, i, j, p->pool_h_ic, "yGap_designflow_cons");
          }


          //2.1.3 Minimize flow variation between consecutive time steps
          if (j > 0) {
            add_delta_variable_cons(model, y(i, j - 1), y(i, j), dy(i, j), i, j, "dy_cons");
          }
          //2.1.4 Absolute-value constraint on flow variation
          add_fabs_cons(model, dy(i, j), fabs_dy(i, j), i, j, "fabs_dy_cons");
          //  add_Equal_cons(model, (1 - p->utilization_ratio), Q(g_source_id, j), 1, infiltrationQ(p->id, j), 0.0, p->id, j, "pool_infiltrationQ_cons");
        }
      
      }



      SCIP_RETCODE retcode;
      retcode = model.solve();

      // Used for iterative solving
      //model.resetModel(model.getSCIP());
      //add_Equal_cons(model, 0, Q(0, 0), 1, W(0, 0), 0., 0, 0, "gate_W_resolveTest_cons");
      //retcode = model.solve();

      if (retcode == SCIP_OKAY) {
        std::cout << "=== SCIP_OKAY ===\n";

      SCIP_STATUS status = SCIPgetStatus(model.getSCIP());
        string model_lp_path = input_param.outputpath + "/model.lp";
        SCIPwriteOrigProblem(model.getSCIP(), model_lp_path.c_str(), nullptr, FALSE);
      // ========== 3. Solve and output ==========
        if (status != SCIP_STATUS_OPTIMAL) {
          std::cout << "[Solve Label] Can't find optimal solution" << std::endl;


          if (input_param.W_based_optimalAllocation == 1 && result.find_optimal_solution_label != 1) {

            SCIP* scip = model.getSCIP();
            //Start iteration
            SCIP_Real global_ub;
            for (int i = 0; i < input_param.num_vars; i++) {
              Ocis_edges::Point* g = &input_param.topo_dicts.gates[i];
              //std::cout << "*****************************************" << g->name << ": " << i << " *************************************" << endl;
              model.resetModel(scip);
              for (int j = 0; j < input_param.time_vars; j++) {
                // Modify the feasible region
     //           SCIPchgVarLb(scip, Q(i,j), 0);  // New lower bound: 2.0
                // Get the upper bound
                SCIP_Real global_ub = SCIPvarGetUbGlobal(Q(i, j));

                SCIPchgVarUb(scip, Q(i, j), 999);   // New upper bound: 8.0
              }
              model.resetModel(scip);
              retcode = model.solve();
              SCIP_STATUS status_debug = SCIPgetStatus(model.getSCIP());

              if (status == SCIP_STATUS_OPTIMAL) {
                result.find_optimal_solution_label = 1;
                //std::cout << "*****************************************" << g->name << ": " << i << " *************************************" << endl;
                break;
              }
            }
          }
        }
      if (status == SCIP_STATUS_INFEASIBLE) {
        std::cout << "[Solve Label] infeasible" << std::endl;
      }
      else if (status == SCIP_STATUS_OPTIMAL) {

        // Even when the status is OPTIMAL, verify that a solution exists
        SCIP_SOL* sol = SCIPgetBestSol(model.getSCIP());
        if (sol == NULL) {
          std::cout << "Warning: Optimal status but no solution available" << std::endl;
          std::cout << "This may be due to numerical issues" << std::endl;
          exit(-1);
        }

        std::cout << "[Solve Label] find optimal solution" << std::endl;
        result.find_optimal_solution_label = 1;

        SCIP* scip = model.getSCIP();


        //Priority queue (pool)
        std::map<int, double> pool_Wdemand;//Reach weight, reach ID
        std::map<int, double> pool_Qmax;//Reach weight, reach ID
        std::map<double, int> pool_queue;//Reach weight, reach ID
        std::map<int, std::map<double, int>> pool_offtakes_queue;//Reach ID, <gate weight, gate ID>


        for (int i = 0; i < input_param.num_pools; ++i) {
          Ocis_edges::Edges* p = &input_param.topo_dicts.pools[i];
          for (int g = 0; g < input_param.topo_dicts.pools[i].turnouts.size(); ++g) {
            string name = input_param.topo_dicts.pools[i].turnouts[g].name;
            Ocis_edges::Point* g_tp = input_param.topo_dicts.get_gate_byName(name);
            if (g_tp->edge_stage > p->pool_Length || g_tp->edge_stage < 0) {
              // Gate stationing within the reach is invalid; assign a random value
              //Method 1.1: use the default random engine and distribution
              std::random_device rd1;  // Nondeterministic random seed
              std::mt19937 gen1(rd1()); // Mersenne Twister 19937 engine
              std::uniform_real_distribution<> dis1(0.2 * p->pool_Length, 0.9 * p->pool_Length);
              g_tp->edge_stage = dis1(gen1);
            }
            if (g_tp->W_input > 0) {
              pool_Wdemand[i] = g_tp->W_input;
              pool_Qmax[i] = g_tp->maxFlow;
            }
          }
        }

        std::vector<std::vector<double> > weights;
        weights.resize(input_param.num_vars);
        double epsilon = 0.1;
        for (int i = 0; i < weights.size(); i++) {
          weights[i].resize(input_param.time_vars);
        }

        if (input_param.W_based_optimalAllocation == 1) {
          // Start iterative reweighting (IRL1) and variable pruning (Fix-and-Optimize)
          
          // Core IRL1 parameters
          double epsilon_irl1 = 1e-3; // Smoothing parameter to avoid a zero denominator
          double C_scale = 10;       // Base penalty coefficient; adjust to the objective-function scale
          double max_weight = 10000.0;// Weight cap to prevent SCIP numerical instability
          double zero_tolerance = 1e-3; // Hard threshold for treating a value as zero
          double epsilon_tv = 1e-4; // Prevent a zero denominator in dQ weighting
          double C_tv = 1.0;        // Base penalty coefficient for dQ
          double max_tv_weight = 10e6; // Maximum dQ penalty
          // Penalize late allocation flow. Since W is fixed, this advances
          // the same task volume instead of shrinking it.
          const double early_delivery_weight = 1.0e6;



          // At the outer level (or as a class member), track the latest currently allowed delivery time
// Initialize to the maximum time step
          int allowed_max_j = input_param.time_vars - 1;
          // If infeasible, temporarily skip this round of hard fixing on the retry
          bool skip_hard_fix_once = false;

          // Allow at most one automatic rollback per iteration to prevent an infinite loop
          int infeasible_retry_count = 0;
                    //Start iteration
          // A fixed five rounds only removes four tail periods, so on a
          // 30-period horizon it still leaves an almost uniform 26-period
          // schedule.  Continue until the active horizon reaches its smallest
          // feasible prefix (or all periods have been tested).
          const int max_irl1_iterations = input_param.time_vars + 1;
          for (int iter = 0; iter < max_irl1_iterations; iter++) {

            model.resetModelPreserveSolution(scip);

            // ============================================================
            // Added: save Q upper bounds before this iteration modifies them
            // Used to undo new Q=0 hard fixes if this iteration is infeasible
            // ============================================================
            std::vector<std::vector<SCIP_Real>> Q_ub_backup(
              input_param.num_vars,
              std::vector<SCIP_Real>(
                input_param.time_vars,
                0.0));

            for (int backup_i = 0;
              backup_i < input_param.num_vars;
              ++backup_i) {

              for (int backup_j = 0;
                backup_j < input_param.time_vars;
                ++backup_j) {

                SCIP_VAR* q_var = Q(backup_i, backup_j);

                if (q_var != nullptr) {
                  Q_ub_backup[backup_i][backup_j] =
                    SCIPvarGetUbGlobal(q_var);
                }
              }
            }

            // Compress the service/off-take schedule. A prescribed source
            // such as Huangbizhuang can remain nonzero throughout the horizon;
            // fixing every conveyance gate after the service prefix would
            // contradict that boundary process and make the model infeasible.
            if (!skip_hard_fix_once) {
              for (int compact_i = 0;
                   compact_i < input_param.num_vars;
                   ++compact_i) {
                const Ocis_edges::Point* compact_gate =
                    &input_param.topo_dicts.gates[compact_i];
                const bool is_service_gate =
                    is_delivery_demand_gate(*compact_gate);
                if (!is_service_gate) {
                  continue;
                }
                for (int compact_j = allowed_max_j + 1;
                     compact_j < input_param.time_vars;
                     ++compact_j) {
                  SCIP_VAR* q_var = Q(compact_i, compact_j);
                  if (q_var != nullptr) {
                    SCIPchgVarUb(scip, q_var, 0.0);
                    SCIPchgVarObj(scip, q_var, 0.0);
                  }
                }
              }
            }

            double epsilon = 1e-3 / (1.0 + pow(10, -iter));

            //Compute arrival time
            for (auto iter = input_param.topo_dicts.vec_graph_id.begin(); iter != input_param.topo_dicts.vec_graph_id.end(); iter++) {
              int i = iter->second;
              string name_s = input_param.topo_dicts.pools[i].source[0].name;
              Ocis_edges::Point* g_s = input_param.topo_dicts.get_gate_byName(name_s);
              Ocis_edges::Edges* p = &input_param.topo_dicts.pools[i];
              Ocis_edges::Edges* p_pre = input_param.topo_dicts.get_pool_byEndName(name_s);

              if (p_pre == nullptr) {
                double Qvalue_pre_s = model.getSolution(Q(g_s->id, 0));
                double hvalue_pre_p = model.getSolution(y(p->id, 0));
                g_s->STime = g_s->GeodesicDistance_in_pool / ((Qvalue_pre_s / (p->pool_Width * hvalue_pre_p)) + sqrt(9.8 * hvalue_pre_p));
              }
              else
              {
                string name_pre_s = input_param.topo_dicts.pools[p_pre->id].source[0].name;
                Ocis_edges::Point* g_pre_s = input_param.topo_dicts.get_gate_byName(name_pre_s);
                double Qvalue_pre_s = model.getSolution(Q(g_pre_s->id, 0));
                double hvalue_pre_p = model.getSolution(y(p_pre->id, 0));
                g_s->STime = g_pre_s->STime + g_s->GeodesicDistance_in_pool / ((Qvalue_pre_s / (p_pre->pool_Width * hvalue_pre_p)) + sqrt(9.8 * hvalue_pre_p));
              }


              double Qvalue_s = model.getSolution(Q(g_s->id, 0));

              double hvalue_p = model.getSolution(y(p->id, 0));
              for (int g = 0; g < input_param.topo_dicts.pools[i].turnouts.size(); ++g) {
                string name = input_param.topo_dicts.pools[i].turnouts[g].name;
                Ocis_edges::Point* g_tp = input_param.topo_dicts.get_gate_byName(name);
                g_tp->STime = g_s->STime + g_tp->GeodesicDistance_in_pool / ((Qvalue_s / (p->pool_Width * hvalue_p)) + sqrt(9.8 * hvalue_p));
                g_tp->STime_ratio = ((double)g_tp->STime / input_param.dt) / input_param.T;

                if (g_tp->STime_ratio == 0) {
                  g_tp->STime_ratio = dis(gen);
                }

              }
            }
            std::map<int, int> sparsity_k;//Current best solution: sparsity of each gate
            double max_sparsity = 0;
            for (int i = 0; i < input_param.num_vars; i++) {
              int opt_sparsity_k = 0;
              Ocis_edges::Point* g = &input_param.topo_dicts.gates[i];
              if (is_delivery_demand_gate(*g)) {
                //Compute ideal sparsity
                double L0_num = 0;
                if (g->W_input > 0 && g->fixed_W == 1) {
                  L0_num = g->W_input / (g->maxFlow * input_param.dt);
                }
                //Compute sparsity
                for (int j = 0; j < input_param.time_vars; ++j) {
                  double value = model.getSolution(Q(i, j));
                  double abs_value = fabs(value);
                  if (value > 0) {
                    opt_sparsity_k++;
                  }
                }


                sparsity_k[i] = opt_sparsity_k;

                if (max_sparsity < fabs(opt_sparsity_k - L0_num) / L0_num) {
                  max_sparsity = fabs(opt_sparsity_k - L0_num) / L0_num;
                }
                if (max_sparsity != 0 && max_sparsity < 0.2) {
                  //continue;
                }
                for (int j = 0; j < input_param.time_vars; ++j) {
                  Ocis_edges::Point* g = &input_param.topo_dicts.gates[i];
                  double opt_L0_num = 0;
                  //Apply reweighting to important adjustments, emphasizing low-sparsity cases
                  double value = model.getSolution(Q(i, j));
                  double abs_value = fabs(value);
                  // Timeline compression can take many rounds.  Cap the
                  // reweighting exponent to avoid numerical overflow while
                  // the feasible active prefix is being searched.
                  double r_weight = pow(10, std::min(iter, 4));
                  //double r_weight = ((1.0 / input_param.topo_dicts.gates[i].maxFlow) * 1.0) / (value + epsilon);
                
                  // =========================================================
                  // Hard cutoff: if the current time step exceeds the latest allowed horizon, fix it directly to 0
                  // =========================================================
                  if (!skip_hard_fix_once &&
                    iter >= 0 &&
                    j > allowed_max_j) {
                    SCIPchgVarUb(scip, Q(i, j), 0.0);  // Physically disallow flow in this period
                    SCIPchgVarObj(scip, Q(i, j), 0.0); // Set the objective coefficient to zero
                    continue; // Skip all subsequent reweighting for this variable
                  }

                  // ==========================================================
                  // Core improvement 1: heuristic pruning (Hard Thresholding / Variable Fixing)
                  // After more than one iteration, fix flow to 0 if it has already become very small in this period
                  // ==========================================================
                  if (!skip_hard_fix_once &&
                    iter >= 1 &&
                    abs_value <= zero_tolerance) {
                    // Force the variable upper bound to 0 to remove this dimension from the search tree
                    //SCIPchgVarUb(scip, Q(i, j), 0.0);
                    SCIPchgVarUb(scip, Q(i, j), 0.0); // Remove it from the search space
                    SCIPchgVarObj(scip, Q(i, j), 0.0); // Set the objective coefficient to 0 after pruning to avoid interference
                    continue; // The variable is fixed, so its objective coefficient no longer needs updating
                  }
                  // ==========================================================
                  // Core improvement 2: actual IRL1 weight-update formula
                  // Smaller flows receive larger penalties to drive them to zero; larger flows receive smaller penalties so they can remain active
                  // ==========================================================
                  r_weight = C_scale / (abs_value + epsilon_irl1);

                  // Numerical safeguard: cap the maximum penalty weight
                  if (r_weight > max_weight) {
                    r_weight = max_weight;
                  }

                  // ==========================================
                  // 2. Core correction: compute dedicated reweighting for dQ (IR-TV)
                  // ==========================================
                  double tv_weight = C_tv; // Use a static TV penalty in the first iteration (iter==0)

                  if (iter >= 1&&(g->type == 2||g->type==-1) && demand_state[j][i] == 1) {
                    // Get dQ from the previous iteration (for time step 0, assume no preceding variation and set dQ=0)
                    double prev_Q = (j > 0) ? model.getSolution(Q(i, j - 1)) : 0.0;
                    double abs_dQ = fabs(value- prev_Q);

                    // Alternatively, read the solution of the defined fabs_dQ variable directly:
                    // double abs_dQ = fabs(model.getSolution(fabs_dQ(i, j)));

                    // Dedicated dQ reweighting: smaller variation receives a larger penalty to enforce smoothness
                    tv_weight = C_tv / (abs_dQ + epsilon_tv);
                    if (tv_weight > max_tv_weight) {
                      tv_weight = max_tv_weight;
                    }
                  }

                  // ==========================================================
                  // Core improvement 3: assign penalty/reward logic based on operational requirements
                  // ==========================================================
                  //Within the sparsity range, use a negative weight to encourage activation
                  if (is_delivery_demand_gate(*g)) {
                    double Q_sol = model.getSolution(Q(i, j));
                    // 1. Get the Gap solution from the previous iteration
                    double gap_sol = model.getSolution(QGap(i, j));
                   // 2. Dedicated IRL1 weight for Gap
                    double qgap_weight = C_scale / (gap_sol + epsilon_irl1);
                    if (qgap_weight > max_weight) qgap_weight = max_weight;
                    //double diff_ratio = fabs(Q_sol - demand_flow[j][i]) / demand_flow[j][i];

                    // Apply an appropriate penalty only to QGap to steer flow toward the demand value
                    SCIPchgVarObj(scip, QGap(i, j), qgap_weight*(1/g->maxFlow));
                    //SCIPchgVarObj(scip, fabs_dQ(i, j),10e8*r_weight * diff_ratio*(1/g->maxFlow));//Use Total Variation (TV) regularization instead of switching constraints

                    // Key: use an independent tv_weight for fabs_dQ; do not multiply by diff_ratio or r_weight.
                   // Multiply by a constant base (e.g., 1000) so the smoothing penalty is sufficiently strong
                    if (j < allowed_max_j) {
                      // Keep smoothing secondary to early completion; the
                      // old amplified TV cost favored month-long flat flow.
                      SCIPchgVarObj(
                          scip, fabs_dQ(i, j),
                          std::min(1.0e4, 1.0e3 * tv_weight));
                    }
                    const double lateness =
                        input_param.time_vars <= 1 ? 0.0 :
                        static_cast<double>(j) /
                            static_cast<double>(input_param.time_vars - 1);
                    const double flow_scale = std::max(1.0, g->maxFlow);
                    SCIPchgVarObj(
                        scip, Q(i, j),
                        early_delivery_weight * lateness / flow_scale);

                    //Manually designed 2025 reweighting strategy
                    //SCIPchgVarObj(scip, QGap(i, j), r_weight*diff);
                    //SCIPchgVarObj(scip, fabs_dQ(i, j), r_weight*diff);
                    //SCIPchgVarObj(scip, Q(i, j), -r_weight);
                  }
                  else if(g->type == 2 && demand_state[j][i] == 0)
                  {
                    //SCIPchgVarObj(scip, Q(i, j), r_weight*10e4);
                  }
                  weights[i][j] = r_weight;
                }
              }

              if (g->type == 4) {
                // Keep canal-control smoothing secondary to the sparse supply
                // objective.  Exponential growth made one shut-down change
                // more expensive than dumping flow through an unrelated
                // outlet for the rest of the horizon.
                const double r_weight = 1.0;
                for (int j = 0; j < input_param.time_vars; ++j) {
                  SCIPchgVarObj(scip, fabs_dQ(i, j), fabs(r_weight));
                }
              }
            }

            for (int i = 0; i < input_param.num_pools; ++i) {
              string g_name = input_param.topo_dicts.pools[i].source[0].name;  /** Use only the upstream regulating gate of the pool to compute flow and velocity*/
              string g_end_name = input_param.topo_dicts.pools[i].targets[0].name;  /** Use only the upstream regulating gate of the pool to compute flow and velocity*/
              Ocis_edges::Point* g_source = input_param.topo_dicts.get_gate_byName(g_name);
              Ocis_edges::Point* g_end = input_param.topo_dicts.get_gate_byName(g_end_name);
              int g_source_id = g_source->id;
              int g_end_id = g_end->id;
              Ocis_edges::Edges* p = &input_param.topo_dicts.pools[i];

              for (int j = 0; j < input_param.time_vars; ++j) {
                time_t k_t = StringToTime_t(input_param.time_start) + (j * input_param.dt);

                for (int g = 0; g < input_param.topo_dicts.pools[i].turnouts.size(); ++g) {
                  string name = input_param.topo_dicts.pools[i].turnouts[g].name;
                  Ocis_edges::Point* g_tp = input_param.topo_dicts.get_gate_byName(name);
                  if (g_tp->type == 6) {
                    double Q_source_sol = model.getSolution(Q(g_source_id, j));

                    double S_A = 2.0;
                    double S_m = 0.5;
                    double S_l = p->pool_Length*0.001;
                    double S_t = input_param.dt;

                    double S = S_A * S_l * pow(Q_source_sol, (1 - S_m))/100;
                    //SCIPchgVarUb(scip, Q(i, j), S*1.2);
                    //SCIPchgVarLb(scip, Q(i, j), S*0.8);

                    double S_check = 0.05 * Q_source_sol;

                    break;
                  }
                }


              }

            }


            // ============================================================
            // Solve
            // ============================================================
            retcode = model.solve();

            SCIP_STATUS status = SCIP_STATUS_UNKNOWN;

            if (retcode == SCIP_OKAY) {
              status = SCIPgetStatus(model.getSCIP());
            }

            // SCIP_OKAY only indicates that the solve interface returned normally;
            // you must also verify that a usable solution actually exists.
            bool has_solution =
              retcode == SCIP_OKAY &&
              SCIPgetBestSol(model.getSCIP()) != nullptr &&
              status != SCIP_STATUS_INFEASIBLE &&
              status != SCIP_STATUS_INFORUNBD &&
              status != SCIP_STATUS_UNBOUNDED;

            // ============================================================
            // Infeasibility handling:
            // 1. Restore all Q upper bounds from the start of this iteration;
            // 2. Retry the current iter once automatically;
            // 3. Temporarily skip both hard-fixing rules during the retry;
            // 4. Leave all other optimization logic unchanged.
            // ============================================================
            if (!has_solution) {

              cout
                << "[IRL1] iter = "
                << iter
                << " 无可用解，status = "
                << static_cast<int>(status)
                << endl;

              // Return the model to a modifiable state
              model.resetModelPreserveSolution(scip);

              // Undo all Q=0 hard fixes added in this iteration
              for (int restore_i = 0;
                restore_i < input_param.num_vars;
                ++restore_i) {

                for (int restore_j = 0;
                  restore_j < input_param.time_vars;
                  ++restore_j) {

                  SCIP_VAR* q_var =
                    Q(restore_i, restore_j);

                  if (q_var == nullptr) {
                    continue;
                  }

                  SCIP_RETCODE restore_retcode =
                    SCIPchgVarUb(
                      scip,
                      q_var,
                      Q_ub_backup[restore_i][restore_j]);

                  if (restore_retcode != SCIP_OKAY) {
                    cout
                      << "[IRL1] 恢复 Q("
                      << restore_i
                      << ", "
                      << restore_j
                      << ") 上限失败。"
                      << endl;
                  }
                }
              }

              // First infeasible solve in the current iter: roll back automatically and retry once
              if (infeasible_retry_count == 0) {

                infeasible_retry_count = 1;
                skip_hard_fix_once = true;

                cout
                  << "[IRL1] 已撤销本轮硬固定，"
                  << "自动重新计算 iter = "
                  << iter
                  << endl;

                // The for loop will still execute iter++ at the end;
                // decrement it here first so the next loop repeats the current iter.
                --iter;
                continue;
              }

              // If the retry remains infeasible after skipping hard fixes, stop iterating;
              // this prevents getSolution() from reading an invalid solution.
              cout
                << "[IRL1] 撤销本轮硬固定后仍然无解，"
                << "停止后续迭代。"
                << endl;

              break;
            }

            // ============================================================
            // The current solve has a usable solution
            // ============================================================
            std::cout << "=== SCIP_OKAY ===\n";

            if (status == SCIP_STATUS_OPTIMAL) {

              cout
                << "[Solve Label] allowed_max_j = "
                << allowed_max_j
                << endl;

              cout << "optimal" << endl;
            }
            else {

              cout
                << "[Solve Label] allowed_max_j = "
                << allowed_max_j
                << endl;

              cout
                << "Not optimal, but feasible solution exists."
                << endl;
            }

            // Remember whether this solve is the recovery solve after a
            // tighter prefix proved infeasible.
            const bool recovered_minimum_prefix = skip_hard_fix_once;

            // Solve succeeded; restore normal hard-fixing logic
            skip_hard_fix_once = false;
            infeasible_retry_count = 0;


            // 1. Detect the latest gate-opening time in the current solution
            int current_last_active_j = -1;
            for (int j = input_param.time_vars - 1; j >= 0; --j) {
              bool has_flow = false;
              for (int i = 0; i < input_param.num_vars; ++i) {
                const Ocis_edges::Point& active_gate =
                    input_param.topo_dicts.gates[i];
                if (is_delivery_demand_gate(active_gate) &&
                    model.getSolution(Q(i, j)) > zero_tolerance) {
                  has_flow = true;
                  break;
                }
              }
              if (has_flow) {
                current_last_active_j = j;
                break;
              }
            }

            // A tighter prefix was infeasible and this recovery solve restored
            // the previous feasible bounds, so the current prefix is minimal.
            if (recovered_minimum_prefix) {
              cout << "[IRL1] minimum feasible active prefix found: 0.."
                   << current_last_active_j << endl;
              break;
            }

            // 2. Core step: if the latest time is found, force the horizon one time step earlier in the next iteration
            if (current_last_active_j != -1 && current_last_active_j <= allowed_max_j) {
              // Remove one time step per iteration (or two for more aggressive compression)
              allowed_max_j = current_last_active_j - 1;
            }
          }



        }
  
        for (int i = 0; i < input_param.num_pools; ++i) {
          Ocis_edges::Edges* p = &input_param.topo_dicts.pools[i];
          for (int j = 0; j < input_param.time_vars; ++j) {
            result.solution["stage"][input_param.topo_dicts.pools[i].name].push_back(model.getSolution(y(i, j)));
            result.solution["WaterVolume"][input_param.topo_dicts.pools[i].name].push_back(model.getSolution(y(i, j))*p->As);
            //result.solution["infiltration"][input_param.topo_dicts.pools[i].name].push_back(model.getSolution(y(i, j))*p->As);
          }
        }
        for (int i = 0; i < input_param.num_vars; ++i) {
          Ocis_edges::Point* g = &input_param.topo_dicts.gates[i];
          double average_value = 0;
          for (int j = 0; j < input_param.time_vars; ++j) {
            double value = model.getSolution(Q(g->id, j));
            if (j == 0) {
              g->Q_sol_first = value;
            }
            result.solution["Q"][g->name].push_back(model.getSolution(Q(i, j)));
            g->Q_sol_vec.push_back(value);
            average_value += value;
          }
          g->Q_sol_average = average_value / input_param.time_vars;
        }
        for (int i = 0; i < input_param.num_vars; ++i) {
          Ocis_edges::Point* g = &input_param.topo_dicts.gates[i];
          result.solution["W"][g->name].push_back(model.getSolution(W(i)));
          double W_sol = model.getSolution(W(i));
          double W_completed_ratio = g->W_input <= 0 ? 0 : W_sol / g->W_input;
          if (g->type == 2) {
            result.solution["W_completed_ratio"][g->name].push_back(W_completed_ratio);
          }
        }
      }
      else if (status == SCIP_STATUS_UNBOUNDED) {
        std::cout << "[Solve Label] no bound" << std::endl;
      }
      else if (status == SCIP_STATUS_UNKNOWN) {

        std::cout << "[Solve Label] Error : unknown" << std::endl;
      }
      else {
        std::cerr << "[Solve Label] Solving failed!\n";
      }
      }
    }
    catch (const std::exception& e) {
      std::cerr << "Error: " << e.what() << std::endl;
      return result;
    }
    return result;
  }



  void OCIS_scip::set_W(process_T STime, MILP_param& input)
  {

    //Search by gate name
    for (auto iter = STime.begin(); iter != STime.end(); iter++) {
      string name = iter->first;

      Ocis_edges::Point* g = input.topo_dicts.get_gate_byName(name);
      auto stime_value = iter->second;
      if (g == nullptr) {
        continue;
      }

      //Get the latest measured record
      for (auto sub_iter = stime_value.begin(); sub_iter != stime_value.end(); sub_iter++) {
        g->W_input = sub_iter->second;
        g->fixed_W = 1;
      }
    }
    double source_flow_cons = 0;

    for (int i = 0; i < input_params.topo_dicts.gates.size(); i++) {
      Ocis_edges::Point* g = &input_params.topo_dicts.gates[i];
      if (g->type == 0) {
        source_flow_cons = g->obs_flow;
      }
    }

    double total_W_demand = 0;
    //Search by management-station name
    for (int i = 0; i < input_params.topo_dicts.gates.size(); i++) {
      
      Ocis_edges::Point* g = &input_params.topo_dicts.gates[i];
      if (STime.contains(g->level1)) {
        g->W_input = STime[g->level1].begin()->second;
        cout << g->W_input << endl;
        if (STime[g->level1].size() > 1) {
          double step_W = 0;
          for (auto sub_iter = STime[g->level1].begin(); sub_iter != STime[g->level1].end(); sub_iter++) {
            time_t t_t = sub_iter->first;
            auto iter_pre = sub_iter; iter_pre--;
            auto iter_next = sub_iter; iter_next++;
            time_t pre_t, next_t;
            if (iter_pre == STime[g->level1].end()) {
              pre_t = t_t;
            }
            else
            {
              pre_t = iter_pre->first;
            }
            if (iter_next == STime[g->level1].end()) {
              next_t = t_t;
            }
            else
            {
              next_t = iter_next->first;
            }
            double dt = max(fabs(pre_t - t_t), fabs(next_t - t_t));

            if (dt <=86400) {
              dt = 86400;
            }

            double Q_demand = sub_iter->second/dt;
            step_W += sub_iter->second;
            input_params.boundary_flow[g->name][t_t] = Q_demand;
          }
          g->W_input = step_W;
          total_W_demand += g->W_input;
          //g->fixed = 1;
          g->fixed_W = 1;
        }
      }
    }

    double shortage = (source_flow_cons * input_params.T * 3600 - total_W_demand) / total_W_demand;
    if (source_flow_cons > 0) {
      for (int i = 0; i < input_params.topo_dicts.gates.size(); i++) {
        Ocis_edges::Point* g = &input_params.topo_dicts.gates[i];
        g->W_shortage_ratio = -(shortage);
      }
    }


    cout << "done" << endl;

  }

  void OCIS_scip::add_min_Gap_cons(SCIPModel& model,SCIP_VAR* var,SCIP_VAR* var_gap,MILP_param& input_param,
    int gates_nindex,int time_nindex,double target_value,string cons_name) {
    double ht =target_value;
      // Constraint 1: y - z <= ht
      model.addLinearConstraint(
        { var, var_gap },
        { 1.0, -1.0 },  // Coefficient
        -SCIPinfinity(model.getSCIP()),
        ht,
        cons_name + "_upper_" + std::to_string(gates_nindex) + "_" + std::to_string(time_nindex)
      );
      // Constraint 2: -z - y <= -ht
      model.addLinearConstraint(
        { var_gap, var },
        { -1.0, -1.0 },  // Coefficient
        -SCIPinfinity(model.getSCIP()),
        -ht,
        cons_name + "_lower_" + std::to_string(gates_nindex) + "_" + std::to_string(time_nindex)
      );

  }
  void OCIS_scip::add_delta_variable_cons(SCIPModel& model, SCIP_VAR* var_pre, SCIP_VAR* var_next, SCIP_VAR* var_delta,
    int row, int col, string cons_name) {
    model.addEqualityConstraint(
      { var_pre,var_next,var_delta },
      { -1,1,-1 },
      0.0,
      cons_name + std::to_string(row)+"_" + std::to_string(col)
    );
  }


  void OCIS_scip::add_fabs_cons(
    SCIPModel& model,
    SCIP_VAR* var,
    SCIP_VAR* var_fabs,
    int row,
    int col,
    string cons_name) {

    // Constraint 1: dx - |dx| <= 0
    model.addLinearConstraint(
      { var, var_fabs },
      { 1.0, -1.0 },  // Coefficient
      -SCIPinfinity(model.getSCIP()),
      0,
      cons_name + "_upper_" + std::to_string(row) + "_" + std::to_string(col)
    );

    // Constraint 2: -|dx| - dx <= 0
    model.addLinearConstraint(
      { var_fabs, var },
      { -1.0, -1.0 },  // Coefficient
      -SCIPinfinity(model.getSCIP()),
      0,
      cons_name + "_lower_" + std::to_string(row) + "_" + std::to_string(col)
    );
  }

  void OCIS_scip::add_Equal_cons(
    SCIPModel& model,
    double var_coef,
    SCIP_VAR* var,
    double value,
    int row,
    int col,
    string cons_name
  ) {
    // Constraint 1: dx - |dx| <= 0
    model.addLinearConstraint(
      { var},
      { var_coef},  // Coefficient
      value,
      value,
      cons_name  + std::to_string(row) + "_" + std::to_string(col)
    );

  }

  void OCIS_scip::add_Equal_cons(
    SCIPModel& model,
    double var_coef,
    SCIP_VAR* var,
    double value,
    double epsilon,
    int row,
    int col,
    string cons_name
  ) {
    // Constraint 1: dx - |dx| <= 0
    model.addLinearConstraint(
      { var},
      { var_coef},  // Coefficient
      value-epsilon,
      value+epsilon,
      cons_name + "_" + std::to_string(row) + "_" + std::to_string(col)
    );

  }


  void OCIS_scip::add_lessOrEqual_cons(
    SCIPModel& model,
    double value,
    SCIP_VAR* var,
    int row,
    int col,
    string cons_name) {

    // Constraint 1: dx - |dx| <= 0
    model.addLinearConstraint(
      { var},
      { 1.0 },  // Coefficient
      -SCIPinfinity(model.getSCIP()),
      value,
      cons_name  + std::to_string(row) + "_" + std::to_string(col)
    );
  }

  void OCIS_scip::add_FlowBalance_internalModel_cons(
    SCIPModel& model,
    MILP_param& input_param,
    SCIPModel::Var2D& Q,
    SCIPModel::Var2D& y,
    bool consider_y,
    string cons_name
  ) {

    for (int i = 0; i < input_param.num_pools; ++i) {
      Ocis_edges::Edges* p = &input_param.topo_dicts.pools[i];
      int discrete_delaytime = p->delaytime / input_params.dt;

      std::vector<string> gates_name_vec_temp;

      // Dynamically add all columns for each row
      for (int j = 0; j < input_param.time_vars ; ++j) {
        std::vector<SCIP_VAR*> row_vars_flow;
        std::vector<double> coeffs_flow;

        if (consider_y && j < input_param.time_vars-1) {
          row_vars_flow.push_back(y(i, j));
          coeffs_flow.push_back(1); // All coefficients are 1

          row_vars_flow.push_back(y(i, j + 1));
          coeffs_flow.push_back(-1); // All coefficients are 1
        }

        for (int g = 0; g < input_param.topo_dicts.pools[i].source.size(); ++g) {
          string name = input_param.topo_dicts.pools[i].source[g].name;
          Ocis_edges::Point* g_tp = input_param.topo_dicts.get_gate_byName(name);
          int g_id = g_tp->id;



          if (j >= discrete_delaytime) {

            row_vars_flow.push_back(Q(g_id, j - discrete_delaytime));
          }
          else
          {
            row_vars_flow.push_back(Q(g_id, j));

          }
          coeffs_flow.push_back(input_param.topo_dicts.massMatrix[i][g_id] * input_param.dt / input_param.topo_dicts.pools[i].As); // All coefficients are 1
          gates_name_vec_temp.push_back(g_tp->name);
          if (input_param.topo_dicts.massMatrix[i][g_id] == 0) {
            std::cout << g_tp->name << " id = " << g_id << " : " << "massMatrix has problem." << std::endl;
            exit(-1);
          }

        }
        for (int g = 0; g < input_param.topo_dicts.pools[i].turnouts.size(); ++g) {
          string name = input_param.topo_dicts.pools[i].turnouts[g].name;
          Ocis_edges::Point* g_tp = input_param.topo_dicts.get_gate_byName(name);
          int g_id = g_tp->id;
          if (j >= discrete_delaytime && g_tp->type == 3) {
            row_vars_flow.push_back(Q(g_id, j - discrete_delaytime));
          }
          else
          {
            row_vars_flow.push_back(Q(g_id, j));
          }

          if (g_tp->type == 5) {
            coeffs_flow.push_back(input_param.topo_dicts.massMatrix[i][g_id] * input_param.dt / input_param.topo_dicts.pools[i].As); // All coefficients are 1
          }
          else
          {
            coeffs_flow.push_back(input_param.topo_dicts.massMatrix[i][g_id] * input_param.dt / (input_param.topo_dicts.pools[i].As*1.)); // All coefficients are 1
          }

          gates_name_vec_temp.push_back(g_tp->name);

          if (input_param.topo_dicts.massMatrix[i][g_id] == 0) {
            std::cout << g_tp->name << " id = " << g_id << " : " << "massMatrix has problem." << std::endl;
            exit(-1);
          }

        }
        for (int g = 0; g < input_param.topo_dicts.pools[i].targets.size(); ++g) {
          string name = input_param.topo_dicts.pools[i].targets[g].name;
          Ocis_edges::Point* g_tp = input_param.topo_dicts.get_gate_byName(name);
          int g_id = g_tp->id;
          row_vars_flow.push_back(Q(g_id, j));
          coeffs_flow.push_back(input_param.topo_dicts.massMatrix[i][g_id]* input_param.dt / input_param.topo_dicts.pools[i].As); // All coefficients are 1
          gates_name_vec_temp.push_back(g_tp->name);

          if (input_param.topo_dicts.massMatrix[i][g_id] == 0) {
            std::cout << g_tp->name << " id = " << g_id << " : " << "massMatrix has problem." << std::endl;
            exit(-1);
          }

        }


        double epsilon = 0.;
        //if (i == 2) {
        //  epsilon = -10;
        //}
        //cout << "\n";
        //model.addEqualityConstraint(
        //  row_vars_flow,
        //  coeffs_flow,
        //  epsilon,
        //  cons_name + std::to_string(i)+"_" + std::to_string(j)
        //);
        model.addLinearConstraint(
          row_vars_flow,
          coeffs_flow,
          -epsilon,
          epsilon,
          //SCIPinfinity(model.getSCIP()),
          cons_name + "biggerinflow" + std::to_string(i) + "_" + std::to_string(j)
        );

      }
    }



  }

  void OCIS_scip::add_sum_t_cons(SCIPModel& model,double a , SCIPModel::Var2D& var, SCIPModel::Var1D& var_sum,
    int row_num,int col_num,string cons_name) {

    double M = 9999;
    double epsilon = 0.001;
    for (int i = 0; i < row_num; i++) {
      std::vector<SCIP_VAR*> row_vars;
      std::vector<double> coeffs;
      for (int j = 0; j < col_num; j++) {
        row_vars.push_back(var(i, j));
        coeffs.push_back(1.0*a); 
      }

      row_vars.push_back(var_sum(i));
      coeffs.push_back(-1.0); 

      model.addEqualityConstraint(
        row_vars,
        coeffs,
        0.0,
        cons_name + std::to_string(i)
      );
    }


  }

  void OCIS_scip::add_Equal_cons(
    SCIPModel& model,
    double var_coef,
    SCIP_VAR* var,
    double value,
    double epsilon_l,
    double epsilon_r,
    int row,
    int col,
    string cons_name
  ) {
    // Constraint 1: dx - |dx| <= 0
    model.addLinearConstraint(
      { var},
      { var_coef},  // Coefficient
      value-epsilon_l,
      value+epsilon_r,
      cons_name + "_" + std::to_string(row) + "_" + std::to_string(col)
    );

  }
