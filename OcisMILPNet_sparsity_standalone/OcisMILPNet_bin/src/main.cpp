#include "standalone_api.h"

#include <cstring>
#include <iostream>
#include <memory>
#include <string>

int main() {
  std::unique_ptr<OcisStandalone, decltype(&ocis_destroy)> solver(
    ocis_create(), ocis_destroy);
  const std::string case_dir = std::string(OCIS_PROJECT_ROOT) + "/data/sj";
  if (!ocis_load_sj(solver.get(), case_dir.c_str())) return 1;
  if (!ocis_solve_sparsity(solver.get())) return 2;
  char* action = ocis_get_action_json(solver.get());
  const bool has_action = action != nullptr && std::strlen(action) > 2;
  ocis_free_string(action);
  if (!has_action || !ocis_write_output(solver.get())) return 3;
  std::cout << "Standalone SJ sparsity calculation completed." << std::endl;
  return 0;
}
