#include "clc/sim/Contracts.hpp"

#include <cstdlib>
#include <iostream>
#include <limits>
#include <string_view>

namespace {

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "Test failed: " << message << '\n';
        std::exit(1);
    }
}

clc::sim::ResourceDeliveryContract make_contract(std::string_view id = "grain_delivery_001", std::uint64_t due_day = 7) {
    return clc::sim::ResourceDeliveryContract{
        .id = std::string{id},
        .display_name = "Grain Delivery",
        .issuer_faction_id = "riverwatch",
        .receiver_faction_id = "traders_guild",
        .resource_id = "grain",
        .quantity = 50,
        .reward_coins = 120,
        .due_day = due_day,
    };
}

clc::sim::FactionCatalog make_factions() {
    clc::sim::FactionCatalog factions;
    require(clc::sim::add_faction(factions, clc::sim::FactionState{.id = "riverwatch", .display_name = "Riverwatch"}).ok(), "riverwatch faction should add");
    require(clc::sim::add_faction(factions, clc::sim::FactionState{.id = "traders_guild", .display_name = "Traders Guild"}).ok(), "traders guild faction should add");
    return factions;
}

clc::sim::SettlementRoute make_route() {
    return clc::sim::SettlementRoute{
        .id = "riverwatch_to_traders",
        .display_name = "Riverwatch to Traders Guild",
        .origin_settlement_id = "riverwatch",
        .destination_settlement_id = "traders_guild",
        .travel_days = 2,
    };
}

} // namespace

int main() {
    require(clc::sim::contract_status_name(clc::sim::ContractStatus::open) == std::string_view{"open"}, "open status name should be stable");
    require(clc::sim::contract_status_name(clc::sim::ContractStatus::fulfilled) == std::string_view{"fulfilled"}, "fulfilled status name should be stable");
    require(clc::sim::contract_status_name(clc::sim::ContractStatus::failed) == std::string_view{"failed"}, "failed status name should be stable");
    require(clc::sim::contract_status_name(clc::sim::ContractStatus::cancelled) == std::string_view{"cancelled"}, "cancelled status name should be stable");

    const auto valid_contract = make_contract();
    require(clc::sim::validate_resource_delivery_contract(valid_contract).ok(), "valid contract should pass validation");
    require(clc::sim::validate_resource_delivery_contract_for_factions(valid_contract, make_factions()).ok(), "valid contract should resolve factions");
    require(clc::sim::contract_is_open(valid_contract), "new contract should be open");
    require(!clc::sim::contract_is_terminal(valid_contract), "new contract should not be terminal");

    const clc::sim::ResourceDeliveryContract empty_contract;
    const auto empty_report = clc::sim::validate_resource_delivery_contract(empty_contract);
    require(!empty_report.ok(), "empty contract should fail validation");
    require(empty_report.error_count() == 8, "empty contract should report required field errors");

    auto self_contract = valid_contract;
    self_contract.receiver_faction_id = self_contract.issuer_faction_id;
    require(!clc::sim::validate_resource_delivery_contract(self_contract).ok(), "contract should reject same issuer and receiver");

    auto unknown_contract = valid_contract;
    unknown_contract.receiver_faction_id = "missing";
    const auto unknown_report = clc::sim::validate_resource_delivery_contract_for_factions(unknown_contract, make_factions());
    require(!unknown_report.ok(), "contract should reject unknown receiver faction");
    require(unknown_report.error_count() == 1, "unknown receiver should report one error");

    clc::sim::ContractCatalog catalog;
    require(clc::sim::contract_count(catalog) == 0, "empty contract catalog should count zero");
    require(clc::sim::contract_by_id(catalog, "grain_delivery_001") == nullptr, "empty contract catalog lookup should miss");
    require(clc::sim::add_contract(catalog, valid_contract).ok(), "valid contract should add to catalog");
    require(clc::sim::contract_count(catalog) == 1, "catalog should count inserted contract");
    require(!clc::sim::add_contract(catalog, valid_contract).ok(), "duplicate contract id should be rejected");
    require(!clc::sim::add_contract(catalog, empty_contract).ok(), "invalid contract should not add");

    auto fulfilled_input_contract = make_contract("already_fulfilled_input");
    fulfilled_input_contract.status = clc::sim::ContractStatus::fulfilled;
    require(!clc::sim::add_contract(catalog, fulfilled_input_contract).ok(), "new fulfilled contract input should be rejected");
    require(clc::sim::contract_by_id(catalog, "already_fulfilled_input") == nullptr, "rejected fulfilled contract input should not be inserted");

    const auto* found = clc::sim::contract_by_id(catalog, "grain_delivery_001");
    require(found != nullptr, "contract lookup should find inserted contract");
    require(found->resource_id == "grain", "contract resource should be preserved");
    require(clc::sim::open_contracts(catalog).size() == 1, "open filter should include open contract");
    require(clc::sim::terminal_contracts(catalog).empty(), "terminal filter should be empty before transition");
    require(clc::sim::contracts_for_faction(catalog, "riverwatch").size() == 1, "faction filter should include issuer contract");
    require(clc::sim::contracts_for_faction(catalog, "traders_guild").size() == 1, "faction filter should include receiver contract");
    require(clc::sim::overdue_open_contracts(catalog, 7).empty(), "contract should not be overdue on due day");
    require(clc::sim::overdue_open_contracts(catalog, 8).size() == 1, "contract should be overdue after due day");

    clc::sim::ResourceStorage insufficient_delivery;
    require(insufficient_delivery.add("grain", 49).ok(), "insufficient delivery storage should accept grain");
    const auto insufficient_result = clc::sim::fulfill_contract_from_storage(catalog, "grain_delivery_001", insufficient_delivery);
    require(!insufficient_result.ok(), "fulfillment should fail when delivered resources are insufficient");
    require(insufficient_delivery.amount("grain") == 49, "failed fulfillment should not debit delivered storage");
    require(clc::sim::contract_by_id(catalog, "grain_delivery_001")->status == clc::sim::ContractStatus::open, "failed fulfillment should not mutate contract status");

    clc::sim::ResourceStorage delivery;
    require(delivery.add("grain", 60).ok(), "delivery storage should accept grain");
    require(delivery.add("wood", 5).ok(), "delivery storage should accept unrelated resource");
    const auto fulfillment = clc::sim::fulfill_contract_from_storage(catalog, "grain_delivery_001", delivery);
    require(fulfillment.ok(), "fulfillment should succeed with enough delivered resources");
    require(delivery.amount("grain") == 10, "successful fulfillment should debit required grain only");
    require(delivery.amount("wood") == 5, "successful fulfillment should not debit unrelated resources");
    require(clc::sim::contract_by_id(catalog, "grain_delivery_001")->status == clc::sim::ContractStatus::fulfilled, "successful fulfillment should mark contract fulfilled");
    require(clc::sim::terminal_contracts(catalog).size() == 1, "terminal filter should include fulfilled contract");
    require(clc::sim::open_contracts(catalog).empty(), "open filter should exclude fulfilled contract");
    require(!clc::sim::fulfill_contract_from_storage(catalog, "grain_delivery_001", delivery).ok(), "terminal contract should reject second fulfillment");
    require(!clc::sim::mark_contract_failed(catalog, "grain_delivery_001").ok(), "terminal contract should reject second transition");
    require(!clc::sim::mark_contract_fulfilled(catalog, "missing").ok(), "unknown contract transition should fail");

    auto reward_contract = make_contract("grain_delivery_reward");
    require(clc::sim::add_contract(catalog, reward_contract).ok(), "reward contract should add");
    clc::sim::ResourceStorage rewarded_delivery;
    require(rewarded_delivery.add("grain", 60).ok(), "rewarded delivery should accept grain");
    clc::economy::Wallet reward_wallet{.coins = 10};
    const auto rewarded_fulfillment = clc::sim::fulfill_contract_from_storage_with_reward(catalog, "grain_delivery_reward", rewarded_delivery, reward_wallet);
    require(rewarded_fulfillment.ok(), "rewarded fulfillment should succeed");
    require(reward_wallet.coins == 130, "rewarded fulfillment should credit reward coins");

    auto reward_overflow_contract = make_contract("grain_delivery_reward_overflow");
    require(clc::sim::add_contract(catalog, reward_overflow_contract).ok(), "reward overflow contract should add");
    clc::sim::ResourceStorage overflow_delivery;
    require(overflow_delivery.add("grain", 60).ok(), "overflow delivery should accept grain");
    clc::economy::Wallet overflow_wallet{.coins = std::numeric_limits<std::uint64_t>::max() - 50};
    const auto overflow_fulfillment = clc::sim::fulfill_contract_from_storage_with_reward(catalog, "grain_delivery_reward_overflow", overflow_delivery, overflow_wallet);
    require(!overflow_fulfillment.ok(), "rewarded fulfillment should reject wallet overflow");
    require(overflow_delivery.amount("grain") == 60, "reward overflow should not debit storage");

    auto caravan_contract = make_contract("grain_delivery_caravan");
    require(clc::sim::add_contract(catalog, caravan_contract).ok(), "caravan contract should add");
    auto caravan = clc::sim::create_caravan_for_route(make_route(), "contract_caravan", "Contract Caravan");
    require(caravan.cargo.add("grain", 55).ok(), "caravan cargo should accept grain");
    require(!clc::sim::fulfill_contract_from_arrived_caravan(catalog, "grain_delivery_caravan", caravan).ok(), "caravan fulfillment should require arrival");
    require(clc::sim::advance_caravan_day(caravan).days_remaining_after == 1, "caravan should move day one");
    require(clc::sim::advance_caravan_day(caravan).arrived, "caravan should arrive day two");
    require(clc::sim::fulfill_contract_from_arrived_caravan(catalog, "grain_delivery_caravan", caravan).ok(), "arrived caravan should fulfill contract from cargo");

    auto owned_contract = make_contract("grain_delivery_owned_caravan");
    require(clc::sim::add_contract(catalog, owned_contract).ok(), "owned caravan contract should add");
    auto owned_caravan = clc::sim::create_caravan_for_route(make_route(), "owned_contract_caravan", "Owned Contract Caravan");
    require(owned_caravan.cargo.add("grain", 60).ok(), "owned caravan cargo should accept grain");
    require(clc::sim::advance_caravan_day(owned_caravan).days_remaining_after == 1, "owned caravan should move day one");
    require(clc::sim::advance_caravan_day(owned_caravan).arrived, "owned caravan should arrive day two");
    clc::sim::OwnershipCatalog ownership;
    require(!clc::sim::fulfill_contract_from_owned_arrived_caravan(catalog, "grain_delivery_owned_caravan", owned_caravan, ownership, "riverwatch").ok(), "owned fulfillment should require assigned caravan owner");
    require(clc::sim::set_caravan_owner(ownership, "owned_contract_caravan", "riverwatch").ok(), "expected caravan owner should set");
    require(clc::sim::fulfill_contract_from_owned_arrived_caravan(catalog, "grain_delivery_owned_caravan", owned_caravan, ownership, "riverwatch").ok(), "owned arrived caravan should fulfill contract");

    clc::sim::ContractCatalog deadline_catalog;
    auto deadline_contract = make_contract("grain_delivery_deadline", 12);
    require(clc::sim::add_contract(deadline_catalog, deadline_contract).ok(), "deadline contract should add");
    auto deadline_boundary_report = clc::sim::fail_overdue_open_contracts(deadline_catalog, 12);
    require(deadline_boundary_report.current_day == 12, "deadline report should include current day");
    require(deadline_boundary_report.failed_count == 0, "contract should not fail on due day");
    require(deadline_boundary_report.failed_contract_ids.empty(), "due day sweep should not report failures");
    require(clc::sim::contract_by_id(deadline_catalog, "grain_delivery_deadline")->status == clc::sim::ContractStatus::open, "due day sweep should leave contract open");
    auto deadline_report = clc::sim::fail_overdue_open_contracts(deadline_catalog, 13);
    require(deadline_report.current_day == 13, "overdue report should include current day");
    require(deadline_report.failed_count == 1, "overdue sweep should fail one open contract");
    require(deadline_report.failed_contract_ids.size() == 1, "overdue sweep should report failed ids");
    require(deadline_report.failed_contract_ids[0] == "grain_delivery_deadline", "overdue sweep should report expected failed contract id");
    require(clc::sim::contract_by_id(deadline_catalog, "grain_delivery_deadline")->status == clc::sim::ContractStatus::failed, "overdue sweep should mark contract failed");
    auto repeat_deadline_report = clc::sim::fail_overdue_open_contracts(deadline_catalog, 100);
    require(repeat_deadline_report.failed_count == 0, "repeat sweep should not fail terminal contracts again");

    return 0;
}
