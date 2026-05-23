# SDK Structure

Version: **1.0.0**

City Life Core is distributed as a source tree, an installable CMake package and an optional SDK ZIP package.

## Repository layout

```text
include/     Public C++ and C headers
src/         SDK implementation
data/        Example `.clcd` data packs
examples/    Source-tree and installed-package consumers
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
- examples and data files when installed by the package configuration.

## Integration modes

- **Source-first**: add the repository to a workspace and link the target directly.
- **Installed package**: use `find_package(CityLifeCore CONFIG REQUIRED)`.
- **SDK ZIP**: unpack the CPack ZIP and point `CMAKE_PREFIX_PATH` to the unpacked prefix.

## Related documents

- [CMake package](cmake-package.md)
- [Packaging](packaging.md)
- [SDK ZIP package](sdk-zip-package.md)
