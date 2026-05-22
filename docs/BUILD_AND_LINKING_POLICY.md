# Build and Linking Policy / Политика сборки и линковки

Version: **0.9.9**

This document describes how City Life Core should be built and linked by SDK users and release maintainers.

Этот документ описывает, как City Life Core должен собираться и линковаться SDK users и release maintainers.

---

## Recommended release model

City Life Core is currently a **source-first C++ SDK**.

Recommended public SDK consumption modes:

1. CMake installed package with `find_package(CityLifeCore CONFIG REQUIRED)`.
2. SDK ZIP package produced from the install layout.
3. Source build through `add_subdirectory(...)` or vendored source tree.

The stable public contract for 0.9.9/pre-1.0 is primarily the **source-level C++ API** and the **minimal C interface** documented in `docs/C_ABI.md`.

---

## Static vs shared builds

`city_life_core` is declared with CMake `add_library(...)` without forcing `STATIC` or `SHARED`. This means CMake's standard `BUILD_SHARED_LIBS` variable can influence the library type.

Recommended default for SDK releases:

```bash
cmake -S . -B build-sdk -DBUILD_SHARED_LIBS=OFF
```

### Static build

Static builds are the recommended default for 0.9.9/pre-1.0 SDK packages.

Reasons:

- simpler distribution;
- no platform-specific symbol export rules required;
- no C++ binary ABI stability promise needed;
- easier consumption by games/tools that vendor or install the SDK.

### Shared build

Shared builds may work on some platforms/toolchains, but they are not the recommended public release artifact yet.

Reasons:

- C++ symbol visibility/export policy is not finalized;
- C++ binary ABI compatibility is not guaranteed;
- Windows DLL export/import annotations are not part of the public API yet;
- runtime/container/template-heavy APIs are better treated as source-level C++ APIs for now.

If a project builds City Life Core as a shared library, that project owns the platform-specific binary distribution and compatibility testing.

---

## C++ ABI policy

For 0.9.9/pre-1.0:

- C++ **source compatibility** is the primary compatibility target.
- C++ **binary ABI compatibility** is not promised.
- Public headers under `include/clc/**` are documented by status in `docs/PUBLIC_API_STATUS.md`.
- Stable-candidate headers should avoid unnecessary breaking changes before 1.0.0.
- Experimental and diagnostics APIs may change more easily.

For 1.0.0:

- the project should explicitly decide whether the release is source-only or binary-distributed;
- if binary artifacts are published, supported platform/toolchain combinations should be listed;
- if shared libraries are published, symbol export/import and ABI rules must be documented.

---

## Minimal C interface policy

The minimal C interface is available through:

```c
#include "clc/c/CityLifeCoreC.h"
```

It currently covers:

- core version;
- C interface version;
- tick constants;
- time conversion preflight helpers;
- saturating time conversion helpers.

It does not expose full runtime state or ownership of core objects.

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

For 0.9.9/pre-1.0 audit builds, release artifacts should be treated as SDK validation artifacts, not final binary ABI commitments.

Recommended artifacts:

```text
city-life-core-sdk-<version>-<system>-<processor>.zip
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

CI should validate:

- build and tests;
- installed C++ consumer;
- installed C interface consumer;
- CPack ZIP creation;
- checksum generation;
- unpacked ZIP C++ consumer;
- unpacked ZIP C interface consumer;
- benchmark baseline output.

---

## Release decision before 1.0.0

Before tagging 1.0.0, the maintainer should explicitly choose one of these positions:

### Option A: source-first 1.0.0

- Publish source and SDK ZIP with install layout.
- Do not promise C++ binary ABI.
- Recommend static linking.
- Treat C++ API compatibility as source-level compatibility.
- Keep minimal C interface stable where possible.

### Option B: binary SDK 1.0.0

- Publish platform/toolchain-specific artifacts.
- Define supported compilers and standard libraries.
- Define shared/static policy.
- Define symbol export/import policy.
- Define C++ ABI policy.
- Add binary compatibility tests.

Recommended current path: **Option A: source-first 1.0.0**.
