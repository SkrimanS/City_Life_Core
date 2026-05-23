# Build and Linking Policy / Политика сборки и линковки

Version: **1.0.0**

This document describes how City Life Core should be built and linked by SDK users and release maintainers.

---

## Recommended release model

City Life Core 1.0.0 is a **source-first C++ SDK**.

Recommended public SDK consumption modes:

1. CMake installed package with `find_package(CityLifeCore CONFIG REQUIRED)`.
2. SDK ZIP package produced from the install layout.
3. Source build through `add_subdirectory(...)` or vendored source tree.

The stable public contract for 1.0.0 is primarily the **source-level C++ API** and the **minimal C interface** documented in `docs/C_ABI.md`.

---

## Static vs shared builds

`city_life_core` is declared with CMake `add_library(...)` without forcing `STATIC` or `SHARED`. This means CMake's standard `BUILD_SHARED_LIBS` variable can influence the library type.

Recommended default for SDK releases:

```bash
cmake -S . -B build-sdk -DBUILD_SHARED_LIBS=OFF
```

### Static build

Static builds are the recommended default for 1.0.0 SDK packages.

Reasons:

- simpler distribution;
- no platform-specific symbol export rules required;
- no C++ binary ABI stability promise needed;
- easier consumption by games/tools that vendor or install the SDK.

### Shared build

Shared builds may work on some platforms/toolchains, but they are not the recommended public release artifact for 1.0.0.

Reasons:

- C++ symbol visibility/export policy is not finalized;
- C++ binary ABI compatibility is not guaranteed;
- Windows DLL export/import annotations are not part of the public API yet;
- runtime/container/template-heavy APIs are better treated as source-level C++ APIs.

If a project builds City Life Core as a shared library, that project owns the platform-specific binary distribution and compatibility testing.

---

## C++ ABI policy

For 1.0.0:

- C++ **source compatibility** is the primary compatibility target.
- C++ **binary ABI compatibility** is not promised.
- Public headers under `include/clc/**` are documented by status in `docs/PUBLIC_API_STATUS.md`.
- Stable-candidate headers should avoid unnecessary breaking changes in 1.x releases.
- Experimental and diagnostics APIs may change more easily.

---

## Minimal C interface policy

The minimal C interface is available through:

```c
#include "clc/c/CityLifeCoreC.h"
```

It currently covers:

- core version;
- C interface version `3`;
- tick constants;
- time conversion preflight helpers;
- saturating time conversion helpers;
- opaque `clc_world` create/destroy;
- basic world state access;
- simple world tick advancement;
- read-only world event accessors.

It does not expose full runtime state, registries, containers, save/load, callbacks, caravans, contracts or economy workflows.

The C interface has its own version:

```c
uint32_t clc_c_interface_version_c(void);
```

This is separate from the core SDK version.

---

## CMake package target

The exported target is:

```cmake
CityLifeCore::core
```

C++ consumers:

```cmake
find_package(CityLifeCore CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE CityLifeCore::core)
```

C consumers for the minimal C interface should compile their `.c` files as C but link with the C++ linker:

```cmake
project(MyCityLifeCoreCConsumer LANGUAGES C CXX)
find_package(CityLifeCore CONFIG REQUIRED)

add_executable(my_consumer main.c)
set_target_properties(my_consumer PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(my_consumer PRIVATE CityLifeCore::core)
```

---

## Package variables

The installed CMake package exposes:

```cmake
CityLifeCore_INCLUDE_DIR
CityLifeCore_DOCS_DIR
CityLifeCore_DATA_DIR
CityLifeCore_EXAMPLES_DIR
```

See:

```text
docs/CMAKE_PACKAGE.md
```

---

## Release artifact policy

Recommended 1.0.0 artifacts:

```text
city-life-core-sdk-1.0.0-<system>-<processor>.zip
SHA256SUMS.txt
release manifest
```

The ZIP package should contain:

- headers;
- static library by default;
- CMake package files;
- documentation;
- examples;
- demo data.

CI or manual release validation should check:

- build and tests;
- installed C++ consumer;
- installed C interface consumer;
- CPack ZIP creation;
- checksum generation;
- unpacked ZIP C++ consumer;
- unpacked ZIP C interface consumer;
- benchmark output.

---

## Current 1.0.0 position

City Life Core 1.0.0 should ship as source-first SDK package:

- publish source and SDK ZIP with install layout;
- do not promise C++ binary ABI;
- recommend static linking;
- treat C++ API compatibility as source-level compatibility;
- keep the minimal C interface stable where possible.
