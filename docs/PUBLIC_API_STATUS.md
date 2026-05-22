# Public API Status / Статус публичного API

Status: **draft for 1.0.0-rc1 / черновик для 1.0.0-rc1**

All headers under `include/clc/` are installed, but they are not all equally stable. This table classifies the current 0.9.9 public header surface before the 1.0.0 API freeze.

Все headers внутри `include/clc/` устанавливаются, но не все имеют одинаковый уровень стабильности. Эта таблица классифицирует текущую public header surface версии 0.9.9 перед freeze API для 1.0.0.

---

## Status legend / Легенда

| Status | Meaning |
| --- | --- |
| `stable-candidate` | Expected to be stable for 1.0.0 after final audit. |
| `experimental` | Public but may change before/after 1.0 unless explicitly promoted. |
| `diagnostics` | Public observability/testing/validation API. Compatibility is narrower and schema-driven. |
| `legacy` | Kept for compatibility; not preferred for new code. |
| `internal-risk` | Installed today, but should not be treated as stable SDK surface until reviewed. |

---

## Header table / Таблица headers

| Header | Status | Intended users | Notes |
| --- | --- | --- | --- |
| `clc/core/Version.hpp` | `stable-candidate` | all SDK users | Version API must stay small and stable. |
| `clc/core/Ids.hpp` | `stable-candidate` | SDK users needing typed IDs | Freeze naming before 1.0. |
| `clc/core/Result.hpp` | `stable-candidate` | low-level SDK users | Error/result conventions should be stable. |
| `clc/core/Time.hpp` | `stable-candidate` | runtime/game/server code | Tick scale must be frozen before 1.0. |
| `clc/core/EventLog.hpp` | `stable-candidate` | diagnostics/runtime users | Event storage API stable candidate; event schemas documented separately. |
| `clc/core/World.hpp` | `legacy` | simple bootstrap users | Lower-level than runtime; not preferred for full simulation integration. |
| `clc/data/Definitions.hpp` | `stable-candidate` | data/model authors | Data schema compatibility still needs final policy. |
| `clc/data/Validation.hpp` | `stable-candidate` | all SDK users | Validation report convention should be stable. |
| `clc/data/DataRegistry.hpp` | `stable-candidate` | data/runtime users | Registry lookup should remain stable. |
| `clc/data/DataPackLoader.hpp` | `experimental` | tool/data-pack users | Data pack schema is not frozen yet. |
| `clc/economy/Market.hpp` | `stable-candidate` | economy users | Demand-only resources and semantics still need hardening. |
| `clc/economy/Trade.hpp` | `stable-candidate` | economy users | Abstract-market model should be documented as non-conservation by default. |
| `clc/economy/Ledger.hpp` | `stable-candidate` | economy/runtime users | Sequence/restore validation needs hardening before 1.0. |
| `clc/economy/Orders.hpp` | `experimental` | market/order users | Decide whether order matching is part of 1.0 MVP. |
| `clc/sim/Storage.hpp` | `stable-candidate` | simulation/runtime users | Transfer rollback/exception safety needs hardening. |
| `clc/sim/Settlement.hpp` | `stable-candidate` | simulation/runtime users | Tick scaling overflow checks needed before final. |
| `clc/sim/SimulationEngine.hpp` | `stable-candidate` | advanced simulation users | Runtime is preferred integration layer. |
| `clc/sim/ScenarioCatalog.hpp` | `experimental` | test/demo/scenario users | Looks like helper/demo layer, not core 1.0 surface. |
| `clc/sim/Routes.hpp` | `stable-candidate` | runtime/world users | Day/tick dual fields must be frozen. |
| `clc/sim/Caravans.hpp` | `stable-candidate` | runtime/world users | `arrived` means arrived after advance; event arrival requires elapsed ticks > 0. |
| `clc/sim/Factions.hpp` | `stable-candidate` | runtime/world users | Stable candidate. |
| `clc/sim/Ownership.hpp` | `stable-candidate` | runtime/world users | Stable candidate; pointer lifetime rules apply. |
| `clc/sim/Contracts.hpp` | `stable-candidate` | runtime/world users | `due_ticks` model must be frozen. |
| `clc/sim/SimulationRuntime.hpp` | `stable-candidate` | primary runtime integrators | Mutable data-bag decision must be accepted or facade introduced. |
| `clc/sim/SimulationRuntimeScenario.hpp` | `stable-candidate` | SDK examples/tests/quickstart | Basic bootstrap API useful but should not be the only integration path. |
| `clc/sim/SimulationRuntimeWorkflow.hpp` | `stable-candidate` | primary runtime integrators | Rollback/transaction safety needs hardening before final. |
| `clc/sim/SimulationRuntimeTick.hpp` | `experimental` | real-time/MMO runtime users | New in 0.9.9; either freeze for 1.0 or keep experimental. |
| `clc/sim/SimulationRuntimeEvents.hpp` | `diagnostics` | diagnostics/replay/backend users | Event names/payload schemas must be frozen separately. |
| `clc/sim/SimulationPersistence.hpp` | `internal-risk` | persistence/tooling users | Raw world-state bridge is powerful but too broad for stable SDK until reviewed. |
| `clc/sim/SimulationRuntimePersistenceValidation.hpp` | `diagnostics` | tests/auditors/backend users | Useful for save/load/replay validation; stable behavior should be documented. |

---

## Required decisions before 1.0.0

- Promote or keep experimental: `SimulationRuntimeTick.hpp`.
- Decide whether `SimulationPersistence.hpp` remains installed stable API or moves to diagnostics/internal-risk documentation.
- Decide whether `ScenarioCatalog.hpp` is part of 1.0 SDK.
- Decide whether `Orders.hpp` is part of 1.0 economy MVP.
- Accept public mutable `SimulationRuntime` as stable or introduce a facade.
- Document pointer/reference invalidation rules in `PUBLIC_API.md`.
- Freeze event payload schemas.
- Freeze persistence compatibility policy.
- Fix documented-but-missing `ContractRewards.hpp` mismatch before release docs freeze.

---

## Notes for SDK users / Заметки для пользователей SDK

For 0.9.9 audit, prefer these headers:

```cpp
#include "clc/core/Time.hpp"
#include "clc/data/DataRegistry.hpp"
#include "clc/sim/SimulationRuntimeScenario.hpp"
#include "clc/sim/SimulationRuntimeWorkflow.hpp"
#include "clc/sim/SimulationRuntimeTick.hpp"
#include "clc/sim/SimulationRuntimeEvents.hpp"
#include "clc/sim/SimulationRuntimePersistenceValidation.hpp"
```

Для 0.9.9 audit лучше начинать с runtime-level API и не завязываться на raw persistence bridge без необходимости.
