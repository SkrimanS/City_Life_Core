#pragma once

#include "clc/core/Time.hpp"
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

struct SettlementTickRemainder final {
    std::string key{};
    std::uint64_t numerator{0};
};

struct SettlementState final {
    std::string id{};
    std::string display_name{};
    std::uint64_t population{0};
    ResourceStorage storage{};
    std::vector<BuildingInstance> buildings{};
    std::vector<SettlementTickRemainder> tick_remainders{};
};

struct SettlementTickReport final {
    std::string settlement_id{};
    clc::GameTime::Tick elapsed_ticks{0};
    std::uint64_t consumed_food{0};
    std::uint64_t consumed_inputs{0};
    std::uint64_t produced_resources{0};
    std::uint32_t active_buildings{0};
    std::uint32_t skipped_buildings{0};
    std::vector<std::string> warnings{};
};

struct ResourceAmount final {
    std::string resource_id{};
    std::uint64_t amount{0};
};

struct BuildingReport final {
    std::string definition_id{};
    std::string display_name{};
    std::uint32_t assigned_workers{0};
    std::uint32_t worker_slots{0};
};

struct SettlementReport final {
    std::string id{};
    std::string display_name{};
    std::uint64_t population{0};
    std::vector<ResourceAmount> storage{};
    std::vector<BuildingReport> buildings{};
    std::uint64_t total_stored_resources{0};
};

[[nodiscard]] SettlementState create_settlement_from_definition(const data::SettlementDefinition& definition);
[[nodiscard]] data::ValidationReport add_building(SettlementState& settlement, const data::DataRegistry& registry, BuildingInstance building);
[[nodiscard]] SettlementTickReport advance_settlement_ticks(
    SettlementState& settlement,
    const data::DataRegistry& registry,
    clc::GameTime::Tick ticks
);
[[nodiscard]] SettlementTickReport advance_settlement_day(SettlementState& settlement, const data::DataRegistry& registry);
[[nodiscard]] SettlementReport make_settlement_report(const SettlementState& settlement, const data::DataRegistry& registry);

} // namespace clc::sim
