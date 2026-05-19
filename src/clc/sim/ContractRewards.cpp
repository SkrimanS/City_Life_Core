#include "clc/sim/Contracts.hpp"

namespace clc::sim {

ContractFulfillmentResult fulfill_contract_from_storage_with_reward_and_ledger(
    ContractCatalog& catalog,
    std::string_view contract_id,
    ResourceStorage& delivered_resources,
    economy::Wallet& reward_wallet,
    economy::EconomyLedger& ledger
) {
    auto result = fulfill_contract_from_storage_with_reward(catalog, contract_id, delivered_resources, reward_wallet);
    if (!result.ok()) {
        return result;
    }

    if (!ledger.record_contract_reward(result.contract_id, result.resource_id, result.quantity, result.reward_coins, "contract reward payout")) {
        result.validation.add_error("simulation.contract." + result.contract_id, "failed to record contract reward ledger entry");
    }
    return result;
}

ContractFulfillmentResult fulfill_contract_from_arrived_caravan_with_reward_and_ledger(
    ContractCatalog& catalog,
    std::string_view contract_id,
    CaravanState& caravan,
    economy::Wallet& reward_wallet,
    economy::EconomyLedger& ledger
) {
    ContractFulfillmentResult result;
    if (!caravan_arrived(caravan)) {
        result.contract_id = std::string{contract_id};
        result.validation.add_error("simulation.contract." + std::string{contract_id}, "caravan must arrive before contract fulfillment");
        return result;
    }

    return fulfill_contract_from_storage_with_reward_and_ledger(catalog, contract_id, caravan.cargo, reward_wallet, ledger);
}

ContractFulfillmentResult fulfill_contract_from_owned_arrived_caravan_with_reward_and_ledger(
    ContractCatalog& catalog,
    std::string_view contract_id,
    CaravanState& caravan,
    const OwnershipCatalog& ownership,
    std::string_view expected_faction_id,
    economy::Wallet& reward_wallet,
    economy::EconomyLedger& ledger
) {
    ContractFulfillmentResult result;
    result.contract_id = std::string{contract_id};
    if (expected_faction_id.empty()) {
        result.validation.add_error("simulation.contract." + std::string{contract_id}, "expected_faction_id must not be empty");
        return result;
    }

    const auto owner = caravan_owner(ownership, caravan.id);
    if (owner.empty()) {
        result.validation.add_error("simulation.caravan." + caravan.id + ".owner", "caravan owner is not assigned");
        return result;
    }
    if (owner != expected_faction_id) {
        result.validation.add_error("simulation.caravan." + caravan.id + ".owner", "caravan owner does not match expected faction");
        return result;
    }

    return fulfill_contract_from_arrived_caravan_with_reward_and_ledger(catalog, contract_id, caravan, reward_wallet, ledger);
}

} // namespace clc::sim
