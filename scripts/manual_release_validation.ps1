param(
    [string]$BuildDir = "build-manual-release-validation"
)

$ErrorActionPreference = "Stop"

function Invoke-Step {
    param(
        [Parameter(Mandatory = $true)]
        [string]$FilePath,

        [Parameter(ValueFromRemainingArguments = $true)]
        [string[]]$Arguments
    )

    Write-Host ""
    Write-Host "+ $FilePath $($Arguments -join ' ')"
    & $FilePath @Arguments
    if ($LASTEXITCODE -ne 0) {
        throw "Command failed with exit code $LASTEXITCODE: $FilePath $($Arguments -join ' ')"
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

$RepoRoot = Resolve-Path (Join-Path $PSScriptRoot "..")
$BuildPath = Join-Path $RepoRoot $BuildDir
$InstallPrefix = Join-Path $BuildPath "install-prefix"
$ConsumerBuild = Join-Path $BuildPath "consumer-build"
$CAbiConsumerBuild = Join-Path $BuildPath "c-abi-consumer-build"
$ZipExtractDir = Join-Path $BuildPath "zip-extracted"
$ZipConsumerBuild = Join-Path $BuildPath "zip-consumer-build"
$ZipCAbiConsumerBuild = Join-Path $BuildPath "zip-c-abi-consumer-build"
$BenchmarkOutput = Join-Path $BuildPath "benchmark-output.txt"
$ChecksumsFile = Join-Path $BuildPath "SHA256SUMS.txt"

Write-Host "Repository: $RepoRoot"
Write-Host "Build dir:  $BuildPath"

Invoke-Step cmake -S $RepoRoot -B $BuildPath `
    -DCLC_BUILD_TESTS=ON `
    -DCLC_BUILD_EXAMPLES=ON `
    -DCLC_BUILD_BENCHMARKS=ON

Invoke-Step cmake --build $BuildPath --config Release
Invoke-Step ctest --test-dir $BuildPath --output-on-failure -C Release

$BenchmarkExe = Find-Executable @(
    (Join-Path $BuildPath "Release/clc_core_benchmarks.exe"),
    (Join-Path $BuildPath "clc_core_benchmarks.exe"),
    (Join-Path $BuildPath "clc_core_benchmarks")
)

Write-Host ""
Write-Host "+ $BenchmarkExe"
& $BenchmarkExe | Tee-Object -FilePath $BenchmarkOutput
if ($LASTEXITCODE -ne 0) {
    throw "Benchmark runner failed with exit code $LASTEXITCODE"
}

Invoke-Step cmake --install $BuildPath --config Release --prefix $InstallPrefix

Invoke-Step cmake -S (Join-Path $RepoRoot "examples/find_package_consumer") `
    -B $ConsumerBuild `
    -DCMAKE_PREFIX_PATH=$InstallPrefix
Invoke-Step cmake --build $ConsumerBuild --config Release
$ConsumerExe = Find-Executable @(
    (Join-Path $ConsumerBuild "Release/city_life_core_consumer.exe"),
    (Join-Path $ConsumerBuild "city_life_core_consumer.exe"),
    (Join-Path $ConsumerBuild "city_life_core_consumer")
)
Invoke-Step $ConsumerExe

Invoke-Step cmake -S (Join-Path $RepoRoot "examples/c_abi_consumer") `
    -B $CAbiConsumerBuild `
    -DCMAKE_PREFIX_PATH=$InstallPrefix
Invoke-Step cmake --build $CAbiConsumerBuild --config Release
$CAbiConsumerExe = Find-Executable @(
    (Join-Path $CAbiConsumerBuild "Release/city_life_core_c_abi_consumer.exe"),
    (Join-Path $CAbiConsumerBuild "city_life_core_c_abi_consumer.exe"),
    (Join-Path $CAbiConsumerBuild "city_life_core_c_abi_consumer")
)
Invoke-Step $CAbiConsumerExe

Invoke-Step cmake -E chdir $BuildPath cpack --config CPackConfig.cmake -G ZIP -C Release

$ZipFiles = Get-ChildItem -Path $BuildPath -Filter "city-life-core-sdk-*.zip" -File
if ($ZipFiles.Count -eq 0) {
    throw "No SDK ZIP package found in $BuildPath"
}
$ZipFile = $ZipFiles[0].FullName

$Hash = Get-FileHash -Algorithm SHA256 $ZipFile
"$($Hash.Hash.ToLowerInvariant())  $([System.IO.Path]::GetFileName($ZipFile))" | Set-Content -Path $ChecksumsFile -Encoding UTF8
Write-Host "SHA256 $([System.IO.Path]::GetFileName($ZipFile)) = $($Hash.Hash.ToLowerInvariant())"

if (Test-Path $ZipExtractDir) {
    Remove-Item -Recurse -Force $ZipExtractDir
}
New-Item -ItemType Directory -Path $ZipExtractDir | Out-Null
Invoke-Step cmake -E chdir $ZipExtractDir cmake -E tar xf $ZipFile

$SdkPrefix = Get-ChildItem -Path $ZipExtractDir -Directory | Select-Object -First 1
if ($null -eq $SdkPrefix) {
    throw "Could not locate unpacked SDK prefix"
}
$SdkPrefixPath = $SdkPrefix.FullName

Invoke-Step cmake -S (Join-Path $RepoRoot "examples/find_package_consumer") `
    -B $ZipConsumerBuild `
    -DCMAKE_PREFIX_PATH=$SdkPrefixPath
Invoke-Step cmake --build $ZipConsumerBuild --config Release
$ZipConsumerExe = Find-Executable @(
    (Join-Path $ZipConsumerBuild "Release/city_life_core_consumer.exe"),
    (Join-Path $ZipConsumerBuild "city_life_core_consumer.exe"),
    (Join-Path $ZipConsumerBuild "city_life_core_consumer")
)
Invoke-Step $ZipConsumerExe

Invoke-Step cmake -S (Join-Path $RepoRoot "examples/c_abi_consumer") `
    -B $ZipCAbiConsumerBuild `
    -DCMAKE_PREFIX_PATH=$SdkPrefixPath
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
