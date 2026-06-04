# City Life Core — Deep Realistic Simulation Roadmap Addendum

Документ: **draft / owner-approved decisions 1–18**  
Дата: **2026-06-02**  
Назначение: обновить стратегию развития City Life Core после v1.4.0 так, чтобы глубокая реалистичная обработка симуляции началась уже в линии v1.x, а не откладывалась до v3/v4.

> Важно: этот файл подготовлен как отдельный документ для владельца проекта.  
> На GitHub ничего не изменялось.

---

# 0. Принятая рамка проекта

City Life Core — это не игра.

City Life Core — это:

```text
headless C++20 simulation SDK
```

Ядро должно обрабатывать симуляционные системы для внешних игр, серверов, редакторов, backend-сервисов, балансировочных инструментов и будущих MMO-like runtime layers.

Ядро не должно становиться:

```text
- готовой игрой;
- UI framework;
- renderer;
- Unity-only SDK;
- Browser-only SDK;
- network framework;
- account/auth/matchmaking service;
- engine-specific gameplay framework.
```

Правильная формулировка направления:

```text
Не "добавляем игровые механики".
А "создаём глубокие реалистичные simulation processors".
```

Разработчик игры подключает ядро и решает:

```text
- какие системы включить;
- какие системы выключить;
- какие зависимости оставить;
- какие зависимости отключить;
- какие коэффициенты изменить;
- какие свои данные добавить;
- как визуализировать результат;
- как превратить обработанное состояние симуляции в gameplay.
```

---

# 1. Принятые решения владельца

## 1.1. Roadmap strategy

Принято:

```text
v1.x — deep realistic simulation processing core
v2.x — polish, authority, consistency, multiplayer-safety, compatibility, advanced preparation
v3.x — large-world / regional / MMO scaling
v4.x — platform, profiles, tooling, long-term ecosystem
```

Смысл:

```text
До v2.0.0 ядро уже должно стать глубоким реалистичным обработчиком симуляции.
v2.x не должен добавлять базовую глубину с нуля.
v2.x должен полировать, стабилизировать, делать authority-safe и готовить v3.
v3.x должен масштабировать уже готовые богатые системы на регионы, большие миры и MMO-like flows.
```

---

## 1.2. Release policy

Принято:

```text
Публичные релизы / Git tags:
- v1.0.0
- v2.0.0
- v3.0.0
- v4.0.0
- и далее major milestones
```

Internal milestones:

```text
v1.1.0, v1.2.0, v1.3.0, v1.4.0, v1.5.0 ...
```

не являются публичными релизами.

Но для каждого internal milestone нужно делать:

```text
internal milestone acceptance note
```

Это не public release manifest, а внутреннее подтверждение:

```text
- что было сделано;
- какие проверки прошли;
- какие документы обновлены;
- какие ограничения остались;
- готова ли ветка к merge в main;
- какие риски переносятся дальше.
```

---

## 1.3. Branch naming rule

Принято жёсткое правило:

```text
Ветки создаются на GitHub исключительно как:
vX.X.X
```

Разрешено:

```text
v1.5.0
v1.6.0
v2.0.0
v2.1.0
v3.0.0
```

Запрещено:

```text
feature/...
release/...
v/1.5.0
deep-simulation
processor-architecture
weather-system
market-engine
```

Вся работа конкретного internal milestone идёт только в ветке этого milestone.

---

## 1.4. Historical v1.1–v1.4

Принято:

```text
v1.1.0–v1.4.0 считаются исторически выполненными internal milestones.
```

Их не надо переписывать задним числом как новую deep simulation line.

С v1.5.0 начинается новая линия:

```text
Deep Realistic Simulation Processing Core
```

---

## 1.5. Simulation Processor Layer

Принято добавить официальный архитектурный слой:

```text
Simulation Processor Layer
```

Смысл слоя:

```text
processor input:
- current state;
- feature config;
- dependency config;
- rules;
- effects;
- modifiers;
- commands/actions;
- deterministic context;

processor output:
- state delta;
- events;
- diagnostics;
- warnings/errors;
- cause breakdown;
- replay data where needed.
```

Simulation Processor не должен:

```text
- обращаться к UI;
- обращаться к renderer;
- обращаться к network;
- зависеть от Unity/browser;
- использовать недетерминированное поведение;
- скрыто менять unrelated state;
- быть жёстко завязанным на одну конкретную игру.
```

---

## 1.6. Optional/configurable systems

Принято:

```text
Все новые simulation systems должны быть optional, configurable и dependency-toggleable.
```

Любая новая система должна быть:

```text
1. Headless.
2. Deterministic.
3. Optional.
4. Configurable.
5. Feature-toggle compatible.
6. Dependency-toggle compatible.
7. Save/load aware.
8. Replay-aware.
9. Diagnostics-friendly.
10. Test-covered.
11. Documented.
12. Engine-agnostic.
```

---

## 1.7. Wording policy

Принято заменить рискованные формулировки:

```text
gameplay mechanics
```

на более точные:

```text
simulation systems
simulation processors
domain processing systems
realistic simulation models
```

Слово `gameplay` можно использовать только когда речь о downstream games, а не о native core responsibilities.

---

## 1.8. Simulation processing doctrine

Принято добавить документ:

```text
docs/simulation-processing-doctrine.md
```

Он должен объяснять:

```text
- ядро не игра;
- ядро обрабатывает системы;
- processors производят state deltas/events/diagnostics;
- downstream product owns UI/gameplay/presentation;
- все зависимости должны быть toggleable;
- realism over arcade defaults;
- no hidden coupling;
- deterministic first;
- diagnostics first.
```

---

## 1.9. Economy direction

Принято:

```text
Экономика должна быть глубокой, с liquidity и volatility.
Ядро должно быть больше реалистичным.
```

Market Engine должен включать:

```text
- supply;
- demand;
- price pressure;
- liquidity;
- volatility;
- market depth;
- shocks;
- stabilization;
- cause breakdown;
- safety clamps.
```

---

## 1.10. Market mode

Принято:

```text
B + C
```

То есть:

```text
- есть simple market mode;
- есть realistic/deep market mode;
- liquidity и volatility могут быть отдельными toggles;
- realistic preset включает liquidity/volatility;
- small/simple projects могут отключить deep market behavior.
```

Пример:

```text
market.mode = realistic
market.liquidity.enabled = true
market.volatility.enabled = true
market.shocks.enabled = true
```

или:

```text
market.mode = simple
market.liquidity.enabled = false
market.volatility.enabled = false
```

---

## 1.11. Default realism

Принято:

```text
B — realistic defaults, simplification optional.
```

То есть по умолчанию ядро должно быть ближе к реалистичной системной симуляции, но разработчик должен иметь возможность упростить поведение через presets/config.

---

## 1.12. Data config strategy

Принято:

```text
C — .clcd для простого, JSON/YAML для сложного.
```

Предложенная стратегия:

```text
.clcd:
- resources;
- currencies;
- professions;
- simple buildings;
- settlements;
- simple routes.

JSON/YAML:
- rules;
- effects;
- modifiers;
- dependency graph;
- feature config;
- market models;
- scenario presets;
- event chains;
- policies;
- complex content packs.
```

---

## 1.13. Versioned schemas

Принято:

```text
Да, нужны versioned schemas.
```

Нужно ввести отдельные версии для:

```text
- data_pack_schema_version;
- rules_schema_version;
- effects_schema_version;
- dependency_schema_version;
- market_schema_version;
- scenario_schema_version;
- save_schema_version;
- replay_schema_version.
```

---

## 1.14. Customization strategy

Принято:

```text
B сейчас, C позже.
```

В v1.x:

```text
- custom data;
- custom resources;
- custom buildings;
- custom markets;
- custom rules;
- custom effects;
- custom events;
- custom configs;
```

Ближе к v2/v3 можно проектировать:

```text
- custom processors through C++ extension API;
- processor plugin/extension boundary;
- registered custom processing stages.
```

Но dynamic plugins и scripting runtime не добавлять преждевременно.

---

## 1.15. Action Bridge expansion strategy

Принято:

```text
C — v1.x local commands, v2.x authority/permissions.
```

В v1.x Action Bridge может расширяться как local tooling/server command layer.

В v2.x добавляются:

```text
- actor/session concepts;
- authority;
- ownership;
- permissions;
- rejected action audit;
- filtered visibility.
```

---

## 1.16. Action Bridge commands in v1.x

Принято:

```text
Да, local commands можно добавлять до multiplayer,
но без player/session authority до v2.x.
```

Кандидаты для v1.x:

```text
- set_feature_enabled;
- set_dependency_enabled;
- validate_config;
- run_dry_run;
- apply_policy;
- create_contract;
- dispatch_caravan;
- pause_processor;
- resume_processor;
- inject_world_event;
- run_scenario;
- advance_ticks;
- request_report;
```

---

## 1.17. Action Bridge for rules/effects

Принято:

```text
В v1.x — только dry-run и config validation.
Runtime mutation через rules/effects — осторожно, лучше после feature/dependency model.
```

То есть в v1.x разрешать:

```text
- validate_rule_config;
- validate_effect_config;
- dry_run_rule_change;
- dry_run_effect_change;
- compare_config_result;
```

Не спешить с:

```text
- live add_rule;
- live remove_rule;
- live mutate_effect;
```

пока нет стабильного dependency graph, save/replay и diagnostics.

---

## 1.18. C ABI strategy for deep systems

Принято:

```text
B — C++ first + read-only C ABI diagnostics.
```

То есть:

```text
- deep systems сначала реализуются в C++ API;
- C ABI не обязан сразу покрывать всю мутацию;
- но read-only diagnostics/report handles через C ABI желательны;
- Unity/WASM/tools должны со временем уметь читать отчёты без доступа к C++ internals.
```

---

# 2. Новая архитектурная доктрина

## 2.1. Core is processing, not product gameplay

City Life Core не должен делать готовую игру.

Он должен делать следующее:

```text
- принимать определения мира и конфиги;
- валидировать данные;
- обрабатывать тики;
- рассчитывать изменения состояния;
- учитывать зависимости между системами;
- выпускать события;
- формировать диагностику;
- объяснять причины изменений;
- поддерживать save/load;
- поддерживать replay;
- давать API для внешних игр, серверов и инструментов.
```

---

## 2.2. Processor contract

Каждый processor должен иметь ясный контракт:

```text
Input:
- world/runtime state;
- processor config;
- feature/dependency config;
- deterministic context;
- relevant queued commands;
- active effects/modifiers.

Processing:
- validate assumptions;
- calculate deterministic state delta;
- emit events;
- emit diagnostics;
- record cause breakdown.

Output:
- state changes;
- events;
- report fragments;
- validation warnings/errors;
- replay-sensitive metadata.
```

---

## 2.3. No hidden coupling

Запрещено:

```text
- погода напрямую и скрыто меняет цену;
- событие напрямую и скрыто ломает рынок;
- фракция напрямую и скрыто меняет риск маршрута;
- рынок напрямую и скрыто меняет счастье населения.
```

Правильно:

```text
weather -> effect -> dependency graph -> market processor -> price pressure diagnostics
```

Каждая связь должна быть:

```text
- именованной;
- документированной;
- включаемой/отключаемой;
- диагностируемой;
- replay-compatible.
```

---

# 3. Обновлённая линия v1.x

## Главная цель v1.x

```text
Создать глубокий реалистичный single-runtime simulation processing core.
```

К `v2.0.0` ядро должно уже уметь обрабатывать:

```text
- feature toggles;
- dependency graph;
- rules/effects/modifiers;
- world/time/calendar;
- resources/goods/storage;
- quality/decay/spoilage;
- production chains;
- workplaces/upkeep/maintenance;
- population/needs/workforce/happiness/migration;
- weather/seasons/climate;
- logistics/routes/transport/caravans/risk;
- market engine with supply/demand;
- liquidity/volatility/depth/shocks;
- taxes/fees/policies;
- factions/influence/control/law/security;
- contracts/commitments/orders;
- ecology/agriculture/regeneration/environmental pressure;
- events/crises/shock propagation;
- autonomous simulation actors;
- content packs/presets/balance profiles;
- scenario runner;
- reports/forecasts/comparative simulation;
- local runtime orchestration;
- deep save/load/replay/migration.
```

---

# v1.5.0 — Simulation Processor Architecture

## Цель

Создать архитектурный фундамент для будущих глубоких simulation processors.

## Реализовать

```text
1. SimulationProcessor interface/concept.
2. Processor input context.
3. Processor output model:
   - state delta;
   - events;
   - diagnostics;
   - warnings;
   - errors.
4. Processor registry.
5. Feature registry.
6. Dependency registry.
7. Deterministic processor ordering.
8. Processor enable/disable support.
9. Basic processor lifecycle:
   - register;
   - validate;
   - process;
   - report.
10. Processor diagnostics foundation.
11. No-unrelated-mutation rule.
12. Tests:
   - processor executes;
   - processor disabled;
   - deterministic order;
   - dependency disabled;
   - diagnostics emitted;
   - no unrelated mutation.
13. docs/simulation-processing-doctrine.md.
14. docs/simulation-processors.md.
15. docs/feature-toggles.md.
16. docs/dependency-registry.md.
17. internal milestone acceptance note.
```

## Не делать

```text
- full weather system;
- full market system;
- full logistics system;
- full population system;
- scripting runtime;
- dynamic plugins;
- UI;
- network;
- multiplayer authority.
```

## Готово, когда

```text
- processor architecture can host future systems;
- processors are deterministic;
- disabled processors do not mutate state;
- docs explain "core processes simulation, not game";
- validation passes;
- internal acceptance note prepared.
```

---

# v1.6.0 — Rules, Effects, Modifiers & Cause Breakdown

## Цель

Создать универсальную модель правил, эффектов, модификаторов и объяснения причин изменений.

## Реализовать

```text
1. Rule model:
   - id;
   - trigger;
   - conditions;
   - effects;
   - priority;
   - enabled/disabled.
2. Condition model:
   - threshold;
   - scope;
   - system state;
   - active condition;
   - feature/dependency state.
3. Effect model:
   - target system;
   - target stat;
   - operation:
     - add;
     - multiply;
     - override;
     - clamp;
   - value;
   - duration;
   - source;
   - stacking rule.
4. Modifier model:
   - permanent;
   - temporary;
   - conditional;
   - event-based.
5. Stacking rules.
6. Priority rules.
7. Duration/expiry rules.
8. Source tracking.
9. Cause breakdown:
   - why value changed;
   - which rule caused it;
   - which dependency allowed it;
   - which modifier applied.
10. Dry-run evaluation.
11. Config validation for rules/effects.
12. Tests:
   - rule triggered;
   - rule not triggered;
   - effect applied;
   - effect expired;
   - disabled dependency blocks effect;
   - deterministic stacking;
   - cause breakdown correct.
13. docs/rules-effects-modifiers.md.
14. docs/cause-breakdown.md.
15. internal milestone acceptance note.
```

## Не делать

```text
- Lua/Python scripting;
- visual node editor;
- live mutation through Action Bridge except dry-run/config validation;
- dynamic plugin rules.
```

---

# v1.7.0 — World, Time, Calendar & Simulation Scope

## Цель

Создать реалистичный контекст мира, времени и scope-aware обработки.

## Реализовать

```text
1. World state model review.
2. Simulation calendar:
   - tick;
   - day;
   - month;
   - season;
   - year.
3. Time scale config:
   - ticks per day;
   - days per month;
   - months per season;
   - seasons per year.
4. Scope model:
   - global;
   - settlement;
   - route;
   - market;
   - faction;
   - resource;
   - future region.
5. World condition base model:
   - id;
   - type;
   - severity;
   - duration;
   - scope;
   - source;
   - active effects.
6. Deterministic scheduling.
7. Calendar-aware processors.
8. Tests:
   - deterministic time advance;
   - season transition;
   - scoped condition application;
   - condition expiry;
   - save/load time state;
   - replay from time state.
9. docs/world-model.md.
10. docs/time-calendar.md.
11. docs/world-scopes.md.
12. internal milestone acceptance note.
```

---

# v1.8.0 — Resources, Goods, Storage, Quality & Decay

## Цель

Сделать ресурсную модель достаточно глубокой для реалистичной экономики и производства.

## Реализовать

```text
1. Resource model.
2. Goods model.
3. Resource categories:
   - food;
   - water;
   - raw materials;
   - processed goods;
   - tools;
   - medicine;
   - luxury goods;
   - strategic goods;
   - fuel/energy if enabled.
4. Storage model:
   - capacity;
   - accepted categories;
   - protection;
   - access rules.
5. Quality model:
   - disabled;
   - tiered;
   - numeric.
6. Decay/spoilage model:
   - disabled by config;
   - time-based;
   - storage-based;
   - weather-ready.
7. Shortage/surplus detection.
8. Stock flow diagnostics:
   - inflow;
   - outflow;
   - net change;
   - ticks until shortage;
   - storage bottlenecks.
9. Data pack schema expansion for resources/storage.
10. Tests:
   - capacity limit;
   - add/remove/transfer;
   - shortage detection;
   - surplus detection;
   - quality affects value if enabled;
   - spoilage if enabled;
   - disabled spoilage no decay.
11. docs/resources-goods.md.
12. docs/storage.md.
13. docs/quality-decay.md.
14. internal milestone acceptance note.
```

---

# v1.9.0 — Production Chains, Workplaces, Upkeep & Maintenance

## Цель

Добавить обработку production chains, workplaces, upkeep и maintenance.

## Реализовать

```text
1. Production chain model:
   - inputs;
   - outputs;
   - interval;
   - efficiency;
   - optional byproducts.
2. Building/workplace model:
   - id;
   - type;
   - level;
   - status;
   - workers required;
   - input resources;
   - output resources;
   - upkeep;
   - maintenance.
3. Worker requirement model.
4. Efficiency model:
   - base efficiency;
   - worker shortage;
   - input shortage;
   - maintenance condition;
   - active modifiers.
5. Building status:
   - active;
   - paused;
   - missing input;
   - missing workers;
   - damaged;
   - maintenance required.
6. Upkeep failure behavior.
7. Production diagnostics:
   - output breakdown;
   - missing input;
   - missing workers;
   - maintenance effect;
   - efficiency cause.
8. Tests:
   - production works;
   - missing input stops production;
   - worker shortage reduces output;
   - maintenance affects output if enabled;
   - upkeep failure changes status if enabled;
   - deterministic production report.
9. docs/production-chains.md.
10. docs/workplaces.md.
11. docs/upkeep-maintenance.md.
12. internal milestone acceptance note.
```

---

# v1.10.0 — Population, Needs, Workforce, Happiness & Migration

## Цель

Добавить агрегированную реалистичную обработку населения без превращения ядра в life-sim.

## Реализовать

```text
1. Population model.
2. Population groups/classes:
   - workers;
   - farmers;
   - artisans;
   - merchants;
   - guards;
   - specialists;
   - custom groups.
3. Workforce model.
4. Employment model.
5. Needs model:
   - food;
   - water;
   - housing;
   - safety;
   - work;
   - goods;
   - health if enabled.
6. Happiness/stability model.
7. Migration model:
   - growth;
   - decline;
   - immigration;
   - emigration.
8. Consequences:
   - shortage -> happiness;
   - housing shortage -> growth cap;
   - unemployment -> stability;
   - low safety -> migration.
9. Toggleable dependencies:
   - needs -> happiness;
   - happiness -> migration;
   - shortage -> unrest;
   - taxes -> happiness;
   - health -> workforce.
10. Diagnostics:
   - need satisfaction;
   - workforce bottleneck;
   - happiness breakdown;
   - migration reason.
11. Tests:
   - population growth;
   - population decline;
   - shortage effect if enabled;
   - disabled dependency no effect;
   - migration deterministic.
12. docs/population.md.
13. docs/needs-workforce.md.
14. docs/happiness-migration.md.
15. internal milestone acceptance note.
```

---

# v1.11.0 — Weather, Seasons, Climate & Environmental Conditions

## Цель

Сделать погоду и климат не визуальной фичей, а системой влияния на другие processors.

## Реализовать

```text
1. Weather model:
   - clear;
   - cloudy;
   - rain;
   - heavy rain;
   - storm;
   - snow;
   - blizzard;
   - fog;
   - drought;
   - heatwave;
   - cold wave.
2. Weather severity.
3. Weather duration.
4. Climate profile.
5. Season-aware probabilities.
6. Deterministic weather generation.
7. Weather effects:
   - logistics;
   - routes;
   - agriculture;
   - storage decay;
   - population health;
   - market pressure.
8. Toggleable dependencies:
   - weather -> caravan speed;
   - weather -> route risk;
   - weather -> crop output;
   - weather -> market prices;
   - weather -> population health.
9. Weather diagnostics:
   - current weather;
   - severity;
   - duration;
   - active effects;
   - affected systems;
   - disabled dependencies.
10. Tests:
   - deterministic weather;
   - storm affects route risk if enabled;
   - weather does not affect market if disabled;
   - drought affects crop output if enabled;
   - weather save/load;
   - weather replay.
11. docs/weather.md.
12. docs/seasons-climate.md.
13. docs/environmental-conditions.md.
14. internal milestone acceptance note.
```

---

# v1.12.0 — Logistics, Routes, Transport, Caravans & Risk

## Цель

Сделать logistics processor: маршруты, транспорт, задержки, риски, доставка и влияние на supply.

## Реализовать

```text
1. Route model:
   - id;
   - source;
   - destination;
   - distance;
   - terrain;
   - base travel time;
   - capacity;
   - danger;
   - road quality.
2. Transport/caravan model:
   - id;
   - owner;
   - cargo;
   - capacity;
   - speed;
   - departure tick;
   - expected arrival tick;
   - actual arrival tick;
   - status.
3. Cargo model.
4. Travel time model.
5. Route risk model:
   - delay;
   - loss;
   - breakdown;
   - attack;
   - weather;
   - blockade.
6. Delivery result:
   - success;
   - delayed;
   - partial loss;
   - failed;
   - blocked.
7. Toggleable dependencies:
   - weather -> speed;
   - weather -> risk;
   - faction conflict -> route danger;
   - route delay -> market supply;
   - cargo loss -> stock level.
8. Diagnostics:
   - travel time breakdown;
   - risk breakdown;
   - delivery failure reason;
   - route bottlenecks.
9. Tests:
   - deterministic delivery;
   - weather delay if enabled;
   - disabled weather dependency no delay;
   - route blockade;
   - cargo loss;
   - delivery affects supply if enabled.
10. docs/logistics.md.
11. docs/routes-transport.md.
12. docs/travel-risk.md.
13. internal milestone acceptance note.
```

---

# v1.13.0 — Market Engine I: Supply, Demand & Price Pressure

## Цель

Создать базовый реалистичный market processor.

## Реализовать

```text
1. Market model:
   - id;
   - listed resources;
   - supply;
   - demand;
   - base price;
   - current price;
   - price pressure.
2. Supply model:
   - local production;
   - storage;
   - imports;
   - deliveries.
3. Demand model:
   - population needs;
   - production inputs;
   - contracts;
   - faction demand;
   - events.
4. Price pressure model:
   - shortage pressure;
   - surplus pressure;
   - delivery pressure;
   - demand pressure.
5. Market states:
   - stable;
   - shortage;
   - surplus;
   - panic;
   - boom;
   - blocked supply.
6. Cause breakdown:
   - why price changed;
   - what caused shortage;
   - what increased demand;
   - what reduced supply.
7. Tests:
   - shortage increases price;
   - surplus decreases price;
   - delivery reduces shortage;
   - population demand affects market if enabled;
   - disabled dependency no effect;
   - deterministic market report.
8. docs/market-engine.md.
9. docs/supply-demand.md.
10. docs/price-pressure.md.
11. internal milestone acceptance note.
```

---

# v1.14.0 — Market Engine II: Liquidity, Volatility, Depth & Shocks

## Цель

Углубить экономику до realistic model with liquidity/volatility.

## Реализовать

```text
1. Liquidity model:
   - tradable volume;
   - market depth;
   - thin market;
   - liquid market;
   - liquidity score.
2. Volatility model:
   - price sensitivity;
   - shock response;
   - stabilization rate;
   - volatility memory.
3. Spread/slippage approximation if safe.
4. Market shock model:
   - supply shock;
   - demand shock;
   - weather shock;
   - route shock;
   - faction shock;
   - event shock.
5. Confidence/stability model if enabled.
6. Price smoothing config.
7. Anti-chaos constraints:
   - max price movement per tick;
   - volatility clamp;
   - liquidity floor;
   - shock decay;
   - stabilization rules.
8. Toggleable dependencies:
   - route disruption -> liquidity;
   - weather -> volatility;
   - event -> shock;
   - faction tax -> spread;
   - panic -> demand spike.
9. Diagnostics:
   - liquidity score;
   - volatility score;
   - market depth;
   - price movement cause;
   - shock source;
   - stabilization reason.
10. Tests:
   - low liquidity increases volatility;
   - high liquidity stabilizes price;
   - supply shock changes price;
   - disabled weather dependency no volatility effect;
   - clamps prevent unrealistic explosion;
   - deterministic volatility report.
11. docs/liquidity.md.
12. docs/volatility.md.
13. docs/market-shocks.md.
14. internal milestone acceptance note.
```

---

# v1.15.0 — Taxes, Fees, Policies & Economic Controls

## Цель

Добавить обработку управленческих правил, влияющих на экономику, логистику, население и фракции.

## Реализовать

```text
1. Tax model.
2. Fee/toll model.
3. Policy/edict model.
4. Policy effects:
   - price modifier;
   - production modifier;
   - happiness modifier;
   - route modifier;
   - faction relation modifier.
5. Policy duration/cooldown.
6. Policy cost.
7. Toggleable dependencies:
   - taxes -> prices;
   - taxes -> happiness;
   - tolls -> route cost;
   - policy -> production;
   - policy -> market confidence.
8. Diagnostics:
   - active policies;
   - tax impact;
   - side effects;
   - policy cause breakdown.
9. Tests:
   - tax affects price if enabled;
   - disabled tax dependency no happiness effect;
   - policy expires;
   - policy deterministic.
10. docs/taxes-fees.md.
11. docs/policies-edicts.md.
12. internal milestone acceptance note.
```

---

# v1.16.0 — Factions, Influence, Control, Law & Security

## Цель

Сделать фракции processing-моделью влияния, контроля, безопасности и экономического давления.

## Реализовать

```text
1. Faction model.
2. Faction types.
3. Relations.
4. Influence model.
5. Control model:
   - market control;
   - route control;
   - resource control;
   - settlement influence.
6. Law/security model if enabled.
7. Crime/corruption risk if enabled.
8. Faction effects:
   - tax;
   - route safety;
   - market access;
   - contract availability;
   - population stability.
9. Toggleable dependencies:
   - faction conflict -> route risk;
   - faction control -> tax;
   - law/security -> caravan safety;
   - corruption -> market stability.
10. Diagnostics:
   - influence breakdown;
   - control impact;
   - faction-caused risks.
11. Tests:
   - influence changes;
   - relation changes;
   - faction control affects route if enabled;
   - disabled dependency no effect.
12. docs/factions.md.
13. docs/influence-control.md.
14. docs/law-security.md.
15. internal milestone acceptance note.
```

---

# v1.17.0 — Contracts, Commitments, Orders & Obligations

## Цель

Добавить контракты как систему обязательств между экономикой, логистикой, производством и фракциями.

## Реализовать

```text
1. Contract model.
2. Contract types:
   - delivery;
   - production;
   - supply;
   - construction;
   - protection if enabled;
   - emergency relief.
3. Lifecycle:
   - offered;
   - accepted;
   - active;
   - fulfilled;
   - failed;
   - expired.
4. Resource reservation.
5. Capacity reservation.
6. Deadline model.
7. Reward/penalty model.
8. Failure consequences.
9. Toggleable dependencies:
   - contract demand -> market pressure;
   - weather delay -> contract risk;
   - failure -> faction relation;
   - shortage -> emergency contracts.
10. Diagnostics:
   - fulfillment chance;
   - failure reason;
   - deadline pressure;
   - missing resources.
11. Tests:
   - contract success;
   - contract failure;
   - deadline expiration;
   - reserved resources;
   - deterministic contract report.
12. docs/contracts.md.
13. docs/commitments-obligations.md.
14. internal milestone acceptance note.
```

---

# v1.18.0 — Ecology, Agriculture, Regeneration & Environmental Pressure

## Цель

Добавить природно-ресурсную обработку: урожай, плодородие, вода, истощение, восстановление и environmental pressure.

## Реализовать

```text
1. Agriculture model.
2. Crop model.
3. Fertility model.
4. Water model.
5. Resource regeneration:
   - forest;
   - fish;
   - quarry/mines if enabled;
   - wildlife if enabled.
6. Resource depletion.
7. Environmental pressure:
   - overfarming;
   - overlogging;
   - pollution if enabled;
   - erosion if enabled.
8. Toggleable dependencies:
   - season -> crop yield;
   - drought -> water supply;
   - weather -> harvest;
   - ecology -> prices;
   - ecology -> health.
9. Diagnostics:
   - yield breakdown;
   - fertility trend;
   - depletion forecast;
   - environmental pressure.
10. Tests:
   - crop grows;
   - drought reduces yield if enabled;
   - disabled dependency no yield effect;
   - resource regenerates;
   - overuse depletes resource if enabled.
11. docs/agriculture.md.
12. docs/ecology-regeneration.md.
13. internal milestone acceptance note.
```

---

# v1.19.0 — Events, Crises, Event Chains & Shock Propagation

## Цель

Сделать события обработчиками изменений состояния, а не декоративным логом.

## Реализовать

```text
1. World event model.
2. Event conditions.
3. Event effects.
4. Event duration.
5. Event scope.
6. Event chain model.
7. Crisis model:
   - food crisis;
   - market crisis;
   - logistics crisis;
   - health crisis;
   - security crisis;
   - political crisis.
8. Shock propagation:
   - event -> supply;
   - event -> demand;
   - event -> liquidity;
   - event -> route risk;
   - event -> population stability.
9. Scenario director:
   - deterministic event pacing;
   - difficulty profile;
   - cooldowns.
10. Diagnostics:
   - event cause;
   - affected systems;
   - chain prediction;
   - crisis severity.
11. Tests:
   - event triggers;
   - event expires;
   - chain starts;
   - crisis updates;
   - disabled dependency no effect.
12. docs/world-events.md.
13. docs/crises.md.
14. docs/shock-propagation.md.
15. internal milestone acceptance note.
```

---

# v1.20.0 — Autonomous Simulation Actors & Decision Profiles

## Цель

Добавить deterministic autonomous processing: фракции, рынки, поселения и logistics могут принимать решения по профилям.

## Реализовать

```text
1. Decision profile model.
2. Actor types:
   - faction actor;
   - market actor;
   - settlement processor;
   - contract issuer;
   - logistics dispatcher.
3. Decision types:
   - issue contract;
   - reroute caravan;
   - adjust supply request;
   - respond to shortage;
   - apply policy;
   - react to crisis.
4. Deterministic decision order.
5. Decision constraints.
6. No ML dependency.
7. Toggleable autonomous systems.
8. Diagnostics:
   - decision reason;
   - rejected option;
   - priority breakdown.
9. Tests:
   - deterministic decision;
   - disabled actor no action;
   - actor responds to shortage;
   - risk tolerance changes decision.
10. docs/autonomous-simulation-actors.md.
11. docs/decision-profiles.md.
12. internal milestone acceptance note.
```

---

# v1.21.0 — Content Packs, Presets, Realistic Balance Profiles

## Цель

Дать разработчику возможность описывать ресурсы, здания, события, фракции, правила и профили без изменения C++ кода.

## Реализовать

```text
1. Content pack metadata.
2. Required/optional features.
3. Resource definitions.
4. Building definitions.
5. Production chain definitions.
6. Market definitions.
7. Event definitions.
8. Faction definitions.
9. Rule/effect definitions.
10. Balance profiles:
    - realistic;
    - soft simulation;
    - hardcore economy;
    - high volatility;
    - low volatility;
    - trade focused;
    - survival focused.
11. Content diagnostics:
    - broken chains;
    - impossible market;
    - missing resource;
    - circular dependency;
    - overpowered chain.
12. Tests:
    - valid content pack;
    - invalid content pack;
    - profile changes result;
    - deterministic content simulation.
13. docs/content-packs.md.
14. docs/balance-profiles.md.
15. docs/mechanics-authoring.md.
16. internal milestone acceptance note.
```

---

# v1.22.0 — Scenario Runner, Reports, Forecasts & Comparative Simulation

## Цель

Добавить инструмент проверки обработчиков: прогонять сценарии, сравнивать конфиги и получать реалистичные отчёты.

## Реализовать

```text
1. Batch scenario runner.
2. Scenario presets:
   - stable economy;
   - high volatility market;
   - drought;
   - route blockade;
   - food shortage;
   - market crash;
   - faction conflict;
   - logistics disruption;
   - long-run economy.
3. Comparative simulation:
   - baseline vs modified;
   - feature on/off;
   - low volatility vs high volatility;
   - with weather vs without weather.
4. Forecasts:
   - shortage forecast;
   - price trend forecast;
   - route risk forecast;
   - population stability forecast.
5. Reports:
   - economy;
   - liquidity;
   - volatility;
   - logistics;
   - population;
   - production;
   - faction;
   - crisis.
6. Cause breakdown.
7. Tests:
   - deterministic scenarios;
   - report correctness;
   - comparison correctness;
   - long-run no-crash.
8. docs/scenario-runner.md.
9. docs/simulation-reports.md.
10. docs/forecasting.md.
11. internal milestone acceptance note.
```

---

# v1.23.0 — Local Runtime Foundation for Deep Simulation

## Цель

Создать локальный server-authoritative runtime loop для всех глубоких систем без настоящего networking layer.

## Реализовать

```text
1. Headless runtime loop.
2. Tick scheduler.
3. Command queue.
4. Runtime processor orchestration.
5. Snapshot export.
6. Event stream export.
7. Bounded event reads.
8. Runtime diagnostics:
   - tick cost;
   - active processors;
   - command results;
   - rejected commands;
   - event volume.
9. Tests:
   - deterministic processing;
   - command order;
   - rejected command no mutation;
   - snapshot consistency;
   - event stream since tick.
10. docs/server-runtime-foundation.md.
11. docs/runtime-snapshots.md.
12. docs/runtime-event-streams.md.
13. internal milestone acceptance note.
```

---

# v1.24.0 — Deep Save/Load, Replay & Migration Hardening

## Цель

Убедиться, что все глубокие системы сохраняются, загружаются, мигрируют и воспроизводятся.

## Реализовать

```text
1. Save coverage for all major systems.
2. Golden save fixtures.
3. Invalid save fixtures.
4. Migration fixtures.
5. Replay from deep save.
6. Replay mismatch diagnostics:
   - market mismatch;
   - liquidity mismatch;
   - volatility mismatch;
   - route mismatch;
   - event mismatch;
   - population mismatch.
7. Compatibility matrix:
   - save;
   - content pack;
   - feature config;
   - dependency config;
   - rules/effects.
8. Tests:
   - valid deep save/load;
   - corrupted deep save;
   - migration success;
   - migration failure;
   - full replay deterministic.
9. docs/deep-save-format.md.
10. docs/gameplay-migration.md.
11. docs/replay-diagnostics.md.
12. docs/compatibility-matrix.md.
13. internal milestone acceptance note.
```

---

# v1.25.0 — Pre-2.0 Deep Simulation Completeness Gate

## Цель

Последняя проверка перед v2.0.0.

v1.25.0 не должна добавлять большие новые systems. Она должна доказать, что v1.x содержит полноценный deep realistic simulation processing core.

## Готово, когда

```text
- processors architecture stable;
- feature toggles work;
- dependency toggles work;
- rules/effects/modifiers work;
- world/time works;
- resources/storage/quality/decay work;
- production chains work;
- population model works;
- weather/climate works;
- logistics/routes work;
- markets work;
- liquidity works;
- volatility works;
- taxes/policies work;
- factions work;
- contracts work;
- ecology/agriculture work;
- events/crises work;
- autonomous actors work;
- content packs work;
- scenario runner works;
- diagnostics explain causes;
- save/load works;
- replay deterministic;
- migration behavior documented;
- v2.0 migration plan ready.
```

## Реализовать

```text
1. Full v1.x system audit.
2. Full processor audit.
3. Full feature/dependency toggle audit.
4. Full schema version audit.
5. Full save/replay audit.
6. Full C++ API status review.
7. C ABI read-only diagnostics readiness review.
8. Pre-v2 migration plan.
9. v2.0 release readiness draft.
10. docs/v1-deep-simulation-completeness.md.
11. docs/pre-v2-readiness-checklist.md.
12. internal milestone acceptance note.
```

---

# 4. Обновлённая линия v2.x

## Главная цель v2.x

```text
Полировать v1 systems, делать их authority-safe, multiplayer-safe, compatibility-safe и готовить к v3 regional scaling.
```

v2.x не должен заново добавлять базовую глубину. Базовая глубина должна быть в v1.x.

---

# v2.0.0 — Stable Deep Simulation SDK Foundation

Публичный релиз.

## Цель

Зафиксировать deep realistic simulation processing core как стабильную major baseline.

## Должно включать

```text
1. Completed v1.x deep simulation systems.
2. Stable public C++ integration path.
3. Stable docs for processor architecture.
4. Acceptance notes summary.
5. Migration guidance from v1.0/v1.x internal milestones.
6. Release notes.
7. Release manifest.
8. Full validation.
9. Known limitations.
10. Post-v2 priorities.
```

---

# v2.1.0 — Authority Foundation for Deep Simulation

```text
- abstract actor ids;
- abstract session ids where needed;
- ownership metadata;
- permission hooks;
- command authority checks;
- rejected action audit;
- actor-scoped diagnostics.
```

Не делать:

```text
- auth;
- accounts;
- matchmaking;
- network sessions;
- anti-cheat client software.
```

---

# v2.2.0 — System Permission Matrix

```text
- who can build;
- who can trade;
- who can dispatch caravan;
- who can issue contract;
- who can change policy;
- who can tax market;
- who can control route;
- who can influence faction;
- who can inject events in tool/server mode.
```

---

# v2.3.0 — Economy Consistency Polish

```text
- ledger ownership;
- transaction audit;
- double-spend prevention;
- deterministic price commit;
- liquidity abuse diagnostics;
- volatility clamp review;
- market manipulation diagnostics;
- realistic coefficient pass.
```

---

# v2.4.0 — Logistics & Contract Authority Polish

```text
- caravan ownership;
- cargo reservation;
- route permission;
- reroute permission;
- blockade authority;
- contract ownership;
- delivery audit;
- failure audit;
- deterministic conflict handling.
```

---

# v2.5.0 — Faction, Policy & Event Authority Polish

```text
- faction action permission;
- policy authority;
- event visibility;
- faction relation audit;
- security/crime authority;
- controlled asset validation.
```

---

# v2.6.0 — Snapshot, Visibility & Client Sync Model

```text
- full snapshot;
- filtered snapshot;
- private/public data;
- market-visible data;
- faction-visible data;
- route-visible data;
- hidden event data;
- visibility diagnostics.
```

---

# v2.7.0 — Multiplayer Replay, Command Logs & Persistence

```text
- actor/player command logs;
- rejected command replay;
- conflict replay;
- multiplayer save/load;
- audit export;
- deterministic replay proof.
```

---

# v2.8.0 — Simulation Polish & Default Coefficient Pass

```text
- realistic default coefficients;
- liquidity/volatility tuning;
- market stabilization tuning;
- logistics risk tuning;
- weather impact tuning;
- population sensitivity tuning;
- report clarity improvements;
- content validation improvements.
```

---

# v2.9.0 — Advanced Mechanics Preparation for v3

Prepare designs/prototypes for:

```text
- regional markets;
- regional liquidity;
- regional volatility;
- inter-region logistics;
- climate zones;
- regional disasters;
- migration between regions;
- faction territories;
- cross-region contracts;
- macro economy;
- world history;
- long-running audit.
```

---

# v2.10.0 — Pre-3.0 Large World Readiness Gate

Ready when:

```text
- all v1 systems are authority-safe;
- all v1 systems are snapshot-compatible;
- all v1 systems are replay-compatible;
- all v1 systems have diagnostics;
- all v1 systems have load/stress scenarios;
- regionalization plan ready;
- v3 migration plan ready.
```

---

# 5. Обновлённая линия v3.x

## Главная цель v3.x

```text
Масштабировать уже глубокие обработчики на регионы, большие миры и MMO-like simulations.
```

---

# v3.0.0 — Regional Foundation

Публичный релиз.

```text
- region model;
- region scopes;
- regional feature config;
- regional dependency config;
- cross-region references;
- regional snapshots;
- regional event streams;
- large-world migration guide.
```

---

# v3.1.0 — Regional Economy, Liquidity & Volatility

```text
- regional supply/demand;
- regional liquidity;
- regional volatility;
- regional market depth;
- inter-region price differences;
- market shock propagation;
- macro shortage/surplus.
```

---

# v3.2.0 — Inter-Region Logistics & Trade Networks

```text
- inter-region routes;
- trade hubs;
- chokepoints;
- route congestion;
- caravan fleets;
- regional route risk;
- supply chain disruption.
```

---

# v3.3.0 — Regional Weather, Climate Zones & Disasters

```text
- climate zones;
- regional weather;
- regional drought;
- regional storms;
- disaster scopes;
- crop impact;
- logistics impact;
- market impact.
```

---

# v3.4.0 — Faction Territories, Influence & Regional Control

```text
- territories;
- regional influence;
- route control;
- market control;
- conflict pressure;
- guild-like organizations if enabled.
```

---

# v3.5.0 — Population Migration, Stability & Regional Health

```text
- migration between regions;
- refugees if enabled;
- regional happiness;
- regional health/disease;
- unrest/stability;
- demographic reports.
```

---

# v3.6.0 — World History, Event Partitioning & Audit Streams

```text
- regional event partitions;
- owner/visibility partitions;
- world history;
- audit streams;
- event retention;
- event compaction.
```

---

# v3.7.0 — Long-running Persistence & Maintenance

```text
- periodic snapshots;
- regional save/load;
- maintenance reports;
- compaction;
- migration of large worlds;
- world health report.
```

---

# v3.8.0 — Large World Load, Benchmarks & Stability

```text
- many regions;
- many markets;
- many routes;
- many caravans;
- many factions;
- many events;
- long-run economy;
- optional benchmarks;
- stability diagnostics.
```

---

# v3.9.0 — Pre-4.0 Platform Preparation

```text
- module boundaries;
- genre/profile requirements;
- content pipeline requirements;
- editor/admin API requirements;
- diagnostics API requirements;
- migration plan v3 to v4.
```

---

# 6. Internal Milestone Acceptance Note Template

Для каждого internal milestone `vX.X.X` создавать файл или changelog section:

```text
Internal Milestone Acceptance Note — vX.X.X

Branch:
vX.X.X

Scope:
[short scope]

Implemented:
- ...

Changed files:
- ...

Tests added:
- ...

Validation:
- cmake configure:
- cmake build:
- ctest:
- affected examples:
- installed consumers if relevant:
- full validation script if relevant:

Documentation updated:
- ...

Compatibility:
- public C++ API:
- C ABI:
- data schema:
- save/load:
- replay:
- Action Bridge:
- migration notes:

Known limitations:
- ...

Deferred:
- ...

Owner decision required:
- ...

Ready for merge to main:
yes/no
```

---

# 7. Standard prompt for AI developer

```text
Работаем в City_Life_Core.

Активная ветка: vX.X.X.

Правила:
- Не создавай новые ветки.
- Не создавай feature/*.
- Не создавай release/*.
- Не создавай v/*.
- Ветка должна называться строго vX.X.X.
- В main ничего не заливай без разрешения владельца.
- GitHub не меняй, если владелец просит только подготовить файл/план.

Контекст:
City Life Core — headless C++20 simulation SDK.
Это не игра, не UI framework, не renderer, не Unity framework, не browser framework и не network framework.

Цель версии:
[цель версии]

Нужно реализовать:
[scope]

Обязательные свойства:
- deterministic;
- optional/configurable where applicable;
- dependency-toggle compatible where applicable;
- diagnostics-friendly;
- save/load aware where applicable;
- replay-aware where applicable;
- no hidden coupling;
- no unrelated refactor.

Не делать:
- UI;
- renderer;
- networking;
- accounts/auth/matchmaking;
- engine-specific code;
- unrelated refactor;
- gameplay product logic outside simulation processing scope.

Перед завершением:
- cmake configure;
- cmake build;
- ctest;
- affected examples;
- docs updated;
- CHANGELOG/internal milestone note updated;
- compatibility notes updated if needed.

После работы дай:
1. что реализовано;
2. какие файлы изменены;
3. какие тесты добавлены;
4. какие проверки прошли;
5. какие ограничения остались;
6. какие future work;
7. готова ли ветка к merge в main.
```

---

# 8. Immediate next task: v1.5.0 prompt

```text
Работаем в City_Life_Core.

Активная ветка: v1.5.0.

Цель:
Создать Simulation Processor Architecture Foundation.

Важно:
City Life Core — не игра.
City Life Core — headless C++20 simulation SDK.
Ядро обрабатывает симуляционные системы и отдаёт state deltas, events, diagnostics и reports.
Не добавлять UI, renderer, network, Unity-specific code, browser-specific code, accounts/auth/matchmaking.

Нужно реализовать:
1. SimulationProcessor interface/concept.
2. Processor input context.
3. Processor output:
   - state delta;
   - events;
   - diagnostics;
   - warnings/errors.
4. Processor registry.
5. Feature registry.
6. Dependency registry.
7. Deterministic processor order.
8. Processor enable/disable behavior.
9. Basic diagnostics foundation.
10. No-unrelated-mutation rule.
11. Tests:
    - processor executes;
    - processor disabled;
    - deterministic order;
    - dependency disabled;
    - diagnostics emitted;
    - no unrelated mutation.
12. docs/simulation-processing-doctrine.md.
13. docs/simulation-processors.md.
14. docs/feature-toggles.md.
15. docs/dependency-registry.md.
16. internal milestone acceptance note for v1.5.0.

Не делать:
- full weather system;
- full market system;
- full logistics system;
- full population system;
- scripting runtime;
- dynamic plugins;
- multiplayer authority;
- network;
- UI;
- unrelated refactor.

Перед завершением:
- cmake configure;
- cmake build;
- ctest;
- affected examples;
- docs updated;
- internal milestone acceptance note prepared.

После работы дай:
1. что реализовано;
2. какие файлы изменены;
3. какие тесты добавлены;
4. какие проверки прошли;
5. какие ограничения остались;
6. готова ли ветка к merge в main.
```

---

# 9. Decisions still pending

Остальные вопросы можно решить позже. Самые важные pending decisions:

```text
1. Насколько строго держим source compatibility в v1.x при deep simulation refactor?
2. Когда именно расширять C ABI beyond read-only diagnostics?
3. Нужно ли переименовывать GameProfiles в IntegrationProfiles в коде или только в документации?
4. Какой формат использовать для сложных configs: JSON, YAML или оба?
5. Где хранить internal acceptance notes: CHANGELOG.md, docs/internal-milestones/, или отдельные docs/acceptance-notes/?
6. Нужны ли отдельные issue templates для internal milestone tasks?
7. Делать ли CLI runner частью v1.x deep simulation или оставить examples-only?
8. Какие systems должны попасть в C ABI read-only reports первыми?
9. Нужны ли benchmark thresholds уже в v1.x или только optional reports?
10. Когда начинать проектировать custom processor extension API?
```

---

# 10. Final positioning

Обновлённая позиция проекта:

```text
City Life Core is a headless C++20 realistic simulation processing SDK.

It does not implement a finished game.
It processes reusable domain simulation systems for games, tools, servers and large-world backends.

The core provides deterministic processors, configurable systems, toggleable dependencies, realistic economy, liquidity/volatility, logistics, weather, population, factions, contracts, events, persistence, replay and diagnostics.

Downstream products own UI, presentation, networking, accounts, gameplay UX and engine-specific integration.
```
