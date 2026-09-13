# 编译与运行

## 环境要求

- Windows x64。
- Visual Studio 2019，安装“使用 C++ 的桌面开发”工作负载。
- CMake 3.20 或更高版本。
- 支持 C++20 的 MSVC 工具链。
- SCIP Optimization Suite 10.0.2 x64 开发包，目录要求见
  [THIRD_PARTY.md](THIRD_PARTY.md)。

以下命令均在 `OcisMILPNet_sparsity_standalone` 目录执行。

## 准备 SCIP

将 SCIP 开发包放到默认位置：

```text
third_party/scip/
├── include/scip/scip.h
├── lib/libscip.lib
└── bin/*.dll
```

也可以放在任意目录，并在 CMake 配置时通过 `SCIP_ROOT` 指定。

## 配置与编译

使用工程内默认 SCIP 路径：

```powershell
cmake -S . -B build-vs2019 -G "Visual Studio 16 2019" -A x64
cmake --build build-vs2019 --config Release --parallel 4
```

使用外部 SCIP 安装目录：

```powershell
cmake -S . -B build-vs2019 -G "Visual Studio 16 2019" -A x64 `
  -DSCIP_ROOT="D:/opt/SCIPOptSuite-10.0.2"
cmake --build build-vs2019 --config Release --parallel 4
```

若系统临时目录空间不足，可先指定工程内临时目录：

```powershell
New-Item -ItemType Directory -Force _tmp | Out-Null
$env:TEMP = (Resolve-Path _tmp).Path
$env:TMP = $env:TEMP
cmake --build build-vs2019 --config Release --parallel 4
```

主要产物：

```text
build-vs2019/OcisMILPNet_dll/Release/OcisMILPNet.dll
build-vs2019/OcisMILPNet_bin/Release/OcisMILPNet.exe
build-vs2019/OcisMILPNet_dlltest/Release/OcisMILPNet_dlltest.exe
```

CMake 会把 `SCIP_RUNTIME_DIR` 下的 DLL 复制到各目标的输出目录。若运行时 DLL
不在 `SCIP_ROOT/bin`，可额外指定：

```powershell
-DSCIP_RUNTIME_DIR="D:/opt/SCIPOptSuite-10.0.2/bin"
```

## 准备案例

仓库仅保留案例目录。运行前将 SJ 案例恢复为至少包含以下文件的结构：

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

`input.json` 引用的其他文件也必须按其相对路径放入案例目录。

## 运行

```powershell
& .\build-vs2019\OcisMILPNet_bin\Release\OcisMILPNet.exe
```

成功结束时输出：

```text
Standalone SJ sparsity calculation completed.
```

计算结果写入 `data/sj/output`。

## 测试

案例数据完整时执行：

```powershell
ctest --test-dir build-vs2019 -C Release --output-on-failure
```

测试包括命令行 SJ 求解和 DLL API 冒烟测试。由于公开仓库暂不包含案例数据，
未恢复 `data/sj` 内容时这两个运行测试会失败，这是预期行为。
