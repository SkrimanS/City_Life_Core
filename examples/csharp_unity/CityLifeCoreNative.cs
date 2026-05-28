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
#if UNITY_IOS && !UNITY_EDITOR
        private const string LibraryName = "__Internal";
#else
        private const string LibraryName = "city_life_core";
#endif

        public static CityLifeCoreVersion Version => clc_core_version_c();
        public static string VersionString => PtrToString(clc_core_version_string_c());
        public static uint CInterfaceVersion => clc_c_interface_version_c();

        public static ulong TicksPerSecond => clc_ticks_per_second_c();
        public static ulong TicksPerMinute => clc_ticks_per_minute_c();
        public static ulong TicksPerHour => clc_ticks_per_hour_c();
        public static ulong TicksPerDay => clc_ticks_per_day_c();

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
            return clc_world_create_c(name, seed);
        }

        internal static void DestroyWorld(IntPtr world)
        {
            if (world != IntPtr.Zero)
            {
                clc_world_destroy_c(world);
            }
        }

        internal static string WorldName(IntPtr world) => PtrToString(clc_world_name_c(world));
        internal static ulong WorldSeed(IntPtr world) => clc_world_seed_c(world);
        internal static ulong WorldCurrentTick(IntPtr world) => clc_world_current_tick_c(world);
        internal static ulong WorldEventCount(IntPtr world) => clc_world_event_count_c(world);
        internal static bool AdvanceWorld(IntPtr world, ulong ticks) => clc_world_advance_c(world, ticks) != 0;

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

        public void Advance(ulong ticks)
        {
            EnsureAlive();
            if (!CityLifeCoreNative.AdvanceWorld(handle, ticks))
            {
                throw new InvalidOperationException("Failed to advance City Life Core world.");
            }
        }

        public CityLifeWorldEvent GetEvent(ulong index)
        {
            EnsureAlive();
            if (index >= EventCount)
            {
                throw new ArgumentOutOfRangeException(nameof(index));
            }

            return new CityLifeWorldEvent(
                CityLifeCoreNative.WorldEventId(handle, index),
                CityLifeCoreNative.WorldEventTick(handle, index),
                CityLifeCoreNative.WorldEventType(handle, index),
                CityLifeCoreNative.WorldEventPayload(handle, index));
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

        private void EnsureAlive()
        {
            if (handle == IntPtr.Zero)
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
