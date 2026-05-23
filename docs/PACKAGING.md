# City Life Core Packaging / Упаковка

Version: **0.9.9**

This document explains how to build, install, and consume City Life Core from an external C++ project.

Этот документ объясняет, как собрать, установить и подключить City Life Core из внешнего C++ проекта.

---

## Русский

### CMake package summary

City Life Core устанавливается как CMake package и экспортирует target:

```cmake
CityLifeCore::core
```

External projects should use:

```cmake
find_package(CityLifeCore CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE CityLifeCore::core)
```

### Build options

Когда City Life Core собирается как top-level project, tests/examples/tools включены по умолчанию. Когда проект подключается через `add_subdirectory(...)`, они выключены по умолчанию, чтобы внешний build не получал лишние targets.

| Option | Top-level default | Subdirectory default | Purpose |
| --- | --- | --- | --- |
| `CLC_BUILD_TESTS` | `ON` | `OFF` | Build unit/integration tests |
| `CLC_BUILD_EXAMPLES` | `ON` | `OFF` | Build SDK examples |
| `CLC_BUILD_TOOLS` | `ON` | `OFF` | Build command-line tools such as `clc_runner` |
| `CLC_BUILD_BENCHMARKS` | `OFF` | `OFF` | Build coarse benchmark executables |

### Install from source

```bash
cmake -S . -B build -DCLC_BUILD_TESTS=OFF -DCLC_BUILD_EXAMPLES=OFF -DCLC_BUILD_TOOLS=OFF -DCMAKE_INSTALL_PREFIX=/path/to/city-life-core-sdk
cmake --build build
cmake --install build
```

Windows / multi-config generators:

```powershell
cmake -S . -B build -DCLC_BUILD_TESTS=OFF -DCLC_BUILD_EXAMPLES=OFF -DCLC_BUILD_TOOLS=OFF -DCMAKE_INSTALL_PREFIX=C:\SDK\CityLifeCore
cmake --build build --config Release
cmake --install build --config Release
```

### Installed layout

Default layout uses GNUInstallDirs:

```text
<prefix>/
  include/
    clc/
      CityLifeCore.hpp
      c/
        CityLifeCoreC.h
      core/
      data/
      economy/
      sim/
  lib/
    libcity_life_core.*
    cmake/CityLifeCore/
      CityLifeCoreConfig.cmake
      CityLifeCoreConfigVersion.cmake
      CityLifeCoreTargets.cmake
  share/
    doc/CityLifeCore/
      README.md
      CHANGELOG.md
      PUBLIC_API.md
      PUBLIC_API_STATUS.md
      SDK_STRUCTURE.md
      VERSIONING.md
      COMPATIBILITY.md
      MIGRATION.md
      PACKAGING.md
      examples/
    CityLifeCore/data/
      demo_fantasy/
```

`lib/` may be `lib64/` or another configured path depending on platform/toolchain.

### Minimal external CMake project

`CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.22)
project(MyCityLifeApp LANGUAGES CXX)

find_package(CityLifeCore CONFIG REQUIRED)

add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE CityLifeCore::core)
```

`main.cpp`:

```cpp
#include "clc/CityLifeCore.hpp"

#include <iostream>

int main() {
    auto bootstrap = clc::sim::make_basic_runtime_scenario();
    if (!bootstrap.ok()) {
        return 1;
    }

    auto& runtime = bootstrap.runtime;
    auto report = clc::sim::advance_runtime_ticks(runtime, clc::minutes_to_ticks(5));
    if (!report.ok()) {
        return 1;
    }

    std::cout << clc::core_version_string() << " " << runtime.time.current_tick() << "\n";
    return 0;
}
```

Configure external project:

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/city-life-core-sdk
cmake --build build
./build/my_app
```

### Installed SDK smoke tests

The repository includes standalone consumer examples:

```text
examples/find_package_consumer/   # C++ consumer
examples/c_abi_consumer/          # C consumer for minimal C ABI
```

End-to-end C++ install check:

```bash
cmake -S . -B build-sdk -DCLC_BUILD_TESTS=OFF -DCLC_BUILD_EXAMPLES=OFF -DCLC_BUILD_TOOLS=OFF
cmake --build build-sdk
cmake --install build-sdk --prefix /tmp/city-life-core-sdk

cmake -S examples/find_package_consumer -B build-consumer -DCMAKE_PREFIX_PATH=/tmp/city-life-core-sdk
cmake --build build-consumer
./build-consumer/city_life_core_consumer
```

End-to-end C ABI install check:

```bash
cmake -S examples/c_abi_consumer -B build-c-abi-consumer -DCMAKE_PREFIX_PATH=/tmp/city-life-core-sdk
cmake --build build-c-abi-consumer
./build-c-abi-consumer/city_life_core_c_abi_consumer
```

These checks verify:

- installed public headers;
- installed C ABI header `clc/c/CityLifeCoreC.h`;
- installed `CityLifeCoreConfig.cmake`;
- exported target `CityLifeCore::core`;
- public include paths from the install prefix;
- a minimal runtime tick scenario in an external C++ project;
- version/time C ABI functions and the minimal opaque `clc_world` handle from an external C project.

### Using add_subdirectory

```cmake
add_subdirectory(external/City_Life_Core)
target_link_libraries(my_app PRIVATE CityLifeCore::core)
```

When embedded this way, enable optional targets explicitly if needed:

```cmake
set(CLC_BUILD_EXAMPLES ON CACHE BOOL "" FORCE)
set(CLC_BUILD_TESTS ON CACHE BOOL "" FORCE)
add_subdirectory(external/City_Life_Core)
```

### SDK data and docs paths

Demo data is installed under:

```text
<prefix>/share/CityLifeCore/data/
```

Docs are installed under:

```text
<prefix>/share/doc/CityLifeCore/
```

If a game needs data-pack discovery at runtime, pass the data directory explicitly from your application configuration.

### Static/shared notes

The current CMake target builds according to the normal CMake library settings of the project/toolchain. Projects that need a strict static/shared distribution policy should enforce it in their own build configuration until a separate binary SDK policy is documented.

---

## English

### CMake package summary

City Life Core installs as a CMake package and exports this target:

```cmake
CityLifeCore::core
```

External projects should use:

```cmake
find_package(CityLifeCore CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE CityLifeCore::core)
```

### Build options

When City Life Core is built as the top-level project, tests/examples/tools default to `ON`. When consumed through `add_subdirectory(...)`, they default to `OFF` so the parent build does not receive extra targets by default.

| Option | Top-level default | Subdirectory default | Purpose |
| --- | --- | --- | --- |
| `CLC_BUILD_TESTS` | `ON` | `OFF` | Build unit/integration tests |
| `CLC_BUILD_EXAMPLES` | `ON` | `OFF` | Build SDK examples |
| `CLC_BUILD_TOOLS` | `ON` | `OFF` | Build command-line tools such as `clc_runner` |
| `CLC_BUILD_BENCHMARKS` | `OFF` | `OFF` | Build coarse benchmark executables |

### Install from source

```bash
cmake -S . -B build -DCLC_BUILD_TESTS=OFF -DCLC_BUILD_EXAMPLES=OFF -DCLC_BUILD_TOOLS=OFF -DCMAKE_INSTALL_PREFIX=/path/to/city-life-core-sdk
cmake --build build
cmake --install build
```

Windows / multi-config generators:

```powershell
cmake -S . -B build -DCLC_BUILD_TESTS=OFF -DCLC_BUILD_EXAMPLES=OFF -DCLC_BUILD_TOOLS=OFF -DCMAKE_INSTALL_PREFIX=C:\SDK\CityLifeCore
cmake --build build --config Release
cmake --install build --config Release
```

### Installed layout

Default layout uses GNUInstallDirs:

```text
<prefix>/
  include/
    clc/
      CityLifeCore.hpp
      c/
        CityLifeCoreC.h
      core/
      data/
      economy/
      sim/
  lib/
    libcity_life_core.*
    cmake/CityLifeCore/
      CityLifeCoreConfig.cmake
      CityLifeCoreConfigVersion.cmake
      CityLifeCoreTargets.cmake
  share/
    doc/CityLifeCore/
      README.md
      CHANGELOG.md
      PUBLIC_API.md
      PUBLIC_API_STATUS.md
      SDK_STRUCTURE.md
      VERSIONING.md
      COMPATIBILITY.md
      MIGRATION.md
      PACKAGING.md
      examples/
    CityLifeCore/data/
      demo_fantasy/
```

`lib/` may be `lib64/` or another configured path depending on platform/toolchain.

### Minimal external CMake project

`CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.22)
project(MyCityLifeApp LANGUAGES CXX)

find_package(CityLifeCore CONFIG REQUIRED)

add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE CityLifeCore::core)
```

`main.cpp`:

```cpp
#include "clc/CityLifeCore.hpp"

#include <iostream>

int main() {
    auto bootstrap = clc::sim::make_basic_runtime_scenario();
    if (!bootstrap.ok()) {
        return 1;
    }

    auto& runtime = bootstrap.runtime;
    auto report = clc::sim::advance_runtime_ticks(runtime, clc::minutes_to_ticks(5));
    if (!report.ok()) {
        return 1;
    }

    std::cout << clc::core_version_string() << " " << runtime.time.current_tick() << "\n";
    return 0;
}
```

Configure external project:

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/city-life-core-sdk
cmake --build build
./build/my_app
```

### Installed SDK smoke tests

The repository includes standalone consumer examples:

```text
examples/find_package_consumer/   # C++ consumer
examples/c_abi_consumer/          # C consumer for minimal C ABI
```

End-to-end C++ install check:

```bash
cmake -S . -B build-sdk -DCLC_BUILD_TESTS=OFF -DCLC_BUILD_EXAMPLES=OFF -DCLC_BUILD_TOOLS=OFF
cmake --build build-sdk
cmake --install build-sdk --prefix /tmp/city-life-core-sdk

cmake -S examples/find_package_consumer -B build-consumer -DCMAKE_PREFIX_PATH=/tmp/city-life-core-sdk
cmake --build build-consumer
./build-consumer/city_life_core_consumer
```

End-to-end C ABI install check:

```bash
cmake -S examples/c_abi_consumer -B build-c-abi-consumer -DCMAKE_PREFIX_PATH=/tmp/city-life-core-sdk
cmake --build build-c-abi-consumer
./build-c-abi-consumer/city_life_core_c_abi_consumer
```

These checks verify:

- installed public headers;
- installed C ABI header `clc/c/CityLifeCoreC.h`;
- installed `CityLifeCoreConfig.cmake`;
- exported target `CityLifeCore::core`;
- public include paths from the install prefix;
- a minimal runtime tick scenario in an external C++ project;
- version/time C ABI functions and the minimal opaque `clc_world` handle from an external C project.

### Using add_subdirectory

```cmake
add_subdirectory(external/City_Life_Core)
target_link_libraries(my_app PRIVATE CityLifeCore::core)
```

When embedded this way, enable optional targets explicitly if needed:

```cmake
set(CLC_BUILD_EXAMPLES ON CACHE BOOL "" FORCE)
set(CLC_BUILD_TESTS ON CACHE BOOL "" FORCE)
add_subdirectory(external/City_Life_Core)
```

### SDK data and docs paths

Demo data is installed under:

```text
<prefix>/share/CityLifeCore/data/
```

Docs are installed under:

```text
<prefix>/share/doc/CityLifeCore/
```

If a game needs data-pack discovery at runtime, pass the data directory explicitly from your application configuration.

### Static/shared notes

The current CMake target builds according to the normal CMake library settings of the project/toolchain. Projects that need a strict static/shared distribution policy should enforce it in their own build configuration until a separate binary SDK policy is documented.
