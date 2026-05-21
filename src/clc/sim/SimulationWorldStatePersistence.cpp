#include "clc/sim/SimulationPersistence.hpp"

#include <cstdint>
#include <fstream>
#include <initializer_list>
#include <limits>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace clc::sim {
namespace {

std::string escape_field(std::string_view value) {
    std::string escaped;
    for (const auto character : value) {
        switch (character) {
        case '%': escaped += "%25"; break;
        case '\t': escaped += "%09"; break;
        case '\n': escaped += "%0A"; break;
        case '\r': escaped += "%0D"; break;
        default: escaped.push_back(character); break;
        }
    }
    return escaped;
}

int hex_value(char character) noexcept {
    if (character >= '0' && character <= '9') return character - '0';
    if (character >= 'A' && character <= 'F') return 10 + character - 'A';
    if (character >= 'a' && character <= 'f') return 10 + character - 'a';
    return -1;
}

std::string unescape_field(std::string_view value, data::ValidationReport& report, std::string_view path) {
    std::string unescaped;
    for (std::size_t index = 0; index < value.size(); ++index) {
        if (value[index] != '%') {
            unescaped.push_back(value[index]);
            continue;
        }
        if (index + 2 >= value.size()) {
            report.add_error(std::string{path}, "truncated escape sequence");
            return unescaped;
        }
        const auto high = hex_value(value[index + 1]);
        const auto low = hex_value(value[index + 2]);
        if (high < 0 || low < 0) {
            report.add_error(std::string{path}, "invalid escape sequence");
            return unescaped;
        }
        unescaped.push_back(static_cast<char>((high << 4) | low));
        index += 2;
    }
    return unescaped;
}

std::vector<std::string_view> split_tab_line(std::string_view line) {
    std::vector<std::string_view> fields;
    std::size_t start = 0;
    while (start <= line.size()) {
        const auto tab = line.find('\t', start);
        if (tab == std::string_view::npos) {
            fields.push_back(line.substr(start));
            break;
        }
        fields.push_back(line.substr(start, tab - start));
        start = tab + 1;
    }
    return fields;
}

bool parse_uint64(std::string_view value, std::uint64_t& output) noexcept {
    if (value.empty()) return false;
    std::uint64_t parsed = 0;
    for (const auto character : value) {
        if (character < '0' || character > '9') return false;
        const auto digit = static_cast<std::uint64_t>(character - '0');
        if (parsed > (std::numeric_limits<std::uint64_t>::max() - digit) / 10) return false;
        parsed = parsed * 10 + digit;
    }
    output = parsed;
    return true;
}

std::uint64_t parse_required_uint64(std::string_view value, data::ValidationReport& report, std::string_view path) {
    std::uint64_t parsed = 0;
    if (!parse_uint64(value, parsed)) report.add_error(std::string{path}, "invalid unsigned integer");
    return parsed;
}

std::uint32_t parse_required_uint32(std::string_view value, data::ValidationReport& report, std::string_view path) {
    const auto parsed = parse_required_uint64(value, report, path);
    if (parsed > std::numeric_limits<std::uint32_t>::max()) {
        report.add_error(std::string{path}, "unsigned integer exceeds uint32 range");
        return 0;
    }
    return static_cast<std::uint32_t>(parsed);
}

std::int64_t parse_required_int64(std::string_view value, data::ValidationReport& report, std::string_view path) {
    if (value.empty()) {
        report.add_error(std::string{path}, "invalid signed integer");
        return 0;
    }
    bool negative = false;
    std::size_t start = 0;
    if (value[0] == '-') {
        negative = true;
        start = 1;
    }
    if (start >= value.size()) {
        report.add_error(std::string{path}, "invalid signed integer");
        return 0;
    }
    std::uint64_t magnitude = 0;
    if (!parse_uint64(value.substr(start), magnitude)) {
        report.add_error(std::string{path}, "invalid signed integer");
        return 0;
    }
    if (!negative && magnitude > static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max())) {
        report.add_error(std::string{path}, "signed integer exceeds int64 range");
        return 0;
    }
    if (negative && magnitude > static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max()) + 1) {
        report.add_error(std::string{path}, "signed integer exceeds int64 range");
        return 0;
    }
    if (negative && magnitude == static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max()) + 1) {
        return std::numeric_limits<std::int64_t>::min();
    }
    const auto value64 = static_cast<std::int64_t>(magnitude);
    return negative ? -value64 : value64;
}

std::string contract_status_to_string(ContractStatus status) {
    return std::string{contract_status_name(status)};
}

ContractStatus parse_contract_status(std::string_view value, data::ValidationReport& report, std::string_view path) {
    if (value == "open") return ContractStatus::open;
    if (value == "fulfilled") return ContractStatus::fulfilled;
    if (value == "failed") return ContractStatus::failed;
    if (value == "cancelled") return ContractStatus::cancelled;
    report.add_error(std::string{path}, "unknown contract status");
    return ContractStatus::open;
}

std::string ledger_type_to_string(economy::LedgerEntryType type) {
    switch (type) {
    case economy::LedgerEntryType::buy: return "buy";
    case economy::LedgerEntryType::sell: return "sell";
    case economy::LedgerEntryType::contract_reward: return "contract_reward";
    }
    return "unknown";
}

economy::LedgerEntryType parse_ledger_type(std::string_view value, data::ValidationReport& report, std::string_view path) {
    if (value == "buy") return economy::LedgerEntryType::buy;
    if (value == "sell") return economy::LedgerEntryType::sell;
    if (value == "contract_reward") return economy::LedgerEntryType::contract_reward;
    report.add_error(std::string{path}, "unknown ledger entry type");
    return economy::LedgerEntryType::buy;
}

void append_line(std::string& output, std::initializer_list<std::string> fields) {
    bool first = true;
    for (const auto& field : fields) {
        if (!first) output.push_back('\t');
        output += field;
        first = false;
    }
    output.push_back('\n');
}

ResourceStorage* caravan_cargo_by_id(CaravanFleet& fleet, std::string_view caravan_id) {
    for (auto& caravan : fleet.caravans) {
        if (caravan.id == caravan_id) return &caravan.cargo;
    }
    return nullptr;
}

ResourceStorage* settlement_storage_by_id(std::vector<SettlementState>& settlements, std::string_view settlement_id) {
    for (auto& settlement : settlements) {
        if (settlement.id == settlement_id) return &settlement.storage;
    }
    return nullptr;
}

SettlementState* settlement_by_id(std::vector<SettlementState>& settlements, std::string_view settlement_id) {
    for (auto& settlement : settlements) {
        if (settlement.id == settlement_id) return &settlement;
    }
    return nullptr;
}

} // namespace

std::string serialize_simulation_world_state(const SimulationWorldState& state) {
    std::string output;
    append_line(output, {"CLC_SIM_WORLD_STATE", "1"});
    append_line(output, {"day", std::to_string(state.engine.current_day)});
    append_line(output, {"wallet", std::to_string(state.wallet.coins)});

    for (const auto& settlement : state.engine.settlements) {
        append_line(output, {"settlement", escape_field(settlement.id), escape_field(settlement.display_name), std::to_string(settlement.population)});
        for (const auto& entry : settlement.storage.entries()) {
            append_line(output, {"settlement_storage", escape_field(settlement.id), escape_field(entry.first), std::to_string(entry.second)});
        }
        for (const auto& building : settlement.buildings) {
            append_line(output, {"settlement_building", escape_field(settlement.id), escape_field(building.definition_id), std::to_string(building.assigned_workers)});
        }
        for (const auto& remainder : settlement.tick_remainders) {
            append_line(output, {"settlement_tick_remainder", escape_field(settlement.id), escape_field(remainder.key), std::to_string(remainder.numerator)});
        }
    }

    for (const auto& demand : state.engine.market_demands) {
        append_line(output, {"market_demand", escape_field(demand.resource_id), std::to_string(demand.demand)});
    }
    for (const auto& event : state.engine.events) {
        append_line(output, {"event", std::to_string(event.day), escape_field(event.type), escape_field(event.message)});
    }
    for (const auto& route : state.routes.routes) {
        append_line(output, {
            "route",
            escape_field(route.id),
            escape_field(route.display_name),
            escape_field(route.origin_settlement_id),
            escape_field(route.destination_settlement_id),
            std::to_string(route.travel_days),
            std::to_string(settlement_route_travel_ticks(route))
        });
    }
    for (const auto& caravan : state.caravans.caravans) {
        append_line(output, {
            "caravan",
            escape_field(caravan.id),
            escape_field(caravan.display_name),
            escape_field(caravan.route_id),
            escape_field(caravan.origin_settlement_id),
            escape_field(caravan.destination_settlement_id),
            std::to_string(caravan.total_travel_days),
            std::to_string(caravan.days_remaining),
            std::to_string(caravan_total_travel_ticks(caravan)),
            std::to_string(caravan_ticks_remaining(caravan))
        });
        for (const auto& entry : caravan.cargo.entries()) {
            append_line(output, {"caravan_cargo", escape_field(caravan.id), escape_field(entry.first), std::to_string(entry.second)});
        }
    }
    for (const auto& faction : state.factions.factions) {
        append_line(output, {"faction", escape_field(faction.id), escape_field(faction.display_name)});
    }
    for (const auto& reputation : state.factions.reputations) {
        append_line(output, {"reputation", escape_field(reputation.from_faction_id), escape_field(reputation.to_faction_id), std::to_string(reputation.value)});
    }
    for (const auto& ownership : state.ownership.settlements) {
        append_line(output, {"settlement_owner", escape_field(ownership.settlement_id), escape_field(ownership.faction_id)});
    }
    for (const auto& ownership : state.ownership.caravans) {
        append_line(output, {"caravan_owner", escape_field(ownership.caravan_id), escape_field(ownership.faction_id)});
    }
    for (const auto& contract : state.contracts.contracts) {
        append_line(output, {"contract", escape_field(contract.id), escape_field(contract.display_name), escape_field(contract.issuer_faction_id), escape_field(contract.receiver_faction_id), escape_field(contract.resource_id), std::to_string(contract.quantity), std::to_string(contract.reward_coins), std::to_string(contract.due_day), contract_status_to_string(contract.status)});
    }
    for (const auto& entry : state.ledger_entries) {
        append_line(output, {"ledger", std::to_string(entry.sequence), ledger_type_to_string(entry.type), escape_field(entry.resource_id), std::to_string(entry.quantity), std::to_string(entry.unit_price), std::to_string(entry.total_price), escape_field(entry.reference_id), escape_field(entry.note)});
    }

    return output;
}

SimulationWorldStateLoadResult deserialize_simulation_world_state(std::string_view content) {
    SimulationWorldStateLoadResult result;
    bool header_seen = false;
    std::size_t line_start = 0;
    std::uint64_t line_number = 1;
    while (line_start <= content.size()) {
        const auto line_end = content.find('\n', line_start);
        auto line = line_end == std::string_view::npos ? content.substr(line_start) : content.substr(line_start, line_end - line_start);
        if (!line.empty() && line.back() == '\r') line.remove_suffix(1);

        if (!line.empty()) {
            const auto fields = split_tab_line(line);
            const auto path = "simulation.world_state.line." + std::to_string(line_number);
            if (fields[0] == "CLC_SIM_WORLD_STATE") {
                if (fields.size() != 2 || fields[1] != "1") result.validation.add_error(path, "unsupported world state format");
                header_seen = true;
            } else if (!header_seen) {
                result.validation.add_error(path, "missing world state header before data");
            } else if (fields[0] == "day") {
                if (fields.size() != 2) result.validation.add_error(path, "day row must have 2 fields");
                else result.state.engine.current_day = parse_required_uint64(fields[1], result.validation, path + ".day");
            } else if (fields[0] == "wallet") {
                if (fields.size() != 2) result.validation.add_error(path, "wallet row must have 2 fields");
                else result.state.wallet.coins = parse_required_uint64(fields[1], result.validation, path + ".coins");
            } else if (fields[0] == "settlement") {
                if (fields.size() != 4) result.validation.add_error(path, "settlement row must have 4 fields");
                else result.state.engine.settlements.push_back(SettlementState{.id = unescape_field(fields[1], result.validation, path + ".id"), .display_name = unescape_field(fields[2], result.validation, path + ".display_name"), .population = parse_required_uint64(fields[3], result.validation, path + ".population")});
            } else if (fields[0] == "settlement_storage") {
                if (fields.size() != 4) result.validation.add_error(path, "settlement_storage row must have 4 fields");
                else if (auto* storage = settlement_storage_by_id(result.state.engine.settlements, unescape_field(fields[1], result.validation, path + ".settlement_id")); storage != nullptr) storage->add(unescape_field(fields[2], result.validation, path + ".resource_id"), parse_required_uint64(fields[3], result.validation, path + ".amount"));
                else result.validation.add_error(path, "settlement_storage references unknown settlement");
            } else if (fields[0] == "settlement_building") {
                if (fields.size() != 4) result.validation.add_error(path, "settlement_building row must have 4 fields");
                else if (auto* settlement = settlement_by_id(result.state.engine.settlements, unescape_field(fields[1], result.validation, path + ".settlement_id")); settlement != nullptr) settlement->buildings.push_back(BuildingInstance{.definition_id = unescape_field(fields[2], result.validation, path + ".definition_id"), .assigned_workers = parse_required_uint32(fields[3], result.validation, path + ".assigned_workers")});
                else result.validation.add_error(path, "settlement_building references unknown settlement");
            } else if (fields[0] == "settlement_tick_remainder") {
                if (fields.size() != 4) result.validation.add_error(path, "settlement_tick_remainder row must have 4 fields");
                else if (auto* settlement = settlement_by_id(result.state.engine.settlements, unescape_field(fields[1], result.validation, path + ".settlement_id")); settlement != nullptr) settlement->tick_remainders.push_back(SettlementTickRemainder{.key = unescape_field(fields[2], result.validation, path + ".key"), .numerator = parse_required_uint64(fields[3], result.validation, path + ".numerator")});
                else result.validation.add_error(path, "settlement_tick_remainder references unknown settlement");
            } else if (fields[0] == "market_demand") {
                if (fields.size() != 3) result.validation.add_error(path, "market_demand row must have 3 fields");
                else result.state.engine.market_demands.push_back(SimulationMarketDemand{.resource_id = unescape_field(fields[1], result.validation, path + ".resource_id"), .demand = parse_required_uint64(fields[2], result.validation, path + ".demand")});
            } else if (fields[0] == "event") {
                if (fields.size() != 4) result.validation.add_error(path, "event row must have 4 fields");
                else result.state.engine.events.push_back(SimulationEvent{.day = parse_required_uint64(fields[1], result.validation, path + ".day"), .type = unescape_field(fields[2], result.validation, path + ".type"), .message = unescape_field(fields[3], result.validation, path + ".message")});
            } else if (fields[0] == "route") {
                if (fields.size() != 6 && fields.size() != 7) result.validation.add_error(path, "route row must have 6 or 7 fields");
                else {
                    const auto travel_days = parse_required_uint64(fields[5], result.validation, path + ".travel_days");
                    const auto travel_ticks = fields.size() == 7
                        ? parse_required_uint64(fields[6], result.validation, path + ".travel_ticks")
                        : clc::days_to_ticks(travel_days);
                    result.state.routes.routes.push_back(SettlementRoute{.id = unescape_field(fields[1], result.validation, path + ".id"), .display_name = unescape_field(fields[2], result.validation, path + ".display_name"), .origin_settlement_id = unescape_field(fields[3], result.validation, path + ".origin"), .destination_settlement_id = unescape_field(fields[4], result.validation, path + ".destination"), .travel_days = travel_days, .travel_ticks = travel_ticks});
                }
            } else if (fields[0] == "caravan") {
                if (fields.size() != 8 && fields.size() != 10) result.validation.add_error(path, "caravan row must have 8 or 10 fields");
                else {
                    const auto total_travel_days = parse_required_uint64(fields[6], result.validation, path + ".total_travel_days");
                    const auto days_remaining = parse_required_uint64(fields[7], result.validation, path + ".days_remaining");
                    const auto total_travel_ticks = fields.size() == 10
                        ? parse_required_uint64(fields[8], result.validation, path + ".total_travel_ticks")
                        : clc::days_to_ticks(total_travel_days);
                    const auto ticks_remaining = fields.size() == 10
                        ? parse_required_uint64(fields[9], result.validation, path + ".ticks_remaining")
                        : clc::days_to_ticks(days_remaining);
                    result.state.caravans.caravans.push_back(CaravanState{.id = unescape_field(fields[1], result.validation, path + ".id"), .display_name = unescape_field(fields[2], result.validation, path + ".display_name"), .route_id = unescape_field(fields[3], result.validation, path + ".route_id"), .origin_settlement_id = unescape_field(fields[4], result.validation, path + ".origin"), .destination_settlement_id = unescape_field(fields[5], result.validation, path + ".destination"), .total_travel_days = total_travel_days, .days_remaining = days_remaining, .total_travel_ticks = total_travel_ticks, .ticks_remaining = ticks_remaining});
                }
            } else if (fields[0] == "caravan_cargo") {
                if (fields.size() != 4) result.validation.add_error(path, "caravan_cargo row must have 4 fields");
                else if (auto* cargo = caravan_cargo_by_id(result.state.caravans, unescape_field(fields[1], result.validation, path + ".caravan_id")); cargo != nullptr) cargo->add(unescape_field(fields[2], result.validation, path + ".resource_id"), parse_required_uint64(fields[3], result.validation, path + ".amount"));
                else result.validation.add_error(path, "caravan_cargo references unknown caravan");
            } else if (fields[0] == "faction") {
                if (fields.size() != 3) result.validation.add_error(path, "faction row must have 3 fields");
                else result.state.factions.factions.push_back(FactionState{.id = unescape_field(fields[1], result.validation, path + ".id"), .display_name = unescape_field(fields[2], result.validation, path + ".display_name")});
            } else if (fields[0] == "reputation") {
                if (fields.size() != 4) result.validation.add_error(path, "reputation row must have 4 fields");
                else result.state.factions.reputations.push_back(FactionReputation{.from_faction_id = unescape_field(fields[1], result.validation, path + ".from"), .to_faction_id = unescape_field(fields[2], result.validation, path + ".to"), .value = parse_required_int64(fields[3], result.validation, path + ".value")});
            } else if (fields[0] == "settlement_owner") {
                if (fields.size() != 3) result.validation.add_error(path, "settlement_owner row must have 3 fields");
                else result.state.ownership.settlements.push_back(SettlementOwnership{.settlement_id = unescape_field(fields[1], result.validation, path + ".settlement_id"), .faction_id = unescape_field(fields[2], result.validation, path + ".faction_id")});
            } else if (fields[0] == "caravan_owner") {
                if (fields.size() != 3) result.validation.add_error(path, "caravan_owner row must have 3 fields");
                else result.state.ownership.caravans.push_back(CaravanOwnership{.caravan_id = unescape_field(fields[1], result.validation, path + ".caravan_id"), .faction_id = unescape_field(fields[2], result.validation, path + ".faction_id")});
            } else if (fields[0] == "contract") {
                if (fields.size() != 10) result.validation.add_error(path, "contract row must have 10 fields");
                else result.state.contracts.contracts.push_back(ResourceDeliveryContract{.id = unescape_field(fields[1], result.validation, path + ".id"), .display_name = unescape_field(fields[2], result.validation, path + ".display_name"), .issuer_faction_id = unescape_field(fields[3], result.validation, path + ".issuer"), .receiver_faction_id = unescape_field(fields[4], result.validation, path + ".receiver"), .resource_id = unescape_field(fields[5], result.validation, path + ".resource_id"), .quantity = parse_required_uint64(fields[6], result.validation, path + ".quantity"), .reward_coins = parse_required_uint64(fields[7], result.validation, path + ".reward"), .due_day = parse_required_uint64(fields[8], result.validation, path + ".due_day"), .status = parse_contract_status(fields[9], result.validation, path + ".status")});
            } else if (fields[0] == "ledger") {
                if (fields.size() != 9) result.validation.add_error(path, "ledger row must have 9 fields");
                else result.state.ledger_entries.push_back(economy::LedgerEntry{.sequence = parse_required_uint64(fields[1], result.validation, path + ".sequence"), .type = parse_ledger_type(fields[2], result.validation, path + ".type"), .resource_id = unescape_field(fields[3], result.validation, path + ".resource_id"), .quantity = parse_required_uint64(fields[4], result.validation, path + ".quantity"), .unit_price = parse_required_uint64(fields[5], result.validation, path + ".unit_price"), .total_price = parse_required_uint64(fields[6], result.validation, path + ".total_price"), .reference_id = unescape_field(fields[7], result.validation, path + ".reference_id"), .note = unescape_field(fields[8], result.validation, path + ".note")});
            } else {
                result.validation.add_error(path, "unknown world state row type");
            }
        }
        if (line_end == std::string_view::npos) break;
        line_start = line_end + 1;
        ++line_number;
    }

    if (!header_seen) result.validation.add_error("simulation.world_state", "missing world state header");
    return result;
}

data::ValidationReport save_simulation_world_state_to_file(const SimulationWorldState& state, const std::filesystem::path& path) {
    data::ValidationReport report;
    std::ofstream output{path};
    if (!output) {
        report.add_error("simulation.world_state.file", "failed to open world state file for writing");
        return report;
    }
    output << serialize_simulation_world_state(state);
    if (!output) report.add_error("simulation.world_state.file", "failed to write world state file");
    return report;
}

SimulationWorldStateLoadResult load_simulation_world_state_from_file(const std::filesystem::path& path) {
    SimulationWorldStateLoadResult result;
    std::ifstream input{path};
    if (!input) {
        result.validation.add_error("simulation.world_state.file", "failed to open world state file for reading");
        return result;
    }
    std::ostringstream buffer;
    buffer << input.rdbuf();
    if (!input.good() && !input.eof()) {
        result.validation.add_error("simulation.world_state.file", "failed to read world state file");
        return result;
    }
    return deserialize_simulation_world_state(buffer.str());
}

} // namespace clc::sim
