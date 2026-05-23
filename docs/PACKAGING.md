# City Life Core Packaging / Упаковка

Version: **1.0.0**

This document explains how to build, install, package, and consume the City Life Core 1.0.0 SDK.

---

## CMake package

City Life Core installs as a CMake package and exports:

```cmake
CityLifeCore::core
```

External C++ projects should use:

```cmake
find_package(CityLifeCore CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE CityLifeCore::core)
```

External C consumers may use `clc/c/CityLifeCoreC.h`, while linking through the C++ implementation:

```cmake
project(MyCityLifeCoreCConsumer LANGUAGES C CXX)
find_package(CityLifeCore CONFIG REQUIRED)
add_executable(my_consumer main.c)
set_target_properties(my_consumer PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(my_consumer PRIVATE CityLifeCore::core)
```

---

## Build options

| Option | Top-level default | Subdirectory default | Purpose |
| --- | --- | --- | --- |
| `CLC_BUILD_TESTS` | `ON` | `OFF` | Build tests. |
| `CLC_BUILD_EXAMPLES` | `ON` | `OFF` | Build examples. |
| `CLC_BUILD_TOOLS` | `ON` | `OFF` | Build command-line tools. |
| `CLC_BUILD_BENCHMARKS` | `OFF` | `OFF` | Build benchmarks. |

---

## Install from source

```bash
cmake -S . -B build -DCLC_BUILD_TESTS=OFF -DCLC_BUILD_EXAMPLES=OFF -DCLC_BUILD_TOOLS=OFF -DCMAKE_INSTALL_PREFIX=/path/to/city-life-core-sdk
cmake --build build
cmake --install build
```

Installed package variables are documented in `docs/CMAKE_PACKAGE.md`:

- `CityLifeCore_INCLUDE_DIR`
- `CityLifeCore_DOCS_DIR`
- `CityLifeCore_DATA_DIR`
- `CityLifeCore_EXAMPLES_DIR`

---

## Installed SDK checks

Standalone consumers:

```text
examples/find_package_consumer/   # C++ consumer
examples/c_abi_consumer/          # C consumer for the minimal C ABI
```

C++ install check:

```bash
cmake -S . -B build-sdk -DCLC_BUILD_TESTS=OFF -DCLC_BUILD_EXAMPLES=OFF -DCLC_BUILD_TOOLS=OFF
cmake --build build-sdk
cmake --install build-sdk --prefix /tmp/city-life-core-sdk
cmake -S examples/find_package_consumer -B build-consumer -DCMAKE_PREFIX_PATH=/tmp/city-life-core-sdk
cmake --build build-consumer
./build-consumer/city_life_core_consumer
```

C ABI install check:

```bash
cmake -S examples/c_abi_consumer -B build-c-abi-consumer -DCMAKE_PREFIX_PATH=/tmp/city-life-core-sdk
cmake --build build-c-abi-consumer
./build-c-abi-consumer/city_life_core_c_abi_consumer
```

These checks verify installed public headers, `CityLifeCoreConfig.cmake`, exported target `CityLifeCore::core`, C++ runtime consumption, and the C ABI version/time/world/event surface.

---

## SDK ZIP package

```bash
cmake -S . -B build-sdk-zip -DCLC_BUILD_TESTS=OFF -DCLC_BUILD_EXAMPLES=OFF -DCLC_BUILD_TOOLS=OFF
cmake --build build-sdk-zip --config Release
cpack --config build-sdk-zip/CPackConfig.cmake -G ZIP
cmake -E sha256sum city-life-core-sdk-*.zip > SHA256SUMS.txt
```

Expected 1.0.0 artifact pattern:

```text
city-life-core-sdk-1.0.0-<system>-<processor>.zip
```

The ZIP can be consumed with `CMAKE_PREFIX_PATH` after unpacking. See `docs/SDK_ZIP_PACKAGE.md` and `docs/CI_ARTIFACT_REVIEW.md` for release validation rules.

---

## Policy

Source-first and binary compatibility policy are documented in `docs/BUILD_AND_LINKING_POLICY.md`.
