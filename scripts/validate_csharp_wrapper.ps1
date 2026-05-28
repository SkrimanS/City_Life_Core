$ErrorActionPreference = "Stop"

$RepoRoot = Resolve-Path (Join-Path $PSScriptRoot "..")
$Project = Join-Path $RepoRoot "examples/csharp_unity/CityLifeCoreNative.CompileCheck.csproj"

$Dotnet = Get-Command dotnet -ErrorAction SilentlyContinue
if (-not $Dotnet) {
    Write-Error "dotnet SDK is required to validate the C# wrapper."
}

dotnet build $Project -c Release
