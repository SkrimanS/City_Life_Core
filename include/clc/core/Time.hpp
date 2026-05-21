#pragma once

#include <cstdint>

namespace clc {

class GameTime final {
public:
    using Tick = std::uint64_t;

    [[nodiscard]] Tick current_tick() const noexcept;
    void advance(Tick ticks) noexcept;

private:
    Tick tick_{};
};

[[nodiscard]] constexpr GameTime::Tick ticks_per_second() noexcept {
    return 1;
}

[[nodiscard]] constexpr GameTime::Tick ticks_per_minute() noexcept {
    return 60 * ticks_per_second();
}

[[nodiscard]] constexpr GameTime::Tick ticks_per_hour() noexcept {
    return 60 * ticks_per_minute();
}

[[nodiscard]] constexpr GameTime::Tick ticks_per_day() noexcept {
    return 24 * ticks_per_hour();
}

[[nodiscard]] constexpr GameTime::Tick seconds_to_ticks(std::uint64_t seconds) noexcept {
    return seconds * ticks_per_second();
}

[[nodiscard]] constexpr GameTime::Tick minutes_to_ticks(std::uint64_t minutes) noexcept {
    return minutes * ticks_per_minute();
}

[[nodiscard]] constexpr GameTime::Tick hours_to_ticks(std::uint64_t hours) noexcept {
    return hours * ticks_per_hour();
}

[[nodiscard]] constexpr GameTime::Tick days_to_ticks(std::uint64_t days) noexcept {
    return days * ticks_per_day();
}

} // namespace clc
