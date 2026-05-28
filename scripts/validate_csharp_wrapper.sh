#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
project="${repo_root}/examples/csharp_unity/CityLifeCoreNative.CompileCheck.csproj"

if ! command -v dotnet >/dev/null 2>&1; then
    echo "dotnet SDK is required to validate the C# wrapper." >&2
    exit 1
fi

dotnet build "${project}" -c Release
