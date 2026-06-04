#pragma once

#include "clc/data/Validation.hpp"
#include "clc/sim/RuntimeCoreSystems.hpp"
#include "clc/sim/SimulationProcessor.hpp"
#include "clc/sim/SimulationRuntimeTick.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace clc::sim {

struct RuntimeProcessorEvent final {
    clc::GameTime::Tick tick{0};
    std::string processor_id{};
    std::string event_type{};
    std::string payload{};
};

struct RuntimeProcessorOrchestrationConfig final {
    clc::GameTime::Tick tick_step{clc::ticks_per_minute()};
    std::uint64_t max_event_count{64};
    RuntimeCoreSystemsConfig core_systems{};
};

struct RuntimeProcessorOrchestrationReport final {
    clc::GameTime::Tick tick_before{0};
    clc::GameTime::Tick tick_after{0};
    SimulationProcessorRunReport processor_run{};
    SimulationRuntimeTickReport tick{};
    RuntimeCoreSystemsReport core_systems{};
    std::vector<RuntimeProcessorEvent> events{};
    std::uint64_t dropped_event_count{0};
    std::uint64_t deep_event_count{0};
    std::uint64_t regional_event_count{0};
    data::ValidationReport validation{};

    [[nodiscard]] bool ok() const noexcept {
        return validation.ok();
    }
};

[[nodiscard]] RuntimeProcessorOrchestrationConfig make_default_runtime_processor_orchestration_config();
[[nodiscard]] RuntimeProcessorOrchestrationReport orchestrate_runtime_core_processors(
    SimulationRuntime& runtime,
    const SimulationProcessorRegistry& registry,
    const RuntimeProcessorOrchestrationConfig& config = make_default_runtime_processor_orchestration_config()
);
[[nodiscard]] RuntimeProcessorOrchestrationReport orchestrate_standard_runtime_core_processors(
    SimulationRuntime& runtime,
    const RuntimeProcessorOrchestrationConfig& config = make_default_runtime_processor_orchestration_config()
);
[[nodiscard]] std::string runtime_processor_orchestration_digest(const RuntimeProcessorOrchestrationReport& report);
[[nodiscard]] std::string runtime_processor_orchestration_markdown(const RuntimeProcessorOrchestrationReport& report);

} // namespace clc::sim
