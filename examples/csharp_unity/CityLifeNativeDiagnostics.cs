namespace CityLifeCore.Unity
{
    public readonly struct CityLifeNativeStatus
    {
        public CityLifeNativeStatus(
            bool nativeLibraryLoaded,
            bool cInterfaceCompatible,
            uint requiredCInterfaceVersion,
            uint actualCInterfaceVersion,
            string versionString,
            ulong ticksPerDay)
        {
            NativeLibraryLoaded = nativeLibraryLoaded;
            CInterfaceCompatible = cInterfaceCompatible;
            RequiredCInterfaceVersion = requiredCInterfaceVersion;
            ActualCInterfaceVersion = actualCInterfaceVersion;
            VersionString = versionString ?? string.Empty;
            TicksPerDay = ticksPerDay;
        }

        public bool NativeLibraryLoaded { get; }
        public bool CInterfaceCompatible { get; }
        public uint RequiredCInterfaceVersion { get; }
        public uint ActualCInterfaceVersion { get; }
        public string VersionString { get; }
        public ulong TicksPerDay { get; }

        public bool IsReady => NativeLibraryLoaded && CInterfaceCompatible;

        public string FailureReason
        {
            get
            {
                if (!NativeLibraryLoaded)
                {
                    return "City Life Core native library is missing or its C ABI version cannot be read.";
                }

                if (!CInterfaceCompatible)
                {
                    return $"City Life Core C ABI version {ActualCInterfaceVersion} is older than required version {RequiredCInterfaceVersion}.";
                }

                return string.Empty;
            }
        }

        public string Summary
        {
            get
            {
                var versionText = string.IsNullOrEmpty(VersionString) ? "unknown" : VersionString;
                var ticksPerDayText = TicksPerDay == 0 ? "unknown" : TicksPerDay.ToString();
                var readinessText = IsReady ? "ready" : "not ready";
                return $"City Life Core native status: {readinessText}; version={versionText}; C ABI={ActualCInterfaceVersion}/{RequiredCInterfaceVersion}; ticksPerDay={ticksPerDayText}.";
            }
        }

        public override string ToString()
        {
            return Summary;
        }
    }

    public static class CityLifeNativeDiagnostics
    {
        public static CityLifeNativeStatus GetStatus()
        {
            var nativeLibraryLoaded = CityLifeCoreNative.TryGetCInterfaceVersion(out var actualCInterfaceVersion);
            var cInterfaceCompatible = nativeLibraryLoaded &&
                CityLifeCoreNative.TryCheckCInterfaceCompatibility(out actualCInterfaceVersion);
            var versionString = CityLifeCoreNative.TryGetVersionString(out var nativeVersionString)
                ? nativeVersionString
                : string.Empty;
            var ticksPerDay = CityLifeCoreNative.TryGetTicksPerDay(out var nativeTicksPerDay)
                ? nativeTicksPerDay
                : 0UL;

            return new CityLifeNativeStatus(
                nativeLibraryLoaded,
                cInterfaceCompatible,
                CityLifeCoreNative.RequiredCInterfaceVersion,
                actualCInterfaceVersion,
                versionString,
                ticksPerDay);
        }
    }
}
