# CMake Package / CMake package

Version: **1.0.0**

City Life Core installs as a CMake package and exports both a target and install-directory variables.

City Life Core устанавливается как CMake package и экспортирует target вместе с переменными install directories.

---

## Target

External projects should use:

```cmake
find_package(CityLifeCore CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE CityLifeCore::core)
```

The exported target is:

```cmake
CityLifeCore::core
```

---

## Installed directory variables

After `find_package(CityLifeCore CONFIG REQUIRED)`, the package config provides:

| Variable | Meaning |
| --- | --- |
| `CityLifeCore_INCLUDE_DIR` | Installed public header root, usually `<prefix>/include`. |
| `CityLifeCore_DOCS_DIR` | Installed documentation directory, usually `<prefix>/share/doc/CityLifeCore`. |
| `CityLifeCore_DATA_DIR` | Installed data directory, usually `<prefix>/share/CityLifeCore/data`. |
| `CityLifeCore_EXAMPLES_DIR` | Installed example source directory, usually `<prefix>/share/doc/CityLifeCore/examples`. |

Example:

```cmake
find_package(CityLifeCore CONFIG REQUIRED)

message(STATUS "City Life Core include dir: ${CityLifeCore_INCLUDE_DIR}")
message(STATUS "City Life Core docs dir: ${CityLifeCore_DOCS_DIR}")
message(STATUS "City Life Core data dir: ${CityLifeCore_DATA_DIR}")
message(STATUS "City Life Core examples dir: ${CityLifeCore_EXAMPLES_DIR}")
```

These paths are checked by `CityLifeCoreConfig.cmake` using CMake package helpers. If a package is incomplete or incorrectly relocated, `find_package` should fail instead of silently returning broken paths.

---

## Validating package variables in a consumer

A consumer can assert the variables exist:

```cmake
foreach(required_dir IN ITEMS
    CityLifeCore_INCLUDE_DIR
    CityLifeCore_DOCS_DIR
    CityLifeCore_DATA_DIR
    CityLifeCore_EXAMPLES_DIR
)
    if(NOT DEFINED ${required_dir} OR NOT EXISTS "${${required_dir}}")
        message(FATAL_ERROR "${required_dir} is not defined or does not exist: ${${required_dir}}")
    endif()
endforeach()
```

Both standalone consumers perform this check:

```text
examples/find_package_consumer/
examples/c_abi_consumer/
```

---

## C++ consumer

```cmake
cmake_minimum_required(VERSION 3.22)
project(MyCityLifeApp LANGUAGES CXX)

find_package(CityLifeCore CONFIG REQUIRED)

add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE CityLifeCore::core)
target_compile_features(my_app PRIVATE cxx_std_20)
```

Recommended include:

```cpp
#include "clc/CityLifeCore.hpp"
```

---

## C consumer for the minimal C interface

`CityLifeCore::core` is implemented in C++, so C consumers should compile their `.c` files as C but link with the C++ linker:

```cmake
cmake_minimum_required(VERSION 3.22)
project(MyCityLifeCoreCConsumer LANGUAGES C CXX)

find_package(CityLifeCore CONFIG REQUIRED)

add_executable(my_consumer main.c)
set_target_properties(my_consumer PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(my_consumer PRIVATE CityLifeCore::core)
```

Recommended C include:

```c
#include "clc/c/CityLifeCoreC.h"
```

---

## ZIP package validation

The release-validation flow checks the package in three ways:

1. `cmake --install` prefix consumers.
2. Unpacked SDK ZIP C++ consumer.
3. Unpacked SDK ZIP C consumer for the minimal C interface.

For 1.0.0 release artifact review, see:

```text
docs/SDK_ZIP_PACKAGE.md
docs/CI_ARTIFACT_REVIEW.md
docs/RELEASE_MANIFEST_DRAFT_1.0.0.md
```
