# City Life Core Packaging / Упаковка

Version: **0.9.9**  
Status: **pre-1.0 audit build / сборка для аудита перед 1.0**

This document describes the current source-package installation flow for City Life Core.

Этот документ описывает текущий source-package installation flow для City Life Core.

---

## Русский

### Статус

City Life Core поддерживает базовый CMake install/export package flow:

- установка публичных headers из `include/`;
- установка `city_life_core` library target;
- экспорт CMake target namespace `CityLifeCore::`;
- генерация `CityLifeCoreConfig.cmake`;
- генерация `CityLifeCoreConfigVersion.cmake`;
- подключение внешним проектом через `find_package(CityLifeCore CONFIG REQUIRED)`;
- установка README, CHANGELOG, docs, examples и demo data.

Это ещё не финальный binary SDK release, но уже practical source-first SDK package для аудита `0.9.9` перед `1.0.0`.

### CMake options

Когда City Life Core собирается как top-level project, tests/examples/tools по умолчанию включены. Когда проект подключается через `add_subdirectory(...)`, они по умолчанию выключены, чтобы внешний build не получал лишние targets.

| Option | Top-level default | Subdirectory default | Purpose |
| --- | --- | --- | --- |
| `CLC_BUILD_TESTS` | `ON` | `OFF` | Build unit/integration tests. |
| `CLC_BUILD_EXAMPLES` | `ON` | `OFF` | Build SDK examples. |
| `CLC_BUILD_TOOLS` | `ON` | `OFF` | Build command-line tools such as `clc_runner`. |

### Source install

```bash
cmake -S . -B build -DCLC_BUILD_TESTS=OFF -DCLC_BUILD_EXAMPLES=OFF -DCLC_BUILD_TOOLS=OFF -DCMAKE_INSTALL_PREFIX=/path/to/city-life-core-sdk
cmake --build build
cmake --install build
```

После установки ожидается структура:

```text
/path/to/city-life-core-sdk/
  include/
    clc/
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
      RELEASE_CHECKLIST.md
      RELEASE_NOTES_0.9.9.md
      examples/
    CityLifeCore/data/
```

На Windows путь библиотеки и расширения будут зависеть от генератора и toolchain.

### Использование через find_package

```cmake
cmake_minimum_required(VERSION 3.22)
project(MyCityLifeApp LANGUAGES CXX)

find_package(CityLifeCore CONFIG REQUIRED)

add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE CityLifeCore::core)
```

Configure example:

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/city-life-core-sdk
cmake --build build
```

### Installed SDK smoke test

Перед `1.0.0-rc1` package flow должен проверяться отдельным external consumer project:

```bash
cmake -S . -B build-sdk -DCLC_BUILD_TESTS=OFF -DCLC_BUILD_EXAMPLES=OFF -DCLC_BUILD_TOOLS=OFF
cmake --build build-sdk
cmake --install build-sdk --prefix /tmp/city-life-core-sdk

cmake -S examples/find_package_consumer -B build-consumer -DCMAKE_PREFIX_PATH=/tmp/city-life-core-sdk
cmake --build build-consumer
./build-consumer/city_life_core_consumer
```

Этот flow проверяет:

- установленный `CityLifeCoreConfig.cmake`;
- exported target `CityLifeCore::core`;
- public include paths из install prefix;
- минимальный runtime tick сценарий во внешнем проекте.

### Target names

Публичный imported target:

```cmake
CityLifeCore::core
```

Локальный build-tree alias в самом репозитории использует то же имя.

### Что пока не входит

Пока не финализировано:

- binary package layout для всех платформ;
- CPack artifacts;
- shared/static variant policy;
- ABI compatibility policy;
- C ABI;
- package manager recipes;
- installed tools policy.

---

## English

### Status

City Life Core supports a basic CMake install/export package flow:

- installs public headers from `include/`;
- installs the `city_life_core` library target;
- exports the `CityLifeCore::` CMake target namespace;
- generates `CityLifeCoreConfig.cmake`;
- generates `CityLifeCoreConfigVersion.cmake`;
- allows external projects to use `find_package(CityLifeCore CONFIG REQUIRED)`;
- installs README, CHANGELOG, docs, examples, and demo data.

This is not the final binary SDK release yet, but it is a practical source-first SDK package for the `0.9.9` audit before `1.0.0`.

### CMake options

When City Life Core is built as the top-level project, tests/examples/tools default to `ON`. When it is consumed through `add_subdirectory(...)`, they default to `OFF` so the parent build does not receive extra targets by default.

| Option | Top-level default | Subdirectory default | Purpose |
| --- | --- | --- | --- |
| `CLC_BUILD_TESTS` | `ON` | `OFF` | Build unit/integration tests. |
| `CLC_BUILD_EXAMPLES` | `ON` | `OFF` | Build SDK examples. |
| `CLC_BUILD_TOOLS` | `ON` | `OFF` | Build command-line tools such as `clc_runner`. |

### Source install

```bash
cmake -S . -B build -DCLC_BUILD_TESTS=OFF -DCLC_BUILD_EXAMPLES=OFF -DCLC_BUILD_TOOLS=OFF -DCMAKE_INSTALL_PREFIX=/path/to/city-life-core-sdk
cmake --build build
cmake --install build
```

Expected installed layout:

```text
/path/to/city-life-core-sdk/
  include/
    clc/
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
      RELEASE_CHECKLIST.md
      RELEASE_NOTES_0.9.9.md
      examples/
    CityLifeCore/data/
```

On Windows, library paths and extensions depend on the generator and toolchain.

### Using find_package

```cmake
cmake_minimum_required(VERSION 3.22)
project(MyCityLifeApp LANGUAGES CXX)

find_package(CityLifeCore CONFIG REQUIRED)

add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE CityLifeCore::core)
```

Configure example:

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/city-life-core-sdk
cmake --build build
```

### Installed SDK smoke test

Before `1.0.0-rc1`, package flow should be verified through a standalone external consumer project:

```bash
cmake -S . -B build-sdk -DCLC_BUILD_TESTS=OFF -DCLC_BUILD_EXAMPLES=OFF -DCLC_BUILD_TOOLS=OFF
cmake --build build-sdk
cmake --install build-sdk --prefix /tmp/city-life-core-sdk

cmake -S examples/find_package_consumer -B build-consumer -DCMAKE_PREFIX_PATH=/tmp/city-life-core-sdk
cmake --build build-consumer
./build-consumer/city_life_core_consumer
```

This flow verifies:

- installed `CityLifeCoreConfig.cmake`;
- exported target `CityLifeCore::core`;
- public include paths from the install prefix;
- a minimal runtime tick scenario in an external project.

### Target names

Public imported target:

```cmake
CityLifeCore::core
```

The in-repository build-tree alias uses the same name.

### Not finalized yet

Still not finalized:

- binary package layout for all platforms;
- CPack artifacts;
- shared/static variant policy;
- ABI compatibility policy;
- C ABI;
- package manager recipes;
- installed tools policy.
