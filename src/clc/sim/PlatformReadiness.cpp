#include "clc/sim/PlatformReadiness.hpp"

#include <sstream>
#include <string>
#include <utility>

namespace clc::sim {

data::ValidationReport validate_platform_readiness_input(const PlatformReadinessInput& input) {
    data::ValidationReport report;
    if (!input.module_boundaries_documented) {
        report.add_error("platform_readiness.module_boundaries", "module boundaries must be documented");
    }
    if (!input.profile_requirements_documented) {
        report.add_error("platform_readiness.profile_requirements", "profile requirements must be documented");
    }
    if (!input.content_pipeline_requirements_documented) {
        report.add_error("platform_readiness.content_pipeline", "content pipeline requirements must be documented");
    }
    if (!input.editor_admin_api_requirements_documented) {
        report.add_error("platform_readiness.editor_admin_api", "editor/admin API requirements must be documented");
    }
    if (!input.diagnostics_api_requirements_documented) {
        report.add_error("platform_readiness.diagnostics_api", "diagnostics API requirements must be documented");
    }
    if (!input.cxx_api_ready) {
        report.add_error("platform_readiness.cxx_api", "public C++ API must be ready");
    }
    if (!input.c_abi_readonly_diagnostics_ready) {
        report.add_warning("platform_readiness.c_abi", "C ABI read-only diagnostics are not marked ready");
    }
    if (!input.save_load_migration_ready) {
        report.add_error("platform_readiness.save_load", "save/load migration must be ready");
    }
    if (!input.replay_migration_ready) {
        report.add_error("platform_readiness.replay", "replay migration must be ready");
    }
    if (!input.regional_large_world_ready) {
        report.add_error("platform_readiness.large_world", "regional large-world foundation must be ready");
    }
    if (input.supported_profile_ids.empty()) {
        report.add_warning("platform_readiness.supported_profile_ids", "no supported platform/profile ids were provided");
    }
    for (const auto& profile_id : input.supported_profile_ids) {
        if (profile_id.empty()) {
            report.add_error("platform_readiness.supported_profile_ids", "supported profile ids must not be empty");
        }
    }
    for (const auto& gap : input.known_gaps) {
        if (gap.empty()) {
            report.add_error("platform_readiness.known_gaps", "known gaps must not be empty");
        }
    }
    return report;
}

PlatformReadinessReport make_platform_readiness_report(const PlatformReadinessInput& input) {
    auto validation = validate_platform_readiness_input(input);
    const auto ready = validation.ok() && input.known_gaps.empty();
    return PlatformReadinessReport{
        .ready = ready,
        .supported_profile_count = input.supported_profile_ids.size(),
        .known_gap_count = input.known_gaps.size(),
        .validation = std::move(validation),
    };
}

std::string platform_readiness_digest(const PlatformReadinessReport& report) {
    std::ostringstream out;
    out << "platform_readiness"
        << ";target=" << report.target_version
        << ";ready=" << (report.ready ? "yes" : "no")
        << ";profiles=" << report.supported_profile_count
        << ";known_gaps=" << report.known_gap_count
        << ";diagnostics=" << report.validation.messages().size();
    return out.str();
}

std::string platform_readiness_markdown(const PlatformReadinessReport& report) {
    std::ostringstream out;
    out << "# Platform Readiness " << report.target_version << "\n\n";
    out << "- Ready: " << (report.ready ? "yes" : "no") << "\n";
    out << "- Supported profiles: " << report.supported_profile_count << "\n";
    out << "- Known gaps: " << report.known_gap_count << "\n";
    out << "- Diagnostics: " << report.validation.messages().size() << "\n";
    return out.str();
}

} // namespace clc::sim
