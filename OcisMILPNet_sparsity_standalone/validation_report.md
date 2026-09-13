# SPARSITY_IRL1 SJ validation

- Toolchain: Visual Studio 2019 / MSVC 19.29, Release.
- Compiler scratch directory: project-local `_tmp` on drive E.
- Build: PASS (`OcisMILPNet.dll`, bin and dlltest).
- Bin test: PASS.
- DLL API test: PASS.
- SCIP: Optimization Suite 10.0.2, API 156.
- SCIP result: optimal solution found.
- CTest result: 2/2 passed.
- Output: `data/sj/output/action.csv` and `action_td.csv`.

Validation was run locally with the complete private SJ case. The public
repository intentionally contains only the `data/sj` directory layout, so this
test can be reproduced after the case files are restored.

## Input alignment

The original SPARSITY_IRL1 branch selects the HDCM SJ case and reads the
configured `sj-unit_need_Q.csv` as boundary flow. Its configured
`input/Wdemand.csv` and default `mesh/obs_STime_td.csv` are absent from the SJ
case. The standalone loader reads `sj-unit_need_Q.csv` and does not invent a
replacement W-demand or observation-duration series.

Using `Wdemand_td.csv` as an extra input was separately checked and makes this
model infeasible; it is not the original branch input combination.
