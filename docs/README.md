# City Life Core Documentation / Документация

Version: **0.9.9**

This directory contains developer-facing documentation for integrating City Life Core into a C++ game, server, backend service, editor, or simulation tool.

Эта папка содержит документацию для разработчиков, которые подключают City Life Core к C++ игре, серверу, backend-сервису, редактору или simulation tool.

---

## Start here / С чего начать

| Document | Purpose |
| --- | --- |
| `CORE_CONCEPTS.md` | Main systems: registry, runtime, ticks, settlements, caravans, economy, contracts, events and persistence. |
| `PUBLIC_API.md` | Public SDK API, recommended include, validation model, workflows and usage rules. |
| `SDK_STRUCTURE.md` | Repository layout, installed SDK layout, CMake options, integration modes and modules. |
| `PACKAGING.md` | Build, install and consume the SDK through CMake. |
| `CMAKE_PACKAGE.md` | Exported CMake target and installed package directory variables. |
| `SDK_ZIP_PACKAGE.md` | Build and consume a CPack ZIP SDK archive. |
| `BUILD_AND_LINKING_POLICY.md` | Static/shared, source-first and binary compatibility policy. |

---

## API and compatibility / API и совместимость

| Document | Purpose |
| --- | --- |
| `PUBLIC_API_STATUS.md` | Header-by-header usage classification. |
| `C_ABI.md` | Minimal C interface for version/time utilities, an opaque world handle and read-only world event access. |
| `COMPATIBILITY.md` | Time, events, save/load, validation and source/binary compatibility rules. |
| `MIGRATION.md` | Instructions for updating older integrations. |

---

## Release documents / Документы релиза

| Document | Purpose |
| --- | --- |
| `READINESS_STATUS.md` | Current subsystem readiness estimate and release-readiness interpretation. |
| `VERSIONING.md` | Versioning and tag rules. |
| `VERIFYING_RELEASES.md` | Release verification guide. |
| `CI_ARTIFACT_REVIEW.md` | Manual CI, benchmark artifact and SDK ZIP artifact review guide. |
| `RELEASE_BLOCKERS.md` | Active release blockers and exit criteria. |
| `RELEASE_MANIFEST_TEMPLATE.md` | Manifest template for release artifacts. |
| `RELEASE_CHECKLIST.md` | Maintainer release checklist. |
| `PROTECTION_STRATEGY.md` | Project protection and distribution strategy. |

Normal SDK users usually start with `CORE_CONCEPTS.md`, `PUBLIC_API.md`, `SDK_STRUCTURE.md` and `PACKAGING.md`.

---

## Quick CMake integration

```cmake
find_package(CityLifeCore CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE CityLifeCore::core)
```

Recommended C++ include:

```cpp
#include "clc/CityLifeCore.hpp"
```

Minimal C interface include:

```c
#include "clc/c/CityLifeCoreC.h"
```

---

## Main runtime path

```cpp
auto bootstrap = clc::sim::make_basic_runtime_scenario();
if (!bootstrap.ok()) {
    return;
}

auto& runtime = bootstrap.runtime;
clc::sim::advance_runtime_ticks(runtime, clc::minutes_to_ticks(5));
```
