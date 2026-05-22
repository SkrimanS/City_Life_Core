#include "clc/core/Time.hpp"

#include <cstdlib>
#include <iostream>
#include <limits>
#include <string_view>

namespace {

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "Test failed: " << message << '\n';
        std::exit(1);
    }
}

} // namespace

int main() {
    static_assert(clc::ticks_per_second() == 1);
    static_assert(clc::ticks_per_minute() == 60);
    static_assert(clc::ticks_per_hour() == 3600);
    static_assert(clc::ticks_per_day() == 86400);

    require(clc::seconds_to_ticks(5) == 5, "seconds should convert directly to ticks");
    require(clc::minutes_to_ticks(2) == 120, "minutes should convert to ticks");
    require(clc::hours_to_ticks(3) == 10800, "hours should convert to ticks");
    require(clc::days_to_ticks(2) == 172800, "days should convert to ticks");

    const auto max_tick = std::numeric_limits<clc::GameTime::Tick>::max();

    require(clc::can_convert_days_to_ticks(max_tick / clc::ticks_per_day()), "largest safe day conversion should be allowed");
    require(!clc::can_convert_days_to_ticks(max_tick / clc::ticks_per_day() + 1), "overflowing day conversion should be rejected by preflight helper");
    require(clc::days_to_ticks(max_tick / clc::ticks_per_day() + 1) == max_tick, "overflowing day conversion should saturate");

    require(clc::can_convert_hours_to_ticks(max_tick / clc::ticks_per_hour()), "largest safe hour conversion should be allowed");
    require(!clc::can_convert_hours_to_ticks(max_tick / clc::ticks_per_hour() + 1), "overflowing hour conversion should be rejected by preflight helper");
    require(clc::hours_to_ticks(max_tick / clc::ticks_per_hour() + 1) == max_tick, "overflowing hour conversion should saturate");

    clc::GameTime time{max_tick - 5};
    require(time.can_advance(5), "time should allow exact max advance");
    time.advance(5);
    require(time.current_tick() == max_tick, "exact max advance should reach max");
    require(!time.can_advance(1), "time at max should not allow further advance");
    time.advance(1);
    require(time.current_tick() == max_tick, "overflowing advance should saturate at max");

    clc::GameTime near_max{max_tick - 3};
    require(!near_max.can_advance(4), "near max time should reject overflowing advance by preflight");
    near_max.advance(4);
    require(near_max.current_tick() == max_tick, "overflowing near max advance should saturate");

    return 0;
}
