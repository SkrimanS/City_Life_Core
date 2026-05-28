# C# / Unity consumer example

This folder contains an initial managed wrapper for using the City Life Core C ABI from C# and Unity.

The wrapper is intentionally small. It demonstrates how Unity can call the native City Life Core library through P/Invoke without making the C++ core depend on Unity.

## Files

| File | Purpose |
| --- | --- |
| `CityLifeCoreNative.cs` | C# P/Invoke declarations and a small safe wrapper around the current `clc_world` C ABI handle. |
| `CityLifeSmokeTest.cs` | Optional Unity `MonoBehaviour` smoke test that creates a world, advances it and prints events to the Unity console. |

## Build the native library

Build City Life Core as a shared native library:

```bash
cmake -S . -B build-unity -DBUILD_SHARED_LIBS=ON -DCLC_BUILD_TESTS=OFF -DCLC_BUILD_EXAMPLES=OFF -DCLC_BUILD_TOOLS=OFF
cmake --build build-unity --config Release
```

Typical output names:

- Windows: `city_life_core.dll`
- Linux: `libcity_life_core.so`
- macOS: `libcity_life_core.dylib`

## Unity layout

Copy the native library, wrapper and optional smoke test into a Unity project:

```text
Assets/
  Plugins/
    x86_64/
      city_life_core.dll
  Scripts/
    CityLifeCoreNative.cs
    CityLifeSmokeTest.cs
```

For Linux or macOS, use the matching native library for that platform.

## Minimal Unity usage

Attach `CityLifeSmokeTest` to an empty GameObject and enter Play Mode. The script should:

- print the native City Life Core version;
- print the C ABI version;
- create a native world;
- advance it by a small number of ticks;
- print any world events returned through the C ABI.

Equivalent minimal code:

```csharp
using CityLifeCore.Unity;
using UnityEngine;

public sealed class CityLifeSmokeTest : MonoBehaviour
{
    private CityLifeWorld world;

    private void Start()
    {
        Debug.Log($"City Life Core {CityLifeCoreNative.VersionString}");

        world = CityLifeWorld.Create("Unity Demo World", 42);
        world.Advance(CityLifeCoreNative.MinutesToTicks(5));

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

## Current scope

The current wrapper exposes only the existing minimal C ABI:

- version information;
- tick conversion helpers;
- opaque world create/destroy;
- world name, seed, current tick and event count;
- simple world advancement;
- read-only event access.

Future C# support should expand only after the native C ABI is expanded and stabilized.
