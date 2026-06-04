#include "clc/sim/SdkHandoffManifest.hpp"

#include <sstream>
#include <string>
#include <utility>

namespace clc::sim {

std::vector<SdkHandoffArtifact> make_standard_sdk_handoff_artifacts(bool present) {
    return {
        {.artifact_id = "public_umbrella_header", .path = "include/clc/CityLifeCore.hpp", .present = present},
        {.artifact_id = "c_abi_header", .path = "include/clc/c/CityLifeCoreC.h", .present = present},
        {.artifact_id = "cmake_package_config", .path = "lib/cmake/CityLifeCore/CityLifeCoreConfig.cmake", .present = present},
        {.artifact_id = "core_concepts_doc", .path = "share/doc/CityLifeCore/core-concepts.md", .present = present},
        {.artifact_id = "public_api_doc", .path = "share/doc/CityLifeCore/public-api.md", .present = present},
        {.artifact_id = "completion_definition_doc", .path = "share/doc/CityLifeCore/core-completion-definition.md", .present = present},
        {.artifact_id = "developer_handoff_doc", .path = "share/doc/CityLifeCore/game-developer-handoff.md", .present = present},
        {.artifact_id = "readiness_doc", .path = "share/doc/CityLifeCore/core-completion-readiness.md", .present = present},
        {.artifact_id = "simulation_processors_doc", .path = "share/doc/CityLifeCore/simulation-processors.md", .present = present},
        {.artifact_id = "deep_systems_doc", .path = "share/doc/CityLifeCore/deep-simulation-systems.md", .present = present},
        {.artifact_id = "regional_systems_doc", .path = "share/doc/CityLifeCore/regional-simulation-systems.md", .present = present},
        {.artifact_id = "runtime_core_systems_doc", .path = "share/doc/CityLifeCore/runtime-core-systems.md", .present = present},
        {.artifact_id = "game_profiles_example", .path = "share/doc/CityLifeCore/examples/game_profiles.cpp", .present = present},
        {.artifact_id = "game_developer_handoff_example", .path = "share/doc/CityLifeCore/examples/game_developer_handoff.cpp", .present = present},
        {.artifact_id = "runtime_core_systems_example", .path = "share/doc/CityLifeCore/examples/runtime_core_systems.cpp", .present = present},
        {.artifact_id = "core_completion_readiness_example", .path = "share/doc/CityLifeCore/examples/core_completion_readiness.cpp", .present = present},
        {.artifact_id = "c_abi_consumer_example", .path = "share/doc/CityLifeCore/examples/c_abi_consumer/main.c", .present = present},
        {.artifact_id = "demo_data_pack", .path = "share/city-life-core/demo_fantasy/core.clcd", .present = present},
        {.artifact_id = "release_validation_script", .path = "share/doc/CityLifeCore/scripts/manual_release_validation.ps1", .present = present, .required = false},
    };
}

data::ValidationReport validate_sdk_handoff_artifact(const SdkHandoffArtifact& artifact) {
    data::ValidationReport report;
    if (artifact.artifact_id.empty()) {
        report.add_error("sdk_handoff.artifact_id", "artifact_id must not be empty");
    }
    if (artifact.path.empty()) {
        report.add_error("sdk_handoff." + artifact.artifact_id + ".path", "path must not be empty");
    }
    if (artifact.required && !artifact.present) {
        report.add_error("sdk_handoff." + artifact.artifact_id + ".present", "required handoff artifact is missing");
    }
    if (!artifact.required && !artifact.present) {
        report.add_warning("sdk_handoff." + artifact.artifact_id + ".present", "optional handoff artifact is missing");
    }
    return report;
}

SdkHandoffManifestReport make_sdk_handoff_manifest_report(SdkHandoffManifestInput input) {
    SdkHandoffManifestReport report{
        .sdk_version = std::move(input.sdk_version),
        .c_interface_version = input.c_interface_version,
        .artifacts = std::move(input.artifacts),
    };

    if (report.sdk_version.empty()) {
        report.validation.add_error("sdk_handoff.sdk_version", "sdk_version must not be empty");
    }
    if (report.c_interface_version == 0) {
        report.validation.add_error("sdk_handoff.c_interface_version", "c_interface_version must be greater than zero");
    }

    for (const auto& artifact : report.artifacts) {
        if (artifact.required) {
            ++report.required_artifact_count;
            if (artifact.present) {
                ++report.present_required_artifact_count;
            } else {
                ++report.missing_required_artifact_count;
            }
        } else {
            ++report.optional_artifact_count;
        }

        const auto validation = validate_sdk_handoff_artifact(artifact);
        for (const auto& message : validation.messages()) {
            if (message.severity == data::ValidationSeverity::warning) {
                report.validation.add_warning(message.path, message.message);
            } else {
                report.validation.add_error(message.path, message.message);
            }
        }
    }

    report.ready = report.required_artifact_count > 0
        && report.missing_required_artifact_count == 0
        && report.validation.error_count() == 0;
    return report;
}

std::string sdk_handoff_manifest_digest(const SdkHandoffManifestReport& report) {
    std::ostringstream out;
    out << "sdk_handoff_manifest"
        << ";ready=" << (report.ready ? "yes" : "no")
        << ";version=" << report.sdk_version
        << ";c_interface=" << report.c_interface_version
        << ";required=" << report.required_artifact_count
        << ";present_required=" << report.present_required_artifact_count
        << ";missing_required=" << report.missing_required_artifact_count
        << ";optional=" << report.optional_artifact_count
        << ";diagnostics=" << report.validation.messages().size();
    return out.str();
}

std::string sdk_handoff_manifest_markdown(const SdkHandoffManifestReport& report) {
    std::ostringstream out;
    out << "# SDK Handoff Manifest\n\n";
    out << "- Ready: " << (report.ready ? "yes" : "no") << "\n";
    out << "- SDK version: " << report.sdk_version << "\n";
    out << "- C interface version: " << report.c_interface_version << "\n";
    out << "- Required artifacts: " << report.required_artifact_count << "\n";
    out << "- Present required artifacts: " << report.present_required_artifact_count << "\n";
    out << "- Missing required artifacts: " << report.missing_required_artifact_count << "\n";
    out << "- Optional artifacts: " << report.optional_artifact_count << "\n";
    out << "\n## Artifacts\n\n";
    for (const auto& artifact : report.artifacts) {
        out << "- `" << artifact.artifact_id << "`: "
            << (artifact.present ? "present" : "missing")
            << (artifact.required ? ", required" : ", optional")
            << " (`" << artifact.path << "`)\n";
    }
    return out.str();
}

} // namespace clc::sim
