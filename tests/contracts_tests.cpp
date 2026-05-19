#include "clc/sim/Contracts.hpp"

#include <cstdlib>
#include <iostream>
#include <string_view>

namespace {

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "Test failed: " << message << '\n';
        std::exit(1);
    }
}

clc::sim::ResourceDeliveryContract make_contract() {
    return clc::sim::ResourceDeliveryContract{
        .id = "grain_delivery_001",
        .display_name = "Grain Delivery 001",
        .issuer_faction_id = "riverwatch",
        .receiver_faction_id = "traders_guild",
        .resource_id = "grain",
        .quantity = 50,
        .reward_coins = 120,
        .due_day = 7,
    };
}

clc::sim::FactionCatalog make_factions() {
    clc::sim::FactionCatalog factions;
    require(clc::sim::add_faction(factions, clc::sim::FactionState{.id = "riverwatch", .display_name = "Riverwatch"}).ok(), "riverwatch faction should add");
    require(clc::sim::add_faction(factions, clc::sim::FactionState{.id = "traders_guild", .display_name = "Traders Guild"}).ok(), "traders guild faction should add");
    require(clc::sim::add_faction(factions, clc::sim::FactionState{.id = "hillford", .display_name = "Hillford"}).ok(), "hillford faction should add");
    return factions;
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
    require(clc::sim::contract_count(catalog) == 1, "duplicate contract should not change catalog size");
    require(!clc::sim::add_contract(catalog, empty_contract).ok(), "invalid contract should not add");
    require(clc::sim::contract_count(catalog) == 1, "invalid contract should not change catalog size");

    const auto* found = clc::sim::contract_by_id(catalog, "grain_delivery_001");
    require(found != nullptr, "contract lookup should find inserted contract");
    require(found->resource_id == "grain", "contract resource should be preserved");
    require(found->quantity == 50, "contract quantity should be preserved");
    require(found->reward_coins == 120, "contract reward should be preserved");
    require(clc::sim::mutable_contract_by_id(catalog, "grain_delivery_001") != nullptr, "mutable lookup should find inserted contract");

    require(clc::sim::open_contracts(catalog).size() == 1, "open filter should include open contract");
    require(clc::sim::terminal_contracts(catalog).empty(), "terminal filter should be empty before transition");
    require(clc::sim::contracts_for_faction(catalog, "riverwatch").size() == 1, "faction filter should include issuer contract");
    require(clc::sim::contracts_for_faction(catalog, "traders_guild").size() == 1, "faction filter should include receiver contract");
    require(clc::sim::contracts_for_faction(catalog, "missing").empty(), "faction filter should be empty for unrelated faction");
    require(clc::sim::overdue_open_contracts(catalog, 7).empty(), "contract should not be overdue on due day");
    require(clc::sim::overdue_open_contracts(catalog, 8).size() == 1, "contract should be overdue after due day");

    require(clc::sim::mark_contract_fulfilled(catalog, "grain_delivery_001").ok(), "open contract should mark fulfilled");
    require(!clc::sim::contract_is_open(*clc::sim::contract_by_id(catalog, "grain_delivery_001")), "fulfilled contract should not be open");
    require(clc::sim::contract_is_terminal(*clc::sim::contract_by_id(catalog, "grain_delivery_001")), "fulfilled contract should be terminal");
    require(clc::sim::terminal_contracts(catalog).size() == 1, "terminal filter should include fulfilled contract");
    require(clc::sim::open_contracts(catalog).empty(), "open filter should exclude fulfilled contract");
    require(clc::sim::overdue_open_contracts(catalog, 100).empty(), "fulfilled contract should not be overdue open");
    require(!clc::sim::mark_contract_failed(catalog, "grain_delivery_001").ok(), "terminal contract should reject second transition");
    require(!clc::sim::cancel_contract(catalog, "grain_delivery_001").ok(), "terminal contract should reject cancellation");
    require(!clc::sim::mark_contract_fulfilled(catalog, "missing").ok(), "unknown contract transition should fail");
    require(!clc::sim::mark_contract_fulfilled(catalog, "").ok(), "empty contract transition id should fail");

    auto failed_contract = valid_contract;
    failed_contract.id = "grain_delivery_failed";
    failed_contract.display_name = "Failed Grain Delivery";
    require(clc::sim::add_contract(catalog, failed_contract).ok(), "second contract should add");
    require(clc::sim::mark_contract_failed(catalog, "grain_delivery_failed").ok(), "open contract should mark failed");
    require(clc::sim::contract_by_id(catalog, "grain_delivery_failed")->status == clc::sim::ContractStatus::failed, "failed transition should set failed status");

    auto cancelled_contract = valid_contract;
    cancelled_contract.id = "grain_delivery_cancelled";
    cancelled_contract.display_name = "Cancelled Grain Delivery";
    require(clc::sim::add_contract(catalog, cancelled_contract).ok(), "third contract should add");
    require(clc::sim::cancel_contract(catalog, "grain_delivery_cancelled").ok(), "open contract should cancel");
    require(clc::sim::contract_by_id(catalog, "grain_delivery_cancelled")->status == clc::sim::ContractStatus::cancelled, "cancel transition should set cancelled status");

    return 0;
}
