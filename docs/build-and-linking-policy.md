# Build and Linking Policy

Version: **1.0.0**

City Life Core is designed for source-first C++ integrations, installed CMake package integrations, local Action Bridge dispatch and controlled foreign-language integrations through the C ABI.

This document describes the intended build and linking rules for native games, tools, servers, local Action Bridge consumers, C ABI consumers, Unity/C# wrappers and future Browser/WebAssembly adapters.

---

## Main policy

- Prefer rebuilding downstream projects against the SDK version they consume.
- Use the installed `CityLifeCore::core` target instead of hard-coded library paths.
- Treat headers under `include/clc` as the supported public include surface.
- Treat the C++ API as source-first; C++ binary ABI stability is not promised.
- Use the local Action Bridge through the public C++ API for transport-agnostic external action validation and dispatch.
- Use the C ABI for C, C#, Unity, WebAssembly and other foreign-language bindings.
- Keep engine-specific and platform-specific adapters outside the native core.
- Use shared-library builds when a host runtime needs to load City Life Core as a native plug-in.

---

## C++ source-first policy

Native C++ users should include:

```cpp
#include "clc/CityLifeCore.hpp"
```

The public C++ API is the richest integration path. It is intended for projects that can compile and link against the SDK normally.

Downstream users should not rely on stable C++ binary ABI between arbitrary builds, compilers, standard libraries or package configurations.

Recommended consumption:

```cmake
find_package(CityLifeCore CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE CityLifeCore::core)
```

---

## Local Action Bridge policy

Native C++ game layers, tools, editors and future server-authoritative adapters can use the local Action Bridge through:

```cpp
#include "clc/sim/ActionBridge.hpp"
```

or the aggregate header:

```cpp
#include "clc/CityLifeCore.hpp"
```

The Action Bridge is part of the source-first C++ SDK surface. It is local and transport-agnostic; HTTP, WebSocket, auth, accounts, matchmaking, multiplayer and UI belong outside this layer.

---

## C ABI linking policy

The C ABI is the stable foreign-function boundary for non-C++ consumers.

C ABI consumers should include:

```c
#include "clc/c/CityLifeCoreC.h"
```

The current C ABI is intentionally minimal. It is not a replacement for the full C++ runtime API yet.

When linking a C executable to the installed SDK, use C++ as the final linker language because the implementation is C++:

```cmake
project(MyCityLifeCoreCConsumer LANGUAGES C CXX)
find_package(CityLifeCore CONFIG REQUIRED)

add_executable(my_consumer main.c)
set_target_properties(my_consumer PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(my_consumer PRIVATE CityLifeCore::core)
```

C ABI expansion should follow [`c-abi-expansion-plan.md`](c-abi-expansion-plan.md).

---

## Static and shared builds

City Life Core can be consumed as a normal CMake target. Whether the target is static or shared depends on the build configuration and downstream packaging requirements.

For normal native C++ usage, either static or shared linking can be acceptable depending on the product.

For plug-in style usage, prefer shared libraries:

```bash
cmake -S . -B build-plugin -DBUILD_SHARED_LIBS=ON -DCLC_BUILD_TESTS=OFF -DCLC_BUILD_EXAMPLES=OFF -DCLC_BUILD_TOOLS=OFF
cmake --build build-plugin --config Release
```

Typical shared library names:

- Windows: `city_life_core.dll`
- Linux: `libcity_life_core.so`
- macOS: `libcity_life_core.dylib`

The exact output directory depends on the generator and build configuration.

---

## Unity / C# linking policy

Unity/C# integrations should use P/Invoke over the C ABI:

```text
Unity C# code
  -> CityLifeCoreNative.cs
  -> C ABI
  -> native shared library
```

Unity projects must include the native shared library for the target platform in `Assets/Plugins` or another Unity-supported native plug-in location.

The C# files in `examples/csharp_unity/` are wrappers and smoke-test scripts. They do not replace the native shared library.

The C++ core must not include Unity-specific code or Unity headers.

---

## Browser / WebAssembly linking policy

Browser/WebAssembly support is planned, not implemented.

The intended shape is:

```text
Browser app
  -> JavaScript or TypeScript adapter
  -> WebAssembly module
  -> C ABI / exported functions
  -> native C++ core
```

Future WebAssembly work should:

- avoid browser dependencies inside the native core;
- expose stable functions through a C-compatible boundary where practical;
- document memory and string ownership between JavaScript and WebAssembly;
- keep browser-specific loading and UI code in the adapter layer.

---

## Packaging policy

The installed SDK should provide:

- public headers;
- the `CityLifeCore::core` CMake target;
- CMake package config files;
- documentation;
- examples;
- validation scripts;
- example data packs.

Installed examples may include native C++ consumers, Action Bridge examples, C ABI consumers and initial C# / Unity wrapper files.

For SDK ZIP packaging, see [`sdk-zip-package.md`](sdk-zip-package.md).

---

## Compatibility notes

- C++ source compatibility is preferred over C++ binary ABI guarantees.
- The local Action Bridge is a C++ source-level SDK surface, not a C ABI or network protocol.
- The C ABI should be versioned and kept intentionally stable where possible.
- C ABI surface changes should be reflected in the C interface version when consumers need to detect them.
- C# / Unity and future Browser/WASM wrappers should track the C ABI, not private C++ internals.
- Downstream package/distribution maintainers may define stronger binary compatibility rules for their own distributed builds.

---

## Non-goals

This policy does not promise:

- stable C++ binary ABI between arbitrary compiler/toolchain combinations;
- Unity package distribution yet;
- WebAssembly support yet;
- prebuilt native binaries for every platform;
- HTTP/WebSocket/auth/multiplayer behavior inside the local Action Bridge;
- engine-specific build systems inside the core.

---

## Related documents

- [Public API status](public-api-status.md)
- [Action Bridge](action-bridge.md)
- [Compatibility](compatibility.md)
- [CMake package](cmake-package.md)
- [Packaging](packaging.md)
- [SDK ZIP package](sdk-zip-package.md)
- [C ABI](c-abi.md)
- [C ABI expansion plan](c-abi-expansion-plan.md)
- [C# and Unity integration](csharp-unity.md)
- [Browser and WebAssembly integration](browser-wasm.md)
- [Integration targets](integration-targets.md)
