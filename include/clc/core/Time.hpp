#pragma once

#include <cstdint>
#include <limits>

namespace clc {

class GameTime final {
public:
    using Tick = std::uint64_t;

    constexpr GameTime() noexcept = default;
    explicit constexpr GameTime(Tick tick) noexcept
        : tick_(tick) {}

    [[nodiscard]] Tick current_tick() const noexcept;
    [[nodiscard]] bool can_advance(Tick ticks) const noexcept;
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

[[nodiscard]] constexpr bool can_multiply_ticks(std::uint64_t value, GameTime::Tick multiplier) noexcept {
    return multiplier == 0 || value <= std::numeric_limits<GameTime::Tick>::max() / multiplier;
}

[[nodiscard]] constexpr GameTime::Tick saturating_multiply_ticks(std::uint64_t value, GameTime::Tick multiplier) noexcept {
    return can_multiply_ticks(value, multiplier)
        ? value * multiplier
        : std::numeric_limits<GameTime::Tick>::max();
}

[[nodiscard]] constexpr bool can_convert_seconds_to_ticks(std::uint64_t seconds) noexcept {
    return can_multiply_ticks(seconds, ticks_per_second());
}

[[nodiscard]] constexpr bool can_convert_minutes_to_ticks(std::uint64_t minutes) noexcept {
    return can_multiply_ticks(minutes, ticks_per_minute());
}

[[nodiscard]] constexpr bool can_convert_hours_to_ticks(std::uint64_t hours) noexcept {
    return can_multiply_ticks(hours, ticks_per_hour());
}

[[nodiscard]] constexpr bool can_convert_days_to_ticks(std::uint64_t days) noexcept {
    return can_multiply_ticks(days, ticks_per_day());
}

[[nodiscard]] constexpr GameTime::Tick seconds_to_ticks(std::uint64_t seconds) noexcept {
    return saturating_multiply_ticks(seconds, ticks_per_second());
}

[[nodiscard]] constexpr GameTime::Tick minutes_to_ticks(std::uint64_t minutes) noexcept {
    return saturating_multiply_ticks(minutes, ticks_per_minute());
}

[[nodiscard]] constexpr GameTime::Tick hours_to_ticks(std::uint64_t hours) noexcept {
    return saturating_multiply_ticks(hours, ticks_per_hour());
}

[[nodiscard]] constexpr GameTime::Tick days_to_ticks(std::uint64_t days) noexcept {
    return saturating_multiply_ticks(days, ticks_per_day());
}

[[nodiscard]] inline GameTime make_game_time_at_tick(GameTime::Tick tick) noexcept {
    return GameTime{tick};
}

} // namespace clc
