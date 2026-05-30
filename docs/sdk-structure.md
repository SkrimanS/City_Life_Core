# SDK Structure

Version: **1.0.0**

City Life Core is distributed as a source tree, an installable CMake package and an optional SDK ZIP package.

## Repository layout

```text
include/     Public C++ and C headers
src/         SDK implementation
data/        Example `.clcd` data packs
examples/    Source-tree and installed-package consumers, including Action Bridge, C ABI and C# / Unity examples
tests/       Unit and integration tests
benchmarks/  Optional benchmark target
scripts/     Manual validation helpers
docs/        English documentation
docs/ru/     Russian documentation
```

## Installed layout

The installed SDK provides:

- headers under the install include directory;
- `CityLifeCore::core` CMake target;
- package config files under the CMake package directory;
- documentation under the install documentation directory;
- examples and data files when installed by the package configuration;
- validation scripts used by integration and release checks.

Installed examples may include:

```text
examples/action_bridge.cpp
examples/find_package_consumer/
examples/c_abi_consumer/
examples/csharp_unity/
```

The Action Bridge example demonstrates local JSON action validation and dispatch through the C++ SDK. The C# / Unity example files are managed wrappers and smoke-test scripts for the C ABI. They are not a replacement for the native library. Unity and other managed runtimes still need the native City Life Core shared library for the target platform.

## Integration modes

- **Source-first**: add the repository to a workspace and link the target directly.
- **Installed package**: use `find_package(CityLifeCore CONFIG REQUIRED)`.
- **SDK ZIP**: unpack the CPack ZIP and point `CMAKE_PREFIX_PATH` to the unpacked prefix.
- **Action Bridge**: send local JSON actions into runtime validation and mutation without coupling an external layer to runtime internals.
- **C ABI consumer**: call the C interface from C or another language/runtime that can bind C-compatible functions.
- **Unity / C# initial wrapper**: use the C ABI through P/Invoke and a platform-specific native plug-in.
- **Browser / WebAssembly planned adapter**: future Emscripten/WASM build and JavaScript or TypeScript adapter.

## Related documents

- [Action Bridge](action-bridge.md)
- [CMake package](cmake-package.md)
- [Packaging](packaging.md)
- [SDK ZIP package](sdk-zip-package.md)
- [Integration targets](integration-targets.md)
- [C ABI expansion plan](c-abi-expansion-plan.md)
- [C# and Unity integration](csharp-unity.md)
- [Browser and WebAssembly integration](browser-wasm.md)
