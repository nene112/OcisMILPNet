# OcisMILPNet sparsity standalone

这是从 OcisMILPNet 中提取的独立 Windows/CMake 工程，保留
`MILP_ocis_solver_LP_PathwaysPlanning_sparsity` 所需的最小源码闭包。
工程提供命令行程序、动态链接库和 DLL 冒烟测试，不引用父工程源码。

## 目录

- `src/`：稀疏调度求解核心。
- `OcisMILPNet_dll/`：独立 DLL 与 C API。
- `OcisMILPNet_bin/`：无参数 SJ 案例运行程序。
- `OcisMILPNet_dlltest/`：DLL API 冒烟测试。
- `include/`：随源码使用的单头文件依赖及兼容层。
- `third_party/scip/`：外部 SCIP 安装目录占位，不提交第三方二进制。
- `data/sj/`：SJ 案例目录占位，案例内容暂不发布。

## 文档

- [编译与运行](BUILD_AND_RUN.md)
- [第三方库说明](THIRD_PARTY.md)

## 当前发布范围

仓库包含可编译的模型代码和工程配置，但不包含 SJ 案例数据。将完整案例文件
放入 `data/sj` 后，命令行程序和 CTest 才能完成实际求解。缺少案例数据不影响
CMake 配置和代码编译。
