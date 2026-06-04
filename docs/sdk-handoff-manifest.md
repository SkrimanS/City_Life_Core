# SDK Handoff Manifest

`SdkHandoffManifest.hpp` provides a small release/package manifest API for teams that need to confirm the installed SDK contains the expected handoff surface.

It tracks:

- public C++ umbrella header;
- public C ABI header;
- exported CMake package config;
- core, API, readiness, deep/regional/runtime and handoff documentation;
- starter examples for profiles, runtime, readiness and C ABI consumers;
- demo data pack;
- optional release validation script.

Useful APIs:

- `make_standard_sdk_handoff_artifacts`
- `make_sdk_handoff_manifest_report`
- `validate_sdk_handoff_artifact`
- `sdk_handoff_manifest_digest`
- `sdk_handoff_manifest_markdown`

`CoreCompletionEvidence` consumes `SdkHandoffManifestReport`. Final packaging readiness is true only when package validation is available and the SDK handoff manifest has all required artifacts.

Example:

- `examples/sdk_handoff_manifest.cpp`
