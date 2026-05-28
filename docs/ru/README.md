# Документация City Life Core

Версия: **1.0.0**  
Статус: **released / v1.0.0**

Эта папка содержит русскоязычную точку входа в документацию City Life Core. Основная документация ведётся на английском языке в [`../README.md`](../README.md); русские страницы и справочные материалы размещаются в `docs/ru/`.

---

## С чего начать

| Документ | Назначение |
| --- | --- |
| [Core concepts](../core-concepts.md) | Основные системы: registry, runtime, ticks, settlements, caravans, economy, contracts, events и persistence. |
| [Public API](../public-api.md) | Публичный SDK API, рекомендуемые include, validation model и правила использования. |
| [SDK structure](../sdk-structure.md) | Структура репозитория, установленного SDK, CMake options и integration modes. |
| [Packaging](../packaging.md) | Сборка, установка и подключение SDK через CMake. |
| [CMake package](../cmake-package.md) | Exported CMake target и installed package variables. |
| [SDK ZIP package](../sdk-zip-package.md) | Создание и использование CPack ZIP SDK archive. |
| [Build and linking policy](../build-and-linking-policy.md) | Политика static/shared, source-first и binary compatibility. |

---

## API и совместимость

| Документ | Назначение |
| --- | --- |
| [Public API status](../public-api-status.md) | Классификация публичных headers и API surface. |
| [C interface](../c-abi.md) | Минимальный C interface для version/time utilities, opaque world handle и read-only world events. |
| [Integration targets](../integration-targets.md) | Текущие и планируемые integration targets: C++, C ABI, Unity/C#, browser/WASM, servers и tools. |
| [C# and Unity integration](../csharp-unity.md) | Интеграция Unity/C# через C ABI и P/Invoke. |
| [Browser and WebAssembly integration](../browser-wasm.md) | План интеграции browser/WASM для web games, web tools и demos. |
| [Compatibility](../compatibility.md) | Правила совместимости time, events, save/load, validation и source/binary compatibility. |
| [Migration](../migration.md) | Инструкции для обновления старых integrations. |

---

## Roadmap и релизные документы

| Документ | Назначение |
| --- | --- |
| [Roadmap](../roadmap.md) | Направление проекта, внутренние milestones и план v2/v3/v4. |
| [Versioning and release policy](../versioning.md) | Политика версий, веток, тегов и релизов. |
| [Release notes 1.0.0](../release-notes-1.0.0.md) | Финальные заметки релиза 1.0.0. |
| [Release manifest 1.0.0](../release-manifest-1.0.0.md) | Финальный manifest релиза и validation summary. |
| [Readiness status](../readiness-status.md) | Срез готовности подсистем для линии 1.0.0. |
| [Verifying releases](../verifying-releases.md) | Проверка официальных релизов. |
| [CI artifact review](../ci-artifact-review.md) | Проверка CI, benchmark artifacts и SDK ZIP artifacts. |
| [Release manifest template](../release-manifest-template.md) | Шаблон будущих release manifests. |
| [Protection strategy](../protection-strategy.md) | Стратегия защиты и распространения проекта. |

---

## Быстрое подключение через CMake

```cmake
find_package(CityLifeCore CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE CityLifeCore::core)
```

Рекомендуемый C++ include:

```cpp
#include "clc/CityLifeCore.hpp"
```

Минимальный C interface include:

```c
#include "clc/c/CityLifeCoreC.h"
```
