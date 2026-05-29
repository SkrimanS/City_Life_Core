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
            var status = CityLifeNativeDiagnostics.GetStatus();
            Debug.Log(status.Summary);

            if (!status.IsReady)
            {
                Debug.LogError(status.FailureReason);
                return;
            }

            var safeSeed = seed < 0 ? 0UL : (ulong)seed;
            var safeAdvanceMinutes = advanceMinutes < 0 ? 0UL : (ulong)advanceMinutes;

            if (!CityLifeWorld.TryCreate(worldName, safeSeed, out world))
            {
                Debug.LogError($"Failed to create City Life Core world '{worldName}'.");
                return;
            }

            var createdName = world.TryGetName(out var actualWorldName) ? actualWorldName : "unknown";
            var createdSeed = world.TryGetSeed(out var actualSeed) ? actualSeed.ToString() : "unknown";
            Debug.Log($"Created world '{createdName}' with seed {createdSeed}.");

            if (!world.TryAdvanceMinutes(safeAdvanceMinutes))
            {
                Debug.LogError($"Failed to advance world by {safeAdvanceMinutes} minute(s).");
                return;
            }

            var ticksText = CityLifeCoreNative.TryMinutesToTicks(safeAdvanceMinutes, out var ticks) ? ticks.ToString() : "unknown";
            var currentTickText = world.TryGetCurrentTick(out var currentTick) ? currentTick.ToString() : "unknown";
            var eventCount = world.TryGetEventCount(out var count) ? count : 0UL;

            Debug.Log($"Advanced world by {safeAdvanceMinutes} minute(s) / {ticksText} ticks. Current tick: {currentTickText}.");
            Debug.Log($"World event count: {eventCount}.");

            for (ulong i = 0; i < eventCount; ++i)
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
