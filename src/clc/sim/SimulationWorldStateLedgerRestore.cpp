#include "clc/sim/SimulationPersistence.hpp"

namespace clc::sim {

data::ValidationReport restore_ledger_from_world_state(
    const SimulationWorldState& state,
    economy::EconomyLedger& ledger
) {
    return ledger.restore_entries(state.ledger_entries);
}

} // namespace clc::sim
