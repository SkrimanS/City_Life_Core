#pragma once

#include "clc/sim/Contracts.hpp"

namespace clc::sim {

// Contract reward helpers are declared in Contracts.hpp for compatibility with
// the existing public contract API. This forwarding header exists because the
// reward/ledger implementation is built from src/clc/sim/ContractRewards.cpp
// and public documentation historically referenced clc/sim/ContractRewards.hpp.
//
// New code may include either Contracts.hpp or ContractRewards.hpp when using:
// - fulfill_contract_from_storage_with_reward_and_ledger(...)
// - fulfill_contract_from_arrived_caravan_with_reward_and_ledger(...)
// - fulfill_contract_from_owned_arrived_caravan_with_reward_and_ledger(...)

} // namespace clc::sim
