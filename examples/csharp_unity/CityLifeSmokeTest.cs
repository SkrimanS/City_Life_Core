using CityLifeCore.Unity;
using UnityEngine;

namespace CityLifeCore.Unity.Examples
{
    /// <summary>
    /// Minimal Unity smoke test for the City Life Core native plug-in.
    ///
    /// Attach this component to an empty GameObject after copying:
    /// - the native City Life Core shared library into Assets/Plugins for your platform;
    /// - CityLifeCoreNative.cs into a Unity Scripts folder.
    /// </summary>
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
            Debug.Log($"City Life Core version: {CityLifeCoreNative.VersionString}");
            Debug.Log($"C ABI version: {CityLifeCoreNative.CInterfaceVersion} / required: {CityLifeCoreNative.RequiredCInterfaceVersion}");
            Debug.Log($"C ABI compatible: {CityLifeCoreNative.IsCInterfaceCompatible}");
            Debug.Log($"Ticks per day: {CityLifeCoreNative.TicksPerDay}");

            CityLifeCoreNative.EnsureCompatibleCInterface();

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

            var ticks = CityLifeCoreNative.MinutesToTicks(safeAdvanceMinutes);
            Debug.Log($"Advanced world by {safeAdvanceMinutes} minute(s) / {ticks} ticks. Current tick: {world.CurrentTick}.");
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
