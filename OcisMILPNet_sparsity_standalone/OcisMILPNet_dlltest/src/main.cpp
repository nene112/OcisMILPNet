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
  const bool passed = action != nullptr && std::strlen(action) > 2;
  ocis_free_string(action);
  std::cout << "DLL API smoke test: " << (passed ? "PASS" : "FAIL") << std::endl;
  return passed ? 0 : 3;
}
