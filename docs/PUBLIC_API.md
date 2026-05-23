# City Life Core Public API / Публичный API

Version: **1.0.0**  
Status: **1.0.0 public SDK surface / публичная поверхность SDK 1.0.0**

This document describes the supported public API surface for City Life Core 1.0.0.

Этот документ описывает поддерживаемую публичную API-поверхность City Life Core 1.0.0.

For the full header-by-header stability table, see:

```text
docs/PUBLIC_API_STATUS.md
```

---

## Recommended SDK include

For most external C++ SDK users, start with the umbrella header:

```cpp
#include "clc/CityLifeCore.hpp"
```

The umbrella header includes the recommended runtime, economy, data, route, caravan, faction, ownership, contract, persistence and diagnostics surface. It intentionally does not hide subsystem headers; larger projects may still include specific headers directly.

For the minimal C interface:

```c
#include "clc/c/CityLifeCoreC.h"
```

The C interface is intentionally minimal in 1.0.0. Full runtime integration remains C++ API.

---

## API layers

City Life Core is split into these public layers:

1. **Core** — version, IDs, results, time, world lifecycle and event log.
2. **Data** — definitions, registry, validation and data-pack loading.
3. **Simulation** — storage, settlements, engine, runtime workflows, routes, caravans, factions, ownership and contracts.
4. **Economy** — market, trade, wallet, ledger and orders.
5. **Persistence / diagnostics** — save/load, validation, deterministic replay and event analysis.
6. **C ABI** — minimal C/FFI entry point for version/time/world smoke integration.

Main namespaces:

```cpp
namespace clc {}
namespace clc::data {}
namespace clc::sim {}
namespace clc::economy {}
```

---

## Stability model

All headers under `include/clc/` are installed, but not all have the same compatibility promise.

Use `docs/PUBLIC_API_STATUS.md` for exact classification. The main categories are:

- `recommended` — default integration entry point.
- `stable-candidate` — expected public SDK surface for 1.0.0.
- `experimental` — public but may change in later releases.
- `diagnostics` — observability, testing, replay or validation API.
- `legacy` — retained compatibility surface, not the recommended path.
- `specialized` — advanced or lower-level integration surface.
- `c-abi-minimal` — intentionally small C ABI surface.

Source compatibility is the primary C++ compatibility target. Binary ABI stability is not promised for the C++ API; see `docs/BUILD_AND_LINKING_POLICY.md`.

---

## Core API

Common headers:

```cpp
#include "clc/core/Version.hpp"
#include "clc/core/Time.hpp"
#include "clc/core/EventLog.hpp"
#include "clc/core/World.hpp"
```

Main concepts:

- `clc::core_version()`;
- `clc::core_version_string()`;
- typed IDs and result/error helpers;
- tick constants and time conversion helpers;
- `clc::World` and basic event log support.

---

## Data API

Common headers:

```cpp
#include "clc/data/DataRegistry.hpp"
#include "clc/data/DataPackLoader.hpp"
#include "clc/data/Validation.hpp"
```

Main concepts:

- resource, currency, profession, building and settlement definitions;
- `clc::data::DataRegistry`;
- validation reports;
- `.clcd` data-pack loading;
- cross-reference validation.

---

## Simulation API

Common headers:

```cpp
#include "clc/sim/Storage.hpp"
#include "clc/sim/Settlement.hpp"
#include "clc/sim/SimulationRuntime.hpp"
#include "clc/sim/SimulationRuntimeWorkflow.hpp"
#include "clc/sim/SimulationRuntimeTick.hpp"
#include "clc/sim/Routes.hpp"
#include "clc/sim/Caravans.hpp"
#include "clc/sim/Factions.hpp"
#include "clc/sim/Ownership.hpp"
#include "clc/sim/Contracts.hpp"
```

Recommended runtime path:

```cpp
auto bootstrap = clc::sim::make_basic_runtime_scenario();
if (!bootstrap.ok()) {
    return;
}

auto& runtime = bootstrap.runtime;
clc::sim::advance_runtime_ticks(runtime, clc::minutes_to_ticks(5));
```

Prefer workflow helpers over direct runtime mutation when possible, because workflow helpers preserve more invariants.

---

## Economy API

Common headers:

```cpp
#include "clc/economy/Market.hpp"
#include "clc/economy/Trade.hpp"
#include "clc/economy/Ledger.hpp"
```

Recommended trade path:

```cpp
clc::economy::buy_resource_with_ledger(wallet, storage, price, quantity, ledger);
clc::economy::sell_resource_with_ledger(wallet, storage, price, quantity, ledger);
```

Market reports include registered supplied resources and registered demand-only resources. Lookup helpers should be preferred over manual scans when available.

---

## Persistence and diagnostics API

Common headers:

```cpp
#include "clc/sim/SimulationRuntimePersistence.hpp"
#include "clc/sim/SimulationRuntimePersistenceValidation.hpp"
#include "clc/sim/SimulationRuntimeEvents.hpp"
```

Main concepts:

- runtime save/load;
- world-state save/load;
- semantic persistence validation;
- deterministic replay diagnostics;
- event-log analysis.

Persistence compatibility and legacy behavior are documented in `docs/COMPATIBILITY.md` and `docs/MIGRATION.md`.

---

## Minimal C ABI

Header:

```c
#include "clc/c/CityLifeCoreC.h"
```

The 1.0.0 C ABI currently covers:

- core version;
- C interface version `3`;
- time constants and conversions;
- opaque `clc_world` create/destroy;
- world name/seed/current tick/event count;
- tick advancement;
- read-only world event id/tick/type/payload accessors.

It does not expose the full runtime, data registry, save/load, callbacks, caravans, contracts, economy workflows or mutable event payload APIs.

---

## Release notes and status

Current release notes:

```text
docs/RELEASE_NOTES_1.0.0.md
```

Current readiness snapshot:

```text
docs/READINESS_STATUS.md
```

Open release blockers:

```text
docs/RELEASE_BLOCKERS.md
```
