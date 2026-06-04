#include "clc/CityLifeCore.hpp"

#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>

namespace {

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "Test failed: " << message << '\n';
        std::exit(1);
    }
}

} // namespace

int main() {
    const auto registry = clc::sim::make_standard_platform_core_registry();
    const auto query = clc::sim::query_platform_core_registry(registry);
    require(query.validation.ok(), "standard platform registry should validate");
    require(query.core_module_count > 0, "standard platform registry should include core modules");
    require(query.optional_module_count > 0, "standard platform registry should include optional modules");
    require(query.genre_module_count >= 5, "standard platform registry should include genre modules");
    require(query.genre_profile_count >= 5, "standard platform registry should include genre profiles");
    require(query.content_template_count >= query.genre_profile_count, "genre profiles should declare content templates");
    require(clc::sim::platform_core_query_digest(query).find("platform_core_query") != std::string::npos, "query digest should identify platform core");

    const auto diagnostics = clc::sim::make_platform_diagnostics_report(
        registry,
        clc::sim::make_standard_platform_content_pack_manifest()
    );
    require(diagnostics.ready, "standard platform diagnostics should be ready");
    require(clc::sim::platform_diagnostics_digest(diagnostics).find("ready=yes") != std::string::npos, "diagnostics digest should report ready");
    require(clc::sim::platform_diagnostics_markdown(diagnostics).find("Platform Diagnostics") != std::string::npos, "diagnostics markdown should include heading");

    const auto broken_content = clc::sim::validate_content_pack_manifest({
        .content_pack_id = "broken",
        .schema_version = 1,
        .required_module_ids = {"missing_module"},
        .compatible_profile_ids = {"idle_tycoon"},
    }, registry);
    require(!broken_content.ok(), "unknown required module should fail content pipeline validation");

    return 0;
}
