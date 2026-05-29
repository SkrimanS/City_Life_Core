#!/usr/bin/env bash
set -euo pipefail

usage() {
  cat <<'EOF'
Usage: scripts/manual_release_validation.sh [build-dir]

Runs the local release validation flow:
  - configure/build with tests, examples and benchmarks enabled
  - ctest
  - shared core build with BUILD_SHARED_LIBS=ON
  - C# wrapper compile validation from the source tree
  - benchmark runner
  - cmake --install into a local prefix
  - installed C++ find_package consumer
  - installed C ABI consumer
  - installed C# wrapper compile-check project presence and build
  - installed validation script presence
  - CPack ZIP package
  - SHA256SUMS.txt generation
  - unpacked ZIP C++ consumer
  - unpacked ZIP C ABI consumer
  - unpacked ZIP C# wrapper compile-check project presence and build
  - unpacked ZIP validation script presence

The optional build-dir defaults to build-manual-release-validation.
EOF
}

if [[ "${1:-}" == "-h" || "${1:-}" == "--help" ]]; then
  usage
  exit 0
fi

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
build_dir="${1:-build-manual-release-validation}"
build_path="${repo_root}/${build_dir}"
shared_build_path="${build_path}/shared-build"
install_prefix="${build_path}/install-prefix"
consumer_build="${build_path}/consumer-build"
c_abi_consumer_build="${build_path}/c-abi-consumer-build"
zip_extract_dir="${build_path}/zip-extracted"
zip_consumer_build="${build_path}/zip-consumer-build"
zip_c_abi_consumer_build="${build_path}/zip-c-abi-consumer-build"
benchmark_output="${build_path}/benchmark-output.txt"
checksums_file="${build_path}/SHA256SUMS.txt"

run() {
  echo
  echo "+ $*"
  "$@"
}

find_executable() {
  local unix_path="$1"
  local windows_path="$2"
  if [[ -x "${unix_path}" ]]; then
    printf '%s\n' "${unix_path}"
    return 0
  fi
  if [[ -x "${windows_path}" ]]; then
    printf '%s\n' "${windows_path}"
    return 0
  fi
  return 1
}

require_file() {
  local path="$1"
  local description="$2"
  if [[ ! -f "${path}" ]]; then
    echo "Missing ${description}: ${path}" >&2
    exit 1
  fi
}

find_csharp_compile_project() {
  local prefix="$1"
  find "${prefix}" -type f -path '*/examples/csharp_unity/CityLifeCoreNative.CompileCheck.csproj' | head -n 1
}

find_installed_script() {
  local prefix="$1"
  local script_name="$2"
  find "${prefix}" -type f -path "*/scripts/${script_name}" | head -n 1
}

require_installed_script() {
  local prefix="$1"
  local script_name="$2"
  local description="$3"
  local script_path
  script_path="$(find_installed_script "${prefix}" "${script_name}")"
  if [[ -z "${script_path}" ]]; then
    echo "Missing ${description} under ${prefix}" >&2
    exit 1
  fi
  require_file "${script_path}" "${description}"
}

case "${build_path}" in
  "${repo_root}"|"${repo_root}/"|"/")
    echo "Refusing to clean unsafe build directory: ${build_path}" >&2
    exit 1
    ;;
esac

rm -rf "${build_path}"

mkdir -p "${build_path}"

echo "Repository: ${repo_root}"
echo "Build dir:  ${build_path}"

run cmake -S "${repo_root}" -B "${build_path}" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCLC_BUILD_TESTS=ON \
  -DCLC_BUILD_EXAMPLES=ON \
  -DCLC_BUILD_BENCHMARKS=ON

run cmake --build "${build_path}" --config Release
run ctest --test-dir "${build_path}" --output-on-failure -C Release

run cmake -S "${repo_root}" -B "${shared_build_path}" \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=ON \
  -DCLC_BUILD_TESTS=OFF \
  -DCLC_BUILD_EXAMPLES=OFF \
  -DCLC_BUILD_BENCHMARKS=OFF \
  -DCLC_BUILD_TOOLS=OFF
run cmake --build "${shared_build_path}" --config Release

run bash "${repo_root}/scripts/validate_csharp_wrapper.sh"

benchmark_exe="$(find_executable \
  "${build_path}/clc_core_benchmarks" \
  "${build_path}/Release/clc_core_benchmarks.exe")"
run "${benchmark_exe}" | tee "${benchmark_output}"

run cmake --install "${build_path}" --config Release --prefix "${install_prefix}"

installed_csharp_project="$(find_csharp_compile_project "${install_prefix}")"
if [[ -z "${installed_csharp_project}" ]]; then
  echo "Installed C# wrapper compile-check project was not found under ${install_prefix}" >&2
  exit 1
fi
require_file "${installed_csharp_project}" "installed C# wrapper compile-check project"
run dotnet build "${installed_csharp_project}" -c Release
require_installed_script "${install_prefix}" "validate_csharp_wrapper.sh" "installed C# wrapper validation shell script"
require_installed_script "${install_prefix}" "validate_csharp_wrapper.ps1" "installed C# wrapper validation PowerShell script"

run cmake -S "${repo_root}/examples/find_package_consumer" \
  -B "${consumer_build}" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="${install_prefix}"
run cmake --build "${consumer_build}" --config Release
consumer_exe="$(find_executable \
  "${consumer_build}/city_life_core_consumer" \
  "${consumer_build}/Release/city_life_core_consumer.exe")"
run "${consumer_exe}"

run cmake -S "${repo_root}/examples/c_abi_consumer" \
  -B "${c_abi_consumer_build}" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="${install_prefix}"
run cmake --build "${c_abi_consumer_build}" --config Release
c_abi_consumer_exe="$(find_executable \
  "${c_abi_consumer_build}/city_life_core_c_abi_consumer" \
  "${c_abi_consumer_build}/Release/city_life_core_c_abi_consumer.exe")"
run "${c_abi_consumer_exe}"

run cmake -E chdir "${build_path}" cpack --config CPackConfig.cmake -G ZIP -C Release
zip_file="$(find "${build_path}" -maxdepth 1 -type f -name 'city-life-core-sdk-*.zip' | head -n 1)"
if [[ -z "${zip_file}" ]]; then
  echo "No SDK ZIP package found in ${build_path}" >&2
  exit 1
fi
zip_basename="$(basename "${zip_file}")"
run cmake -E chdir "${build_path}" sha256sum "${zip_basename}"
cmake -E chdir "${build_path}" sha256sum "${zip_basename}" > "${checksums_file}"

rm -rf "${zip_extract_dir}"
mkdir -p "${zip_extract_dir}"

run cmake -E chdir "${zip_extract_dir}" cmake -E tar xf "${zip_file}"
sdk_prefix="$(find "${zip_extract_dir}" -mindepth 1 -maxdepth 1 -type d | head -n 1)"
if [[ -z "${sdk_prefix}" ]]; then
  echo "Could not locate unpacked SDK prefix" >&2
  exit 1
fi

zip_csharp_project="$(find_csharp_compile_project "${sdk_prefix}")"
if [[ -z "${zip_csharp_project}" ]]; then
  echo "Unpacked SDK C# wrapper compile-check project was not found under ${sdk_prefix}" >&2
  exit 1
fi
require_file "${zip_csharp_project}" "unpacked SDK C# wrapper compile-check project"
run dotnet build "${zip_csharp_project}" -c Release
require_installed_script "${sdk_prefix}" "validate_csharp_wrapper.sh" "unpacked SDK C# wrapper validation shell script"
require_installed_script "${sdk_prefix}" "validate_csharp_wrapper.ps1" "unpacked SDK C# wrapper validation PowerShell script"

run cmake -S "${repo_root}/examples/find_package_consumer" \
  -B "${zip_consumer_build}" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="${sdk_prefix}"
run cmake --build "${zip_consumer_build}" --config Release
zip_consumer_exe="$(find_executable \
  "${zip_consumer_build}/city_life_core_consumer" \
  "${zip_consumer_build}/Release/city_life_core_consumer.exe")"
run "${zip_consumer_exe}"

run cmake -S "${repo_root}/examples/c_abi_consumer" \
  -B "${zip_c_abi_consumer_build}" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="${sdk_prefix}"
run cmake --build "${zip_c_abi_consumer_build}" --config Release
zip_c_abi_consumer_exe="$(find_executable \
  "${zip_c_abi_consumer_build}/city_life_core_c_abi_consumer" \
  "${zip_c_abi_consumer_build}/Release/city_life_core_c_abi_consumer.exe")"
run "${zip_c_abi_consumer_exe}"

echo
echo "Manual release validation completed successfully."
echo "Benchmark output: ${benchmark_output}"
echo "Checksums:        ${checksums_file}"
echo "SDK ZIP:          ${zip_file}"
echo "Unpacked prefix:  ${sdk_prefix}"
echo "Shared build:     ${shared_build_path}"
echo "C# check project: ${installed_csharp_project}"
