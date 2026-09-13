# Overview

OcisMILPNet is a graph-based mixed-integer linear programming framework for the operation of open-channel irrigation networks. The framework represents the irrigation system as a directed graph and formulates the coordinated operation of control structures and channel reaches as a network-scale optimization problem.

The associated paper has been published in Water Resources Management:

OcisMILPNet: A Graph-Based MILP Approach for Sparse Operations in Open-Channel Irrigation Networks
Water Resources Management, 2026.
DOI: 10.1007/s11269-026-04842-6
Publisher page: Springer Nature Link

Repository Status

The source code is currently being organized, reviewed, and documented for public release.

This repository has been created in advance as the official code repository associated with the paper. The complete implementation may therefore not yet be visible. The absence of source files at this stage does not indicate that the code is unavailable permanently.

Before release, the following items are being checked:

consistency between the published mathematical formulation and the implementation;
removal of project-specific paths and private configuration information;
organization of the graph, optimization, input, and output modules;
preparation of representative irrigation-network test cases;
documentation of input parameters and configuration files;
verification of compilation and runtime dependencies;
preparation of reproducibility scripts for the numerical experiments.

The code will be uploaded after these checks are completed to ensure that the released version is understandable, executable, and reproducible.

Please watch this repository for updates.

## Source release

The first organized source release is available in
[`OcisMILPNet_sparsity_standalone`](OcisMILPNet_sparsity_standalone/README.md).
It contains the standalone sparsity MILP solver, command-line and DLL targets,
build instructions, and third-party dependency documentation.

The case directory layout is included, but case data are intentionally not part
of this release. See the standalone build guide for the files required to run
the SJ case after obtaining the data separately.
