# CMake Package

Version: **1.0.0**

City Life Core installs a CMake package config for external consumers.

## Consumer usage

```cmake
find_package(CityLifeCore CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE CityLifeCore::core)
```

## Minimal C consumer

```cmake
project(MyCityLifeCoreCConsumer LANGUAGES C CXX)
find_package(CityLifeCore CONFIG REQUIRED)

add_executable(my_consumer main.c)
set_target_properties(my_consumer PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(my_consumer PRIVATE CityLifeCore::core)
```

## Related documents

- [Packaging](packaging.md)
- [SDK structure](sdk-structure.md)
- [SDK ZIP package](sdk-zip-package.md)
