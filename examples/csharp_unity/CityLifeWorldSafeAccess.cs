using System;

namespace CityLifeCore.Unity
{
    public static class CityLifeWorldSafeAccess
    {
        public static bool TryGetName(this CityLifeWorld world, out string name)
        {
            try
            {
                if (world == null)
                {
                    name = string.Empty;
                    return false;
                }

                name = world.Name;
                return true;
            }
            catch (Exception)
            {
                name = string.Empty;
                return false;
            }
        }

        public static bool TryGetSeed(this CityLifeWorld world, out ulong seed)
        {
            try
            {
                if (world == null)
                {
                    seed = 0;
                    return false;
                }

                seed = world.Seed;
                return true;
            }
            catch (Exception)
            {
                seed = 0;
                return false;
            }
        }

        public static bool TryGetCurrentTick(this CityLifeWorld world, out ulong currentTick)
        {
            try
            {
                if (world == null)
                {
                    currentTick = 0;
                    return false;
                }

                currentTick = world.CurrentTick;
                return true;
            }
            catch (Exception)
            {
                currentTick = 0;
                return false;
            }
        }

        public static bool TryGetEventCount(this CityLifeWorld world, out ulong eventCount)
        {
            try
            {
                if (world == null)
                {
                    eventCount = 0;
                    return false;
                }

                eventCount = world.EventCount;
                return true;
            }
            catch (Exception)
            {
                eventCount = 0;
                return false;
            }
        }
    }
}
