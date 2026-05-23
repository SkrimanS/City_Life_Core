#include "clc/data/DataRegistry.hpp"
#include "clc/data/Validation.hpp"

#include <cstdlib>
#include <iostream>
#include <string_view>

namespace {

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "Test failed: " << message << '\n';
        std::exit(1);
    }
}

bool contains_message(const clc::data::ValidationReport& report, clc::data::ValidationSeverity severity, std::string_view needle) {
    for (const auto& message : report.messages()) {
        if (message.severity == severity && message.message.find(needle) != std::string::npos) {
            return true;
        }
    }
    return false;
}

} // namespace

int main() {
    clc::data::DataRegistry registry;

    auto resource_warning = registry.add(clc::data::ResourceDefinition{
        .id = "grain",
        .display_name = "Grain",
        .category = "",
        .base_value = 10,
    });
    require(resource_warning.ok(), "resource category warning should not block insertion");
    require(resource_warning.warning_count() == 1, "resource with empty category should produce one warning");
    require(resource_warning.error_count() == 0, "resource warning should not produce errors");
    require(registry.resource_count() == 1, "warning-only resource should be inserted");
    require(registry.resource("grain") != nullptr, "warning-only resource should be findable");

    const auto duplicate_resource = registry.add(clc::data::ResourceDefinition{
        .id = "grain",
        .display_name = "Duplicate Grain",
        .category = "food",
        .base_value = 11,
    });
    require(!duplicate_resource.ok(), "duplicate resource id should fail");
    require(contains_message(duplicate_resource, clc::data::ValidationSeverity::error, "duplicate id"), "duplicate resource should explain duplicate id");
    require(registry.resource_count() == 1, "duplicate resource should not be inserted");

    const auto invalid_currency = registry.add(clc::data::CurrencyDefinition{
        .id = "coin",
        .display_name = "Coin",
        .fractional_digits = 9,
    });
    require(!invalid_currency.ok(), "currency fractional_digits above eight should fail");
    require(contains_message(invalid_currency, clc::data::ValidationSeverity::error, "fractional_digits"), "invalid currency should explain fractional_digits limit");
    require(registry.currency_count() == 0, "invalid currency should not be inserted");
    require(registry.currency("coin") == nullptr, "invalid currency should not be findable");

    require(registry.add(clc::data::CurrencyDefinition{
        .id = "coin",
        .display_name = "Coin",
        .fractional_digits = 8,
    }).ok(), "currency with eight fractional digits should register");
    require(registry.currency_count() == 1, "valid currency should be inserted");

    const auto profession_warning = registry.add(clc::data::ProfessionDefinition{
        .id = "farmer",
        .display_name = "Farmer",
        .category = "",
    });
    require(profession_warning.ok(), "profession category warning should not block insertion");
    require(profession_warning.warning_count() == 1, "profession with empty category should warn");
    require(registry.profession_count() == 1, "warning-only profession should be inserted");

    const auto settlement_warning = registry.add(clc::data::SettlementDefinition{
        .id = "frontier",
        .display_name = "Frontier",
        .starting_population = 0,
    });
    require(settlement_warning.ok(), "zero-population settlement warning should not block insertion");
    require(settlement_warning.warning_count() == 1, "zero-population settlement should warn");
    require(registry.settlement_count() == 1, "warning-only settlement should be inserted");

    const auto invalid_building = registry.add(clc::data::BuildingDefinition{
        .id = "",
        .display_name = "",
        .category = "production",
        .worker_slots = 1,
    });
    require(!invalid_building.ok(), "empty building id and name should fail");
    require(registry.building_count() == 0, "invalid building should not be inserted");

    const auto broken_refs_building = registry.add(clc::data::BuildingDefinition{
        .id = "mystery_workshop",
        .display_name = "Mystery Workshop",
        .category = "production",
        .worker_slots = 3,
        .required_profession_id = "unknown_profession",
        .input_resource_ids = {"unknown_input"},
        .output_resource_ids = {"unknown_output"},
    });
    require(broken_refs_building.ok(), "building insertion should allow deferred reference validation");
    require(registry.building_count() == 1, "building with deferred broken refs should be inserted");

    const auto reference_report = registry.validate_references();
    require(!reference_report.ok(), "broken building references should fail reference validation");
    require(reference_report.error_count() == 3, "reference validation should report all broken building references");
    require(contains_message(reference_report, clc::data::ValidationSeverity::error, "unknown required_profession_id"), "reference validation should report unknown profession");
    require(contains_message(reference_report, clc::data::ValidationSeverity::error, "unknown input_resource_id"), "reference validation should report unknown input resource");
    require(contains_message(reference_report, clc::data::ValidationSeverity::error, "unknown output_resource_id"), "reference validation should report unknown output resource");

    return 0;
}
