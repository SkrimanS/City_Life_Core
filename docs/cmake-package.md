# CMake Package

Version: **1.0.0**

City Life Core installs a CMake package config for external consumers.

The installed package is the recommended way for native C++ projects, C ABI consumers, tools, servers and packaging workflows to consume the SDK.

---

## Exported target

The package exports:

```cmake
CityLifeCore::core
```

Use this target instead of hard-coded include directories or library paths.

---

## C++ consumer usage

```cmake
find_package(CityLifeCore CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE CityLifeCore::core)
```

Recommended C++ include:

```cpp
#include "clc/CityLifeCore.hpp"
```

The C++ API is source-first. Rebuild the consumer against the SDK version it uses.

---

## Minimal C ABI consumer

C consumers can use the installed package too. Because City Life Core is implemented in C++, use C++ as the final linker language:

```cmake
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

The C ABI is the intended boundary for C#, Unity, Browser/WASM and other non-C++ bindings.

---

## Installed package variables

The package config exposes install-layout paths used by consumers and examples.

Expected variables include:

```cmake
CityLifeCore_DATA_DIR
CityLifeCore_EXAMPLES_DIR
```

Use these for installed example data and example source discovery instead of assuming repository-relative paths.

---

## Installed examples

Installed examples may include:

```text
examples/find_package_consumer/
examples/c_abi_consumer/
examples/csharp_unity/
```

The C# / Unity files are wrappers and smoke-test scripts. They still require a native City Life Core library for the target platform.

---

## Shared-library native plug-in builds

For Unity or other native plug-in hosts, build City Life Core as a shared library:

```bash
cmake -S . -B build-plugin -DBUILD_SHARED_LIBS=ON -DCLC_BUILD_TESTS=OFF -DCLC_BUILD_EXAMPLES=OFF -DCLC_BUILD_TOOLS=OFF
cmake --build build-plugin --config Release
```

Typical output names:

- Windows: `city_life_core.dll`
- Linux: `libcity_life_core.so`
- macOS: `libcity_life_core.dylib`

A Unity project should place the native library under `Assets/Plugins` for the target platform and use the C# wrapper from `examples/csharp_unity/`.

---

## Browser / WebAssembly note

Browser/WebAssembly support is planned, not implemented.

Future WebAssembly work should define its own CMake configure/build guidance and adapter layer. It should not make browser APIs part of the native core package.

---

## Related documents

- [Packaging](packaging.md)
- [SDK structure](sdk-structure.md)
- [SDK ZIP package](sdk-zip-package.md)
- [Build and linking policy](build-and-linking-policy.md)
- [C ABI](c-abi.md)
- [C ABI expansion plan](c-abi-expansion-plan.md)
- [C# and Unity integration](csharp-unity.md)
- [Browser and WebAssembly integration](browser-wasm.md)
- [Integration validation](integration-validation.md)
