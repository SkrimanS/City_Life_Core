#pragma once

#include <string_view>

namespace clc {

struct Version final {
    int major{};
    int minor{};
    int patch{};
};

[[nodiscard]] constexpr Version core_version() noexcept {
    return Version{0, 8, 5};
}

[[nodiscard]] constexpr std::string_view core_version_string() noexcept {
    return "0.8.5";
}

} // namespace clc
