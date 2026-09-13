#pragma once

#if defined(_WIN32)
#  if defined(OCIS_STANDALONE_EXPORTS)
#    define OCIS_API __declspec(dllexport)
#  else
#    define OCIS_API __declspec(dllimport)
#  endif
#else
#  define OCIS_API
#endif

struct OcisStandalone;

extern "C" {
OCIS_API OcisStandalone* ocis_create();
OCIS_API void ocis_destroy(OcisStandalone* instance);
OCIS_API int ocis_load_sj(OcisStandalone* instance, const char* case_dir);
OCIS_API int ocis_solve_sparsity(OcisStandalone* instance);
OCIS_API char* ocis_get_action_json(OcisStandalone* instance);
OCIS_API void ocis_free_string(char* value);
OCIS_API int ocis_write_output(OcisStandalone* instance);
}
