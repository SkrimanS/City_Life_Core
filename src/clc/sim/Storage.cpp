#include "clc/sim/Storage.hpp"

#include <limits>
#include <string>
#include <utility>

namespace clc::sim {

data::ValidationReport ResourceStorage::add(std::string resource_id, std::uint64_t amount) {
    data::ValidationReport report;
    if (resource_id.empty()) {
        report.add_error("storage", "resource_id must not be empty");
        return report;
    }

    if (amount == 0) {
        report.add_error("storage." + resource_id, "amount must be greater than zero");
        return report;
    }

    const auto it = entries_.find(resource_id);
    if (it != entries_.end()) {
        if (amount > std::numeric_limits<std::uint64_t>::max() - it->second) {
            report.add_error("storage." + resource_id, "amount overflow");
            return report;
        }

        it->second += amount;
        return report;
    }

    entries_.emplace(std::move(resource_id), amount);
    return report;
}

bool ResourceStorage::try_remove(std::string_view resource_id, std::uint64_t amount) {
    if (amount == 0) {
        return false;
    }

    auto it = entries_.find(std::string{resource_id});
    if (it == entries_.end() || it->second < amount) {
        return false;
    }

    it->second -= amount;
    if (it->second == 0) {
        entries_.erase(it);
    }
    return true;
}

std::uint64_t ResourceStorage::remove_up_to(std::string_view resource_id, std::uint64_t amount) {
    if (amount == 0) {
        return 0;
    }

    auto it = entries_.find(std::string{resource_id});
    if (it == entries_.end()) {
        return 0;
    }

    const auto removed = it->second < amount ? it->second : amount;
    it->second -= removed;
    if (it->second == 0) {
        entries_.erase(it);
    }
    return removed;
}

std::uint64_t ResourceStorage::amount(std::string_view resource_id) const {
    const auto it = entries_.find(std::string{resource_id});
    if (it == entries_.end()) {
        return 0;
    }
    return it->second;
}

bool ResourceStorage::empty() const noexcept {
    return entries_.empty();
}

const std::unordered_map<std::string, std::uint64_t>& ResourceStorage::entries() const noexcept {
    return entries_;
}

data::ValidationReport transfer(ResourceStorage& from, ResourceStorage& to, std::string_view resource_id, std::uint64_t amount) {
    data::ValidationReport report;
    if (resource_id.empty()) {
        report.add_error("storage.transfer", "resource_id must not be empty");
        return report;
    }
    if (amount == 0) {
        report.add_error("storage.transfer." + std::string{resource_id}, "amount must be greater than zero");
        return report;
    }

    if (&from == &to) {
        if (from.amount(resource_id) < amount) {
            report.add_error("storage.transfer." + std::string{resource_id}, "not enough resources to transfer");
        }
        return report;
    }

    auto from_copy = from;
    auto to_copy = to;

    if (!from_copy.try_remove(resource_id, amount)) {
        report.add_error("storage.transfer." + std::string{resource_id}, "not enough resources to transfer");
        return report;
    }

    auto add_report = to_copy.add(std::string{resource_id}, amount);
    if (!add_report.ok()) {
        return add_report;
    }

    from = std::move(from_copy);
    to = std::move(to_copy);
    return report;
}

} // namespace clc::sim
