#pragma once

#include "clc/data/Validation.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace clc::sim {

struct SdkHandoffArtifact final {
    std::string artifact_id{};
    std::string path{};
    bool present{false};
    bool required{true};
};

struct SdkHandoffManifestInput final {
    std::string sdk_version{};
    std::uint32_t c_interface_version{0};
    std::vector<SdkHandoffArtifact> artifacts{};
};

struct SdkHandoffManifestReport final {
    bool ready{false};
    std::string sdk_version{};
    std::uint32_t c_interface_version{0};
    std::uint64_t required_artifact_count{0};
    std::uint64_t present_required_artifact_count{0};
    std::uint64_t missing_required_artifact_count{0};
    std::uint64_t optional_artifact_count{0};
    std::vector<SdkHandoffArtifact> artifacts{};
    data::ValidationReport validation{};
};

[[nodiscard]] std::vector<SdkHandoffArtifact> make_standard_sdk_handoff_artifacts(bool present);
[[nodiscard]] data::ValidationReport validate_sdk_handoff_artifact(const SdkHandoffArtifact& artifact);
[[nodiscard]] SdkHandoffManifestReport make_sdk_handoff_manifest_report(SdkHandoffManifestInput input);
[[nodiscard]] std::string sdk_handoff_manifest_digest(const SdkHandoffManifestReport& report);
[[nodiscard]] std::string sdk_handoff_manifest_markdown(const SdkHandoffManifestReport& report);

} // namespace clc::sim
