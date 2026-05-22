#include "clc/data/DataRegistry.hpp"
#include "clc/economy/Ledger.hpp"
#include "clc/economy/Market.hpp"
#include "clc/economy/Orders.hpp"
#include "clc/economy/Trade.hpp"
#include "clc/sim/Storage.hpp"

#include <cstdlib>
#include <iostream>
#include <string_view>
#include <vector>

namespace {

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "Test failed: " << message << '\n';
        std::exit(1);
    }
}

} // namespace

int main() {
    clc::data::DataRegistry registry;
    require(registry.add(clc::data::ResourceDefinition{
        .id = "grain",
        .display_name = "Grain",
        .category = "food",
        .base_value = 10,
    }).ok(), "grain resource should register");
    require(registry.add(clc::data::ResourceDefinition{
        .id = "wood",
        .display_name = "Wood",
        .category = "construction",
        .base_value = 6,
    }).ok(), "wood resource should register");
    require(registry.add(clc::data::ResourceDefinition{
        .id = "stone",
        .display_name = "Stone",
        .category = "construction",
        .base_value = 8,
    }).ok(), "stone resource should register");

    const auto* grain = registry.resource("grain");
    const auto* wood = registry.resource("wood");
    const auto* stone = registry.resource("stone");
    require(grain != nullptr, "grain should exist");
    require(wood != nullptr, "wood should exist");
    require(stone != nullptr, "stone should exist");

    const auto shortage = clc::economy::calculate_price(*grain, 16, 32);
    require(shortage.price > shortage.base_value, "shortage should raise price above base");
    require(shortage.reason == "demand exceeds supply", "shortage should explain reason");

    const auto surplus = clc::economy::calculate_price(*wood, 24, 6);
    require(surplus.price < surplus.base_value, "surplus should lower price below base");
    require(surplus.price >= 1, "surplus price should stay positive");

    const auto balanced = clc::economy::calculate_price(*grain, 10, 10);
    require(balanced.price == balanced.base_value, "balanced market should keep base price");
    require(balanced.reason == "balanced", "balanced market should explain reason");

    clc::economy::MarketState market;
    require(market.set_demand("grain", 32).ok(), "market should accept grain demand");
    require(market.set_demand("wood", 6).ok(), "market should accept wood demand");
    require(market.set_demand("stone", 12).ok(), "market should accept demand-only stone");
    require(market.set_demand("unknown_resource", 99).ok(), "market may store demand for unknown resources but reports should ignore it");
    require(market.demand("grain") == 32, "market should report grain demand");
    require(market.set_demand("wood", 0).ok(), "zero demand should clear demand entry");
    require(market.demand("wood") == 0, "cleared demand should be zero");
    require(market.set_demand("wood", 6).ok(), "market should restore wood demand");

    clc::sim::ResourceStorage storage;
    require(storage.add("grain", 16).ok(), "storage should accept grain supply");
    require(storage.add("wood", 24).ok(), "storage should accept wood supply");

    const auto prices = clc::economy::calculate_market_prices(registry, storage, market);
    require(prices.size() == 3, "market should price storage resources and registered demand-only resources");
    require(prices[0].resource_id == "grain", "market prices should be sorted by resource id");
    require(prices[1].resource_id == "stone", "market prices should include demand-only stone");
    require(prices[1].supply == 0, "demand-only resource should report zero supply");
    require(prices[1].demand == 12, "demand-only resource should report configured demand");
    require(prices[1].price > prices[1].base_value, "demand-only resource should get shortage premium");
    require(prices[1].reason == "demand exceeds supply", "demand-only resource should explain shortage");
    require(prices[2].resource_id == "wood", "market prices should include wood");

    const auto report = clc::economy::make_market_report(registry, storage, market);
    require(report.prices.size() == 3, "market report should include demand-only price rows");
    require(report.total_supply == 40, "market report should aggregate total supply");
    require(report.total_demand == 50, "market report should aggregate total demand including demand-only resources");
    require(report.min_price >= 1, "market report min price should stay positive");
    require(report.max_price >= report.min_price, "market report max price should be >= min price");
    require(report.average_price >= report.min_price, "market report average price should be >= min price");
    require(report.average_price <= report.max_price, "market report average price should be <= max price");

    clc::sim::ResourceStorage empty_storage;
    const auto empty_report = clc::economy::make_market_report(registry, empty_storage, market);
    require(empty_report.prices.size() == 3, "empty storage with registered demand should still report demand-only prices");
    require(empty_report.total_supply == 0, "empty storage demand-only report should have zero supply");
    require(empty_report.total_demand == 50, "empty storage demand-only report should aggregate demand");
    require(empty_report.average_price >= 1, "demand-only market report should have positive average price");
    require(empty_report.min_price >= 1, "demand-only market report should have positive min price");
    require(empty_report.max_price >= empty_report.min_price, "demand-only market report max price should be >= min price");

    clc::economy::Wallet wallet{.coins = 100};
    clc::sim::ResourceStorage trade_storage;
    const auto buy = clc::economy::buy_resource(wallet, trade_storage, balanced, 3);
    require(buy.ok, "buy should succeed with enough coins");
    require(buy.total_price == 30, "buy total should be unit price times quantity");
    require(wallet.coins == 70, "buy should debit wallet");
    require(trade_storage.amount("grain") == 3, "buy should add resource to storage");

    const auto failed_buy = clc::economy::buy_resource(wallet, trade_storage, balanced, 100);
    require(!failed_buy.ok, "buy should fail without enough coins");
    require(wallet.coins == 70, "failed buy should not debit wallet");
    require(trade_storage.amount("grain") == 3, "failed buy should not add resources");

    const auto sell = clc::economy::sell_resource(wallet, trade_storage, balanced, 2);
    require(sell.ok, "sell should succeed with enough resources");
    require(sell.total_price == 20, "sell total should be unit price times quantity");
    require(wallet.coins == 90, "sell should credit wallet");
    require(trade_storage.amount("grain") == 1, "sell should remove resource from storage");

    const auto failed_sell = clc::economy::sell_resource(wallet, trade_storage, balanced, 2);
    require(!failed_sell.ok, "sell should fail without enough resources");
    require(wallet.coins == 90, "failed sell should not credit wallet");
    require(trade_storage.amount("grain") == 1, "failed sell should not remove resources");

    clc::economy::EconomyLedger ledger;
    require(ledger.record(clc::economy::LedgerEntryType::buy, buy, "initial purchase"), "ledger should record successful buy");
    require(ledger.record(clc::economy::LedgerEntryType::sell, sell, "partial sale"), "ledger should record successful sell");
    require(!ledger.record(clc::economy::LedgerEntryType::buy, failed_buy, "failed purchase"), "ledger should reject failed trade results");
    require(ledger.record_contract_reward("contract_001", "grain", 50, 120, "contract paid"), "ledger should record contract reward");
    require(!ledger.record_contract_reward("", "grain", 50, 120, "invalid contract"), "ledger should reject empty reward contract id");
    require(!ledger.record_contract_reward("contract_002", "", 50, 120, "invalid resource"), "ledger should reject empty reward resource id");
    require(!ledger.record_contract_reward("contract_002", "grain", 0, 120, "invalid quantity"), "ledger should reject zero reward quantity");
    require(!ledger.record_contract_reward("contract_002", "grain", 50, 0, "invalid reward"), "ledger should reject zero reward coins");
    require(ledger.entries().size() == 3, "ledger should contain two trade entries and one reward entry");
    require(ledger.entries()[0].sequence == 1, "first ledger entry should have sequence 1");
    require(ledger.entries()[1].sequence == 2, "second ledger entry should have sequence 2");
    require(ledger.entries()[2].sequence == 3, "third ledger entry should have sequence 3");
    require(ledger.entries()[2].type == clc::economy::LedgerEntryType::contract_reward, "third ledger entry should be contract reward");
    require(ledger.entries()[2].reference_id == "contract_001", "reward entry should preserve contract reference id");
    require(ledger.entries()[2].total_price == 120, "reward entry should store reward coins as total price");
    require(ledger.next_sequence() == 4, "ledger next sequence should advance");
    require(ledger.total_bought("grain") == 3, "ledger should aggregate bought quantity");
    require(ledger.total_sold("grain") == 2, "ledger should aggregate sold quantity");
    require(ledger.total_contract_rewards("grain") == 120, "ledger should aggregate reward coins by resource");
    require(ledger.entries()[0].note == "initial purchase", "ledger should preserve notes");
    require(ledger.entries()[2].note == "contract paid", "ledger should preserve reward notes");

    const auto invalid_order_report = clc::economy::validate_order(clc::economy::MarketOrder{}, "invalid_order");
    require(!invalid_order_report.ok(), "empty market order should fail validation");

    const auto matched_orders = clc::economy::match_orders(
        std::vector<clc::economy::MarketOrder>{
            clc::economy::MarketOrder{.resource_id = "grain", .quantity = 5, .limit_price = 12},
            clc::economy::MarketOrder{.resource_id = "wood", .quantity = 3, .limit_price = 7},
        },
        std::vector<clc::economy::MarketOrder>{
            clc::economy::MarketOrder{.resource_id = "grain", .quantity = 2, .limit_price = 9},
            clc::economy::MarketOrder{.resource_id = "wood", .quantity = 10, .limit_price = 6},
        }
    );
    require(matched_orders.trades.size() == 2, "compatible orders should match into trades");
    require(matched_orders.total_quantity == 5, "matched orders should aggregate quantity");
    require(matched_orders.total_value == 36, "matched orders should aggregate value at sell limits");

    const auto no_match_orders = clc::economy::match_orders(
        std::vector<clc::economy::MarketOrder>{clc::economy::MarketOrder{.resource_id = "grain", .quantity = 1, .limit_price = 5}},
        std::vector<clc::economy::MarketOrder>{clc::economy::MarketOrder{.resource_id = "grain", .quantity = 1, .limit_price = 6}}
    );
    require(no_match_orders.trades.empty(), "orders should not match when buy limit is below sell limit");

    const auto invalid_match_orders = clc::economy::match_orders(
        std::vector<clc::economy::MarketOrder>{clc::economy::MarketOrder{.resource_id = "grain", .quantity = 0, .limit_price = 5}},
        std::vector<clc::economy::MarketOrder>{clc::economy::MarketOrder{.resource_id = "grain", .quantity = 1, .limit_price = 4}}
    );
    require(invalid_match_orders.trades.empty(), "invalid buy orders should be ignored");
    require(!invalid_match_orders.warnings.empty(), "invalid orders should produce warnings");

    return 0;
}
