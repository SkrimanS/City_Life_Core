#pragma once

#include "clc/data/Validation.hpp"

#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>

namespace clc::sim {

class ResourceStorage final {
public:
    [[nodiscard]] data::ValidationReport add(std::string resource_id, std::uint64_t amount);
    [[nodiscard]] bool try_remove(std::string_view resource_id, std::uint64_t amount);
    [[nodiscard]] std::uint64_t remove_up_to(std::string_view resource_id, std::uint64_t amount);

    [[nodiscard]] std::uint64_t amount(std::string_view resource_id) const;
    [[nodiscard]] bool empty() const noexcept;
    [[nodiscard]] const std::unordered_map<std::string, std::uint64_t>& entries() const noexcept;

private:
    std::unordered_map<std::string, std::uint64_t> entries_{};
};

[[nodiscard]] data::ValidationReport transfer(ResourceStorage& from, ResourceStorage& to, std::string_view resource_id, std::uint64_t amount);

} // namespace clc::sim
