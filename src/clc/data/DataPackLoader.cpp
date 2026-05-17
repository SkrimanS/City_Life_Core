#include "clc/data/DataPackLoader.hpp"

#include <charconv>
#include <fstream>
#include <sstream>
#include <string_view>
#include <utility>

namespace clc::data {
namespace {

std::string trim(std::string_view value) {
    const auto first = value.find_first_not_of(" \t\r\n");
    if (first == std::string_view::npos) {
        return {};
    }
    const auto last = value.find_last_not_of(" \t\r\n");
    return std::string{value.substr(first, last - first + 1)};
}

std::string strip_optional_quotes(std::string value) {
    if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
        return value.substr(1, value.size() - 2);
    }
    return value;
}

bool parse_u64(std::string_view text, std::uint64_t& out) {
    const auto trimmed = trim(text);
    const char* begin = trimmed.data();
    const char* end = trimmed.data() + trimmed.size();
    const auto result = std::from_chars(begin, end, out);
    return result.ec == std::errc{} && result.ptr == end;
}

bool parse_u32(std::string_view text, std::uint32_t& out) {
    std::uint64_t parsed{};
    if (!parse_u64(text, parsed)) {
        return false;
    }
    if (parsed > static_cast<std::uint64_t>(std::numeric_limits<std::uint32_t>::max())) {
        return false;
    }
    out = static_cast<std::uint32_t>(parsed);
    return true;
}

const std::string* find_field(const DefinitionRecord& record, std::string_view key) {
    const auto it = record.find(std::string{key});
    if (it == record.end()) {
        return nullptr;
    }
    return &it->second;
}

std::string field_or_empty(const DefinitionRecord& record, std::string_view key) {
    if (const auto* value = find_field(record, key)) {
        return *value;
    }
    return {};
}

bool is_section(std::string_view line) {
    return line.size() >= 2 && line.front() == '[' && line.back() == ']';
}

} // namespace

ValidationReport DataPackLoader::load_file(const std::filesystem::path& path, DataRegistry& registry) const {
    std::ifstream input{path};
    if (!input) {
        ValidationReport report;
        report.add_error(path.string(), "failed to open data pack file");
        return report;
    }

    std::ostringstream buffer;
    buffer << input.rdbuf();
    return load_string(path.string(), buffer.str(), registry);
}

ValidationReport DataPackLoader::load_string(std::string_view source_name, std::string_view content, DataRegistry& registry) const {
    DataPackDocument document;
    auto parse_report = parse(source_name, content, document);
    if (!parse_report.ok()) {
        return parse_report;
    }

    auto register_report = register_document(source_name, document, registry);
    for (const auto& message : parse_report.messages()) {
        if (message.severity == ValidationSeverity::warning) {
            register_report.add_warning(message.path, message.message);
        }
    }
    return register_report;
}

ValidationReport DataPackLoader::parse(std::string_view source_name, std::string_view content, DataPackDocument& document) const {
    ValidationReport report;
    std::istringstream input{std::string{content}};
    std::string line;
    std::string section;
    std::size_t line_number = 0;

    while (std::getline(input, line)) {
        ++line_number;
        auto cleaned = trim(line);
        if (cleaned.empty() || cleaned.front() == '#') {
            continue;
        }

        if (is_section(cleaned)) {
            section = cleaned.substr(1, cleaned.size() - 2);
            continue;
        }

        const auto equals = cleaned.find('=');
        if (equals == std::string::npos) {
            report.add_error(std::string{source_name} + ":" + std::to_string(line_number), "expected key=value line");
            continue;
        }

        auto key = trim(std::string_view{cleaned}.substr(0, equals));
        auto value = strip_optional_quotes(trim(std::string_view{cleaned}.substr(equals + 1)));

        if (section.empty()) {
            if (key == "schema_version") {
                document.schema_version = std::move(value);
            } else {
                report.add_warning(std::string{source_name} + ":" + std::to_string(line_number), "unknown top-level key: " + key);
            }
            continue;
        }

        DefinitionRecord record;
        record.emplace(std::move(key), std::move(value));

        if (section == "resource") {
            document.resources.push_back(std::move(record));
        } else if (section == "currency") {
            document.currencies.push_back(std::move(record));
        } else if (section == "building") {
            document.buildings.push_back(std::move(record));
        } else if (section == "profession") {
            document.professions.push_back(std::move(record));
        } else if (section == "settlement") {
            document.settlements.push_back(std::move(record));
        } else {
            report.add_error(std::string{source_name} + ":" + std::to_string(line_number), "unknown section: " + section);
        }
    }

    if (document.schema_version.empty()) {
        report.add_error(std::string{source_name}, "schema_version is required");
    } else if (document.schema_version != "0.2.1") {
        report.add_error(std::string{source_name}, "unsupported schema_version: " + document.schema_version);
    }

    return report;
}

ValidationReport DataPackLoader::register_document(std::string_view source_name, const DataPackDocument& document, DataRegistry& registry) const {
    ValidationReport report;

    for (const auto& record : document.resources) {
        std::uint64_t base_value{};
        if (const auto* raw = find_field(record, "base_value"); raw != nullptr && !parse_u64(*raw, base_value)) {
            report.add_error(std::string{source_name}, "resource base_value must be a non-negative integer");
            continue;
        }

        auto item_report = registry.add(ResourceDefinition{
            .id = field_or_empty(record, "id"),
            .display_name = field_or_empty(record, "display_name"),
            .category = field_or_empty(record, "category"),
            .base_value = base_value,
        });
        for (const auto& message : item_report.messages()) {
            if (message.severity == ValidationSeverity::error) {
                report.add_error(message.path, message.message);
            } else {
                report.add_warning(message.path, message.message);
            }
        }
    }

    for (const auto& record : document.currencies) {
        std::uint32_t fractional_digits{};
        if (const auto* raw = find_field(record, "fractional_digits"); raw != nullptr && !parse_u32(*raw, fractional_digits)) {
            report.add_error(std::string{source_name}, "currency fractional_digits must be a non-negative integer");
            continue;
        }

        auto item_report = registry.add(CurrencyDefinition{
            .id = field_or_empty(record, "id"),
            .display_name = field_or_empty(record, "display_name"),
            .fractional_digits = fractional_digits,
        });
        for (const auto& message : item_report.messages()) {
            if (message.severity == ValidationSeverity::error) {
                report.add_error(message.path, message.message);
            } else {
                report.add_warning(message.path, message.message);
            }
        }
    }

    for (const auto& record : document.buildings) {
        std::uint32_t worker_slots{};
        if (const auto* raw = find_field(record, "worker_slots"); raw != nullptr && !parse_u32(*raw, worker_slots)) {
            report.add_error(std::string{source_name}, "building worker_slots must be a non-negative integer");
            continue;
        }

        auto item_report = registry.add(BuildingDefinition{
            .id = field_or_empty(record, "id"),
            .display_name = field_or_empty(record, "display_name"),
            .category = field_or_empty(record, "category"),
            .worker_slots = worker_slots,
        });
        for (const auto& message : item_report.messages()) {
            if (message.severity == ValidationSeverity::error) {
                report.add_error(message.path, message.message);
            } else {
                report.add_warning(message.path, message.message);
            }
        }
    }

    for (const auto& record : document.professions) {
        auto item_report = registry.add(ProfessionDefinition{
            .id = field_or_empty(record, "id"),
            .display_name = field_or_empty(record, "display_name"),
            .category = field_or_empty(record, "category"),
        });
        for (const auto& message : item_report.messages()) {
            if (message.severity == ValidationSeverity::error) {
                report.add_error(message.path, message.message);
            } else {
                report.add_warning(message.path, message.message);
            }
        }
    }

    for (const auto& record : document.settlements) {
        std::uint64_t starting_population{};
        if (const auto* raw = find_field(record, "starting_population"); raw != nullptr && !parse_u64(*raw, starting_population)) {
            report.add_error(std::string{source_name}, "settlement starting_population must be a non-negative integer");
            continue;
        }

        auto item_report = registry.add(SettlementDefinition{
            .id = field_or_empty(record, "id"),
            .display_name = field_or_empty(record, "display_name"),
            .starting_population = starting_population,
        });
        for (const auto& message : item_report.messages()) {
            if (message.severity == ValidationSeverity::error) {
                report.add_error(message.path, message.message);
            } else {
                report.add_warning(message.path, message.message);
            }
        }
    }

    return report;
}

} // namespace clc::data
