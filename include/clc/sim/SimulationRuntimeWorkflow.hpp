#pragma once

#include "clc/sim/Contracts.hpp"
#include "clc/sim/SimulationRuntime.hpp"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace clc::sim {

struct RuntimeCaravanCreationResult final {
    std::string caravan_id{};
    bool created{false};
    data::ValidationReport validation{};

    [[nodiscard]] bool ok() const noexcept {
        return created && validation.ok();
    }
};

struct RuntimeCaravanAdvanceResult final {
    CaravanAdvanceReport report{};
    data::ValidationReport validation{};

    [[nodiscard]] bool ok() const noexcept {
        return validation.ok();
    }
};

struct RuntimeCargoDeliveryEntry final {
    std::string resource_id{};
    std::uint64_t amount{0};
};

struct RuntimeCaravanCargoDeliveryResult final {
    std::string caravan_id{};
    std::string destination_settlement_id{};
    std::vector<RuntimeCargoDeliveryEntry> delivered{};
    std::uint64_t total_amount{0};
    data::ValidationReport validation{};

    [[nodiscard]] bool ok() const noexcept {
        return validation.ok();
    }
};

[[nodiscard]] data::ValidationReport create_runtime_settlement(
    SimulationRuntime& runtime,
    std::string settlement_definition_id
);

[[nodiscard]] data::ValidationReport add_runtime_route(
    SimulationRuntime& runtime,
    SettlementRoute route
);

[[nodiscard]] data::ValidationReport add_runtime_faction(
    SimulationRuntime& runtime,
    FactionState faction
);

[[nodiscard]] data::ValidationReport set_runtime_faction_reputation(
    SimulationRuntime& runtime,
    std::string from_faction_id,
    std::string to_faction_id,
    std::int64_t value
);

[[nodiscard]] data::ValidationReport set_runtime_settlement_owner(
    SimulationRuntime& runtime,
    std::string settlement_id,
    std::string faction_id
);

[[nodiscard]] data::ValidationReport set_runtime_caravan_owner(
    SimulationRuntime& runtime,
    std::string caravan_id,
    std::string faction_id
);

[[nodiscard]] data::ValidationReport add_runtime_resource_delivery_contract(
    SimulationRuntime& runtime,
    ResourceDeliveryContract contract
);

[[nodiscard]] RuntimeCaravanCreationResult create_runtime_caravan_for_route(
    SimulationRuntime& runtime,
    std::string_view route_id,
    std::string caravan_id,
    std::string display_name,
    ResourceStorage cargo = {}
);

[[nodiscard]] data::ValidationReport load_runtime_caravan_at_origin(
    SimulationRuntime& runtime,
    std::string_view caravan_id,
    std::string_view resource_id,
    std::uint64_t amount
);

[[nodiscard]] RuntimeCaravanAdvanceResult advance_runtime_caravan_day(
    SimulationRuntime& runtime,
    std::string_view caravan_id
);

[[nodiscard]] data::ValidationReport unload_runtime_caravan_at_destination(
    SimulationRuntime& runtime,
    std::string_view caravan_id,
    std::string_view resource_id,
    std::uint64_t amount
);

[[nodiscard]] RuntimeCaravanCargoDeliveryResult deliver_runtime_arrived_caravan_cargo_to_destination(
    SimulationRuntime& runtime,
    std::string_view caravan_id
);

[[nodiscard]] ContractFulfillmentResult fulfill_runtime_contract_from_arrived_caravan_with_reward_and_ledger(
    SimulationRuntime& runtime,
    std::string_view contract_id,
    std::string_view caravan_id
);

[[nodiscard]] ContractFulfillmentResult fulfill_runtime_contract_from_owned_arrived_caravan_with_reward_and_ledger(
    SimulationRuntime& runtime,
    std::string_view contract_id,
    std::string_view caravan_id,
    std::string_view expected_faction_id
);

[[nodiscard]] ContractFulfillmentResult fulfill_first_runtime_contract_for_owned_arrived_caravan_with_reward_and_ledger(
    SimulationRuntime& runtime,
    std::string_view caravan_id,
    std::string_view expected_faction_id
);

} // namespace clc::sim
