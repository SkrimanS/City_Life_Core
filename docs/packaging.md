# Packaging

Version: **1.0.0**

City Life Core can be built from source, installed as a CMake package and packaged as an SDK ZIP archive.

The installed SDK is intended for native C++ consumers first. It also includes documentation and examples for C ABI and initial C# / Unity integration guidance.

## Build and test

```bash
cmake -S . -B build -DCLC_BUILD_TESTS=ON -DCLC_BUILD_EXAMPLES=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Install

```bash
cmake -S . -B build -DCLC_BUILD_TESTS=OFF -DCLC_BUILD_EXAMPLES=OFF -DCMAKE_INSTALL_PREFIX=/path/to/city-life-core-sdk
cmake --build build
cmake --install build
```

The install step provides:

- public C++ and C headers;
- the `CityLifeCore::core` CMake target;
- CMake package config files;
- documentation;
- example source files, including C++, C and initial C# / Unity examples;
- example `.clcd` data packs.

## Consume after install

```cmake
find_package(CityLifeCore CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE CityLifeCore::core)
```

## C ABI and C# / Unity notes

C, C# / Unity, WebAssembly and other non-C++ integrations should build on the C ABI instead of binding directly to the C++ API.

Relevant installed examples include:

```text
examples/c_abi_consumer/
examples/csharp_unity/
```

Unity projects still need a platform-specific shared native library placed in the Unity `Assets/Plugins` tree. The C# files are examples/wrappers; they do not replace the native library.

For Unity-specific guidance, see [C# and Unity integration](csharp-unity.md).

## Shared library build for native plug-ins

For Unity or other native plug-in use, build with shared libraries enabled:

```bash
cmake -S . -B build-plugin -DBUILD_SHARED_LIBS=ON -DCLC_BUILD_TESTS=OFF -DCLC_BUILD_EXAMPLES=OFF -DCLC_BUILD_TOOLS=OFF
cmake --build build-plugin --config Release
```

Typical shared library names:

- Windows: `city_life_core.dll`
- Linux: `libcity_life_core.so`
- macOS: `libcity_life_core.dylib`

## Related documents

- [CMake package](cmake-package.md)
- [SDK ZIP package](sdk-zip-package.md)
- [SDK structure](sdk-structure.md)
- [Integration targets](integration-targets.md)
- [C ABI expansion plan](c-abi-expansion-plan.md)
- [C# and Unity integration](csharp-unity.md)
