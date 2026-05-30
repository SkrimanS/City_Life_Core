$ErrorActionPreference = "Stop"

$RepoRoot = Resolve-Path (Join-Path $PSScriptRoot "..")
$Project = Join-Path $RepoRoot "examples/csharp_unity/CityLifeCoreNative.CompileCheck.csproj"

$Dotnet = Get-Command dotnet -ErrorAction SilentlyContinue
if (-not $Dotnet) {
    throw "dotnet SDK is required to validate the C# wrapper."
}

dotnet build $Project -c Release
if ($LASTEXITCODE -ne 0) {
    throw "C# wrapper compile check failed with exit code $LASTEXITCODE."
}
