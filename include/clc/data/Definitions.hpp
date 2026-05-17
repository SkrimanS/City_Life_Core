#pragma once

#include <cstdint>
#include <string>

namespace clc::data {

struct ResourceDefinition final {
    std::string id{};
    std::string display_name{};
    std::string category{};
    std::uint64_t base_value{0};
};

struct CurrencyDefinition final {
    std::string id{};
    std::string display_name{};
    std::uint32_t fractional_digits{0};
};

struct BuildingDefinition final {
    std::string id{};
    std::string display_name{};
    std::string category{};
    std::uint32_t worker_slots{0};
};

struct ProfessionDefinition final {
    std::string id{};
    std::string display_name{};
    std::string category{};
};

struct SettlementDefinition final {
    std::string id{};
    std::string display_name{};
    std::uint64_t starting_population{0};
};

} // namespace clc::data
