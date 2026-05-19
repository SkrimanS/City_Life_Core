#include "clc/sim/SimulationPersistence.hpp"

#include <cstdint>
#include <fstream>
#include <initializer_list>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

namespace clc::sim {
namespace {

std::string escape_field(std::string_view value) {
    std::string escaped;
    for (const auto character : value) {
        switch (character) {
        case '%':
            escaped += "%25";
            break;
        case '\t':
            escaped += "%09";
            break;
        case '\n':
            escaped += "%0A";
            break;
        case '\r':
            escaped += "%0D";
            break;
        default:
            escaped.push_back(character);
            break;
        }
    }
    return escaped;
}

int hex_value(char character) noexcept {
    if (character >= '0' && character <= '9') {
        return character - '0';
    }
    if (character >= 'A' && character <= 'F') {
        return 10 + character - 'A';
    }
    if (character >= 'a' && character <= 'f') {
        return 10 + character - 'a';
    }
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
    if (value.empty()) {
        return false;
    }

    std::uint64_t parsed = 0;
    for (const auto character : value) {
        if (character < '0' || character > '9') {
            return false;
        }
        const auto digit = static_cast<std::uint64_t>(character - '0');
        if (parsed > (std::numeric_limits<std::uint64_t>::max() - digit) / 10) {
            return false;
        }
        parsed = parsed * 10 + digit;
    }

    output = parsed;
    return true;
}

std::uint64_t parse_required_uint64(
    std::string_view value,
    data::ValidationReport& report,
    std::string_view path
) {
    std::uint64_t parsed = 0;
    if (!parse_uint64(value, parsed)) {
        report.add_error(std::string{path}, "invalid unsigned integer");
    }
    return parsed;
}

std::uint32_t parse_required_uint32(
    std::string_view value,
    data::ValidationReport& report,
    std::string_view path
) {
    const auto parsed = parse_required_uint64(value, report, path);
    if (parsed > std::numeric_limits<std::uint32_t>::max()) {
        report.add_error(std::string{path}, "unsigned integer exceeds uint32 range");
        return 0;
    }
    return static_cast<std::uint32_t>(parsed);
}

void append_line(std::string& output, std::initializer_list<std::string> fields) {
    bool first = true;
    for (const auto& field : fields) {
        if (!first) {
            output.push_back('\t');
        }
        output += field;
        first = false;
    }
    output.push_back('\n');
}

} // namespace

std::string serialize_simulation_snapshot(const SimulationSnapshot& snapshot) {
    std::string output;
    append_line(output, {"CLC_SIM_SNAPSHOT", "1"});
    append_line(output, {"day", std::to_string(snapshot.day)});

    for (const auto& settlement : snapshot.settlements) {
        append_line(output, {
            "settlement",
            escape_field(settlement.id),
            escape_field(settlement.display_name),
            std::to_string(settlement.population),
            std::to_string(settlement.total_stored_resources),
        });
        for (const auto& resource : settlement.storage) {
            append_line(output, {
                "storage",
                escape_field(settlement.id),
                escape_field(resource.resource_id),
                std::to_string(resource.amount),
            });
        }
        for (const auto& building : settlement.buildings) {
            append_line(output, {
                "building",
                escape_field(settlement.id),
                escape_field(building.definition_id),
                escape_field(building.display_name),
                std::to_string(building.assigned_workers),
                std::to_string(building.worker_slots),
            });
        }
    }

    append_line(output, {
        "market",
        std::to_string(snapshot.market.total_supply),
        std::to_string(snapshot.market.total_demand),
        std::to_string(snapshot.market.average_price),
        std::to_string(snapshot.market.min_price),
        std::to_string(snapshot.market.max_price),
    });
    for (const auto& price : snapshot.market.prices) {
        append_line(output, {
            "price",
            escape_field(price.resource_id),
            std::to_string(price.base_value),
            std::to_string(price.supply),
            std::to_string(price.demand),
            std::to_string(price.price),
            escape_field(price.reason),
        });
    }

    for (const auto& event : snapshot.events) {
        append_line(output, {
            "event",
            std::to_string(event.day),
            escape_field(event.type),
            escape_field(event.message),
        });
    }

    return output;
}

SimulationSnapshotLoadResult deserialize_simulation_snapshot(std::string_view content) {
    SimulationSnapshotLoadResult result;
    bool header_seen = false;
    bool market_seen = false;

    std::size_t line_start = 0;
    std::uint64_t line_number = 1;
    while (line_start <= content.size()) {
        const auto line_end = content.find('\n', line_start);
        auto line = line_end == std::string_view::npos
            ? content.substr(line_start)
            : content.substr(line_start, line_end - line_start);
        if (!line.empty() && line.back() == '\r') {
            line.remove_suffix(1);
        }

        if (!line.empty()) {
            const auto fields = split_tab_line(line);
            const auto path = "simulation.snapshot.line." + std::to_string(line_number);

            if (fields[0] == "CLC_SIM_SNAPSHOT") {
                if (fields.size() != 2 || fields[1] != "1") {
                    result.validation.add_error(path, "unsupported snapshot format");
                }
                header_seen = true;
            } else if (!header_seen) {
                result.validation.add_error(path, "missing snapshot header before data");
            } else if (fields[0] == "day") {
                if (fields.size() != 2) {
                    result.validation.add_error(path, "day row must have 2 fields");
                } else {
                    result.snapshot.day = parse_required_uint64(fields[1], result.validation, path + ".day");
                }
            } else if (fields[0] == "settlement") {
                if (fields.size() != 5) {
                    result.validation.add_error(path, "settlement row must have 5 fields");
                } else {
                    result.snapshot.settlements.push_back(SettlementReport{
                        .id = unescape_field(fields[1], result.validation, path + ".id"),
                        .display_name = unescape_field(fields[2], result.validation, path + ".display_name"),
                        .population = parse_required_uint64(fields[3], result.validation, path + ".population"),
                        .total_stored_resources = parse_required_uint64(fields[4], result.validation, path + ".total_stored_resources"),
                    });
                }
            } else if (fields[0] == "storage") {
                if (fields.size() != 4) {
                    result.validation.add_error(path, "storage row must have 4 fields");
                } else {
                    const auto settlement_id = unescape_field(fields[1], result.validation, path + ".settlement_id");
                    auto* settlement = [&]() -> SettlementReport* {
                        for (auto& candidate : result.snapshot.settlements) {
                            if (candidate.id == settlement_id) {
                                return &candidate;
                            }
                        }
                        return nullptr;
                    }();
                    if (settlement == nullptr) {
                        result.validation.add_error(path, "storage references unknown settlement");
                    } else {
                        settlement->storage.push_back(ResourceAmount{
                            .resource_id = unescape_field(fields[2], result.validation, path + ".resource_id"),
                            .amount = parse_required_uint64(fields[3], result.validation, path + ".amount"),
                        });
                    }
                }
            } else if (fields[0] == "building") {
                if (fields.size() != 6) {
                    result.validation.add_error(path, "building row must have 6 fields");
                } else {
                    const auto settlement_id = unescape_field(fields[1], result.validation, path + ".settlement_id");
                    auto* settlement = [&]() -> SettlementReport* {
                        for (auto& candidate : result.snapshot.settlements) {
                            if (candidate.id == settlement_id) {
                                return &candidate;
                            }
                        }
                        return nullptr;
                    }();
                    if (settlement == nullptr) {
                        result.validation.add_error(path, "building references unknown settlement");
                    } else {
                        settlement->buildings.push_back(BuildingReport{
                            .definition_id = unescape_field(fields[2], result.validation, path + ".definition_id"),
                            .display_name = unescape_field(fields[3], result.validation, path + ".display_name"),
                            .assigned_workers = parse_required_uint32(fields[4], result.validation, path + ".assigned_workers"),
                            .worker_slots = parse_required_uint32(fields[5], result.validation, path + ".worker_slots"),
                        });
                    }
                }
            } else if (fields[0] == "market") {
                if (fields.size() != 6) {
                    result.validation.add_error(path, "market row must have 6 fields");
                } else {
                    result.snapshot.market.total_supply = parse_required_uint64(fields[1], result.validation, path + ".total_supply");
                    result.snapshot.market.total_demand = parse_required_uint64(fields[2], result.validation, path + ".total_demand");
                    result.snapshot.market.average_price = parse_required_uint64(fields[3], result.validation, path + ".average_price");
                    result.snapshot.market.min_price = parse_required_uint64(fields[4], result.validation, path + ".min_price");
                    result.snapshot.market.max_price = parse_required_uint64(fields[5], result.validation, path + ".max_price");
                    market_seen = true;
                }
            } else if (fields[0] == "price") {
                if (fields.size() != 7) {
                    result.validation.add_error(path, "price row must have 7 fields");
                } else {
                    result.snapshot.market.prices.push_back(economy::MarketPrice{
                        .resource_id = unescape_field(fields[1], result.validation, path + ".resource_id"),
                        .base_value = parse_required_uint64(fields[2], result.validation, path + ".base_value"),
                        .supply = parse_required_uint64(fields[3], result.validation, path + ".supply"),
                        .demand = parse_required_uint64(fields[4], result.validation, path + ".demand"),
                        .price = parse_required_uint64(fields[5], result.validation, path + ".price"),
                        .reason = unescape_field(fields[6], result.validation, path + ".reason"),
                    });
                }
            } else if (fields[0] == "event") {
                if (fields.size() != 4) {
                    result.validation.add_error(path, "event row must have 4 fields");
                } else {
                    result.snapshot.events.push_back(SimulationEvent{
                        .day = parse_required_uint64(fields[1], result.validation, path + ".day"),
                        .type = unescape_field(fields[2], result.validation, path + ".type"),
                        .message = unescape_field(fields[3], result.validation, path + ".message"),
                    });
                }
            } else {
                result.validation.add_error(path, "unknown snapshot row type");
            }
        }

        if (line_end == std::string_view::npos) {
            break;
        }
        line_start = line_end + 1;
        ++line_number;
    }

    if (!header_seen) {
        result.validation.add_error("simulation.snapshot", "missing snapshot header");
    }
    if (!market_seen) {
        result.validation.add_warning("simulation.snapshot.market", "missing market row; using empty market report");
    }

    return result;
}

data::ValidationReport save_simulation_snapshot_to_file(
    const SimulationSnapshot& snapshot,
    const std::filesystem::path& path
) {
    data::ValidationReport report;
    std::ofstream output{path};
    if (!output) {
        report.add_error("simulation.snapshot.file", "failed to open snapshot file for writing");
        return report;
    }

    output << serialize_simulation_snapshot(snapshot);
    if (!output) {
        report.add_error("simulation.snapshot.file", "failed to write snapshot file");
    }
    return report;
}

SimulationSnapshotLoadResult load_simulation_snapshot_from_file(const std::filesystem::path& path) {
    SimulationSnapshotLoadResult result;
    std::ifstream input{path};
    if (!input) {
        result.validation.add_error("simulation.snapshot.file", "failed to open snapshot file for reading");
        return result;
    }

    std::ostringstream buffer;
    buffer << input.rdbuf();
    if (!input.good() && !input.eof()) {
        result.validation.add_error("simulation.snapshot.file", "failed to read snapshot file");
        return result;
    }

    return deserialize_simulation_snapshot(buffer.str());
}

} // namespace clc::sim
