#include "clc/core/Time.hpp"

#include <limits>

namespace clc {

GameTime::Tick GameTime::current_tick() const noexcept {
    return tick_;
}

bool GameTime::can_advance(Tick ticks) const noexcept {
    return ticks <= std::numeric_limits<Tick>::max() - tick_;
}

void GameTime::advance(Tick ticks) noexcept {
    if (!can_advance(ticks)) {
        tick_ = std::numeric_limits<Tick>::max();
        return;
    }

    tick_ += ticks;
}

} // namespace clc
