#include "clc/core/Time.hpp"

namespace clc {

GameTime::Tick GameTime::current_tick() const noexcept {
    return tick_;
}

void GameTime::advance(Tick ticks) noexcept {
    tick_ += ticks;
}

} // namespace clc
