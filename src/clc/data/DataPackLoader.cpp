#include "clc/data/DataPackLoader.hpp"

#include <charconv>
#include <fstream>
#include <limits>
#include <sstream>
#include <string_view>
#include <utility>
#include <vector>

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

std::vector<std::string> split_csv(std::string_view text) {
    std::vector<std::string> values;
    std::size_t start = 0;
    while (start <= text.size()) {
        const auto comma = text.find(',', start);
        const auto end = comma == std::string_view::npos ? text.size() : comma;
        auto value = trim(text.substr(start, end - start));
        if (!value.empty()) {
            values.push_back(std::move(value));
        }
        if (comma == std::string_view::npos) {
            break;
        }
        start = comma + 1;
    }
    return values;
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

std::vector<std::string> csv_field_or_empty(const DefinitionRecord& record, std::string_view key) {
    if (const auto* value = find_field(record, key)) {
        return split_csv(*value);
    }
    return {};
}

bool is_section(std::string_view line) {
    return line.size() >= 2 && line.front() == '[' && line.back() == ']';
}

void append_record(DataPackDocument& document, std::string_view section, DefinitionRecord record) {
    if (record.empty()) {
        return;
    }
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
    }
}

bool is_known_record_section(std::string_view section) {
    return section == "resource" || section == "currency" || section == "building" || section == "profession" || section == "settlement";
}

void append_messages(ValidationReport& target, const ValidationReport& source) {
    for (const auto& message : source.messages()) {
        if (message.severity == ValidationSeverity::error) {
            target.add_error(message.path, message.message);
        } else {
            target.add_warning(message.path, message.message);
        }
    }
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
    append_messages(register_report, registry.validate_references());
    return register_report;
}

ValidationReport DataPackLoader::parse(std::string_view source_name, std::string_view content, DataPackDocument& document) const {
    ValidationReport report;
    std::istringstream input{std::string{content}};
    std::string line;
    std::string section;
    DefinitionRecord current_record;
    std::size_t line_number = 0;

    while (std::getline(input, line)) {
        ++line_number;
        auto cleaned = trim(line);
        if (cleaned.empty() || cleaned.front() == '#') {
            continue;
        }

        if (is_section(cleaned)) {
            append_record(document, section, std::move(current_record));
            current_record = DefinitionRecord{};
            section = cleaned.substr(1, cleaned.size() - 2);

            if (!section.empty() && !is_known_record_section(section)) {
                report.add_error(std::string{source_name} + ":" + std::to_string(line_number), "unknown section: " + section);
            }
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

        if (!is_known_record_section(section)) {
            continue;
        }

        if (current_record.find(key) != current_record.end()) {
            report.add_error(std::string{source_name} + ":" + std::to_string(line_number), "duplicate key in section: " + key);
            continue;
        }
        current_record.emplace(std::move(key), std::move(value));
    }

    append_record(document, section, std::move(current_record));

    if (document.schema_version.empty()) {
        report.add_error(std::string{source_name}, "schema_version is required");
    } else if (document.schema_version != "0.2.2") {
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

        append_messages(report, registry.add(ResourceDefinition{
            .id = field_or_empty(record, "id"),
            .display_name = field_or_empty(record, "display_name"),
            .category = field_or_empty(record, "category"),
            .base_value = base_value,
        }));
    }

    for (const auto& record : document.currencies) {
        std::uint32_t fractional_digits{};
        if (const auto* raw = find_field(record, "fractional_digits"); raw != nullptr && !parse_u32(*raw, fractional_digits)) {
            report.add_error(std::string{source_name}, "currency fractional_digits must be a non-negative integer");
            continue;
        }

        append_messages(report, registry.add(CurrencyDefinition{
            .id = field_or_empty(record, "id"),
            .display_name = field_or_empty(record, "display_name"),
            .fractional_digits = fractional_digits,
        }));
    }

    for (const auto& record : document.buildings) {
        std::uint32_t worker_slots{};
        if (const auto* raw = find_field(record, "worker_slots"); raw != nullptr && !parse_u32(*raw, worker_slots)) {
            report.add_error(std::string{source_name}, "building worker_slots must be a non-negative integer");
            continue;
        }

        append_messages(report, registry.add(BuildingDefinition{
            .id = field_or_empty(record, "id"),
            .display_name = field_or_empty(record, "display_name"),
            .category = field_or_empty(record, "category"),
            .worker_slots = worker_slots,
            .required_profession_id = field_or_empty(record, "required_profession_id"),
            .input_resource_ids = csv_field_or_empty(record, "input_resource_ids"),
            .output_resource_ids = csv_field_or_empty(record, "output_resource_ids"),
        }));
    }

    for (const auto& record : document.professions) {
        append_messages(report, registry.add(ProfessionDefinition{
            .id = field_or_empty(record, "id"),
            .display_name = field_or_empty(record, "display_name"),
            .category = field_or_empty(record, "category"),
        }));
    }

    for (const auto& record : document.settlements) {
        std::uint64_t starting_population{};
        if (const auto* raw = find_field(record, "starting_population"); raw != nullptr && !parse_u64(*raw, starting_population)) {
            report.add_error(std::string{source_name}, "settlement starting_population must be a non-negative integer");
            continue;
        }

        append_messages(report, registry.add(SettlementDefinition{
            .id = field_or_empty(record, "id"),
            .display_name = field_or_empty(record, "display_name"),
            .starting_population = starting_population,
        }));
    }

    return report;
}

} // namespace clc::data
