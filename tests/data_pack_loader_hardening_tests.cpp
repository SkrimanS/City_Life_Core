#include "clc/data/DataPackLoader.hpp"

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

bool contains_message(const clc::data::ValidationReport& report, std::string_view needle) {
    for (const auto& message : report.messages()) {
        if (message.message.find(needle) != std::string::npos) {
            return true;
        }
    }
    return false;
}

} // namespace

int main() {
    clc::data::DataPackLoader loader;

    {
        clc::data::DataRegistry registry;
        const auto report = loader.load_string("missing-schema", R"CLC(
[resource]
id=grain
display_name=Grain
category=food
base_value=10
)CLC", registry);

        require(!report.ok(), "missing schema_version should fail");
        require(contains_message(report, "schema_version is required"), "missing schema should explain schema_version requirement");
        require(registry.resource_count() == 0, "parse-level failure should not register resources");
    }

    {
        clc::data::DataRegistry registry;
        const auto report = loader.load_string("unknown-section", R"CLC(
schema_version=0.2.2

[unknown]
id=ignored
)CLC", registry);

        require(!report.ok(), "unknown section should fail");
        require(contains_message(report, "unknown section"), "unknown section should be reported");
        require(registry.resource_count() == 0, "unknown section should not register resources");
    }

    {
        clc::data::DataRegistry registry;
        const auto report = loader.load_string("duplicate-key", R"CLC(
schema_version=0.2.2

[resource]
id=grain
id=grain_again
display_name=Grain
category=food
base_value=10
)CLC", registry);

        require(!report.ok(), "duplicate key should fail");
        require(contains_message(report, "duplicate key"), "duplicate key should be reported");
        require(registry.resource_count() == 0, "duplicate key parse failure should not register resources");
    }

    {
        clc::data::DataRegistry registry;
        const auto report = loader.load_string("invalid-resource-value", R"CLC(
schema_version=0.2.2

[resource]
id=worthless
display_name=Worthless Resource
category=test
base_value=0
)CLC", registry);

        require(!report.ok(), "zero base_value resource should fail through loader");
        require(contains_message(report, "base_value"), "zero base_value should be reported");
        require(registry.resource_count() == 0, "invalid resource should not be registered through loader");
    }

    {
        clc::data::DataRegistry registry;
        const auto report = loader.load_string("invalid-building-workers", R"CLC(
schema_version=0.2.2

[resource]
id=grain
display_name=Grain
category=food
base_value=10

[profession]
id=farmer
display_name=Farmer
category=food

[building]
id=empty_workshop
display_name=Empty Workshop
category=production
worker_slots=0
required_profession_id=farmer
input_resource_ids=grain
output_resource_ids=grain
)CLC", registry);

        require(!report.ok(), "zero worker_slots building should fail through loader");
        require(contains_message(report, "worker_slots"), "zero worker_slots should be reported");
        require(registry.resource_count() == 1, "valid resource before invalid building should remain registered");
        require(registry.profession_count() == 1, "valid profession before invalid building should remain registered");
        require(registry.building_count() == 0, "invalid building should not be registered through loader");
    }

    {
        clc::data::DataRegistry registry;
        const auto report = loader.load_string("broken-reference", R"CLC(
schema_version=0.2.2

[resource]
id=grain
display_name=Grain
category=food
base_value=10

[building]
id=mill
display_name=Mill
category=production
worker_slots=2
required_profession_id=missing_profession
input_resource_ids=grain
output_resource_ids=missing_resource
)CLC", registry);

        require(!report.ok(), "broken building references should fail through loader");
        require(contains_message(report, "unknown required_profession_id"), "missing profession reference should be reported");
        require(contains_message(report, "unknown output_resource_id"), "missing resource reference should be reported");
        require(registry.building_count() == 1, "building with deferred reference errors is registered for reference validation");
    }

    return 0;
}
