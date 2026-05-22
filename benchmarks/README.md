# Benchmarks / Бенчмарки

Status: **pre-1.0 baseline tooling / инструмент baseline перед 1.0**

The benchmark runner is intentionally separate from normal tests. It prints baseline metrics and does not fail CI based on timing thresholds.

Benchmark runner намеренно отделён от обычных тестов. Он печатает baseline metrics и не валит CI по временным порогам.

---

## Build

```bash
cmake -S . -B build-bench -DCLC_BUILD_TESTS=OFF -DCLC_BUILD_EXAMPLES=OFF -DCLC_BUILD_BENCHMARKS=ON
cmake --build build-bench --target clc_core_benchmarks
```

## Run

```bash
./build-bench/clc_core_benchmarks
```

On Windows/MSVC the executable path may be under the configuration directory, for example:

```powershell
.\build-bench\Debug\clc_core_benchmarks.exe
```

---

## Current benchmark coverage

`clc_core_benchmarks` currently records:

- `engine_advance_day_100_settlements_365_days`
- `engine_run_scenario_25_settlements_90_days`
- `world_state_serialize_100_settlements_30_days`
- `world_state_deserialize_100_settlements_30_days`
- `runtime_run_days_1000_caravans_30_days`

Output is CSV-like:

```text
benchmark,metric,value,detail,value
engine_advance_day_100_settlements_365_days,elapsed_ms,12,events,37230
```

---

## Release use

Before 1.0.0-rc1, run the benchmark binary on the release branch and record results in release notes or a separate benchmark report.

Перед 1.0.0-rc1 нужно запустить benchmark binary на release branch и записать результаты в release notes или отдельный benchmark report.

Recommended release checklist items:

- run on at least one Linux machine;
- run on at least one Windows/MSVC machine if possible;
- save raw output;
- compare with previous RC if one exists;
- investigate large regressions before tagging final 1.0.0.

---

## Not a microbenchmark suite

This is not a precise microbenchmark framework. It is a coarse smoke benchmark gate for detecting obvious performance and memory-shape risks before public SDK release.

Это не точный microbenchmark framework. Это coarse smoke benchmark gate, чтобы перед public SDK release увидеть очевидные performance/memory-shape риски.
