# City Life Core / Ядро

**City Life Core** is a headless C++20 simulation core for living settlements, resources, economy, routes, caravans, factions, ownership, contracts, persistence, deterministic replay, and future server-authoritative game backends.

**City Life Core / Ядро** — это headless-ядро симуляции на C++20 для живых поселений, ресурсов, экономики, маршрутов, караванов, фракций, владения, контрактов, сохранений, детерминированного replay и будущих server-authoritative игровых серверов.

Current version: **0.9.2**

---

## Русский

### Что это такое

City Life Core — это не игра и не клиентский фреймворк. Это низкоуровневое симуляционное ядро, которое можно встроить в игру, сервер, редактор мира, backend-сервис, инструмент балансировки экономики или будущий MMO/runtime слой.

Ядро отвечает за то, чтобы мир был:

- **детерминированным** — одинаковые действия дают одинаковое состояние;
- **проверяемым** — данные, runtime state и persistence проходят валидацию;
- **сохраняемым** — состояние мира можно сериализовать и восстановить;
- **headless** — без зависимости от графики, UI, платформы или движка;
- **SDK-friendly** — публичные C++ API постепенно стабилизируются вокруг runtime сценариев;
- **готовым к server-authoritative архитектуре** — состояние, действия и replay отделены от клиента.

### Какие задачи закрывает ядро

City Life Core полезен, если нужно:

- моделировать поселения, склады, производство и потребление ресурсов;
- строить экономику на ресурсах, спросе, торговле, кошельке и ledger;
- связывать поселения маршрутами и караванами;
- добавлять фракции, репутацию, владение поселениями/караванами;
- создавать контракты на доставку ресурсов и вознаграждения;
- автоматически отмечать прибытия караванов и просроченные контракты во время runtime tick;
- сохранять и загружать runtime state;
- проверять, что загруженный мир семантически равен исходному;
- продолжать симуляцию после загрузки без расхождения состояния;
- использовать ядро как основу SDK или backend simulation service.

### Текущий статус

Проект находится на стадии **0.9.x Runtime Persistence & Public SDK Readiness**.

Готовые и активно укрепляемые области:

| Подсистема | Статус |
| --- | --- |
| Foundation/Core | зрелая основа |
| Data Registry | рабочий registry definitions + validation |
| Settlement/Storage | рабочая симуляция поселений и складов |
| Basic Economy/Market | базовая экономика, рынок, trade, ledger |
| Routes | маршруты между поселениями |
| Caravans | караваны, cargo, movement, arrival state, arrival tick reports |
| Factions/Ownership | фракции, reputation, ownership |
| Contracts | delivery contracts, fulfillment, reward ledger, overdue failure tick consequences |
| Persistence | world/runtime save/load, corrupted-load coverage |
| Runtime/World Integration | runtime bundle, scenario bootstrap, workflows, deterministic tick consequences |
| Runtime Events/Diagnostics | event log, caravan arrival events, contract fulfilled/failed events, diagnostics |
| Public SDK/API | в процессе стабилизации |
| C ABI | ещё не готово |
| Packaging/Release | начальная подготовка |
| External Docs | активно оформляется |

### Быстрый старт

```bash
cmake -S . -B build -DCLC_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

Запуск bootstrap CLI:

```bash
./build/clc_runner
```

Windows / multi-config generators:

```powershell
cmake -S . -B build -DCLC_BUILD_TESTS=ON
cmake --build build --config Release
ctest --test-dir build --output-on-failure -C Release
```

### Минимальный runtime-сценарий

```cpp
#include "clc/sim/SimulationRuntimeScenario.hpp"
#include "clc/sim/SimulationRuntimePersistenceValidation.hpp"

#include <filesystem>

int main() {
    auto bootstrap = clc::sim::make_basic_runtime_scenario();
    if (!bootstrap.ok()) {
        return 1;
    }

    auto& runtime = bootstrap.runtime;

    auto caravan = clc::sim::create_runtime_caravan_for_route(
        runtime,
        "riverwatch_to_hillford",
        "demo_caravan",
        "Demo Caravan"
    );

    if (!caravan.ok()) {
        return 1;
    }

    clc::sim::SimulationRuntime loaded{clc::sim::make_basic_runtime_scenario_registry()};
    auto result = clc::sim::validate_simulation_runtime_save_load_roundtrip(
        runtime,
        loaded,
        std::filesystem::temp_directory_path() / "runtime.clcs"
    );

    return result.ok() ? 0 : 1;
}
```

### Документация

- [Public API / Публичный API](docs/PUBLIC_API.md)
- [SDK Structure / Структура SDK](docs/SDK_STRUCTURE.md)
- [Changelog](CHANGELOG.md)

### Ближайшие цели до 1.0.0

- расширить arrival consequences до более глубоких settlement-side effects;
- стабилизировать Public SDK/API;
- подготовить release SDK layout;
- расширить external documentation;
- начать C ABI только после стабилизации C++ SDK surface;
- держать `main` стабильным и вливать блоки через fast-forward.

---

## English

### What is City Life Core?

City Life Core is not a game and not a client framework. It is a low-level simulation core that can be embedded into a game, server, world editor, economy balancing tool, backend service, or future MMO/runtime layer.

The core is designed to make world simulation:

- **deterministic** — the same actions produce the same state;
- **validated** — data, runtime state, and persistence are checked explicitly;
- **persistent** — runtime state can be saved, loaded, and compared semantically;
- **headless** — no rendering, UI, platform, or engine dependency;
- **SDK-friendly** — public C++ APIs are being stabilized around runtime workflows;
- **server-authoritative ready** — state, actions, and replay are separated from clients.

### What problems does it solve?

City Life Core is useful when you need to:

- simulate settlements, storage, production, and resource consumption;
- build resource-driven economy with demand, trade, wallet, and ledger;
- connect settlements with routes and caravans;
- model factions, reputation, and ownership;
- create delivery contracts and reward flows;
- automatically report caravan arrivals and overdue contract failures during runtime ticks;
- save and load complete runtime state;
- validate that loaded runtime state is semantically equivalent;
- continue simulation after load without deterministic drift;
- use the core as a foundation for a public SDK or backend simulation service.

### Current status

The project is currently in **0.9.x Runtime Persistence & Public SDK Readiness**.

Implemented and actively hardened areas:

| Subsystem | Status |
| --- | --- |
| Foundation/Core | mature foundation |
| Data Registry | working definitions registry + validation |
| Settlement/Storage | working settlement and storage simulation |
| Basic Economy/Market | basic economy, market, trade, ledger |
| Routes | settlement routes |
| Caravans | caravans, cargo, movement, arrival state, arrival tick reports |
| Factions/Ownership | factions, reputation, ownership |
| Contracts | delivery contracts, fulfillment, reward ledger, overdue failure tick consequences |
| Persistence | world/runtime save-load, corrupted-load coverage |
| Runtime/World Integration | runtime bundle, scenario bootstrap, workflows, deterministic tick consequences |
| Runtime Events/Diagnostics | event log, caravan arrival events, contract fulfilled/failed events, diagnostics |
| Public SDK/API | being stabilized |
| C ABI | not ready yet |
| Packaging/Release | early preparation |
| External Docs | being actively written |

### Quick start

```bash
cmake -S . -B build -DCLC_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

Run the bootstrap CLI:

```bash
./build/clc_runner
```

Windows / multi-config generators:

```powershell
cmake -S . -B build -DCLC_BUILD_TESTS=ON
cmake --build build --config Release
ctest --test-dir build --output-on-failure -C Release
```

### Minimal runtime scenario

```cpp
#include "clc/sim/SimulationRuntimeScenario.hpp"
#include "clc/sim/SimulationRuntimePersistenceValidation.hpp"

#include <filesystem>

int main() {
    auto bootstrap = clc::sim::make_basic_runtime_scenario();
    if (!bootstrap.ok()) {
        return 1;
    }

    auto& runtime = bootstrap.runtime;

    auto caravan = clc::sim::create_runtime_caravan_for_route(
        runtime,
        "riverwatch_to_hillford",
        "demo_caravan",
        "Demo Caravan"
    );

    if (!caravan.ok()) {
        return 1;
    }

    clc::sim::SimulationRuntime loaded{clc::sim::make_basic_runtime_scenario_registry()};
    auto result = clc::sim::validate_simulation_runtime_save_load_roundtrip(
        runtime,
        loaded,
        std::filesystem::temp_directory_path() / "runtime.clcs"
    );

    return result.ok() ? 0 : 1;
}
```

### Documentation

- [Public API](docs/PUBLIC_API.md)
- [SDK Structure](docs/SDK_STRUCTURE.md)
- [Changelog](CHANGELOG.md)

### Near-term goals before 1.0.0

- extend arrival consequences into deeper settlement-side effects;
- stabilize the Public SDK/API surface;
- prepare release SDK layout;
- expand external user documentation;
- start C ABI only after the C++ SDK surface is stable;
- keep `main` stable and merge completed blocks by fast-forward only.
