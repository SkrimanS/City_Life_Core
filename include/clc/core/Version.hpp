#pragma once

#include <string_view>

namespace clc {

struct Version final {
    int major{};
    int minor{};
    int patch{};
};

[[nodiscard]] constexpr Version core_version() noexcept {
    return Version{0, 7, 7};
}

[[nodiscard]] constexpr std::string_view core_version_string() noexcept {
    return "0.7.7";
}

} // namespace clc
