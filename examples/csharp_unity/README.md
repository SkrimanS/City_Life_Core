# C# / Unity consumer example

This folder contains an initial managed wrapper for using the City Life Core C ABI from C# and Unity.

The wrapper is intentionally small. It demonstrates how Unity can call the native City Life Core library through P/Invoke without making the C++ core depend on Unity.

## Files

| File | Purpose |
| --- | --- |
| `CityLifeCoreNative.cs` | C# P/Invoke declarations and a small safe wrapper around the current `clc_world` C ABI handle. |
| `CityLifeWorldSafeAccess.cs` | Optional extension helpers for non-throwing world property reads in Unity gameplay code. |
| `CityLifeSmokeTest.cs` | Optional Unity `MonoBehaviour` smoke test that creates a world, advances it and prints events to the Unity console. |
| `CityLifeCoreNative.CompileCheck.csproj` | Minimal .NET project used to compile-check the wrapper outside Unity. |

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

The root CMake project enables position-independent code for the core target and Windows symbol auto-export to make this shared-library path friendlier for native plug-in scenarios.

## Validate the C# wrapper

The wrapper can be compile-checked outside Unity with the .NET SDK:

```bash
scripts/validate_csharp_wrapper.sh
```

On Windows PowerShell:

```powershell
scripts/validate_csharp_wrapper.ps1
```

Equivalent direct command:

```bash
dotnet build examples/csharp_unity/CityLifeCoreNative.CompileCheck.csproj -c Release
```

This check validates the managed wrapper syntax and public managed API. It does not load the native library and does not replace Unity Play Mode validation.

## Unity layout

Copy the native library, wrapper, safe-access helpers and optional smoke test into a Unity project:

```text
Assets/
  Plugins/
    x86_64/
      city_life_core.dll
  Scripts/
    CityLifeCoreNative.cs
    CityLifeWorldSafeAccess.cs
    CityLifeSmokeTest.cs
```

For Linux or macOS, use the matching native library for that platform.

## Minimal Unity usage

Attach `CityLifeSmokeTest` to an empty GameObject and enter Play Mode. The script should:

- soft-check that the native plug-in can be loaded;
- print the native City Life Core version;
- print the actual and required C ABI versions;
- create a native world;
- advance it by a small number of minutes;
- read world name, seed, current tick and event count through safe helpers;
- print any world events returned through the C ABI;
- log a Unity error instead of throwing for normal gameplay-style failures.

Equivalent minimal non-throwing code:

```csharp
using CityLifeCore.Unity;
using UnityEngine;

public sealed class CityLifeSmokeTest : MonoBehaviour
{
    private CityLifeWorld world;

    private void Start()
    {
        if (!CityLifeCoreNative.TryGetCInterfaceVersion(out var actualAbiVersion) ||
            !CityLifeCoreNative.TryCheckCInterfaceCompatibility(out actualAbiVersion))
        {
            Debug.LogError($"City Life Core native plug-in is missing or incompatible. Required C ABI: {CityLifeCoreNative.RequiredCInterfaceVersion}.");
            return;
        }

        var versionText = CityLifeCoreNative.TryGetVersionString(out var versionString) ? versionString : "unknown";
        Debug.Log($"City Life Core {versionText}");
        Debug.Log($"C ABI {actualAbiVersion} / required {CityLifeCoreNative.RequiredCInterfaceVersion}");

        if (!CityLifeWorld.TryCreate("Unity Demo World", 42, out world))
        {
            Debug.LogError("Failed to create City Life Core world.");
            return;
        }

        var worldNameText = world.TryGetName(out var worldName) ? worldName : "unknown";
        var seedText = world.TryGetSeed(out var seed) ? seed.ToString() : "unknown";
        Debug.Log($"Created world '{worldNameText}' with seed {seedText}.");

        if (!world.TryAdvanceMinutes(5))
        {
            Debug.LogError("Failed to advance City Life Core world.");
            return;
        }

        var eventCount = world.TryGetEventCount(out var count) ? count : 0UL;
        for (ulong i = 0; i < eventCount; ++i)
        {
            if (!world.TryGetEvent(i, out var ev))
            {
                Debug.LogError($"Failed to read City Life Core event at index {i}.");
                continue;
            }

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

Throwing helpers such as `CityLifeWorld.Create`, `AdvanceMinutes`, `GetEvent` and direct properties such as `Name`, `Seed`, `CurrentTick` and `EventCount` are still available for editor tooling, tests or code that prefers exceptions.

## Current scope

The current wrapper exposes only the existing minimal C ABI:

- version information;
- C ABI compatibility checks;
- tick conversion helpers;
- opaque world create/destroy;
- world name, seed, current tick and event count;
- safe managed property read helpers for Unity gameplay code;
- simple world advancement by ticks, seconds, minutes, hours or days;
- read-only event access.

Future C# support should expand only after the native C ABI is expanded and stabilized.
