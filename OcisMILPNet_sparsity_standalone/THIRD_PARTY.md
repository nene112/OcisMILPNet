# 第三方库说明

## SCIP Optimization Suite

- 使用版本：SCIP Optimization Suite 10.0.2，SCIP API 156。
- 用途：MILP 建模、约束构建与求解。
- 代码引用：`#include <scip/...>`。
- 链接方式：CMake 查找 `SCIP_ROOT/include` 和 `SCIP_ROOT/lib/libscip.lib`。
- 运行依赖：`SCIP_ROOT/bin` 中与所下载 Windows x64 包匹配的 DLL。
- 官方网站：https://www.scipopt.org/
- 下载说明：https://www.scipopt.org/#download
- 许可证：SCIP 8.0.3 及以后版本采用 Apache License 2.0；Optimization Suite
  的预编译包还可能包含具有各自许可证的组件，发布或再分发前应核对下载包内的
  notices 与官方依赖许可证表。

本仓库不提交 SCIP 的头文件、导入库和 DLL。下载 Windows x64 开发包后，可以
放入 `third_party/scip`，或用 `-DSCIP_ROOT=<目录>` 指向外部安装位置。

## nlohmann/json

- 使用版本：3.11.3。
- 文件：`include/nlohmann/json.hpp`。
- 用途：读取配置、组织结果和生成 JSON。
- 上游项目：https://github.com/nlohmann/json
- 许可证：MIT，SPDX 信息保留在单头文件内。

## SimpleIni

- 使用版本：4.22。
- 文件：`include/SimpleIni.h`。
- 用途：INI 配置读取兼容。
- 上游项目：https://github.com/brofield/simpleini
- 许可证：MIT，原始版权与许可声明保留在源文件内。

## Unicode ConvertUTF

- 文件：`include/ConvertUTF.h`。
- 用途：SimpleIni 的字符编码转换支持。
- 来源：Unicode, Inc. ConvertUTF 示例代码。
- 许可与再分发条件：保留在文件头部，使用或再分发时不得移除。

## 系统与标准库

Windows 构建还使用 MSVC C++ 运行库、Windows SDK 和 C++20 标准库。这些由
Visual Studio 工具链提供，不随仓库分发。非 Windows 分支中的 `iconv` 代码不在
当前 Visual Studio 构建路径内；若移植到 Linux/macOS，需要提供相应的 iconv
实现并补充平台构建配置。
