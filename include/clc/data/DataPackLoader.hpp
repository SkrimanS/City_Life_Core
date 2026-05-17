#pragma once

#include "clc/data/DataRegistry.hpp"
#include "clc/data/Validation.hpp"

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

namespace clc::data {

using DefinitionRecord = std::unordered_map<std::string, std::string>;

struct DataPackDocument final {
    std::string schema_version{};
    std::vector<DefinitionRecord> resources{};
    std::vector<DefinitionRecord> currencies{};
    std::vector<DefinitionRecord> buildings{};
    std::vector<DefinitionRecord> professions{};
    std::vector<DefinitionRecord> settlements{};
};

class DataPackLoader final {
public:
    [[nodiscard]] ValidationReport load_file(const std::filesystem::path& path, DataRegistry& registry) const;
    [[nodiscard]] ValidationReport load_string(std::string_view source_name, std::string_view content, DataRegistry& registry) const;

private:
    [[nodiscard]] ValidationReport parse(std::string_view source_name, std::string_view content, DataPackDocument& document) const;
    [[nodiscard]] ValidationReport register_document(std::string_view source_name, const DataPackDocument& document, DataRegistry& registry) const;
};

} // namespace clc::data
