# Release Manifest 3.0.0

Status: release-candidate manifest with local Windows/MSVC validation recorded.

Included:

- `include/clc/sim/LargeWorld.hpp`
- `src/clc/sim/LargeWorld.cpp`
- completed-core APIs for runtime orchestration, save/replay fixtures, core systems audit, SDK handoff manifest and game developer handoff catalog.
- `docs/large-world-foundation.md`
- completed-core handoff docs and examples.
- shared v2-to-v3 tests and example.

## Validation

Local Windows/MSVC validation recorded on 2026-06-04:

- [x] Configure passed.
- [x] Build passed.
- [x] Tests passed: 72/72.
- [x] Examples built.
- [x] New `clc_example_multiplayer_to_large_world` example ran successfully.
- [x] Installed SDK generated.
- [x] Installed C++ consumer configured, built and ran.
- [x] Installed C ABI consumer configured, built and ran.
- [x] SDK ZIP generated.
- [x] Public C++ version reports `3.0.0`.
- [x] C ABI interface version is `8` with read-only SDK handoff, runtime, regional, replay, completion, package-manifest and platform diagnostics.
- [x] Completed-core library target built after runtime orchestration, save/replay fixtures, systems audit and handoff catalog additions.
- [x] New focused targets built: SDK manifest, runtime processor orchestration, save/replay fixtures, core systems audit and handoff catalog tests/examples.
- [x] C# wrapper compile-check passed for C ABI v7.
- [x] Installed SDK regenerated after completed-core additions.
- [x] SDK ZIP regenerated after completed-core additions: `build-codex-v2/city-life-core-sdk-3.0.0-Windows.zip`.
- [x] Full `ctest` execution after completed-core and C ABI v8 additions: 85/85 passed.

Commands:

```powershell
cmake -S . -B build-codex-v2 -DCLC_BUILD_TESTS=ON -DCLC_BUILD_EXAMPLES=ON -DCLC_BUILD_BENCHMARKS=ON
cmake --build build-codex-v2 --config Release
ctest --test-dir build-codex-v2 --output-on-failure -C Release
.\build-codex-v2\examples\Release\clc_example_multiplayer_to_large_world.exe
cmake --install build-codex-v2 --config Release --prefix build-codex-v3\install
cmake -S examples\find_package_consumer -B build-codex-v3-consumer -DCMAKE_PREFIX_PATH=C:\Users\Yuta\Desktop\City_Life_Core-1.5.0\build-codex-v3\install
cmake --build build-codex-v3-consumer --config Release
.\build-codex-v3-consumer\Release\city_life_core_consumer.exe
cmake -S examples\c_abi_consumer -B build-codex-v3-c-abi-consumer -DCMAKE_PREFIX_PATH=C:\Users\Yuta\Desktop\City_Life_Core-1.5.0\build-codex-v3\install
cmake --build build-codex-v3-c-abi-consumer --config Release
.\build-codex-v3-c-abi-consumer\Release\city_life_core_c_abi_consumer.exe
cmake --build build-codex-v2 --target package --config Release
```

Notes:

- Tag creation was not performed.
- Generated package: `build-codex-v2/city-life-core-sdk-3.0.0-Windows.zip`.
