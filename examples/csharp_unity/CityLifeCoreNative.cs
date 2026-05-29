using System;
using System.Runtime.InteropServices;

namespace CityLifeCore.Unity
{
    [StructLayout(LayoutKind.Sequential)]
    public struct CityLifeCoreVersion
    {
        public int Major;
        public int Minor;
        public int Patch;

        public override string ToString()
        {
            return $"{Major}.{Minor}.{Patch}";
        }
    }

    public readonly struct CityLifeWorldEvent
    {
        public CityLifeWorldEvent(ulong id, ulong tick, string type, string payload)
        {
            Id = id;
            Tick = tick;
            Type = type ?? string.Empty;
            Payload = payload ?? string.Empty;
        }

        public ulong Id { get; }
        public ulong Tick { get; }
        public string Type { get; }
        public string Payload { get; }
    }

    public static class CityLifeCoreNative
    {
        public const uint RequiredCInterfaceVersion = 4;

#if UNITY_IOS && !UNITY_EDITOR
        private const string LibraryName = "__Internal";
#else
        private const string LibraryName = "city_life_core";
#endif

        public static CityLifeCoreVersion Version => clc_core_version_c();
        public static string VersionString => PtrToString(clc_core_version_string_c());
        public static uint CInterfaceVersion => clc_c_interface_version_c();
        public static bool IsCInterfaceCompatible => TryCheckCInterfaceCompatibility(out _);

        public static ulong TicksPerSecond => clc_ticks_per_second_c();
        public static ulong TicksPerMinute => clc_ticks_per_minute_c();
        public static ulong TicksPerHour => clc_ticks_per_hour_c();
        public static ulong TicksPerDay => clc_ticks_per_day_c();

        public static bool TryGetCInterfaceVersion(out uint actualVersion)
        {
            try
            {
                actualVersion = CInterfaceVersion;
                return true;
            }
            catch (Exception)
            {
                actualVersion = 0;
                return false;
            }
        }

        public static bool TryCheckCInterfaceCompatibility(out uint actualVersion)
        {
            return TryGetCInterfaceVersion(out actualVersion) && actualVersion >= RequiredCInterfaceVersion;
        }

        public static bool TryGetVersionString(out string versionString)
        {
            try
            {
                versionString = VersionString;
                return true;
            }
            catch (Exception)
            {
                versionString = string.Empty;
                return false;
            }
        }

        public static bool TryGetTicksPerDay(out ulong ticksPerDay)
        {
            try
            {
                ticksPerDay = TicksPerDay;
                return true;
            }
            catch (Exception)
            {
                ticksPerDay = 0;
                return false;
            }
        }

        public static bool TryMinutesToTicks(ulong minutes, out ulong ticks)
        {
            try
            {
                ticks = MinutesToTicks(minutes);
                return true;
            }
            catch (Exception)
            {
                ticks = 0;
                return false;
            }
        }

        public static void EnsureCompatibleCInterface()
        {
            if (!TryGetCInterfaceVersion(out var actual))
            {
                throw new InvalidOperationException(
                    $"Failed to load City Life Core native library or read its C ABI version. Required C ABI version: {RequiredCInterfaceVersion}.");
            }

            if (actual < RequiredCInterfaceVersion)
            {
                throw new NotSupportedException(
                    $"City Life Core C ABI version {actual} is too old for this C# wrapper. Required C ABI version: {RequiredCInterfaceVersion}.");
            }
        }

        public static ulong SecondsToTicks(ulong seconds) => clc_seconds_to_ticks_c(seconds);
        public static ulong MinutesToTicks(ulong minutes) => clc_minutes_to_ticks_c(minutes);
        public static ulong HoursToTicks(ulong hours) => clc_hours_to_ticks_c(hours);
        public static ulong DaysToTicks(ulong days) => clc_days_to_ticks_c(days);

        public static bool CanConvertSecondsToTicks(ulong seconds) => clc_can_convert_seconds_to_ticks_c(seconds) != 0;
        public static bool CanConvertMinutesToTicks(ulong minutes) => clc_can_convert_minutes_to_ticks_c(minutes) != 0;
        public static bool CanConvertHoursToTicks(ulong hours) => clc_can_convert_hours_to_ticks_c(hours) != 0;
        public static bool CanConvertDaysToTicks(ulong days) => clc_can_convert_days_to_ticks_c(days) != 0;

        internal static IntPtr CreateWorld(string name, ulong seed)
        {
            EnsureCompatibleCInterface();
            return clc_world_create_c(name, seed);
        }

        internal static bool TryCreateWorld(string name, ulong seed, out IntPtr world)
        {
            world = IntPtr.Zero;
            if (!TryCheckCInterfaceCompatibility(out _))
            {
                return false;
            }

            try
            {
                world = clc_world_create_c(name, seed);
                return world != IntPtr.Zero;
            }
            catch (Exception)
            {
                world = IntPtr.Zero;
                return false;
            }
        }

        internal static void DestroyWorld(IntPtr world)
        {
            if (world == IntPtr.Zero)
            {
                return;
            }

            try
            {
                clc_world_destroy_c(world);
            }
            catch (Exception)
            {
            }
        }

        internal static string WorldName(IntPtr world) => PtrToString(clc_world_name_c(world));
        internal static ulong WorldSeed(IntPtr world) => clc_world_seed_c(world);
        internal static ulong WorldCurrentTick(IntPtr world) => clc_world_current_tick_c(world);
        internal static ulong WorldEventCount(IntPtr world) => clc_world_event_count_c(world);
        internal static bool AdvanceWorld(IntPtr world, ulong ticks) => clc_world_advance_c(world, ticks) != 0;
        internal static bool AdvanceWorldSeconds(IntPtr world, ulong seconds) => clc_world_advance_seconds_c(world, seconds) != 0;
        internal static bool AdvanceWorldMinutes(IntPtr world, ulong minutes) => clc_world_advance_minutes_c(world, minutes) != 0;
        internal static bool AdvanceWorldHours(IntPtr world, ulong hours) => clc_world_advance_hours_c(world, hours) != 0;
        internal static bool AdvanceWorldDays(IntPtr world, ulong days) => clc_world_advance_days_c(world, days) != 0;

        internal static ulong WorldEventId(IntPtr world, ulong index) => clc_world_event_id_c(world, index);
        internal static ulong WorldEventTick(IntPtr world, ulong index) => clc_world_event_tick_c(world, index);
        internal static string WorldEventType(IntPtr world, ulong index) => PtrToString(clc_world_event_type_c(world, index));
        internal static string WorldEventPayload(IntPtr world, ulong index) => PtrToString(clc_world_event_payload_c(world, index));

        private static string PtrToString(IntPtr ptr)
        {
            return ptr == IntPtr.Zero ? string.Empty : Marshal.PtrToStringAnsi(ptr) ?? string.Empty;
        }

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern CityLifeCoreVersion clc_core_version_c();

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr clc_core_version_string_c();

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern uint clc_c_interface_version_c();

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern ulong clc_ticks_per_second_c();

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern ulong clc_ticks_per_minute_c();

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern ulong clc_ticks_per_hour_c();

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern ulong clc_ticks_per_day_c();

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern int clc_can_convert_seconds_to_ticks_c(ulong seconds);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern int clc_can_convert_minutes_to_ticks_c(ulong minutes);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern int clc_can_convert_hours_to_ticks_c(ulong hours);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern int clc_can_convert_days_to_ticks_c(ulong days);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern ulong clc_seconds_to_ticks_c(ulong seconds);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern ulong clc_minutes_to_ticks_c(ulong minutes);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern ulong clc_hours_to_ticks_c(ulong hours);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern ulong clc_days_to_ticks_c(ulong days);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern IntPtr clc_world_create_c(string name, ulong seed);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern void clc_world_destroy_c(IntPtr world);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr clc_world_name_c(IntPtr world);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern ulong clc_world_seed_c(IntPtr world);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern ulong clc_world_current_tick_c(IntPtr world);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern ulong clc_world_event_count_c(IntPtr world);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern int clc_world_advance_c(IntPtr world, ulong ticks);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern int clc_world_advance_seconds_c(IntPtr world, ulong seconds);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern int clc_world_advance_minutes_c(IntPtr world, ulong minutes);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern int clc_world_advance_hours_c(IntPtr world, ulong hours);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern int clc_world_advance_days_c(IntPtr world, ulong days);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern ulong clc_world_event_id_c(IntPtr world, ulong index);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern ulong clc_world_event_tick_c(IntPtr world, ulong index);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr clc_world_event_type_c(IntPtr world, ulong index);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr clc_world_event_payload_c(IntPtr world, ulong index);
    }

    public sealed class CityLifeWorld : IDisposable
    {
        private IntPtr handle;

        private CityLifeWorld(IntPtr handle)
        {
            this.handle = handle;
        }

        public string Name => EnsureAliveAndGet(() => CityLifeCoreNative.WorldName(handle));
        public ulong Seed => EnsureAliveAndGet(() => CityLifeCoreNative.WorldSeed(handle));
        public ulong CurrentTick => EnsureAliveAndGet(() => CityLifeCoreNative.WorldCurrentTick(handle));
        public ulong EventCount => EnsureAliveAndGet(() => CityLifeCoreNative.WorldEventCount(handle));

        public static CityLifeWorld Create(string name, ulong seed)
        {
            var world = CityLifeCoreNative.CreateWorld(name, seed);
            if (world == IntPtr.Zero)
            {
                throw new InvalidOperationException("Failed to create City Life Core world.");
            }

            return new CityLifeWorld(world);
        }

        public static bool TryCreate(string name, ulong seed, out CityLifeWorld world)
        {
            world = null;
            if (!CityLifeCoreNative.TryCreateWorld(name, seed, out var handle))
            {
                return false;
            }

            world = new CityLifeWorld(handle);
            return true;
        }

        public bool TryAdvance(ulong ticks)
        {
            try
            {
                return IsAlive && CityLifeCoreNative.AdvanceWorld(handle, ticks);
            }
            catch (Exception)
            {
                return false;
            }
        }

        public bool TryAdvanceSeconds(ulong seconds)
        {
            try
            {
                return IsAlive && CityLifeCoreNative.AdvanceWorldSeconds(handle, seconds);
            }
            catch (Exception)
            {
                return false;
            }
        }

        public bool TryAdvanceMinutes(ulong minutes)
        {
            try
            {
                return IsAlive && CityLifeCoreNative.AdvanceWorldMinutes(handle, minutes);
            }
            catch (Exception)
            {
                return false;
            }
        }

        public bool TryAdvanceHours(ulong hours)
        {
            try
            {
                return IsAlive && CityLifeCoreNative.AdvanceWorldHours(handle, hours);
            }
            catch (Exception)
            {
                return false;
            }
        }

        public bool TryAdvanceDays(ulong days)
        {
            try
            {
                return IsAlive && CityLifeCoreNative.AdvanceWorldDays(handle, days);
            }
            catch (Exception)
            {
                return false;
            }
        }

        public void Advance(ulong ticks)
        {
            EnsureAdvanceSucceeded(TryAdvance(ticks));
        }

        public void AdvanceSeconds(ulong seconds)
        {
            EnsureAdvanceSucceeded(TryAdvanceSeconds(seconds));
        }

        public void AdvanceMinutes(ulong minutes)
        {
            EnsureAdvanceSucceeded(TryAdvanceMinutes(minutes));
        }

        public void AdvanceHours(ulong hours)
        {
            EnsureAdvanceSucceeded(TryAdvanceHours(hours));
        }

        public void AdvanceDays(ulong days)
        {
            EnsureAdvanceSucceeded(TryAdvanceDays(days));
        }

        public bool TryGetEvent(ulong index, out CityLifeWorldEvent worldEvent)
        {
            worldEvent = default;
            try
            {
                if (!IsAlive)
                {
                    return false;
                }

                var eventCount = CityLifeCoreNative.WorldEventCount(handle);
                if (index >= eventCount)
                {
                    return false;
                }

                worldEvent = new CityLifeWorldEvent(
                    CityLifeCoreNative.WorldEventId(handle, index),
                    CityLifeCoreNative.WorldEventTick(handle, index),
                    CityLifeCoreNative.WorldEventType(handle, index),
                    CityLifeCoreNative.WorldEventPayload(handle, index));
                return true;
            }
            catch (Exception)
            {
                worldEvent = default;
                return false;
            }
        }

        public CityLifeWorldEvent GetEvent(ulong index)
        {
            EnsureAlive();
            if (!TryGetEvent(index, out var worldEvent))
            {
                throw new ArgumentOutOfRangeException(nameof(index));
            }

            return worldEvent;
        }

        public void Dispose()
        {
            CityLifeCoreNative.DestroyWorld(handle);
            handle = IntPtr.Zero;
            GC.SuppressFinalize(this);
        }

        ~CityLifeWorld()
        {
            CityLifeCoreNative.DestroyWorld(handle);
        }

        private bool IsAlive => handle != IntPtr.Zero;

        private void EnsureAdvanceSucceeded(bool advanced)
        {
            EnsureAlive();
            if (!advanced)
            {
                throw new InvalidOperationException("Failed to advance City Life Core world.");
            }
        }

        private void EnsureAlive()
        {
            if (!IsAlive)
            {
                throw new ObjectDisposedException(nameof(CityLifeWorld));
            }
        }

        private T EnsureAliveAndGet<T>(Func<T> getter)
        {
            EnsureAlive();
            return getter();
        }
    }
}
