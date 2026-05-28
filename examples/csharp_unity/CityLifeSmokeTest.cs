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
        private ulong seed = 42;

        [SerializeField]
        private ulong advanceMinutes = 5;

        private CityLifeWorld world;

        private void Start()
        {
            Debug.Log($"City Life Core version: {CityLifeCoreNative.VersionString}");
            Debug.Log($"C ABI version: {CityLifeCoreNative.CInterfaceVersion}");
            Debug.Log($"Ticks per day: {CityLifeCoreNative.TicksPerDay}");

            world = CityLifeWorld.Create(worldName, seed);
            Debug.Log($"Created world '{world.Name}' with seed {world.Seed}.");

            var ticks = CityLifeCoreNative.MinutesToTicks(advanceMinutes);
            world.Advance(ticks);

            Debug.Log($"Advanced world by {ticks} ticks. Current tick: {world.CurrentTick}.");
            Debug.Log($"World event count: {world.EventCount}.");

            for (ulong i = 0; i < world.EventCount; ++i)
            {
                var ev = world.GetEvent(i);
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
