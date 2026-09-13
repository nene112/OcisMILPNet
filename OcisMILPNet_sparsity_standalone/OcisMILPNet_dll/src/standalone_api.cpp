#include "standalone_api.h"

#include "FileInteraction.h"
#include "ocis_scip_utils.h"

#include <algorithm>
#include <cstring>
#include <filesystem>
#include <iostream>

struct OcisStandalone {
  OCIS_scip solver;
  std::filesystem::path case_dir;
};

namespace {
std::string slash_path(const std::filesystem::path& path) {
  std::string value = std::filesystem::absolute(path).generic_string();
  if (value.empty() || value.back() != '/') value.push_back('/');
  return value;
}
}

OcisStandalone* ocis_create() { return new OcisStandalone(); }

void ocis_destroy(OcisStandalone* instance) { delete instance; }

int ocis_load_sj(OcisStandalone* instance, const char* case_dir) {
  if (instance == nullptr || case_dir == nullptr) return 0;
  instance->case_dir = std::filesystem::absolute(case_dir);
  const std::string root = slash_path(instance->case_dir);
  if (!std::filesystem::exists(instance->case_dir / "input/input.json") ||
      !std::filesystem::exists(instance->case_dir / "mesh/edges.csv")) {
    std::cerr << "Missing SJ input under " << root << std::endl;
    return 0;
  }

  auto& solver = instance->solver;
  solver.dirpath = root;
  solver.mesh_path = "mesh/";
  solver.input_params = solver.MILP_read_input(
      root, "mesh/edges.csv", "input/input.json");
  auto& params = solver.input_params;
  params.boundary_flow_string = root + "input/sj-unit_need_Q.csv";
  params.boundary_flow = read_csv_tm(params.boundary_flow_string);

  // sj-unit_need_Q.csv is indexed by allocation-unit codes and by day number,
  // not directly by gate names and timestamps.  Reproduce the mapping path
  // used by the full DLL loader before deriving the MILP demand constraints.
  ConvertBoundaryFlowTime(
      params.boundary_flow, params.time_start_t, params.T, 86400);
  WaterUnitMapper unit_mapper;
  if (!unit_mapper.Init(root + "config.dat", root + "unit.csv")) {
    std::cerr << "Failed to load SJ water-unit mapping files under " << root
              << std::endl;
    return 0;
  }
  process_T mapped_boundary_flow;
  int mapped_count = 0;
  for (const auto& [code, process] : params.boundary_flow) {
    const std::string target_name = unit_mapper.GetTarget(code);
    if (target_name.empty()) continue;
    mapped_boundary_flow[target_name] = process;
    ++mapped_count;
  }
  params.boundary_flow = std::move(mapped_boundary_flow);
  std::cout << "[SJ unit mapping] mapped allocation units=" << mapped_count
            << std::endl;

  if (params.boundary_flow.empty()) {
    std::cerr << "No SJ allocation-unit demand could be mapped to gates."
              << std::endl;
    return 0;
  }

  // Derive hard delivery-volume demands before adding prescribed inflows.
  // In particular, Yinjiangkou is a type-1 internal-inflow auxiliary, not a
  // delivery demand, even though its boundary process is supplied by CSV.
  solver.set_demand_from_boundary_flow(params);

  // Huangbizhuang and Yinjiangkou are prescribed inflows rather than
  // scheduling demands. Merge both daily processes only after demand
  // derivation so they remain fixed boundary auxiliaries in the MILP.
  const process_T source_flow =
      read_csv_tm(root + "input/action_obs100.csv");
  const std::vector<std::string> fixed_inflows = {"黄壁庄", "引江口"};
  for (const std::string& source_name : fixed_inflows) {
    const auto source_it = source_flow.find(source_name);
    if (source_it == source_flow.end() || source_it->second.empty()) {
      std::cerr << "Missing fixed inflow process for " << source_name
                << " in " << root << "input/action_obs100.csv" << std::endl;
      return 0;
    }
    params.boundary_flow[source_name] = source_it->second;
    std::cout << "[SJ fixed inflow] " << source_name << " periods="
              << source_it->second.size() << std::endl;
  }

  process_T water_demand;
  solver.set_W(water_demand, params);
  params.input_path = root + "input/";
  params.outputpath = root + "output/";
  std::filesystem::create_directories(params.outputpath);
  return 1;
}

int ocis_solve_sparsity(OcisStandalone* instance) {
  if (instance == nullptr) return 0;
  auto& solver = instance->solver;
  solver.result = solver.MILP_ocis_solver_LP_PathwaysPlanning_sparsity(
      solver.input_params);
  return solver.result.solution.count("Q") != 0;
}

char* ocis_get_action_json(OcisStandalone* instance) {
  if (instance == nullptr) return nullptr;
  nlohmann::json action;
  const auto found = instance->solver.result.solution.find("Q");
  if (found != instance->solver.result.solution.end()) {
    for (const auto& [name, values] : found->second) {
      for (std::size_t index = 0; index < values.size(); ++index) {
        const time_t timestamp = instance->solver.input_params.time_start_t +
            static_cast<time_t>(index * instance->solver.input_params.dt);
        action[name][FormatTime(timestamp)] = values[index];
      }
    }
  }
  const std::string text = action.dump();
  char* result = new char[text.size() + 1];
  std::memcpy(result, text.c_str(), text.size() + 1);
  return result;
}

void ocis_free_string(char* value) { delete[] value; }

int ocis_write_output(OcisStandalone* instance) {
  if (instance == nullptr) return 0;
  auto& solver = instance->solver;
  solver.Summary_dyunamic_border(solver.input_params, solver.result.solution);
  solver.result.solution["action"] = solver.result.solution["Q"];
  solver.MILP_write_output(
      solver.input_params, solver.result, slash_path(instance->case_dir),
      "action_td.csv");
  solver.balance_check(solver.input_params);
  return 1;
}
