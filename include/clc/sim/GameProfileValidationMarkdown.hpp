#pragma once

#include "clc/data/Validation.hpp"
#include "clc/sim/GameProfileValidation.hpp"

#include <string>

namespace clc::sim {

[[nodiscard]] inline std::string game_profile_catalog_validation_markdown(
    const clc::data::ValidationReport& report
) {
    std::string output = "# Game Profile Catalog Validation\n\n";
    output += "- digest: `";
    output += game_profile_catalog_validation_digest(report);
    output += "`\n";
    output += "- ok: ";
    output += report.ok() ? "yes" : "no";
    output += "\n";
    output += "- errors: ";
    output += std::to_string(report.error_count());
    output += "\n";
    output += "- warnings: ";
    output += std::to_string(report.warning_count());
    output += "\n";

    if (report.messages().empty()) {
        output += "\nNo validation diagnostics.\n";
        return output;
    }

    output += "\n## Errors\n\n";
    bool wrote_errors = false;
    for (const auto& message : report.messages()) {
        if (message.severity != clc::data::ValidationSeverity::error) {
            continue;
        }
        wrote_errors = true;
        output += "- `";
        output += message.path;
        output += "` - ";
        output += message.message;
        output += "\n";
    }
    if (!wrote_errors) {
        output += "- none\n";
    }

    output += "\n## Warnings\n\n";
    bool wrote_warnings = false;
    for (const auto& message : report.messages()) {
        if (message.severity != clc::data::ValidationSeverity::warning) {
            continue;
        }
        wrote_warnings = true;
        output += "- `";
        output += message.path;
        output += "` - ";
        output += message.message;
        output += "\n";
    }
    if (!wrote_warnings) {
        output += "- none\n";
    }

    return output;
}

} // namespace clc::sim
