# C Interface / C ABI

Version: **4.0.0**
C interface version: **8**

City Life Core is primarily a C++20 SDK. A small C-facing interface is provided for version and time utilities, a minimal opaque world handle, read-only world event access and simple FFI smoke checks.

The C ABI is also the planned stable foreign-function boundary for C#, Unity, WebAssembly, scripting languages and other non-C++ integrations. See [`c-abi-expansion-plan.md`](c-abi-expansion-plan.md) for the staged expansion plan.

## Header

```c
#include "clc/c/CityLifeCoreC.h"
```

The header is installed with the SDK under `include/clc/c/CityLifeCoreC.h`.

## Scope

The C interface currently covers:

- version string and numeric version helpers;
- tick/time conversion helpers;
- opaque `clc_world` create/destroy;
- basic world state access;
- simple tick advancement;
- world advancement by seconds, minutes, hours or days;
- read-only world event count and indexed event access;
- read-only SDK handoff diagnostics for profile count, profile ids, global readiness and profile-specific handoff digest;
- read-only core completion, deep replay coverage, runtime core systems, regional simulation, SDK handoff manifest and platform diagnostics digests.

Full gameplay/runtime integration should use the C++ API until the relevant C ABI surfaces are expanded and stabilized.

## CMake consumer note

When linking a C executable to `CityLifeCore::core`, set the linker language to C++ because the SDK implementation is C++:

```cmake
project(MyCityLifeCoreCConsumer LANGUAGES C CXX)
find_package(CityLifeCore CONFIG REQUIRED)

add_executable(my_consumer main.c)
set_target_properties(my_consumer PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(my_consumer PRIVATE CityLifeCore::core)
```

## Related documents

- [Public API](public-api.md)
- [C ABI expansion plan](c-abi-expansion-plan.md)
- [C# and Unity integration](csharp-unity.md)
- [Browser and WebAssembly integration](browser-wasm.md)
- [Integration targets](integration-targets.md)
- [CMake package](cmake-package.md)
- [Compatibility](compatibility.md)
