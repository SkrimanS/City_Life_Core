#pragma once

#include "clc/core/Time.hpp"
#include "clc/data/Validation.hpp"
#include "clc/economy/Market.hpp"
#include "clc/sim/Caravans.hpp"
#include "clc/sim/Contracts.hpp"
#include "clc/sim/Factions.hpp"
#include "clc/sim/Ownership.hpp"
#include "clc/sim/Storage.hpp"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace clc::sim {

enum class ContractResourceFlowSource {
    storage,
    caravan,
};

struct ContractEconomyAssessment final {
    std::string contract_id{};
    std::string resource_id{};
    std::string issuer_faction_id{};
    std::string receiver_faction_id{};
    std::uint64_t quantity{0};
    std::uint64_t reward_coins{0};
    std::uint64_t market_unit_price{0};
    std::uint64_t market_total_value{0};
    std::int64_t reward_margin{0};
    std::uint64_t available_quantity{0};
    std::uint64_t missing_quantity{0};
    clc::GameTime::Tick due_tick{0};
    clc::GameTime::Tick current_tick{0};
    clc::GameTime::Tick ticks_until_due{0};
    bool known_contract{false};
    bool open{false};
    bool overdue{false};
    bool has_required_resources{false};
    bool reward_covers_market_value{false};
    FactionAccessReport access{};
    bool accepted_by_factions{false};
    bool deliverable{false};
    std::string reason{};
};

struct ContractResourceFlowPlan final {
    std::string contract_id{};
    std::string resource_id{};
    std::string source_id{};
    ContractResourceFlowSource source{ContractResourceFlowSource::storage};
    std::uint64_t required_quantity{0};
    std::uint64_t available_quantity{0};
    std::uint64_t transferable_quantity{0};
    std::uint64_t missing_quantity{0};
    bool source_ready{true};
    bool owner_matches{true};
    bool can_fulfill{false};
    std::string reason{};
};

struct ContractEconomyPortfolioSummary final {
    std::uint64_t contract_count{0};
    std::uint64_t deliverable_count{0};
    std::uint64_t blocked_by_status_count{0};
    std::uint64_t blocked_by_reputation_count{0};
    std::uint64_t blocked_by_resources_count{0};
    std::uint64_t overdue_count{0};
    std::uint64_t total_reward_coins{0};
    std::uint64_t total_market_value{0};
    std::int64_t total_reward_margin{0};
    std::vector<std::string> deliverable_contract_ids{};
    std::vector<std::string> blocked_contract_ids{};
};

[[nodiscard]] std::string_view contract_resource_flow_source_name(ContractResourceFlowSource source) noexcept;

[[nodiscard]] ContractEconomyAssessment make_contract_economy_assessment(
    const ContractCatalog& catalog,
    std::string_view contract_id,
    const FactionCatalog& factions,
    const ResourceStorage& available_resources,
    const economy::MarketReport& market,
    clc::GameTime::Tick current_tick = 0
);

[[nodiscard]] ContractResourceFlowPlan plan_contract_flow_from_storage(
    const ContractCatalog& catalog,
    std::string_view contract_id,
    const ResourceStorage& storage,
    std::string source_id = {}
);

[[nodiscard]] ContractResourceFlowPlan plan_contract_flow_from_caravan(
    const ContractCatalog& catalog,
    std::string_view contract_id,
    const CaravanState& caravan,
    const OwnershipCatalog* ownership = nullptr,
    std::string_view expected_owner_faction_id = {}
);

[[nodiscard]] ContractEconomyPortfolioSummary make_contract_economy_portfolio_summary(
    const ContractCatalog& catalog,
    const FactionCatalog& factions,
    const ResourceStorage& available_resources,
    const economy::MarketReport& market,
    clc::GameTime::Tick current_tick = 0
);

[[nodiscard]] data::ValidationReport validate_contract_economy_assessment(
    const ContractEconomyAssessment& assessment
);

[[nodiscard]] std::string contract_economy_assessment_digest(const ContractEconomyAssessment& assessment);
[[nodiscard]] std::string contract_resource_flow_plan_digest(const ContractResourceFlowPlan& plan);
[[nodiscard]] std::string contract_economy_portfolio_summary_digest(const ContractEconomyPortfolioSummary& summary);

} // namespace clc::sim
