#include "clc/sim/Contracts.hpp"

#include <utility>

namespace clc::sim {
namespace {

data::ValidationReport mark_contract_status(
    ContractCatalog& catalog,
    std::string_view contract_id,
    ContractStatus status,
    std::string_view action_name
) {
    data::ValidationReport report;
    if (contract_id.empty()) {
        report.add_error("simulation.contract", "contract_id must not be empty");
        return report;
    }

    auto* contract = mutable_contract_by_id(catalog, contract_id);
    if (contract == nullptr) {
        report.add_error("simulation.contract." + std::string{contract_id}, "unknown contract");
        return report;
    }
    if (!contract_is_open(*contract)) {
        report.add_error("simulation.contract." + contract->id, std::string{action_name} + " requires an open contract");
        return report;
    }

    contract->status = status;
    return report;
}

} // namespace

std::string_view contract_status_name(ContractStatus status) noexcept {
    switch (status) {
    case ContractStatus::open:
        return "open";
    case ContractStatus::fulfilled:
        return "fulfilled";
    case ContractStatus::failed:
        return "failed";
    case ContractStatus::cancelled:
        return "cancelled";
    }
    return "unknown";
}

bool contract_is_open(const ResourceDeliveryContract& contract) noexcept {
    return contract.status == ContractStatus::open;
}

bool contract_is_terminal(const ResourceDeliveryContract& contract) noexcept {
    return !contract_is_open(contract);
}

data::ValidationReport validate_resource_delivery_contract(const ResourceDeliveryContract& contract) {
    data::ValidationReport report;
    if (contract.id.empty()) {
        report.add_error("simulation.contract", "contract id must not be empty");
    }
    if (contract.display_name.empty()) {
        report.add_error("simulation.contract." + contract.id, "contract display_name must not be empty");
    }
    if (contract.issuer_faction_id.empty()) {
        report.add_error("simulation.contract." + contract.id, "issuer_faction_id must not be empty");
    }
    if (contract.receiver_faction_id.empty()) {
        report.add_error("simulation.contract." + contract.id, "receiver_faction_id must not be empty");
    }
    if (!contract.issuer_faction_id.empty()
        && !contract.receiver_faction_id.empty()
        && contract.issuer_faction_id == contract.receiver_faction_id) {
        report.add_error("simulation.contract." + contract.id, "issuer and receiver factions must be different");
    }
    if (contract.resource_id.empty()) {
        report.add_error("simulation.contract." + contract.id, "resource_id must not be empty");
    }
    if (contract.quantity == 0) {
        report.add_error("simulation.contract." + contract.id, "quantity must be greater than zero");
    }
    if (contract.reward_coins == 0) {
        report.add_error("simulation.contract." + contract.id, "reward_coins must be greater than zero");
    }
    if (contract.due_day == 0) {
        report.add_error("simulation.contract." + contract.id, "due_day must be greater than zero");
    }
    return report;
}

data::ValidationReport validate_resource_delivery_contract_for_factions(
    const ResourceDeliveryContract& contract,
    const FactionCatalog& factions
) {
    auto report = validate_resource_delivery_contract(contract);
    if (!report.ok()) {
        return report;
    }

    if (!faction_exists(factions, contract.issuer_faction_id)) {
        report.add_error("simulation.contract." + contract.id + ".issuer", "unknown issuer faction");
    }
    if (!faction_exists(factions, contract.receiver_faction_id)) {
        report.add_error("simulation.contract." + contract.id + ".receiver", "unknown receiver faction");
    }
    return report;
}

data::ValidationReport add_contract(ContractCatalog& catalog, ResourceDeliveryContract contract) {
    auto report = validate_resource_delivery_contract(contract);
    if (!report.ok()) {
        return report;
    }

    if (contract_by_id(catalog, contract.id) != nullptr) {
        report.add_error("simulation.contract." + contract.id, "duplicate contract id");
        return report;
    }

    catalog.contracts.push_back(std::move(contract));
    return report;
}

std::uint64_t contract_count(const ContractCatalog& catalog) noexcept {
    return catalog.contracts.size();
}

const ResourceDeliveryContract* contract_by_id(const ContractCatalog& catalog, std::string_view contract_id) noexcept {
    for (const auto& contract : catalog.contracts) {
        if (contract.id == contract_id) {
            return &contract;
        }
    }
    return nullptr;
}

ResourceDeliveryContract* mutable_contract_by_id(ContractCatalog& catalog, std::string_view contract_id) noexcept {
    for (auto& contract : catalog.contracts) {
        if (contract.id == contract_id) {
            return &contract;
        }
    }
    return nullptr;
}

ContractFulfillmentResult fulfill_contract_from_storage(
    ContractCatalog& catalog,
    std::string_view contract_id,
    ResourceStorage& delivered_resources
) {
    ContractFulfillmentResult result;
    if (contract_id.empty()) {
        result.validation.add_error("simulation.contract", "contract_id must not be empty");
        return result;
    }

    auto* contract = mutable_contract_by_id(catalog, contract_id);
    if (contract == nullptr) {
        result.contract_id = std::string{contract_id};
        result.validation.add_error("simulation.contract." + std::string{contract_id}, "unknown contract");
        return result;
    }

    result.contract_id = contract->id;
    result.resource_id = contract->resource_id;
    result.quantity = contract->quantity;
    result.reward_coins = contract->reward_coins;

    if (!contract_is_open(*contract)) {
        result.validation.add_error("simulation.contract." + contract->id, "fulfillment requires an open contract");
        return result;
    }

    if (!delivered_resources.try_remove(contract->resource_id, contract->quantity)) {
        result.validation.add_error("simulation.contract." + contract->id, "delivered storage does not contain required resources");
        return result;
    }

    contract->status = ContractStatus::fulfilled;
    result.fulfilled = true;
    return result;
}

data::ValidationReport mark_contract_fulfilled(ContractCatalog& catalog, std::string_view contract_id) {
    return mark_contract_status(catalog, contract_id, ContractStatus::fulfilled, "fulfill");
}

data::ValidationReport mark_contract_failed(ContractCatalog& catalog, std::string_view contract_id) {
    return mark_contract_status(catalog, contract_id, ContractStatus::failed, "fail");
}

data::ValidationReport cancel_contract(ContractCatalog& catalog, std::string_view contract_id) {
    return mark_contract_status(catalog, contract_id, ContractStatus::cancelled, "cancel");
}

std::vector<ResourceDeliveryContract> open_contracts(const ContractCatalog& catalog) {
    std::vector<ResourceDeliveryContract> contracts;
    for (const auto& contract : catalog.contracts) {
        if (contract_is_open(contract)) {
            contracts.push_back(contract);
        }
    }
    return contracts;
}

std::vector<ResourceDeliveryContract> terminal_contracts(const ContractCatalog& catalog) {
    std::vector<ResourceDeliveryContract> contracts;
    for (const auto& contract : catalog.contracts) {
        if (contract_is_terminal(contract)) {
            contracts.push_back(contract);
        }
    }
    return contracts;
}

std::vector<ResourceDeliveryContract> contracts_for_faction(const ContractCatalog& catalog, std::string_view faction_id) {
    std::vector<ResourceDeliveryContract> contracts;
    for (const auto& contract : catalog.contracts) {
        if (contract.issuer_faction_id == faction_id || contract.receiver_faction_id == faction_id) {
            contracts.push_back(contract);
        }
    }
    return contracts;
}

std::vector<ResourceDeliveryContract> overdue_open_contracts(const ContractCatalog& catalog, std::uint64_t current_day) {
    std::vector<ResourceDeliveryContract> contracts;
    for (const auto& contract : catalog.contracts) {
        if (contract_is_open(contract) && current_day > contract.due_day) {
            contracts.push_back(contract);
        }
    }
    return contracts;
}

} // namespace clc::sim
