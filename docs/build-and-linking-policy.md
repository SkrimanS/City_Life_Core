# Build and Linking Policy

Version: **1.0.0**

City Life Core is designed for source-first and CMake-package integrations.

## Policy

- Prefer rebuilding downstream projects against the SDK version they consume.
- Use the installed `CityLifeCore::core` target instead of hard-coded library paths.
- Treat headers under `include/clc` as the supported public include surface.
- The minimal C interface is intentionally narrow and is not a replacement for the C++ runtime API.
- Binary compatibility guarantees should be defined by downstream package/distribution maintainers if required.

## Related documents

- [Public API status](public-api-status.md)
- [Compatibility](compatibility.md)
- [CMake package](cmake-package.md)
