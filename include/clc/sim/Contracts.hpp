#pragma once

#include "clc/data/Validation.hpp"
#include "clc/sim/Caravans.hpp"
#include "clc/sim/Factions.hpp"
#include "clc/sim/Storage.hpp"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace clc::sim {

enum class ContractStatus {
    open,
    fulfilled,
    failed,
    cancelled,
};

struct ResourceDeliveryContract final {
    std::string id{};
    std::string display_name{};
    std::string issuer_faction_id{};
    std::string receiver_faction_id{};
    std::string resource_id{};
    std::uint64_t quantity{0};
    std::uint64_t reward_coins{0};
    std::uint64_t due_day{0};
    ContractStatus status{ContractStatus::open};
};

struct ContractCatalog final {
    std::vector<ResourceDeliveryContract> contracts{};
};

struct ContractFulfillmentResult final {
    std::string contract_id{};
    std::string resource_id{};
    std::uint64_t quantity{0};
    std::uint64_t reward_coins{0};
    bool fulfilled{false};
    data::ValidationReport validation{};

    [[nodiscard]] bool ok() const noexcept {
        return fulfilled && validation.ok();
    }
};

struct ContractDeadlineReport final {
    std::uint64_t current_day{0};
    std::uint64_t failed_count{0};
    std::vector<std::string> failed_contract_ids{};
};

[[nodiscard]] std::string_view contract_status_name(ContractStatus status) noexcept;
[[nodiscard]] bool contract_is_open(const ResourceDeliveryContract& contract) noexcept;
[[nodiscard]] bool contract_is_terminal(const ResourceDeliveryContract& contract) noexcept;

[[nodiscard]] data::ValidationReport validate_resource_delivery_contract(const ResourceDeliveryContract& contract);
[[nodiscard]] data::ValidationReport validate_resource_delivery_contract_for_factions(
    const ResourceDeliveryContract& contract,
    const FactionCatalog& factions
);

[[nodiscard]] data::ValidationReport add_contract(ContractCatalog& catalog, ResourceDeliveryContract contract);
[[nodiscard]] std::uint64_t contract_count(const ContractCatalog& catalog) noexcept;
[[nodiscard]] const ResourceDeliveryContract* contract_by_id(const ContractCatalog& catalog, std::string_view contract_id) noexcept;
[[nodiscard]] ResourceDeliveryContract* mutable_contract_by_id(ContractCatalog& catalog, std::string_view contract_id) noexcept;

[[nodiscard]] ContractFulfillmentResult fulfill_contract_from_storage(
    ContractCatalog& catalog,
    std::string_view contract_id,
    ResourceStorage& delivered_resources
);

[[nodiscard]] ContractFulfillmentResult fulfill_contract_from_arrived_caravan(
    ContractCatalog& catalog,
    std::string_view contract_id,
    CaravanState& caravan
);

[[nodiscard]] ContractDeadlineReport fail_overdue_open_contracts(ContractCatalog& catalog, std::uint64_t current_day);

[[nodiscard]] data::ValidationReport mark_contract_fulfilled(ContractCatalog& catalog, std::string_view contract_id);
[[nodiscard]] data::ValidationReport mark_contract_failed(ContractCatalog& catalog, std::string_view contract_id);
[[nodiscard]] data::ValidationReport cancel_contract(ContractCatalog& catalog, std::string_view contract_id);

[[nodiscard]] std::vector<ResourceDeliveryContract> open_contracts(const ContractCatalog& catalog);
[[nodiscard]] std::vector<ResourceDeliveryContract> terminal_contracts(const ContractCatalog& catalog);
[[nodiscard]] std::vector<ResourceDeliveryContract> contracts_for_faction(const ContractCatalog& catalog, std::string_view faction_id);
[[nodiscard]] std::vector<ResourceDeliveryContract> overdue_open_contracts(const ContractCatalog& catalog, std::uint64_t current_day);

} // namespace clc::sim
