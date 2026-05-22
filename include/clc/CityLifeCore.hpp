#pragma once

// City Life Core recommended SDK umbrella header.
//
// This header intentionally includes the stable-candidate runtime/economy/data
// surface for external SDK users. It does not include raw persistence internals,
// demo-only helpers, or experimental order/data-pack APIs by default.

#include "clc/core/EventLog.hpp"
#include "clc/core/Ids.hpp"
#include "clc/core/Result.hpp"
#include "clc/core/Time.hpp"
#include "clc/core/Version.hpp"

#include "clc/data/DataRegistry.hpp"
#include "clc/data/Definitions.hpp"
#include "clc/data/Validation.hpp"

#include "clc/economy/Ledger.hpp"
#include "clc/economy/Market.hpp"
#include "clc/economy/Trade.hpp"

#include "clc/sim/Caravans.hpp"
#include "clc/sim/ContractRewards.hpp"
#include "clc/sim/Contracts.hpp"
#include "clc/sim/Factions.hpp"
#include "clc/sim/Ownership.hpp"
#include "clc/sim/Routes.hpp"
#include "clc/sim/Settlement.hpp"
#include "clc/sim/SimulationEngine.hpp"
#include "clc/sim/SimulationRuntime.hpp"
#include "clc/sim/SimulationRuntimeEvents.hpp"
#include "clc/sim/SimulationRuntimePersistenceValidation.hpp"
#include "clc/sim/SimulationRuntimeScenario.hpp"
#include "clc/sim/SimulationRuntimeTick.hpp"
#include "clc/sim/SimulationRuntimeWorkflow.hpp"
#include "clc/sim/Storage.hpp"
