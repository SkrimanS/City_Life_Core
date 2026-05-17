#pragma once

#include "clc/data/Definitions.hpp"
#include "clc/data/DataRegistry.hpp"
#include "clc/data/Validation.hpp"
#include "clc/sim/Storage.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace clc::sim {

struct BuildingInstance final {
    std::string definition_id{};
    std::uint32_t assigned_workers{0};
};

struct SettlementState final {
    std::string id{};
    std::string display_name{};
    std::uint64_t population{0};
    ResourceStorage storage{};
    std::vector<BuildingInstance> buildings{};
};

struct SettlementTickReport final {
    std::string settlement_id{};
    std::uint64_t consumed_food{0};
    std::uint64_t produced_resources{0};
    std::vector<std::string> warnings{};
};

[[nodiscard]] SettlementState create_settlement_from_definition(const data::SettlementDefinition& definition);
[[nodiscard]] data::ValidationReport add_building(SettlementState& settlement, const data::DataRegistry& registry, BuildingInstance building);
[[nodiscard]] SettlementTickReport advance_settlement_day(SettlementState& settlement, const data::DataRegistry& registry);

} // namespace clc::sim
