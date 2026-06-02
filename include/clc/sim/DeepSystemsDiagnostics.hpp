#pragma once

#include "clc/economy/Ledger.hpp"
#include "clc/economy/Market.hpp"
#include "clc/sim/Contracts.hpp"
#include "clc/sim/Logistics.hpp"
#include "clc/sim/Production.hpp"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace clc::sim {

struct DeepSystemsDiagnostics final {
    economy::MarketSnapshot market{};
    economy::LedgerSummary ledger{};
    ContractLifecycleSummary contracts{};
    SettlementProductionSnapshot production{};
    LogisticsNetworkSnapshot logistics{};
    std::uint64_t warning_count{0};
    std::uint64_t critical_count{0};
    std::vector<std::string> warnings{};
};

[[nodiscard]] DeepSystemsDiagnostics make_deep_systems_diagnostics(
    economy::MarketSnapshot market,
    economy::LedgerSummary ledger,
    ContractLifecycleSummary contracts,
    SettlementProductionSnapshot production,
    LogisticsNetworkSnapshot logistics
);

[[nodiscard]] std::string deep_systems_diagnostics_digest(const DeepSystemsDiagnostics& diagnostics);
[[nodiscard]] std::string deep_systems_diagnostics_markdown(const DeepSystemsDiagnostics& diagnostics);
[[nodiscard]] bool deep_systems_diagnostics_has_warnings(const DeepSystemsDiagnostics& diagnostics) noexcept;
[[nodiscard]] bool deep_systems_diagnostics_has_critical_warnings(const DeepSystemsDiagnostics& diagnostics) noexcept;
[[nodiscard]] const std::string* deep_systems_warning_by_prefix(
    const DeepSystemsDiagnostics& diagnostics,
    std::string_view prefix
) noexcept;

} // namespace clc::sim
