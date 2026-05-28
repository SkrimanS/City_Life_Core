# C# and Unity Integration

Status: **planned / initial guidance for v1.1.0**

City Life Core is a native C++ SDK. C# and Unity integrations should use the stable C ABI layer instead of calling the C++ API directly.

The recommended integration path is:

1. Build City Life Core as a shared native library.
2. Place the native library in a Unity `Assets/Plugins` folder for the target platform.
3. Add a small C# P/Invoke wrapper that imports the C ABI functions.
4. Build Unity gameplay, tools or editor workflows on top of that wrapper.

This keeps the core engine-agnostic and allows Unity to consume it as a native plug-in.

---

## Why C ABI first

The public C++ API is source-first and does not promise binary ABI stability. C# and Unity need a stable unmanaged boundary. The C ABI provides that boundary.

The current C ABI exposes:

- version utilities;
- C interface version;
- tick conversion helpers;
- an opaque `clc_world` handle;
- basic world state access;
- simple tick advancement;
- read-only world event inspection.

Header:

```c
#include "clc/c/CityLifeCoreC.h"
```

Example C consumer:

```text
examples/c_abi_consumer/
```

Initial C# wrapper example:

```text
examples/csharp_unity/CityLifeCoreNative.cs
```

Optional Unity smoke-test component:

```text
examples/csharp_unity/CityLifeSmokeTest.cs
```

---

## Build a native plug-in library

For Unity, build City Life Core as a shared library:

```bash
cmake -S . -B build-unity -DBUILD_SHARED_LIBS=ON -DCLC_BUILD_TESTS=OFF -DCLC_BUILD_EXAMPLES=OFF -DCLC_BUILD_TOOLS=OFF
cmake --build build-unity --config Release
```

Installable SDK build:

```bash
cmake -S . -B build-unity-sdk -DBUILD_SHARED_LIBS=ON -DCLC_BUILD_TESTS=OFF -DCLC_BUILD_EXAMPLES=OFF -DCLC_BUILD_TOOLS=OFF -DCMAKE_INSTALL_PREFIX=/path/to/city-life-core-sdk
cmake --build build-unity-sdk --config Release
cmake --install build-unity-sdk --config Release
```

Typical output names:

- Windows: `city_life_core.dll`
- Linux: `libcity_life_core.so`
- macOS: `libcity_life_core.dylib`

The exact output location depends on the generator, platform and install prefix.

---

## Unity project layout

A simple Unity layout can look like this:

```text
Assets/
  Plugins/
    x86_64/
      city_life_core.dll          # Windows example
    Linux/
      libcity_life_core.so        # Linux example
    macOS/
      libcity_life_core.dylib     # macOS example
  Scripts/
    CityLifeCoreNative.cs
    CityLifeSmokeTest.cs          # optional smoke test
```

For iOS-style static linking, Unity commonly uses `__Internal` as the import name. The sample wrapper handles that through platform defines.

---

## Minimal C# usage

The example folder includes `CityLifeSmokeTest.cs`, which can be attached to an empty Unity GameObject to verify that the native plug-in loads and that the C ABI wrapper can create and advance a world.

```csharp
using CityLifeCore.Unity;
using UnityEngine;

public sealed class CityLifeSmokeTest : MonoBehaviour
{
    private CityLifeWorld world;

    private void Start()
    {
        Debug.Log($"City Life Core {CityLifeCoreNative.VersionString}");
        Debug.Log($"Ticks per day: {CityLifeCoreNative.TicksPerDay}");

        world = CityLifeWorld.Create("Unity Demo World", 42);
        world.Advance(300);

        for (ulong i = 0; i < world.EventCount; ++i)
        {
            var ev = world.GetEvent(i);
            Debug.Log($"{ev.Id}:{ev.Tick}:{ev.Type}:{ev.Payload}");
        }
    }

    private void OnDestroy()
    {
        world?.Dispose();
        world = null;
    }
}
```

---

## Ownership rules

The C ABI world handle is native memory. C# callers must release it when it is no longer needed.

Use `CityLifeWorld.Dispose()` or a `using` statement for non-Unity test code:

```csharp
using var world = CityLifeWorld.Create("Tool World", 7);
world.Advance(CityLifeCoreNative.MinutesToTicks(5));
```

Do not store raw native pointers in gameplay code. Keep them inside a managed wrapper class.

---

## String rules

The C ABI currently returns `const char*` strings owned by the native library or native world handle. The C# wrapper copies those strings into managed `string` values.

Do not attempt to free returned string pointers from C#.

---

## Troubleshooting

### `DllNotFoundException: city_life_core`

Check that the native library is present under `Assets/Plugins` for the current Unity editor/player platform.

Typical names:

- Windows: `city_life_core.dll`
- Linux: `libcity_life_core.so`
- macOS: `libcity_life_core.dylib`

Also check the plug-in import settings in the Unity Inspector. The native plug-in must be enabled for the platform you are running.

### `EntryPointNotFoundException`

This usually means Unity found a native library, but it does not export the expected C ABI symbol.

Check that:

- the library was built from the same City Life Core version as the C# wrapper;
- the C ABI implementation was included in the native build;
- the library exports functions such as `clc_core_version_string_c` and `clc_world_create_c`;
- the C# wrapper function names match `include/clc/c/CityLifeCoreC.h`.

### Architecture mismatch

Unity Editor and the native plug-in must use compatible architectures. For modern desktop Unity editor builds, this usually means a 64-bit native library.

Examples:

- 64-bit Windows Unity Editor needs a 64-bit `.dll`;
- Linux Unity Editor needs a matching `.so`;
- macOS Unity Editor needs a matching `.dylib` or bundle-compatible native plug-in.

### Native dependency missing

If the plug-in exists but still fails to load, one of its native dependencies may be missing.

Check platform-specific dependency tools, for example:

```text
Windows: dumpbin /DEPENDENTS or Dependencies.exe
Linux:   ldd
macOS:   otool -L
```

### Crash or invalid native handle

Keep native handles inside `CityLifeWorld` and call `Dispose()` when done. Do not copy raw `IntPtr` values into gameplay scripts.

If a crash happens after domain reloads or scene changes, make sure Unity objects dispose their native world in `OnDestroy()`.

---

## Scope for the first Unity support stage

Initial C# / Unity support should remain intentionally small:

- C# P/Invoke wrapper for the existing C ABI;
- Unity smoke-test guidance and `MonoBehaviour` smoke-test example;
- native plug-in packaging notes;
- no Unity dependency in the C++ core;
- no generated C# package until the C ABI is wider and more stable.

Future work may add:

- a Unity package layout;
- editor import validation;
- prebuilt native artifacts per platform;
- C# tests for wrapper behavior;
- higher-level managed APIs for runtime scenarios;
- expanded C ABI coverage for settlements, resources, contracts, persistence and replay.

---

## Non-goals

The C# / Unity layer should not turn City Life Core into a Unity-specific framework.

The core should remain:

- headless;
- renderer-independent;
- network-stack-independent;
- usable by non-Unity games, servers, tools and editors.
