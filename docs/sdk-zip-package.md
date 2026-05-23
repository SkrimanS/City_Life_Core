# SDK ZIP Package

Version: **1.0.0**

The SDK ZIP package is an install-layout archive produced by CPack.

## Build ZIP package

```bash
cmake -S . -B build-sdk-zip -DCLC_BUILD_TESTS=OFF -DCLC_BUILD_EXAMPLES=OFF -DCLC_BUILD_TOOLS=OFF
cmake --build build-sdk-zip --config Release
cpack --config build-sdk-zip/CPackConfig.cmake -G ZIP
cmake -E sha256sum city-life-core-sdk-*.zip > SHA256SUMS.txt
```

## Consume ZIP package

Unpack the archive and point `CMAKE_PREFIX_PATH` at the unpacked install prefix. Consumers can then use:

```cmake
find_package(CityLifeCore CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE CityLifeCore::core)
```

## Related documents

- [Packaging](packaging.md)
- [CMake package](cmake-package.md)
- [Release verification](verifying-releases.md)
