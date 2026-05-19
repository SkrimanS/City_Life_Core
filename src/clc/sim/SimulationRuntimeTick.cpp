#include "clc/sim/SimulationRuntimeTick.hpp"

#include <utility>

namespace clc::sim {

SimulationRuntimeDayReport advance_runtime_day(SimulationRuntime& runtime) {
    SimulationRuntimeDayReport report{};
    report.engine = runtime.engine.advance_day();

    for (const auto& caravan : runtime.caravans.caravans) {
        RuntimeCaravanTickReport caravan_report{};
        caravan_report.caravan_id = caravan.id;

        const auto advanced = advance_runtime_caravan_day(runtime, caravan.id);
        caravan_report.validation = advanced.validation;

        if (advanced.ok()) {
            caravan_report.advance = advanced.report;
        }

        for (const auto& message : advanced.validation.messages()) {
            if (message.severity == data::ValidationSeverity::error) {
                report.validation.add_error(message.path, message.message);
            } else {
                report.validation.add_warning(message.path, message.message);
            }
        }

        report.caravans.push_back(std::move(caravan_report));
    }

    return report;
}

} // namespace clc::sim
