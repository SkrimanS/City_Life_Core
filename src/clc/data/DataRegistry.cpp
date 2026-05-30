#include "clc/data/DataRegistry.hpp"

#include <utility>

namespace clc::data {
namespace {

ValidationReport validate_common(std::string_view type, std::string_view id, std::string_view display_name) {
    ValidationReport report;
    if (id.empty()) {
        report.add_error(std::string{type}, "id must not be empty");
    }
    if (display_name.empty()) {
        report.add_error(std::string{type}, "display_name must not be empty");
    }
    return report;
}

template <typename Map>
void add_duplicate_error_if_needed(ValidationReport& report, const Map& map, std::string_view path, std::string_view id) {
    if (!id.empty() && map.find(std::string{id}) != map.end()) {
        report.add_error(std::string{path}, "duplicate id: " + std::string{id});
    }
}

template <typename Map, typename Definition>
const Definition* find_by_id(const Map& map, std::string_view id) {
    const auto it = map.find(std::string{id});
    if (it == map.end()) {
        return nullptr;
    }
    return &it->second;
}

} // namespace

ValidationReport DataRegistry::add(ResourceDefinition definition) {
    auto report = validate_common("resource", definition.id, definition.display_name);
    add_duplicate_error_if_needed(report, resources_, "resource", definition.id);
    if (definition.category.empty()) {
        report.add_warning("resource." + definition.id, "category is empty");
    }
    if (definition.base_value == 0) {
        report.add_error("resource." + definition.id, "base_value must be greater than zero");
    }
    if (!report.ok()) {
        return report;
    }
    resources_.emplace(definition.id, std::move(definition));
    return report;
}

ValidationReport DataRegistry::add(CurrencyDefinition definition) {
    auto report = validate_common("currency", definition.id, definition.display_name);
    add_duplicate_error_if_needed(report, currencies_, "currency", definition.id);
    if (definition.fractional_digits > 8) {
        report.add_error("currency." + definition.id, "fractional_digits must be <= 8");
    }
    if (!report.ok()) {
        return report;
    }
    currencies_.emplace(definition.id, std::move(definition));
    return report;
}

ValidationReport DataRegistry::add(BuildingDefinition definition) {
    auto report = validate_common("building", definition.id, definition.display_name);
    add_duplicate_error_if_needed(report, buildings_, "building", definition.id);
    if (definition.category.empty()) {
        report.add_warning("building." + definition.id, "category is empty");
    }
    if (definition.worker_slots == 0) {
        report.add_error("building." + definition.id, "worker_slots must be greater than zero");
    }
    if (!report.ok()) {
        return report;
    }
    buildings_.emplace(definition.id, std::move(definition));
    return report;
}

ValidationReport DataRegistry::add(ProfessionDefinition definition) {
    auto report = validate_common("profession", definition.id, definition.display_name);
    add_duplicate_error_if_needed(report, professions_, "profession", definition.id);
    if (definition.category.empty()) {
        report.add_warning("profession." + definition.id, "category is empty");
    }
    if (!report.ok()) {
        return report;
    }
    professions_.emplace(definition.id, std::move(definition));
    return report;
}

ValidationReport DataRegistry::add(SettlementDefinition definition) {
    auto report = validate_common("settlement", definition.id, definition.display_name);
    add_duplicate_error_if_needed(report, settlements_, "settlement", definition.id);
    if (definition.starting_population == 0) {
        report.add_warning("settlement." + definition.id, "starting_population is zero");
    }
    if (!report.ok()) {
        return report;
    }
    settlements_.emplace(definition.id, std::move(definition));
    return report;
}

ValidationReport DataRegistry::validate_references() const {
    ValidationReport report;

    for (const auto& [building_id, building_definition] : buildings_) {
        const auto path = "building." + building_id;
        if (!building_definition.required_profession_id.empty() && profession(building_definition.required_profession_id) == nullptr) {
            report.add_error(path, "unknown required_profession_id: " + building_definition.required_profession_id);
        }

        for (const auto& resource_id : building_definition.input_resource_ids) {
            if (resource(resource_id) == nullptr) {
                report.add_error(path, "unknown input_resource_id: " + resource_id);
            }
        }

        for (const auto& resource_id : building_definition.output_resource_ids) {
            if (resource(resource_id) == nullptr) {
                report.add_error(path, "unknown output_resource_id: " + resource_id);
            }
        }
    }

    return report;
}

const ResourceDefinition* DataRegistry::resource(std::string_view id) const {
    return find_by_id<decltype(resources_), ResourceDefinition>(resources_, id);
}

const CurrencyDefinition* DataRegistry::currency(std::string_view id) const {
    return find_by_id<decltype(currencies_), CurrencyDefinition>(currencies_, id);
}

const BuildingDefinition* DataRegistry::building(std::string_view id) const {
    return find_by_id<decltype(buildings_), BuildingDefinition>(buildings_, id);
}

const ProfessionDefinition* DataRegistry::profession(std::string_view id) const {
    return find_by_id<decltype(professions_), ProfessionDefinition>(professions_, id);
}

const SettlementDefinition* DataRegistry::settlement(std::string_view id) const {
    return find_by_id<decltype(settlements_), SettlementDefinition>(settlements_, id);
}

std::size_t DataRegistry::resource_count() const noexcept {
    return resources_.size();
}

std::size_t DataRegistry::currency_count() const noexcept {
    return currencies_.size();
}

std::size_t DataRegistry::building_count() const noexcept {
    return buildings_.size();
}

std::size_t DataRegistry::profession_count() const noexcept {
    return professions_.size();
}

std::size_t DataRegistry::settlement_count() const noexcept {
    return settlements_.size();
}

} // namespace clc::data
