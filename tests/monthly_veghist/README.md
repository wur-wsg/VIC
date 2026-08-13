# Monthly vegetation-history forcing — 编译、运行与测试说明

本目录是 `monthly_veghist_forcing` branch 的验证套件。它对应的源码改动让
VIC-WUR image driver 可以直接读取 **monthly** 的 LAI / fcanopy / albedo
NetCDF，而不必把月值复制成逐日记录。

设计与审计文档：
`/lustre/nobackup/WUR/ESG/liu297/vic_global/vic_parameter/docs/Natural_monthly_veghist_forcing源码审计与设计.md`

---

## 1. 新的 global parameter 语法

`FORCE_TYPE` 增加了一个**可选**的频率 token：

```
FORCE_TYPE  <VARNAME>  <nc_varname>  [<FREQ>]  <path_prefix>
```

- 省略频率（3 个 token）= 历史行为 = `STEP`。**现有 global parameter 文件不需要任何修改。**
- `FREQ` 目前接受 `STEP` 和 `MONTH`。
- `MONTH` 只允许用于 `LAI`、`FCANOPY`、`ALBEDO`；用在别的变量上会在初始化期报错。
- `DAY` 和 `YEAR` 会被识别但明确拒绝（它们只在 `PLUGIN_FORCE_TYPE` 上可用），
  避免"写了以为生效"的静默失效。

示例：

```
LAI_SRC     FROM_VEGHIST
FCAN_SRC    FROM_VEGHIST
ALB_SRC     FROM_VEGHIST

FORCE_TYPE  AIR_TEMP  tas      /data/met/met_                 # 旧语法，STEP
FORCE_TYPE  LAI       lai      MONTH  /data/veghist/veghist_
FORCE_TYPE  FCANOPY   fcanopy  MONTH  /data/veghist/veghist_
FORCE_TYPE  ALBEDO    albedo   MONTH  /data/veghist/veghist_
```

完整的最小示例见运行 `make_synthetic_case.py` 后生成的
`<outdir>/global_monthly.txt`（monthly）与 `<outdir>/global_daily.txt`（旧 daily 语法）。

---

## 2. monthly NetCDF 文件契约

**一年一个文件**：`<prefix><YYYY>.nc`（文件名格式 `%s%4d.nc` 是 core 里的硬约定）。

每个文件必须满足：

| 项目 | 要求 |
|---|---|
| `time` 维长 | 恰好 12 |
| `time` 内容 | 该年 1—12 月，按顺序，无重复、无缺失 |
| `time` 时间戳 | 每月第一天 00:00 |
| `time` 属性 | 合法 `units`（`<unit> since <origin>`）与 `calendar` |
| `calendar` | 必须与 global parameter 的 `CALENDAR` 一致 |
| `veg_class` 维长 | 必须等于 parameter 文件的 `NVEGTYPES` |
| 变量维序 | `(time, veg_class, lat, lon)` |
| lat/lon | 与 domain 一致（由既有 `compare_ncdomain_with_global_domain` 校验） |

三个变量可以放在同一个文件里（配置中三次指向同一 prefix），也可以分开放。
同文件时 VIC 会为每个变量各开一个只读 handle；这是安全的，offset 互不污染。

**模拟期跨越的每一年都必须有对应文件**：所有年份在初始化期就会被探测一次，
缺文件立即失败，不会跑到第 300 天才报错。

---

## 3. 编译（面向另一台 HPC）

依赖：MPI C 编译器、netCDF-C（含 `nc-config`）、OpenMP。

```bash
cd <VIC_ROOT>/vic/drivers/image
make model
```

Makefile 会自动调用 `nc-config --cflags/--libs`。如果 `nc-config` 不在 PATH 上，
显式传入：

```bash
make model NC_CFLAGS="-I/path/to/netcdf/include" \
           NC_LIBS="-L/path/to/netcdf/lib -lnetcdf" \
           MPICC=mpicc
```

产物是 `vic/drivers/image/vic_image.exe`。

**运行时**：可执行文件需要能找到编译时用的 `libnetcdf.so`。在本机上是

```bash
export LD_LIBRARY_PATH=/home/WUR/liu297/miniconda3/envs/nco_env/lib:$LD_LIBRARY_PATH
```

在别的机器上换成该机 netCDF 的 lib 目录。忘记设置会得到
`error while loading shared libraries: libnetcdf.so.19`。

### AddressSanitizer / UBSan 构建

```bash
cd <VIC_ROOT>/vic/drivers/image
INC="-I ./include -I ../../vic_run/include -I ../shared_all/include \
     -I ../shared_image/include $(find ../../plugins -type d -name include | sed s/^/-I/ | tr '\n' ' ')"
make model \
  CFLAGS="$INC $(nc-config --cflags) -g -O1 -fcommon -std=c99 \
          -fsanitize=address,undefined -fno-omit-frame-pointer -DLOG_LVL=5 \
          -DGIT_VERSION=\\\"asan\\\" -DUSERNAME=\\\"$USER\\\" -DHOSTNAME=\\\"asan\\\"" \
  LIBRARY="-lm $(nc-config --libs) -fsanitize=address,undefined"
```

注意关掉了 `-O3` 和 `-fopenmp`，以减少误报。用单进程直接运行（不经 `mpirun`）。

---

## 4. 运行测试套件

**所有机器相关的事实都在 `sites/<name>.sh` 里**，脚本本身不含任何具体路径。
换一台机器 = 写一个新的 site profile，不用改代码。

```bash
source sites/anunna.sh          # 或 sites/snellius.sh

$VIC_TEST_PYTHON run_all.py \
  --workdir $VIC_TEST_WORKDIR/suite \
  --exe     <VIC_ROOT>/vic/drivers/image/vic_image.exe \
  --json    $VIC_TEST_WORKDIR/suite/monthly_veghist_summary.json
```

`--baseline-exe` 可选，指向改动前 commit（`c0350feb`）编译出的可执行文件，
用来证明旧 daily 接口逐位不变。

site profile 提供的变量：

| 变量 | 含义 |
|---|---|
| `VIC_TEST_PYTHON` | 分析环境的绝对解释器路径（绝不用裸 `python3`） |
| `VIC_TEST_MPI_RUN` | 启动器前缀，Anunna 是 `mpirun -np`，Snellius 是 `srun -n` |
| `VIC_TEST_MPI_RUN_MULTI` | 多 rank 用的前缀（Anunna login 节点需要 `--oversubscribe`） |
| `VIC_TEST_TEMPLATE_DOMAIN/PARAMS` | 生成合成 case 的模板输入，**只 Anunna 有** |
| `VIC_TEST_WORKDIR` | 测试输出根目录 |
| `LD_LIBRARY_PATH` / `module load` | netCDF 运行时，两站方式不同 |

生效的 site 事实会被写进 summary JSON 的 `site` 段，跨站点比对时可以直接看到
两边到底跑在什么环境上。

Slurm 提交：

```bash
VIC_TEST_SITE=anunna VIC_EXE=<VIC_ROOT>/vic/drivers/image/vic_image.exe \
  sbatch smoke_monthly_veghist.slurm
```

若 Anunna 本地 slurm 客户端不可用：

```bash
ssh -o BatchMode=yes login201 /shared/apps/slurm/24.11.6/bin/sbatch <path>
```

### 跨站点验证（bootstrap-snellius step 5）

Anunna 是 MPICH 3.1.3，Snellius 是 OpenMPI 5.0.3——这是 MPI 实现的更换而不是
版本升级，runbook 明确要求在两站跑同一个 case 做数值比对。做法：

1. 在 Anunna 生成 case（只有 Anunna 有模板输入）：
   ```bash
   source sites/anunna.sh
   $VIC_TEST_PYTHON make_synthetic_case.py --outdir <somewhere>/case
   ```
2. 把 `case/` 传到 Snellius（约 2.8 MB；**case 是数据，数据才走文件传输**，
   代码走 git SHA——见 vic-infra ADR-0003）；
3. 两边都用 `--skip-case` 跑：
   ```bash
   source sites/snellius.sh
   $VIC_TEST_PYTHON run_all.py --workdir <dir> --exe <exe> --skip-case --json <out>
   ```
   `--skip-case` 会自动把 global parameter 文件里的绝对路径重定向到新位置，
   并创建 `RESULT_DIR`（VIC 不会自己建，而且报的是误导性的 "Permission denied"）。
4. 比较两份 summary JSON。套件内部所有比对本来就是 bitwise 的，
   所以跨站点差异会直接暴露出来。

### 各脚本用途

| 脚本 | 作用 |
|---|---|
| `sites/*.sh` | 站点事实的唯一存放处 |
| `site_config.py` | 从环境变量读站点事实（命名避开 stdlib 的 `site` 模块） |
| `make_synthetic_case.py` | 生成最小 synthetic case（4×4 cell、14 veg_class、2003+2004），含 monthly 与 daily-expanded 两套等价 forcing |
| `compare_outputs.py` | monthly vs daily-expanded 逐日比对，输出 JSON |
| `test_restart_continuity.py` | 连续运行 vs 分段 restart（月中 + 年末断点）比对 |
| `test_error_paths.py` | 12 个畸形输入/配置用例，验证是否按契约失败 |
| `run_all.py` | 串起以上全部，输出单个 JSON 摘要 |
| `smoke_monthly_veghist.slurm` | Slurm 封装，按 `VIC_TEST_SITE` 选 profile |

合成 case 的 domain 与 parameter 是从既有 Indus 参数集**只读**裁剪出来的，
因此保证内部自洽；原始文件不会被修改。

---

## 5. 已验证结果

在 4×4 cell、14 veg_class、2003（平年）+2004（闰年）共 731 天上：

| 检验 | 结果 |
|---|---|
| monthly vs daily-expanded，12 个输出变量逐日 | **bitwise identical**（11696 个比较点/变量，max abs diff = 0） |
| 等价性测试的敏感性（能否检出错误记录） | 每月内恒定；12 个月值互不相同；2003 年 1 月 ≠ 2004 年 1 月 |
| 输出值 vs 源 monthly 文件（Cv 加权） | 24 个月全部一致 |
| MPI 1 rank vs 4 rank | **bitwise identical** |
| 连续运行 vs 分段 restart（断点 2003-07-15 月中、2004-01-01 年末） | **bitwise identical** |
| 旧 daily 3-token 配置：baseline 二进制 vs 新二进制 | **bitwise identical** |
| 畸形输入 10 个用例 | 全部按契约失败/警告 |
| ASan + UBSan（单进程、完整 2 年 monthly 运行） | **0 report** |

bitwise 一致的原因：monthly 与 daily 文件存的是相同的 float32 位模式，
netCDF 读入后统一提升为 double（IEEE-754 float→double 精确无舍入），
`NF = 1` 时 `average(x, 1) == x`。因此不需要任何容差。

畸形输入用例：

| 用例 | 期望 |
|---|---|
| 缺一个月（time=11） | `log_err` |
| 月份重复 | `log_err` |
| 月份乱序 | `log_err` |
| calendar 与 global 不一致 | `log_err` |
| `veg_class` 维长不符 | `log_err` |
| `FORCE_TYPE PREC prcp MONTH ...` | `log_err`（白名单） |
| `MONTH` 但 `LAI_SRC != FROM_VEGHIST` | `log_err` |
| 未知频率 `WEEK` | `log_err` |
| `DAY` 频率 | `log_err`（core 未实现） |
| active tile 上 FillValue / NaN | `log_warn` + 回退当月 climatology，运行继续 |

---

## 6. 向后兼容性

1. **旧 global parameter 文件零改动可运行**，且结果 bitwise 不变（已用 baseline
   二进制对照验证）。
2. **NAT-S（`FROM_VEGPARAM` / `FROM_VEGLIB`）完全不受影响**：改动只在
   `*_SRC == FROM_VEGHIST` 分支内生效。
3. **state / restart 文件格式未改变**。monthly 索引是当前日期的纯函数，
   不需要持久化任何计数器。
4. **气象 forcing 的代码路径未改变**：met 变量走 STEP 分支，与改动前完全相同。
5. **唯一的行为变化**：veg-history forcing 中 active tile 上的无效值
   （非有限值，或 LAI ∉ [0,20]、fcanopy/albedo ∉ [0,1]），
   原先会把未屏蔽的 `_FillValue`（约 1e36）直接带进能量平衡，
   现在改为限流警告 + 回退当月 parameter climatology，并在运行结束时报告累计数量。
   **数据干净时数值结果 bitwise 不变。**

---

## 7. 已知限制与后续风险

1. **`DAY` / `YEAR` 频率在 core 未实现**，只在 `PLUGIN_FORCE_TYPE` 上可用。
   若 NAT-PCV 之后需要 core 侧的 annual veg forcing，需要再次扩展。
2. **每步仍然读一次 monthly 记录。** 当前实现把"索引正确"放在第一位：
   monthly 模式下每个 model step 仍会读取当月记录（但相比 daily 少了 NF 倍的
   sub-step 读取）。真正的"月内只读一次"缓存优化尚未做，因为
   `vic_force.c` 每步都会先用 parameter climatology 重填 `veg_hist`，
   跳过读取会让月内第 2 天起退回 climatology。要做这个优化必须同时引入月值缓存，
   并**重跑本套件的等价性测试**。
3. **首尾年不允许只含模拟月份子集**：每个年文件都必须是完整 12 条。
4. **plugin 侧 `plugin_get_forcing_file_skip()` 的 `unsigned` 下溢缺陷仍然存在**
   （`start_dmy.month - force_dmy.month` 在起始月早于文件首月时会下溢）。
   本次改动**未修**（属 plugin 逻辑，超出范围），建议单独开 issue。
5. **`vic_force.c` 的 `options.LAKES` 分支使用了循环外的陈旧 `j`**，是既有 bug。
   本项目 `LAKES = FALSE` 不触发，本次**未修**，仅记录。
6. **正式 monthly forcing 必须等待修正后的 Cv/canopy 产品**。现有 1979 daily pilot
   （Slurm job `42017349_1979`）只作 daily-reference 候选，不是正式 forcing，
   本次改动也没有读写它。
7. 本套件在 4×4 cell 上验证。**全球 5min（1680×4320）尚未做规模化 smoke**，
   建议在正式 forcing 就绪后先跑一个月的全球单核 smoke，再进入长模拟。
