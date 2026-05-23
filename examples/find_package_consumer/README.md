# City Life Core find_package Consumer Example

This directory is a standalone external CMake consumer project. It is intentionally outside the main build graph and is meant to verify that an installed City Life Core SDK can be consumed with `find_package(CityLifeCore CONFIG REQUIRED)`.

Этот каталог — отдельный внешний CMake consumer-проект. Он намеренно не является частью основного build graph и нужен для проверки, что установленный SDK City Life Core подключается через `find_package(CityLifeCore CONFIG REQUIRED)`.

## Build flow

From the repository root, install City Life Core first:

```bash
cmake -S . -B build-sdk -DCLC_BUILD_TESTS=OFF -DCLC_BUILD_EXAMPLES=OFF
cmake --build build-sdk
cmake --install build-sdk --prefix /tmp/city-life-core-sdk
```

Then configure this consumer project against the installed SDK:

```bash
cmake -S examples/find_package_consumer -B build-consumer -DCMAKE_PREFIX_PATH=/tmp/city-life-core-sdk
cmake --build build-consumer
./build-consumer/city_life_core_consumer
```

## What it verifies

- `CityLifeCoreConfig.cmake` is installed and discoverable.
- `CityLifeCore::core` is exported correctly.
- Public include paths work from the install prefix.
- A minimal runtime scenario can be created and advanced by ticks.

## Release checklist role

Before publishing 1.0.0, this example should be run in CI as an installed-SDK smoke test:

1. configure and build City Life Core;
2. install it into a temporary prefix;
3. configure this consumer with `CMAKE_PREFIX_PATH`;
4. build and run `city_life_core_consumer`.

The unpacked SDK ZIP validation should also build and run this consumer against the extracted archive prefix.
