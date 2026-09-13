# OcisMILPNet sparsity standalone

This standalone Windows/CMake project was extracted from OcisMILPNet and keeps
the minimum source closure required by
`MILP_ocis_solver_LP_PathwaysPlanning_sparsity`. It provides a command-line
program, a dynamic library, and a DLL smoke test without referencing source
files from the parent project.

## Repository layout

- `src/`: core sparsity-scheduling solver.
- `OcisMILPNet_dll/`: standalone DLL and C API.
- `OcisMILPNet_bin/`: argument-free SJ case runner.
- `OcisMILPNet_dlltest/`: DLL API smoke test.
- `include/`: bundled single-header dependencies and compatibility helpers.
- `third_party/scip/`: placeholder for an external SCIP installation; SCIP
  binaries are not committed.
- `data/sj/`: placeholder for the SJ case; case contents are not published yet.

## Documentation

- [Build and run](BUILD_AND_RUN.md)
- [Third-party dependencies](THIRD_PARTY.md)

## Current release scope

The repository contains the model source and build configuration but not the SJ
case data. Restore the complete case under `data/sj` before running the command-
line program or CTest. Missing case data does not prevent CMake configuration or
source compilation.
