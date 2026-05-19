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

} // namespace clc::sim
