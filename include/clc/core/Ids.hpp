#pragma once

#include <cstdint>

namespace clc {

struct WorldId final {
    std::uint64_t value{};
};

struct EventId final {
    std::uint64_t value{};
};

struct SettlementId final {
    std::uint64_t value{};
};

[[nodiscard]] constexpr bool operator==(WorldId lhs, WorldId rhs) noexcept {
    return lhs.value == rhs.value;
}

[[nodiscard]] constexpr bool operator==(EventId lhs, EventId rhs) noexcept {
    return lhs.value == rhs.value;
}

[[nodiscard]] constexpr bool operator==(SettlementId lhs, SettlementId rhs) noexcept {
    return lhs.value == rhs.value;
}

} // namespace clc
