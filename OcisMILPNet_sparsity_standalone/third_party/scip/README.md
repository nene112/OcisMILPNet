# SCIP directory

Place the SCIP Optimization Suite 10.0.2 Windows x64 development package here,
or configure CMake with `-DSCIP_ROOT=<path>`.

Expected layout:

```text
include/scip/scip.h
lib/libscip.lib
bin/*.dll
```

The SCIP package itself is intentionally not stored in this repository. See
`../../THIRD_PARTY.md` for source and license information.
