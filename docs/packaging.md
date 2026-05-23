# Packaging

Version: **1.0.0**

City Life Core can be built from source, installed as a CMake package and packaged as an SDK ZIP archive.

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

## Consume after install

```cmake
find_package(CityLifeCore CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE CityLifeCore::core)
```

## Related documents

- [CMake package](cmake-package.md)
- [SDK ZIP package](sdk-zip-package.md)
- [SDK structure](sdk-structure.md)
