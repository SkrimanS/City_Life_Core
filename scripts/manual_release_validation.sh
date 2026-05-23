#!/usr/bin/env bash
set -euo pipefail

usage() {
  cat <<'EOF'
Usage: scripts/manual_release_validation.sh [build-dir]

Runs the local release validation flow:
  - configure/build with tests, examples and benchmarks enabled
  - ctest
  - benchmark runner
  - cmake --install into a local prefix
  - installed C++ find_package consumer
  - installed C ABI consumer
  - CPack ZIP package
  - SHA256SUMS.txt generation
  - unpacked ZIP C++ consumer
  - unpacked ZIP C ABI consumer

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

echo "Repository: ${repo_root}"
echo "Build dir:  ${build_path}"

run cmake -S "${repo_root}" -B "${build_path}" \
  -DCLC_BUILD_TESTS=ON \
  -DCLC_BUILD_EXAMPLES=ON \
  -DCLC_BUILD_BENCHMARKS=ON

run cmake --build "${build_path}" --config Release
run ctest --test-dir "${build_path}" --output-on-failure -C Release

benchmark_exe="$(find_executable \
  "${build_path}/clc_core_benchmarks" \
  "${build_path}/Release/clc_core_benchmarks.exe")"
run "${benchmark_exe}" | tee "${benchmark_output}"

run cmake --install "${build_path}" --config Release --prefix "${install_prefix}"

run cmake -S "${repo_root}/examples/find_package_consumer" \
  -B "${consumer_build}" \
  -DCMAKE_PREFIX_PATH="${install_prefix}"
run cmake --build "${consumer_build}" --config Release
consumer_exe="$(find_executable \
  "${consumer_build}/city_life_core_consumer" \
  "${consumer_build}/Release/city_life_core_consumer.exe")"
run "${consumer_exe}"

run cmake -S "${repo_root}/examples/c_abi_consumer" \
  -B "${c_abi_consumer_build}" \
  -DCMAKE_PREFIX_PATH="${install_prefix}"
run cmake --build "${c_abi_consumer_build}" --config Release
c_abi_consumer_exe="$(find_executable \
  "${c_abi_consumer_build}/city_life_core_c_abi_consumer" \
  "${c_abi_consumer_build}/Release/city_life_core_c_abi_consumer.exe")"
run "${c_abi_consumer_exe}"

run cmake -E chdir "${build_path}" cpack --config CPackConfig.cmake -G ZIP -C Release
run cmake -E chdir "${build_path}" sha256sum city-life-core-sdk-*.zip
cmake -E chdir "${build_path}" sha256sum city-life-core-sdk-*.zip > "${checksums_file}"

rm -rf "${zip_extract_dir}"
mkdir -p "${zip_extract_dir}"
zip_file="$(find "${build_path}" -maxdepth 1 -type f -name 'city-life-core-sdk-*.zip' | head -n 1)"
if [[ -z "${zip_file}" ]]; then
  echo "No SDK ZIP package found in ${build_path}" >&2
  exit 1
fi

run cmake -E chdir "${zip_extract_dir}" cmake -E tar xf "${zip_file}"
sdk_prefix="$(find "${zip_extract_dir}" -mindepth 1 -maxdepth 1 -type d | head -n 1)"
if [[ -z "${sdk_prefix}" ]]; then
  echo "Could not locate unpacked SDK prefix" >&2
  exit 1
fi

run cmake -S "${repo_root}/examples/find_package_consumer" \
  -B "${zip_consumer_build}" \
  -DCMAKE_PREFIX_PATH="${sdk_prefix}"
run cmake --build "${zip_consumer_build}" --config Release
zip_consumer_exe="$(find_executable \
  "${zip_consumer_build}/city_life_core_consumer" \
  "${zip_consumer_build}/Release/city_life_core_consumer.exe")"
run "${zip_consumer_exe}"

run cmake -S "${repo_root}/examples/c_abi_consumer" \
  -B "${zip_c_abi_consumer_build}" \
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
