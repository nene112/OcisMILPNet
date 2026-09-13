# Third-party dependencies

## SCIP Optimization Suite

- Version used: SCIP Optimization Suite 10.0.2, SCIP API 156.
- Purpose: MILP modeling, constraint construction, and optimization.
- Source includes: `#include <scip/...>`.
- Linking: CMake searches `SCIP_ROOT/include` and
  `SCIP_ROOT/lib/libscip.lib`.
- Runtime: DLLs matching the downloaded Windows x64 package under
  `SCIP_ROOT/bin`.
- Official website: https://www.scipopt.org/
- Download instructions: https://www.scipopt.org/#download
- License: SCIP 8.0.3 and later use the Apache License 2.0. Precompiled
  Optimization Suite packages may also contain components under their own
  licenses. Review the notices included in the downloaded package and the
  official dependency-license table before redistribution.

This repository does not commit SCIP headers, import libraries, or DLLs. After
downloading the Windows x64 development package, place it under
`third_party/scip` or point to an external installation with
`-DSCIP_ROOT=<directory>`.

## nlohmann/json

- Version used: 3.11.3.
- File: `include/nlohmann/json.hpp`.
- Purpose: configuration parsing, result organization, and JSON generation.
- Upstream project: https://github.com/nlohmann/json
- License: MIT; SPDX information is retained in the single-header source.

## SimpleIni

- Version used: 4.22.
- File: `include/SimpleIni.h`.
- Purpose: compatible INI configuration parsing.
- Upstream project: https://github.com/brofield/simpleini
- License: MIT; the original copyright and license notices remain in the file.

## Unicode ConvertUTF

- File: `include/ConvertUTF.h`.
- Purpose: character-encoding conversion support used by SimpleIni.
- Source: Unicode, Inc. ConvertUTF sample code.
- License and redistribution terms: retained in the file header and must not be
  removed when using or redistributing the file.

## System and standard libraries

The Windows build also uses the MSVC C++ runtime, Windows SDK, and C++20
standard library supplied by the Visual Studio toolchain. They are not
distributed with this repository. The `iconv` code in non-Windows branches is
not part of the current Visual Studio build path. A Linux or macOS port must
provide a compatible iconv implementation and additional platform build
configuration.
