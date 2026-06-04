#pragma once

#include "clc/data/Validation.hpp"
#include "clc/sim/DeepSimulationSystems.hpp"
#include "clc/sim/RegionalSimulationSystems.hpp"
#include "clc/sim/SimulationProcessor.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace clc::sim {

struct CoreSystemsAuditArea final {
    std::string area_id{};
    bool ready{false};
    std::uint64_t covered_item_count{0};
    std::uint64_t expected_item_count{0};
    std::vector<std::string> notes{};
};

struct CoreSystemsAuditReport final {
    bool ready{false};
    std::uint64_t ready_area_count{0};
    std::uint64_t area_count{0};
    std::vector<CoreSystemsAuditArea> areas{};
    data::ValidationReport validation{};
};

[[nodiscard]] CoreSystemsAuditReport make_core_systems_audit_report(const SimulationProcessorRegistry& registry);
[[nodiscard]] CoreSystemsAuditReport make_standard_core_systems_audit_report();
[[nodiscard]] std::string core_systems_audit_digest(const CoreSystemsAuditReport& report);
[[nodiscard]] std::string core_systems_audit_markdown(const CoreSystemsAuditReport& report);

} // namespace clc::sim
