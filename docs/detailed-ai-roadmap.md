# City Life Core — Detailed AI Roadmap v1.1.0 → v4.0.0

Версия документа: **2**

Назначение документа: дать владельцу проекта готовый, подробный и безопасный план развития City Life Core по версиям, чтобы можно было отправлять ИИ-разработчику задачу в формате:

> “Работаем в ветке `vX.X.X`. Реализуй список этой версии. В `main` не заливай, пока версия не пройдёт локальную проверку.”

---

# 0. Почему теперь есть v2.1.0, v2.2.0, v3.1.0 и т.д.

Предыдущий план слишком резко переходил:

```text
v1.6.0 -> v2.0.0 -> v3.0.0 -> v4.0.0
```

Это неправильно для работы с ИИ, потому что `v2.0.0`, `v3.0.0` и `v4.0.0` становятся слишком огромными задачами.

Правильнее так:

```text
v1.x — SDK/integration/server-foundation линия
v2.x — multiplayer линия
v3.x — MMO/large-world линия
v4.x — multi-game platform линия
```

Каждый major-релиз открывает большую архитектурную ступень, а minor-версии внутри неё доводят эту ступень до нормального состояния.

---

# 1. Главные правила проекта

## 1.1. Правило веток

Ветки версий называются только так:

```text
vX.X.X
```

Правильно:

```text
v1.1.0
v1.2.0
v1.3.0
v2.0.0
v2.1.0
v3.0.0
v3.1.0
v4.0.0
```

Запрещено:

```text
feature/...
release/...
v/1.1.0
unity-support
browser-wasm
1.1.0
```

ИИ не должен создавать новые ветки без прямого разрешения владельца проекта.

---

## 1.2. Правило main

`main` — только стабильная проверенная версия.

ИИ не должен сам заливать в `main`.

Перед merge в `main` обязательна локальная проверка:

```text
- cmake configure проходит;
- cmake build проходит;
- ctest проходит;
- затронутые examples проверены;
- docs обновлены;
- release notes обновлены;
- release manifest обновлён;
- compatibility/migration notes добавлены, если нужны;
- нет unrelated refactor;
- нет скрытой смены архитектуры.
```

GitHub checks / GitHub Actions сейчас не считаются обязательными, если они не работают.

---

## 1.3. Архитектурное правило

City Life Core — это:

```text
headless C++20 simulation SDK
```

Это не:

```text
- готовая игра;
- UI framework;
- renderer;
- Unity framework;
- browser framework;
- network framework;
- готовый MMO-server из коробки;
- визуальный редактор;
- аккаунт-сервис.
```

Unity, C#, Browser, WebAssembly, серверы, редакторы и инструменты — это потребители ядра.

---

# 2. Универсальный промт для ИИ

```text
Ты работаешь в репозитории City_Life_Core.

Активная ветка: vX.X.X.

Главное правило веток:
- Не создавай новые ветки.
- Не создавай feature/*.
- Не создавай release/*.
- Не создавай v/*.
- Не создавай ветки с описательными названиями.
- Все версии проекта называются строго vX.X.X.
- Вся работа этой версии ведётся только в текущей ветке vX.X.X.
- В main ничего не заливать до финального разрешения владельца проекта.

Контекст проекта:
City Life Core — headless C++20 simulation SDK.
Это не игра, не UI framework, не renderer и не network framework.
Ядро должно оставаться engine-agnostic и usable для игр, серверов, редакторов, backend-сервисов и инструментов.

Цель версии:
[цель версии]

Нужно реализовать:
[список задач версии]

Не делать:
[список запретов версии]

Перед завершением:
- выполнить локальный CMake configure/build;
- выполнить локальный ctest;
- проверить затронутые examples;
- обновить docs;
- обновить release notes;
- обновить release manifest;
- описать public API changes;
- описать compatibility/migration notes, если нужны;
- убедиться, что нет unrelated refactor;
- подготовить ветку vX.X.X к merge в main, но не мержить без разрешения владельца.
```

---

# 3. Линия v1.x — SDK, integrations, tooling, server foundation

Главная цель v1.x: сделать стабильный, удобный, документированный и расширяемый SDK, который можно подключать к внешним играм, Unity/C#, browser tooling, серверным backend-слоям и будущему multiplayer.

---

# v1.1.0 — Integration Foundation Update

## Цель

Первый официальный фундамент для C# / Unity и будущего Browser/WASM через C ABI.

## Реализовать

```text
1. Обновить статус v1.1.0 в README/docs.
2. Добавить docs/release-plan-1.1.0.md.
3. Добавить docs/release-notes-1.1.0.md.
4. Добавить docs/release-manifest-1.1.0.md.
5. Довести examples/csharp_unity/CityLifeCoreNative.cs.
6. Добавить examples/csharp_unity/README.md.
7. Добавить Unity-style smoke example:
   - create world;
   - print version;
   - advance ticks;
   - read events;
   - Dispose.
8. Усилить C ABI safety:
   - null handle tests;
   - invalid index tests;
   - destroy null tests;
   - no exception across C ABI.
9. Обновить docs/c-abi.md.
10. Обновить docs/c-abi-expansion-plan.md.
11. Обновить docs/csharp-unity.md.
12. Довести docs/browser-wasm.md как planning/feasibility roadmap.
13. Обновить docs/README.md.
14. Обновить docs/public-api-status.md.
15. Обновить docs/compatibility.md.
```

## Не делать

```text
- полноценный Unity package;
- полноценный WASM runtime;
- browser game;
- multiplayer;
- MMO;
- network layer;
- UI;
- renderer;
- unrelated refactor.
```

## Готово, когда

```text
- CMake build проходит;
- ctest проходит;
- C ABI tests проходят;
- C# wrapper синтаксически корректен;
- docs честно разделяют implemented/planned/future;
- release docs готовы;
- ветка готова к merge в main.
```

---

# v1.2.0 — Action Bridge Update

## Цель

Добавить универсальный локальный слой действий:

```text
external action -> validation -> runtime mutation -> result/events
```

Это подготовка к server-authoritative логике, но ещё не multiplayer.

## Реализовать

```text
1. Action model:
   - action id/type;
   - actor id, если уместно;
   - target id;
   - payload;
   - validation status.
2. JSON action input format.
3. JSON action result format.
4. Stable action error codes.
5. Runtime command dispatcher:
   - validate action;
   - apply action;
   - reject invalid action without mutation;
   - return result.
6. Produced events / diagnostics output.
7. Tests:
   - valid action;
   - invalid action type;
   - malformed JSON;
   - missing fields;
   - rejected action no mutation;
   - deterministic result.
8. C++ example for action bridge.
9. docs/action-bridge.md.
10. docs/release-plan-1.2.0.md.
11. docs/release-notes-1.2.0.md.
12. docs/release-manifest-1.2.0.md.
```

## Не делать

```text
- HTTP API;
- WebSocket;
- accounts;
- auth;
- matchmaking;
- multiplayer;
- MMO;
- UI.
```

## Готово, когда

```text
- action tests проходят;
- invalid action не мутирует runtime;
- action result format документирован;
- docs/release files готовы.
```

---

# v1.3.0 — Data Registry & Tooling Update

## Цель

Сделать data registry удобным для инструментов, редакторов, C# / Unity и будущих browser tools.

## Реализовать

```text
1. Structured validation reports.
2. Validation codes.
3. Severity levels:
   - info;
   - warning;
   - error;
   - fatal.
4. Errors/warnings with path/context/source when possible.
5. Read-only registry query API:
   - resources;
   - currencies;
   - buildings;
   - professions;
   - settlements;
   - routes;
   - factions;
   - contracts/templates, если стабильно.
6. Valid data pack example.
7. Invalid data pack example.
8. Tooling report example.
9. Tests:
   - valid data;
   - invalid data;
   - warning-only;
   - fatal validation;
   - deterministic diagnostic order, if required.
10. docs/data-registry-tooling.md.
11. docs/release-plan-1.3.0.md.
12. docs/release-notes-1.3.0.md.
13. docs/release-manifest-1.3.0.md.
```

## Не делать

```text
- visual editor;
- Unity editor plugin;
- browser editor;
- UI;
- codegen game creator;
- network layer.
```

## Готово, когда

```text
- validation tests проходят;
- examples понятные;
- diagnostics usable for tools;
- docs готовы.
```

---

# v1.4.0 — Persistence & Migration Update

## Цель

Укрепить save/load, миграции и совместимость сохранений до перехода к multiplayer.

## Реализовать

```text
1. Explicit save schema version.
2. Unsupported save version diagnostics.
3. Migration result:
   - success/failure;
   - from version;
   - to version;
   - warnings;
   - errors.
4. Save validation:
   - missing fields;
   - invalid ids;
   - corrupted values;
   - impossible runtime state;
   - incompatible version.
5. Replay from save.
6. Replay mismatch diagnostics.
7. Tests:
   - valid save/load;
   - corrupted save;
   - unsupported version;
   - migration success;
   - migration failure;
   - replay from save.
8. docs/save-format-versioning.md.
9. docs/persistence-migration.md.
10. docs/release-plan-1.4.0.md.
11. docs/release-notes-1.4.0.md.
12. docs/release-manifest-1.4.0.md.
```

## Не делать

```text
- cloud saves;
- database backend;
- multiplayer persistence;
- accounts;
- UI.
```

## Готово, когда

```text
- persistence tests проходят;
- corrupted saves не приводят к silent failure;
- migration behavior документирован;
- compatibility notes готовы.
```

---

# v1.5.0 — Scenario & Balancing Update

## Цель

Дать разработчику инструменты для проверки экономики, симуляции и баланса.

## Реализовать

```text
1. Batch scenario runner.
2. Scenario presets.
3. Repeatable seed behavior.
4. Deterministic scenario summaries.
5. Balance reports:
   - resource bottleneck;
   - settlement growth;
   - production/consumption;
   - market pressure;
   - trade route efficiency, если система готова.
6. Economy stress scenarios:
   - shortage;
   - overproduction;
   - route disruption;
   - market imbalance;
   - long-run simulation.
7. Optional CLI/tool example for batch simulation.
8. Tests:
   - deterministic batch results;
   - scenario preset validation;
   - report correctness;
   - long-run no-crash;
   - economy invariants.
9. docs/scenario-balancing.md.
10. docs/release-plan-1.5.0.md.
11. docs/release-notes-1.5.0.md.
12. docs/release-manifest-1.5.0.md.
```

## Не делать

```text
- visual editor;
- charts inside core;
- web dashboard;
- ML/AI balancing;
- network layer.
```

## Готово, когда

```text
- scenario/balance tests проходят;
- reports stable and documented;
- examples проверены;
- release docs готовы.
```

---

# v1.6.0 — Server Runtime Foundation

## Цель

Подготовить локальную server-authoritative foundation, но ещё без реального multiplayer.

## Реализовать

```text
1. Headless runtime loop abstraction.
2. Tick scheduler abstraction:
   - fixed tick step;
   - manual advance;
   - deterministic behavior.
3. Local command queue:
   - deterministic order;
   - rejected commands;
   - diagnostics.
4. Snapshot export.
5. Event stream export:
   - events since tick;
   - events by type;
   - bounded reads.
6. Server runtime example:
   - create runtime;
   - enqueue commands;
   - run ticks;
   - print snapshot summary;
   - print event summary.
7. Tests:
   - deterministic command processing;
   - queue ordering;
   - rejected command no mutation;
   - snapshot consistency;
   - event stream since tick.
8. docs/server-runtime-foundation.md.
9. docs/release-plan-1.6.0.md.
10. docs/release-notes-1.6.0.md.
11. docs/release-manifest-1.6.0.md.
```

## Не делать

```text
- HTTP;
- WebSocket;
- TCP/UDP server;
- accounts;
- auth;
- matchmaking;
- multiplayer sessions;
- MMO shards.
```

## Готово, когда

```text
- server runtime tests проходят;
- server example работает локально;
- no network dependencies in core;
- release docs готовы.
```

---

# v1.7.0 — SDK Hardening & Public API Cleanup

## Цель

Перед v2.0.0 привести SDK к более чистому и устойчивому состоянию.

## Реализовать

```text
1. Public API audit.
2. Header-by-header API classification.
3. Убрать или пометить experimental опасные API.
4. Уточнить stable/experimental/internal boundaries.
5. Улучшить error/result naming consistency.
6. Проверить include hygiene.
7. Проверить CMake install/export layout.
8. Проверить docs consistency:
   - README;
   - docs/README;
   - public-api;
   - compatibility;
   - migration.
9. Добавить/обновить API stability tests where possible.
10. docs/sdk-hardening.md.
11. docs/release-plan-1.7.0.md.
12. docs/release-notes-1.7.0.md.
13. docs/release-manifest-1.7.0.md.
```

## Не делать

```text
- новые крупные gameplay systems;
- multiplayer;
- network;
- MMO;
- unrelated refactor.
```

## Готово, когда

```text
- public API boundaries понятны;
- docs не противоречат друг другу;
- build/install/package не сломаны;
- release docs готовы.
```

---

# v1.8.0 — Pre-2.0 Compatibility Gate

## Цель

Последняя подготовительная версия перед `v2.0.0`.

## Реализовать

```text
1. Full compatibility review for v1.x.
2. Identify all breaking changes needed for v2.0.0.
3. Create docs/migration-plan-v1-to-v2.md.
4. Add deprecation notes for APIs that will change in v2.0.0.
5. Add pre-v2 validation checklist.
6. Lock v1.x compatibility expectations.
7. Verify:
   - action bridge;
   - persistence;
   - C ABI;
   - server runtime foundation;
   - examples;
   - release docs.
8. Add docs/release-plan-1.8.0.md.
9. Add docs/release-notes-1.8.0.md.
10. Add docs/release-manifest-1.8.0.md.
```

## Не делать

```text
- начинать полноценный multiplayer внутри v1.x;
- network layer;
- MMO;
- huge rewrites.
```

## Готово, когда

```text
- v2.0.0 migration plan готов;
- v1.x API/deprecations понятны;
- локальные проверки проходят;
- ветка готова к main.
```

---

# 4. Линия v2.x — Multiplayer-ready Core

Главная цель v2.x: сделать ядро пригодным для server-authoritative multiplayer, но без привязки к конкретной сети, аккаунтам или UI.

---

# v2.0.0 — Multiplayer Foundation

## Цель

Открыть multiplayer-ready архитектурную линию.

## Реализовать

```text
1. Player/session identity model.
2. Transport-agnostic session concept.
3. Ownership model:
   - player ownership;
   - faction ownership;
   - shared ownership, если нужно.
4. Permission checks for actions.
5. Multiplayer-safe action validation.
6. Deterministic conflict handling.
7. Server-side snapshot model.
8. Client-visible event filtering.
9. Rejected action audit.
10. Tests:
   - accepted player action;
   - rejected permission;
   - deterministic conflict;
   - filtered events;
   - snapshot metadata.
11. docs/multiplayer-core.md.
12. docs/migration-v1-to-v2.md.
13. docs/release-plan-2.0.0.md.
14. docs/release-notes-2.0.0.md.
15. docs/release-manifest-2.0.0.md.
```

## Не делать

```text
- HTTP/WebSocket/TCP server;
- accounts;
- passwords;
- auth;
- matchmaking;
- lobby;
- MMO shards;
- UI.
```

## Готово, когда

```text
- multiplayer foundation tests проходят;
- breaking changes описаны;
- migration guide готов.
```

---

# v2.1.0 — Multiplayer Action Authority

## Цель

Укрепить server-authoritative обработку действий игроков.

## Реализовать

```text
1. Actor-scoped action validation.
2. Action authority rules:
   - who can perform;
   - when can perform;
   - target ownership;
   - resource permission.
3. Action rejection reasons:
   - unauthorized;
   - invalid target;
   - stale target;
   - insufficient resource;
   - conflict.
4. Audit trail for player actions.
5. Deterministic ordering policy.
6. Tests:
   - same tick multiple actions;
   - rejected unauthorized action;
   - stale target action;
   - replay action sequence.
7. docs/multiplayer-action-authority.md.
8. docs/release-plan-2.1.0.md.
9. docs/release-notes-2.1.0.md.
10. docs/release-manifest-2.1.0.md.
```

## Не делать

```text
- real networking;
- anti-cheat client software;
- account service;
- ban system;
- UI.
```

## Готово, когда

```text
- all action authority cases tested;
- rejected actions never mutate state;
- audit trail documented.
```

---

# v2.2.0 — Snapshot & Client Sync Model

## Цель

Сделать стабильную модель snapshots и client-visible state для будущего сетевого слоя.

## Реализовать

```text
1. Full snapshot format.
2. Filtered snapshot format.
3. Snapshot metadata:
   - tick;
   - version;
   - region/scope if available;
   - visibility scope.
4. Delta/diff planning or minimal implementation, если безопасно.
5. Snapshot validation.
6. Client-visible state rules.
7. Tests:
   - full snapshot consistency;
   - filtered snapshot hides private data;
   - snapshot tick ordering;
   - invalid snapshot request diagnostics.
8. docs/snapshot-sync-model.md.
9. docs/release-plan-2.2.0.md.
10. docs/release-notes-2.2.0.md.
11. docs/release-manifest-2.2.0.md.
```

## Не делать

```text
- network replication protocol;
- WebSocket;
- client prediction;
- rollback netcode;
- UI.
```

## Готово, когда

```text
- snapshot format documented;
- visibility rules tested;
- future networking can consume snapshots.
```

---

# v2.3.0 — Multiplayer Persistence & Replay

## Цель

Укрепить сохранения, replay и audit для multiplayer-сценариев.

## Реализовать

```text
1. Player action replay.
2. Multiplayer command log format.
3. Replay validation:
   - same input -> same output;
   - rejected action replay;
   - conflict replay.
4. Save/load with player/session/ownership data.
5. Audit event export.
6. Tests:
   - save/load multiplayer state;
   - replay player actions;
   - replay rejected actions;
   - replay conflict scenario.
7. docs/multiplayer-persistence-replay.md.
8. docs/release-plan-2.3.0.md.
9. docs/release-notes-2.3.0.md.
10. docs/release-manifest-2.3.0.md.
```

## Не делать

```text
- database backend;
- cloud saves;
- real server storage;
- accounts;
- moderation tools UI.
```

## Готово, когда

```text
- multiplayer replay deterministic;
- audit logs useful;
- persistence compatibility documented.
```

---

# v2.4.0 — Multiplayer Economy & Trade Safety

## Цель

Сделать экономику безопасной для нескольких игроков.

## Реализовать

```text
1. Multiplayer-safe wallet operations.
2. Ledger ownership and audit.
3. Trade validation:
   - ownership;
   - sufficient balance;
   - resource availability;
   - deterministic commit.
4. Market action validation.
5. Contract action validation.
6. Tests:
   - valid player trade;
   - invalid ownership trade;
   - insufficient funds;
   - double-spend prevention;
   - ledger audit.
7. docs/multiplayer-economy.md.
8. docs/release-plan-2.4.0.md.
9. docs/release-notes-2.4.0.md.
10. docs/release-manifest-2.4.0.md.
```

## Не делать

```text
- real-money economy;
- payment/billing;
- marketplace service;
- anti-fraud service;
- UI.
```

## Готово, когда

```text
- economy operations deterministic and audited;
- double-spend-like cases covered;
- docs ready.
```

---

# v2.5.0 — Multiplayer Load & Diagnostics

## Цель

Проверить multiplayer core на нагрузку и диагностируемость.

## Реализовать

```text
1. Load scenarios:
   - many players;
   - many actions;
   - many rejected actions;
   - many snapshots;
   - long action replay.
2. Diagnostics:
   - action throughput;
   - rejection reasons;
   - snapshot cost;
   - event volume;
   - replay cost.
3. Optional benchmarks.
4. Regression tests for invariants.
5. docs/multiplayer-load-diagnostics.md.
6. docs/release-plan-2.5.0.md.
7. docs/release-notes-2.5.0.md.
8. docs/release-manifest-2.5.0.md.
```

## Не делать

```text
- promise exact player count;
- real networking benchmark;
- cloud scaling;
- MMO.
```

## Готово, когда

```text
- load scenarios run locally;
- diagnostics produce useful reports;
- no hidden performance regressions.
```

---

# v2.6.0 — Pre-3.0 Large World Preparation

## Цель

Подготовить v3.0.0 MMO/large-world line.

## Реализовать

```text
1. Review multiplayer assumptions.
2. Identify required world partition changes.
3. Identify event stream scaling changes.
4. Identify economy scaling changes.
5. Create docs/migration-plan-v2-to-v3.md.
6. Deprecation notes for APIs that will change in v3.0.0.
7. Pre-3.0 readiness checklist.
8. docs/release-plan-2.6.0.md.
9. docs/release-notes-2.6.0.md.
10. docs/release-manifest-2.6.0.md.
```

## Не делать

```text
- full MMO implementation;
- shards;
- cluster;
- cloud infra;
- database-specific architecture.
```

## Готово, когда

```text
- v3.0.0 scope is clear;
- migration plan ready;
- v2.x line is stable.
```

---

# 5. Линия v3.x — MMO-ready / Large World Core

Главная цель v3.x: большие долгоживущие миры, регионы, масштабируемая экономика, event partitions и audit diagnostics.

---

# v3.0.0 — Large World Foundation

## Цель

Открыть MMO/large-world архитектурную линию.

## Реализовать

```text
1. Region/world partition model.
2. Region metadata:
   - id;
   - scope/bounds;
   - tick metadata;
   - event partition metadata.
3. Long-running tick safety.
4. Periodic snapshot strategy.
5. Event compaction planning.
6. Tests:
   - many regions;
   - region tick metadata;
   - long tick ranges;
   - partition event reads.
7. docs/large-world-core.md.
8. docs/world-partitioning.md.
9. docs/migration-v2-to-v3.md.
10. docs/release-plan-3.0.0.md.
11. docs/release-notes-3.0.0.md.
12. docs/release-manifest-3.0.0.md.
```

## Не делать

```text
- cluster networking;
- real shards;
- database implementation;
- game client;
- MMO gameplay.
```

## Готово, когда

```text
- region model tested;
- long-running basics safe;
- migration guide ready.
```

---

# v3.1.0 — Regional Economy & Trade Networks

## Цель

Сделать экономику пригодной для больших региональных миров.

## Реализовать

```text
1. Regional market model.
2. Regional supply/demand.
3. Inter-region trade routes.
4. Market pressure diagnostics per region.
5. Regional ledger/audit.
6. Tests:
   - regional price differences;
   - inter-region trade;
   - route disruption;
   - economy invariants.
7. docs/regional-economy.md.
8. docs/release-plan-3.1.0.md.
9. docs/release-notes-3.1.0.md.
10. docs/release-manifest-3.1.0.md.
```

## Не делать

```text
- real-world economy simulation;
- payment systems;
- marketplace service;
- UI dashboards.
```

## Готово, когда

```text
- regional economy deterministic;
- diagnostics useful;
- tests pass.
```

---

# v3.2.0 — Faction, Guild & Influence Scaling

## Цель

Укрепить factions/guild-like systems для больших миров.

## Реализовать

```text
1. Scalable faction relations.
2. Region influence model.
3. Guild-like ownership foundation, если подходит архитектуре.
4. Faction contract scaling.
5. Reputation/influence diagnostics.
6. Tests:
   - many factions;
   - region influence changes;
   - faction ownership;
   - relation conflicts.
7. docs/faction-guild-scaling.md.
8. docs/release-plan-3.2.0.md.
9. docs/release-notes-3.2.0.md.
10. docs/release-manifest-3.2.0.md.
```

## Не делать

```text
- chat;
- social network;
- guild UI;
- moderation UI;
- real accounts.
```

## Готово, когда

```text
- faction scaling tested;
- influence model documented;
- no UI/social dependencies.
```

---

# v3.3.0 — Event Partitioning & Audit Streams

## Цель

Сделать события пригодными для больших миров и админ-аудита.

## Реализовать

```text
1. Event partitions:
   - by region;
   - by owner/visibility;
   - by type;
   - since tick.
2. Bounded event reads.
3. Event retention/compaction strategy.
4. Audit stream categories:
   - action audit;
   - economy audit;
   - ownership audit;
   - permission audit.
5. Tests:
   - partitioned event reads;
   - visibility filtering;
   - retention behavior;
   - audit event correctness.
6. docs/event-partitioning.md.
7. docs/audit-streams.md.
8. docs/release-plan-3.3.0.md.
9. docs/release-notes-3.3.0.md.
10. docs/release-manifest-3.3.0.md.
```

## Не делать

```text
- network streaming;
- Kafka/RabbitMQ dependency;
- database dependency;
- admin UI.
```

## Готово, когда

```text
- event partitions tested;
- audit categories documented;
- no external service dependency.
```

---

# v3.4.0 — Long-running Persistence & Maintenance

## Цель

Поддержать долгоживущие миры: snapshots, maintenance, compaction, migration.

## Реализовать

```text
1. Long-running snapshot strategy.
2. Save compaction planning or implementation.
3. Maintenance diagnostics.
4. World health report.
5. Region maintenance hooks.
6. Tests:
   - long-run save/load;
   - compacted event history;
   - maintenance report;
   - migration with large world.
7. docs/long-running-persistence.md.
8. docs/world-maintenance.md.
9. docs/release-plan-3.4.0.md.
10. docs/release-notes-3.4.0.md.
11. docs/release-manifest-3.4.0.md.
```

## Не делать

```text
- actual database engine;
- cloud backup;
- cron service;
- ops dashboard.
```

## Готово, когда

```text
- large-world persistence behavior documented;
- maintenance diagnostics available;
- tests pass.
```

---

# v3.5.0 — MMO Load, Benchmarks & Stability

## Цель

Проверить large-world/MMO-ready core под нагрузкой.

## Реализовать

```text
1. Optional benchmark suite:
   - many regions;
   - many settlements;
   - many factions;
   - many markets;
   - many events;
   - long-running economy.
2. Stability tests.
3. Performance diagnostics.
4. Memory/event volume reports, если доступно.
5. Regression thresholds, если безопасно.
6. docs/mmo-load-benchmarks.md.
7. docs/release-plan-3.5.0.md.
8. docs/release-notes-3.5.0.md.
9. docs/release-manifest-3.5.0.md.
```

## Не делать

```text
- обещать конкретное число игроков;
- cloud benchmark;
- cluster benchmark;
- production SLA.
```

## Готово, когда

```text
- benchmarks run optionally;
- stability scenarios pass;
- diagnostics useful.
```

---

# v3.6.0 — Pre-4.0 Platform Preparation

## Цель

Подготовить переход от large-world core к multi-game platform core.

## Реализовать

```text
1. Review all v3.x systems.
2. Identify module boundaries.
3. Identify genre profile needs.
4. Identify content pipeline needs.
5. Create docs/migration-plan-v3-to-v4.md.
6. Create docs/platform-architecture-plan.md.
7. Deprecation notes for v4.0.0.
8. Pre-4.0 readiness checklist.
9. docs/release-plan-3.6.0.md.
10. docs/release-notes-3.6.0.md.
11. docs/release-manifest-3.6.0.md.
```

## Не делать

```text
- full platform rewrite;
- plugin system implementation if not ready;
- editor UI;
- marketplace.
```

## Готово, когда

```text
- v4.0.0 scope is clear;
- migration plan ready;
- module boundaries planned.
```

---

# 6. Линия v4.x — Multi-Game Platform Core

Главная цель v4.x: сделать City Life Core не просто simulation SDK, а модульной платформенной основой для разных жанров игр.

---

# v4.0.0 — Multi-Game Platform Foundation

## Цель

Открыть platform core line: modules, genre profiles, content pipeline, editor/admin API и unified diagnostics.

## Реализовать

```text
1. Module architecture:
   - core modules;
   - optional modules;
   - genre modules;
   - module metadata;
   - dependency rules;
   - compatibility rules.
2. Static module registry first, если dynamic plugins опасны.
3. Genre profiles:
   - idle/tycoon;
   - city builder;
   - strategy;
   - settlement RPG;
   - survival economy.
4. Profile metadata:
   - enabled systems;
   - default rules;
   - expected data templates;
   - recommended action types;
   - recommended reports.
5. Content pipeline:
   - content pack metadata;
   - module requirements;
   - validation pipeline;
   - compatibility checks;
   - migration checks.
6. Editor/admin integration API:
   - query registry;
   - query runtime;
   - validate content;
   - run scenario;
   - produce reports;
   - inspect events/audit logs.
7. Unified diagnostics API:
   - validation diagnostics;
   - runtime diagnostics;
   - economy diagnostics;
   - action diagnostics;
   - replay diagnostics;
   - migration diagnostics.
8. Multi-game configuration profiles.
9. Examples:
   - profile_idle_tycoon;
   - profile_city_builder;
   - profile_strategy;
   - profile_settlement_rpg;
   - profile_survival_economy.
10. docs/platform-core.md.
11. docs/module-architecture.md.
12. docs/genre-profiles.md.
13. docs/content-pipeline.md.
14. docs/editor-admin-api.md.
15. docs/diagnostics-api.md.
16. docs/migration-v3-to-v4.md.
17. docs/release-plan-4.0.0.md.
18. docs/release-notes-4.0.0.md.
19. docs/release-manifest-4.0.0.md.
```

## Не делать

```text
- готовую игру;
- игровой клиент;
- UI;
- renderer;
- visual editor;
- web dashboard;
- marketplace;
- accounts;
- billing;
- привязку только к Unity;
- привязку только к browser;
- cloud-specific architecture.
```

## Готово, когда

```text
- module architecture tested;
- genre profiles work as small headless examples;
- content pipeline validates packs;
- diagnostics API documented;
- migration guide ready.
```

---

# v4.1.0 — Genre Profile Hardening

## Цель

Довести жанровые профили до практичного состояния.

## Реализовать

```text
1. Улучшить idle/tycoon profile.
2. Улучшить city builder profile.
3. Улучшить strategy profile.
4. Улучшить settlement RPG profile.
5. Улучшить survival economy profile.
6. Для каждого profile добавить:
   - validation rules;
   - default scenario;
   - balance report;
   - example content pack.
7. Tests for each profile.
8. docs/genre-profile-authoring.md.
9. docs/release-plan-4.1.0.md.
10. docs/release-notes-4.1.0.md.
11. docs/release-manifest-4.1.0.md.
```

## Не делать

```text
- готовую игру;
- UI;
- графику;
- Unity-specific gameplay;
- browser-specific gameplay.
```

## Готово, когда

```text
- каждый профиль запускается как headless scenario;
- reports useful;
- examples понятны.
```

---

# v4.2.0 — Editor/Admin API Expansion

## Цель

Расширить API для будущих внешних редакторов и админских инструментов.

## Реализовать

```text
1. Query API for:
   - modules;
   - profiles;
   - content packs;
   - runtime state;
   - diagnostics;
   - audit events.
2. Safe mutation API for tools, если допустимо:
   - validate before apply;
   - dry-run;
   - apply with result.
3. Editor-friendly diagnostics.
4. Admin-friendly audit summaries.
5. Tests:
   - query consistency;
   - dry-run no mutation;
   - apply mutation;
   - diagnostics output.
6. docs/editor-admin-api-expansion.md.
7. docs/release-plan-4.2.0.md.
8. docs/release-notes-4.2.0.md.
9. docs/release-manifest-4.2.0.md.
```

## Не делать

```text
- visual editor;
- web admin panel;
- auth service;
- user roles UI;
- database-specific admin backend.
```

## Готово, когда

```text
- external tools can inspect core state;
- dry-run/apply behavior tested;
- no UI dependency.
```

---

# v4.3.0 — Live Balancing & What-if Simulation

## Цель

Дать безопасные hooks для балансировки и what-if симуляций.

## Реализовать

```text
1. What-if scenario API.
2. Balance inspection:
   - resource rates;
   - production/consumption;
   - economy pressure;
   - faction pressure;
   - contract pressure.
3. Compare baseline vs modified scenario.
4. Dry-run changes.
5. Tests:
   - what-if no mutation;
   - baseline comparison;
   - balance report correctness.
6. docs/live-balancing.md.
7. docs/what-if-simulation.md.
8. docs/release-plan-4.3.0.md.
9. docs/release-notes-4.3.0.md.
10. docs/release-manifest-4.3.0.md.
```

## Не делать

```text
- automatic AI balancing as core requirement;
- UI graphs;
- web dashboard;
- production hotpatch system.
```

## Готово, когда

```text
- what-if simulation does not mutate main runtime;
- reports stable;
- docs ready.
```

---

# v4.4.0 — Long-term Compatibility & Platform Packaging

## Цель

Закрепить long-term compatibility policy и platform packaging.

## Реализовать

```text
1. Long-term compatibility policy:
   - source compatibility;
   - C ABI compatibility;
   - save compatibility;
   - content pack compatibility;
   - module compatibility;
   - profile compatibility.
2. Platform package layout.
3. Module/profile manifest validation.
4. Version compatibility matrix.
5. Release verification template for platform releases.
6. Tests for package layout and manifest validation.
7. docs/long-term-compatibility.md.
8. docs/platform-packaging.md.
9. docs/release-plan-4.4.0.md.
10. docs/release-notes-4.4.0.md.
11. docs/release-manifest-4.4.0.md.
```

## Не делать

```text
- package marketplace;
- online updater;
- license server;
- account/billing system.
```

## Готово, когда

```text
- compatibility policy clear;
- package layout validated;
- future platform releases have a checklist.
```

---

# 7. Краткая карта версий

```text
v1.1.0 — Integration Foundation
C# / Unity foundation, Browser/WASM planning, C ABI safety.

v1.2.0 — Action Bridge
JSON/action model, command dispatcher, result format.

v1.3.0 — Data Registry & Tooling
Validation reports, tooling-friendly query API, data pack diagnostics.

v1.4.0 — Persistence & Migration
Save schema versioning, migration, corrupted save diagnostics, replay from save.

v1.5.0 — Scenario & Balancing
Batch scenarios, balance reports, economy stress tests.

v1.6.0 — Server Runtime Foundation
Headless runtime loop, command queue, tick scheduler, snapshot/event export.

v1.7.0 — SDK Hardening
Public API cleanup, docs consistency, install/package hardening.

v1.8.0 — Pre-2.0 Compatibility Gate
v2 migration plan, deprecations, compatibility review.

v2.0.0 — Multiplayer Foundation
Player/session model, ownership, permissions, multiplayer-safe actions.

v2.1.0 — Multiplayer Action Authority
Actor-scoped validation, action authority, audit trail.

v2.2.0 — Snapshot & Client Sync Model
Full/filtered snapshots, visibility rules, metadata.

v2.3.0 — Multiplayer Persistence & Replay
Command logs, player action replay, multiplayer save/load.

v2.4.0 — Multiplayer Economy & Trade Safety
Wallet/ledger/trade safety, double-spend prevention.

v2.5.0 — Multiplayer Load & Diagnostics
Load scenarios, rejection diagnostics, snapshot/event volume.

v2.6.0 — Pre-3.0 Large World Preparation
Migration plan, region/event/economy scaling plan.

v3.0.0 — Large World Foundation
Regions, world partitioning, long-running tick safety.

v3.1.0 — Regional Economy
Regional markets, inter-region trade, economy pressure.

v3.2.0 — Faction/Guild Scaling
Influence, relations, faction ownership at scale.

v3.3.0 — Event Partitioning & Audit Streams
Partitioned events, bounded reads, audit streams.

v3.4.0 — Long-running Persistence
Large-world save/load, maintenance, compaction strategy.

v3.5.0 — MMO Load & Stability
Optional benchmarks, stress scenarios, stability diagnostics.

v3.6.0 — Pre-4.0 Platform Preparation
Module boundaries, genre profiles plan, migration plan.

v4.0.0 — Multi-Game Platform Foundation
Modules, genre profiles, content pipeline, editor/admin API, diagnostics.

v4.1.0 — Genre Profile Hardening
Improve genre profiles, examples, reports.

v4.2.0 — Editor/Admin API Expansion
Tooling query API, dry-run/apply, admin diagnostics.

v4.3.0 — Live Balancing & What-if Simulation
What-if scenario API, balancing reports, dry-run changes.

v4.4.0 — Long-term Compatibility & Platform Packaging
Compatibility policy, package layout, manifest validation.
```

---

# 8. Минимальная команда владельца проекта для ИИ

```text
Работаем в City_Life_Core.

Активная ветка: vX.X.X.

Используй план версии vX.X.X из roadmap.
Новые ветки не создавай.
В main не заливай.
GitHub checks не являются обязательными, если они не работают.

Главная проверка локально:
- cmake configure;
- cmake build;
- ctest;
- affected examples;
- docs;
- release notes;
- release manifest.

Реализуй только scope этой версии.
Не добавляй unrelated refactor.
Не превращай core в Unity/browser/network/UI framework.

После работы дай:
1. что реализовано;
2. какие файлы изменены;
3. какие тесты добавлены;
4. какие локальные проверки прошли;
5. что осталось future work;
6. готова ли ветка к merge в main.
```
