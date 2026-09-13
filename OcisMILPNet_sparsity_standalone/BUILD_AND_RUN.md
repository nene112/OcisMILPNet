# Build and run

## Requirements

- Windows x64.
- Visual Studio 2019 with the **Desktop development with C++** workload.
- CMake 3.20 or later.
- An MSVC toolchain with C++20 support.
- The SCIP Optimization Suite 10.0.2 x64 development package. See
  [THIRD_PARTY.md](THIRD_PARTY.md).

Run all commands below from the `OcisMILPNet_sparsity_standalone` directory.

## Prepare SCIP

Place the SCIP development package in the default location:

```text
third_party/scip/
├── include/scip/scip.h
├── lib/libscip.lib
└── bin/*.dll
```

Alternatively, keep SCIP anywhere and pass its location through `SCIP_ROOT`
during CMake configuration.

## Configure and build

Using the default in-project SCIP location:

```powershell
cmake -S . -B build-vs2019 -G "Visual Studio 16 2019" -A x64
cmake --build build-vs2019 --config Release --parallel 4
```

Using an external SCIP installation:

```powershell
cmake -S . -B build-vs2019 -G "Visual Studio 16 2019" -A x64 `
  -DSCIP_ROOT="D:/opt/SCIPOptSuite-10.0.2"
cmake --build build-vs2019 --config Release --parallel 4
```

If the system temporary directory is short on space, use a project-local
temporary directory:

```powershell
New-Item -ItemType Directory -Force _tmp | Out-Null
$env:TEMP = (Resolve-Path _tmp).Path
$env:TMP = $env:TEMP
cmake --build build-vs2019 --config Release --parallel 4
```

Main build artifacts:

```text
build-vs2019/OcisMILPNet_dll/Release/OcisMILPNet.dll
build-vs2019/OcisMILPNet_bin/Release/OcisMILPNet.exe
build-vs2019/OcisMILPNet_dlltest/Release/OcisMILPNet_dlltest.exe
```

CMake copies the DLLs from `SCIP_RUNTIME_DIR` to each target output directory.
If the runtime DLLs are not under `SCIP_ROOT/bin`, specify their directory
separately:

```powershell
-DSCIP_RUNTIME_DIR="D:/opt/SCIPOptSuite-10.0.2/bin"
```

## Prepare the case

Only the case directory layout is stored in the repository. Before running,
restore an SJ case containing at least the following files:

```text
data/sj/
├── config.dat
├── unit.csv
├── input/
│   ├── input.json
│   ├── sj-unit_need_Q.csv
│   └── action_obs100.csv
├── mesh/
│   └── edges.csv
└── output/
```

Any additional files referenced by `input.json` must also be placed at their
expected relative paths within the case directory.

## Run

```powershell
& .\build-vs2019\OcisMILPNet_bin\Release\OcisMILPNet.exe
```

A successful run ends with:

```text
Standalone SJ sparsity calculation completed.
```

Results are written to `data/sj/output`.

## Test

With complete case data, run:

```powershell
ctest --test-dir build-vs2019 -C Release --output-on-failure
```

The test suite contains the command-line SJ solve and the DLL API smoke test.
Because the public repository does not currently contain case data, both runtime
tests are expected to fail until the contents of `data/sj` are restored.
