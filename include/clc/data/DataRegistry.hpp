#pragma once

#include "clc/data/Definitions.hpp"
#include "clc/data/Validation.hpp"

#include <string>
#include <string_view>
#include <unordered_map>

namespace clc::data {

class DataRegistry final {
public:
    ValidationReport add(ResourceDefinition definition);
    ValidationReport add(CurrencyDefinition definition);
    ValidationReport add(BuildingDefinition definition);
    ValidationReport add(ProfessionDefinition definition);
    ValidationReport add(SettlementDefinition definition);

    [[nodiscard]] const ResourceDefinition* resource(std::string_view id) const;
    [[nodiscard]] const CurrencyDefinition* currency(std::string_view id) const;
    [[nodiscard]] const BuildingDefinition* building(std::string_view id) const;
    [[nodiscard]] const ProfessionDefinition* profession(std::string_view id) const;
    [[nodiscard]] const SettlementDefinition* settlement(std::string_view id) const;

    [[nodiscard]] std::size_t resource_count() const noexcept;
    [[nodiscard]] std::size_t currency_count() const noexcept;
    [[nodiscard]] std::size_t building_count() const noexcept;
    [[nodiscard]] std::size_t profession_count() const noexcept;
    [[nodiscard]] std::size_t settlement_count() const noexcept;

private:
    std::unordered_map<std::string, ResourceDefinition> resources_{};
    std::unordered_map<std::string, CurrencyDefinition> currencies_{};
    std::unordered_map<std::string, BuildingDefinition> buildings_{};
    std::unordered_map<std::string, ProfessionDefinition> professions_{};
    std::unordered_map<std::string, SettlementDefinition> settlements_{};
};

} // namespace clc::data
