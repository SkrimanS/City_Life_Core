using CityLifeCore.Unity;
using UnityEngine;

namespace CityLifeCore.Unity.Examples
{
    public sealed class CityLifeSmokeTest : MonoBehaviour
    {
        [SerializeField]
        private string worldName = "Unity Demo World";

        [SerializeField]
        private int seed = 42;

        [SerializeField]
        private int advanceMinutes = 5;

        private CityLifeWorld world;

        private void Start()
        {
            if (!CityLifeCoreNative.TryGetCInterfaceVersion(out var actualAbiVersion))
            {
                Debug.LogError($"Failed to load City Life Core native library or read its C ABI version. Required C ABI version: {CityLifeCoreNative.RequiredCInterfaceVersion}.");
                return;
            }

            var compatible = CityLifeCoreNative.TryCheckCInterfaceCompatibility(out actualAbiVersion);
            var versionText = CityLifeCoreNative.TryGetVersionString(out var versionString) ? versionString : "unknown";
            var ticksPerDayText = CityLifeCoreNative.TryGetTicksPerDay(out var ticksPerDay) ? ticksPerDay.ToString() : "unknown";

            Debug.Log($"City Life Core version: {versionText}");
            Debug.Log($"C ABI version: {actualAbiVersion} / required: {CityLifeCoreNative.RequiredCInterfaceVersion}");
            Debug.Log($"C ABI compatible: {compatible}");
            Debug.Log($"Ticks per day: {ticksPerDayText}");

            if (!compatible)
            {
                Debug.LogError($"City Life Core native library C ABI version {actualAbiVersion} is not compatible with this wrapper.");
                return;
            }

            var safeSeed = seed < 0 ? 0UL : (ulong)seed;
            var safeAdvanceMinutes = advanceMinutes < 0 ? 0UL : (ulong)advanceMinutes;

            if (!CityLifeWorld.TryCreate(worldName, safeSeed, out world))
            {
                Debug.LogError($"Failed to create City Life Core world '{worldName}'.");
                return;
            }

            Debug.Log($"Created world '{world.Name}' with seed {world.Seed}.");

            if (!world.TryAdvanceMinutes(safeAdvanceMinutes))
            {
                Debug.LogError($"Failed to advance world by {safeAdvanceMinutes} minute(s).");
                return;
            }

            var ticksText = CityLifeCoreNative.TryMinutesToTicks(safeAdvanceMinutes, out var ticks) ? ticks.ToString() : "unknown";
            Debug.Log($"Advanced world by {safeAdvanceMinutes} minute(s) / {ticksText} ticks. Current tick: {world.CurrentTick}.");
            Debug.Log($"World event count: {world.EventCount}.");

            for (ulong i = 0; i < world.EventCount; ++i)
            {
                if (!world.TryGetEvent(i, out var ev))
                {
                    Debug.LogError($"Failed to read City Life Core event at index {i}.");
                    continue;
                }

                Debug.Log($"CityLifeEvent id={ev.Id} tick={ev.Tick} type={ev.Type} payload={ev.Payload}");
            }
        }

        private void OnDestroy()
        {
            world?.Dispose();
            world = null;
        }
    }
}
