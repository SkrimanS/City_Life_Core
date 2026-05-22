# SDK ZIP Package / SDK ZIP архив

Version: **0.9.9**

City Life Core can produce an install-layout SDK archive through CPack.

City Life Core может собрать SDK архив с install-layout через CPack.

---

## Build ZIP package

```bash
cmake -S . -B build-sdk-zip -DCLC_BUILD_TESTS=OFF -DCLC_BUILD_EXAMPLES=OFF -DCLC_BUILD_TOOLS=OFF
cmake --build build-sdk-zip --config Release
cpack --config build-sdk-zip/CPackConfig.cmake -G ZIP
cmake -E sha256sum city-life-core-sdk-*.zip > SHA256SUMS.txt
```

The generated archive name follows this pattern:

```text
city-life-core-sdk-<version>-<system>-<processor>.zip
```

Example:

```text
city-life-core-sdk-0.9.9-Linux-x86_64.zip
```

Checksum file:

```text
SHA256SUMS.txt
```

---

## Expected archive layout

The archive contains the CMake install layout:

```text
city-life-core-sdk-0.9.9-<system>-<processor>/
  include/
    clc/
      CityLifeCore.hpp
      c/
      core/
      data/
      economy/
      sim/
  lib/
    libcity_life_core.*
    cmake/CityLifeCore/
      CityLifeCoreConfig.cmake
      CityLifeCoreConfigVersion.cmake
      CityLifeCoreTargets.cmake
  share/
    doc/CityLifeCore/
    CityLifeCore/data/
```

`lib/` can be `lib64/` or another platform/toolchain-specific GNUInstallDirs path.

---

## Consume ZIP package

Unpack the archive and pass the unpacked prefix to CMake:

```bash
unzip city-life-core-sdk-0.9.9-Linux-x86_64.zip -d /tmp/city-life-core-sdk
cmake -S my_game -B build -DCMAKE_PREFIX_PATH=/tmp/city-life-core-sdk/city-life-core-sdk-0.9.9-Linux-x86_64
cmake --build build
```

External project CMake:

```cmake
find_package(CityLifeCore CONFIG REQUIRED)
target_link_libraries(my_game PRIVATE CityLifeCore::core)
```

Recommended C++ include:

```cpp
#include "clc/CityLifeCore.hpp"
```

Recommended C ABI include:

```c
#include "clc/c/CityLifeCoreC.h"
```

---

## Verify ZIP package checksum

After downloading or copying the archive, verify the checksum:

```bash
cmake -E sha256sum city-life-core-sdk-0.9.9-Linux-x86_64.zip
cat SHA256SUMS.txt
```

The hash printed for the ZIP should match the hash in `SHA256SUMS.txt`.

---

## Verify ZIP package with bundled consumer

After unpacking, build the standalone consumer example against the installed package:

```bash
cmake -S examples/find_package_consumer -B build-consumer -DCMAKE_PREFIX_PATH=/tmp/city-life-core-sdk/city-life-core-sdk-0.9.9-Linux-x86_64
cmake --build build-consumer
./build-consumer/city_life_core_consumer
```

This checks:

- installed public headers;
- exported CMake target `CityLifeCore::core`;
- package config files;
- minimal runtime tick execution from an external project.

---

## CI artifact and validation

The CI workflow builds the ZIP package and uploads both:

```text
city-life-core-sdk-*.zip
SHA256SUMS.txt
```

CI also unpacks the generated ZIP and builds `examples/find_package_consumer` against the unpacked archive prefix. This verifies that the archive itself is consumable, not only the intermediate `cmake --install` directory.

---

## Release notes

A release should publish the ZIP archive together with a manifest/checksum file. See:

```text
docs/RELEASE_MANIFEST_TEMPLATE.md
```
