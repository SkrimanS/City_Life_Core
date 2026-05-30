param(
    [string]$BuildDir = "build-manual-release-validation"
)

$ErrorActionPreference = "Stop"

function Invoke-Step {
    if ($args.Count -lt 1) {
        throw "Invoke-Step requires an executable name"
    }

    $FilePath = [string]$args[0]
    $Arguments = @()
    if ($args.Count -gt 1) {
        $Arguments = @($args[1..($args.Count - 1)] | ForEach-Object { [string]$_ })
    }

    Write-Host ""
    Write-Host "+ $FilePath $($Arguments -join ' ')"
    & $FilePath @Arguments
    if ($LASTEXITCODE -ne 0) {
        throw "Command failed with exit code $($LASTEXITCODE): $FilePath $($Arguments -join ' ')"
    }
}

function Find-Executable {
    param(
        [Parameter(Mandatory = $true)]
        [string[]]$Candidates
    )

    foreach ($Candidate in $Candidates) {
        if (Test-Path $Candidate) {
            return (Resolve-Path $Candidate).Path
        }
    }

    throw "Executable not found. Candidates: $($Candidates -join ', ')"
}

function Remove-TreeIfExists {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Path
    )

    if (Test-Path $Path) {
        Remove-Item -Recurse -Force $Path
    }
}

function Require-File {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Path,
        [Parameter(Mandatory = $true)]
        [string]$Description
    )

    if (!(Test-Path $Path)) {
        throw "Missing ${Description}: ${Path}"
    }
}

function Find-CSharpCompileProject {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Prefix
    )

    $Projects = @(Get-ChildItem -Path $Prefix -Recurse -File -Filter "CityLifeCoreNative.CompileCheck.csproj" -ErrorAction SilentlyContinue |
        Where-Object { $_.FullName -like "*examples*csharp_unity*" } |
        Select-Object -First 1)

    if ($Projects.Count -eq 0) {
        return $null
    }

    return $Projects[0].FullName
}

function Find-InstalledScript {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Prefix,
        [Parameter(Mandatory = $true)]
        [string]$ScriptName
    )

    $Scripts = @(Get-ChildItem -Path $Prefix -Recurse -File -Filter $ScriptName -ErrorAction SilentlyContinue |
        Where-Object { $_.FullName -like "*scripts*$ScriptName" } |
        Select-Object -First 1)

    if ($Scripts.Count -eq 0) {
        return $null
    }

    return $Scripts[0].FullName
}

function Require-InstalledScript {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Prefix,
        [Parameter(Mandatory = $true)]
        [string]$ScriptName,
        [Parameter(Mandatory = $true)]
        [string]$Description
    )

    $ScriptPath = Find-InstalledScript -Prefix $Prefix -ScriptName $ScriptName
    if ([string]::IsNullOrWhiteSpace($ScriptPath)) {
        throw "Missing ${Description} under ${Prefix}"
    }
    Require-File -Path $ScriptPath -Description $Description
}

$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$BuildPath = Join-Path $RepoRoot $BuildDir
$SharedBuildPath = Join-Path $BuildPath "shared-build"
$InstallPrefix = Join-Path $BuildPath "install-prefix"
$ConsumerBuild = Join-Path $BuildPath "consumer-build"
$CAbiConsumerBuild = Join-Path $BuildPath "c-abi-consumer-build"
$ZipExtractDir = Join-Path $BuildPath "zip-extracted"
$ZipConsumerBuild = Join-Path $BuildPath "zip-consumer-build"
$ZipCAbiConsumerBuild = Join-Path $BuildPath "zip-c-abi-consumer-build"
$BenchmarkOutput = Join-Path $BuildPath "benchmark-output.txt"
$ChecksumsFile = Join-Path $BuildPath "SHA256SUMS.txt"

if ($BuildPath -eq $RepoRoot -or $BuildPath -eq (Split-Path -Path $RepoRoot -Parent)) {
    throw "Refusing to clean unsafe build directory: $BuildPath"
}

Remove-TreeIfExists $BuildPath
New-Item -ItemType Directory -Path $BuildPath | Out-Null

Write-Host "Repository: $RepoRoot"
Write-Host "Build dir:  $BuildPath"

Invoke-Step cmake -S $RepoRoot -B $BuildPath `
    -DCMAKE_BUILD_TYPE=Release `
    -DCLC_BUILD_TESTS=ON `
    -DCLC_BUILD_EXAMPLES=ON `
    -DCLC_BUILD_BENCHMARKS=ON

Invoke-Step cmake --build $BuildPath --config Release
Invoke-Step ctest --test-dir $BuildPath --output-on-failure -C Release

Invoke-Step cmake -S $RepoRoot -B $SharedBuildPath `
    -DCMAKE_BUILD_TYPE=Release `
    -DBUILD_SHARED_LIBS=ON `
    -DCLC_BUILD_TESTS=OFF `
    -DCLC_BUILD_EXAMPLES=OFF `
    -DCLC_BUILD_BENCHMARKS=OFF `
    -DCLC_BUILD_TOOLS=OFF
Invoke-Step cmake --build $SharedBuildPath --config Release

Invoke-Step bash (Join-Path $RepoRoot "scripts/validate_csharp_wrapper.sh")

$BenchmarkExe = Find-Executable @(
    (Join-Path $BuildPath "Release/clc_core_benchmarks.exe"),
    (Join-Path $BuildPath "clc_core_benchmarks.exe"),
    (Join-Path $BuildPath "clc_core_benchmarks")
)

Write-Host ""
Write-Host "+ $BenchmarkExe"
& $BenchmarkExe | Tee-Object -FilePath $BenchmarkOutput
if ($LASTEXITCODE -ne 0) {
    throw "Benchmark runner failed with exit code $($LASTEXITCODE)"
}

Invoke-Step cmake --install $BuildPath --config Release --prefix $InstallPrefix

$InstalledConfig = Join-Path $InstallPrefix "lib/cmake/CityLifeCore/CityLifeCoreConfig.cmake"
Require-File -Path $InstalledConfig -Description "installed CMake package config"

$InstalledCSharpProject = Find-CSharpCompileProject -Prefix $InstallPrefix
if ([string]::IsNullOrWhiteSpace($InstalledCSharpProject)) {
    throw "Installed C# wrapper compile-check project was not found under $InstallPrefix"
}
Require-File -Path $InstalledCSharpProject -Description "installed C# wrapper compile-check project"
Invoke-Step dotnet build $InstalledCSharpProject -c Release
Require-InstalledScript -Prefix $InstallPrefix -ScriptName "validate_csharp_wrapper.sh" -Description "installed C# wrapper validation shell script"
Require-InstalledScript -Prefix $InstallPrefix -ScriptName "validate_csharp_wrapper.ps1" -Description "installed C# wrapper validation PowerShell script"

Invoke-Step cmake -S (Join-Path $RepoRoot "examples/find_package_consumer") `
    -B $ConsumerBuild `
    -DCMAKE_BUILD_TYPE=Release `
    "-DCMAKE_PREFIX_PATH=$InstallPrefix"
Invoke-Step cmake --build $ConsumerBuild --config Release
$ConsumerExe = Find-Executable @(
    (Join-Path $ConsumerBuild "Release/city_life_core_consumer.exe"),
    (Join-Path $ConsumerBuild "city_life_core_consumer.exe"),
    (Join-Path $ConsumerBuild "city_life_core_consumer")
)
Invoke-Step $ConsumerExe

Invoke-Step cmake -S (Join-Path $RepoRoot "examples/c_abi_consumer") `
    -B $CAbiConsumerBuild `
    -DCMAKE_BUILD_TYPE=Release `
    "-DCMAKE_PREFIX_PATH=$InstallPrefix"
Invoke-Step cmake --build $CAbiConsumerBuild --config Release
$CAbiConsumerExe = Find-Executable @(
    (Join-Path $CAbiConsumerBuild "Release/city_life_core_c_abi_consumer.exe"),
    (Join-Path $CAbiConsumerBuild "city_life_core_c_abi_consumer.exe"),
    (Join-Path $CAbiConsumerBuild "city_life_core_c_abi_consumer")
)
Invoke-Step $CAbiConsumerExe

Invoke-Step cmake -E chdir $BuildPath cpack --config CPackConfig.cmake -G ZIP -C Release

$ZipFiles = @(Get-ChildItem -Path $BuildPath -Filter "city-life-core-sdk-*.zip" -File | Sort-Object LastWriteTime -Descending)
if ($ZipFiles.Count -ne 1) {
    throw "Expected exactly one SDK ZIP package in $BuildPath, found $($ZipFiles.Count)"
}
$ZipFile = $ZipFiles[0].FullName

$Hash = Get-FileHash -Algorithm SHA256 $ZipFile
"$($Hash.Hash.ToLowerInvariant())  $([System.IO.Path]::GetFileName($ZipFile))" | Set-Content -Path $ChecksumsFile -Encoding UTF8
Write-Host "SHA256 $([System.IO.Path]::GetFileName($ZipFile)) = $($Hash.Hash.ToLowerInvariant())"

New-Item -ItemType Directory -Path $ZipExtractDir | Out-Null
Invoke-Step cmake -E chdir $ZipExtractDir cmake -E tar xf $ZipFile

$SdkPrefixes = @(Get-ChildItem -Path $ZipExtractDir -Directory)
if ($SdkPrefixes.Count -ne 1) {
    throw "Expected exactly one unpacked SDK prefix in $ZipExtractDir, found $($SdkPrefixes.Count)"
}
$SdkPrefixPath = $SdkPrefixes[0].FullName

$UnpackedConfig = Join-Path $SdkPrefixPath "lib/cmake/CityLifeCore/CityLifeCoreConfig.cmake"
Require-File -Path $UnpackedConfig -Description "unpacked CMake package config"

$ZipCSharpProject = Find-CSharpCompileProject -Prefix $SdkPrefixPath
if ([string]::IsNullOrWhiteSpace($ZipCSharpProject)) {
    throw "Unpacked SDK C# wrapper compile-check project was not found under $SdkPrefixPath"
}
Require-File -Path $ZipCSharpProject -Description "unpacked SDK C# wrapper compile-check project"
Invoke-Step dotnet build $ZipCSharpProject -c Release
Require-InstalledScript -Prefix $SdkPrefixPath -ScriptName "validate_csharp_wrapper.sh" -Description "unpacked SDK C# wrapper validation shell script"
Require-InstalledScript -Prefix $SdkPrefixPath -ScriptName "validate_csharp_wrapper.ps1" -Description "unpacked SDK C# wrapper validation PowerShell script"

Invoke-Step cmake -S (Join-Path $RepoRoot "examples/find_package_consumer") `
    -B $ZipConsumerBuild `
    -DCMAKE_BUILD_TYPE=Release `
    "-DCMAKE_PREFIX_PATH=$SdkPrefixPath"
Invoke-Step cmake --build $ZipConsumerBuild --config Release
$ZipConsumerExe = Find-Executable @(
    (Join-Path $ZipConsumerBuild "Release/city_life_core_consumer.exe"),
    (Join-Path $ZipConsumerBuild "city_life_core_consumer.exe"),
    (Join-Path $ZipConsumerBuild "city_life_core_consumer")
)
Invoke-Step $ZipConsumerExe

Invoke-Step cmake -S (Join-Path $RepoRoot "examples/c_abi_consumer") `
    -B $ZipCAbiConsumerBuild `
    -DCMAKE_BUILD_TYPE=Release `
    "-DCMAKE_PREFIX_PATH=$SdkPrefixPath"
Invoke-Step cmake --build $ZipCAbiConsumerBuild --config Release
$ZipCAbiConsumerExe = Find-Executable @(
    (Join-Path $ZipCAbiConsumerBuild "Release/city_life_core_c_abi_consumer.exe"),
    (Join-Path $ZipCAbiConsumerBuild "city_life_core_c_abi_consumer.exe"),
    (Join-Path $ZipCAbiConsumerBuild "city_life_core_c_abi_consumer")
)
Invoke-Step $ZipCAbiConsumerExe

Write-Host ""
Write-Host "Manual release validation completed successfully."
Write-Host "Benchmark output: $BenchmarkOutput"
Write-Host "Checksums:        $ChecksumsFile"
Write-Host "SDK ZIP:          $ZipFile"
Write-Host "Unpacked prefix:  $SdkPrefixPath"
Write-Host "Shared build:     $SharedBuildPath"
Write-Host "C# check project: $InstalledCSharpProject"
