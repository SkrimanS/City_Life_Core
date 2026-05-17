#include "clc/core/World.hpp"

#include <utility>

namespace clc {

World::World(WorldConfig config) : config_(std::move(config)) {
    event_log_.append(time_.current_tick(), "world.created", config_.name);
}

const WorldConfig& World::config() const noexcept {
    return config_;
}

const GameTime& World::time() const noexcept {
    return time_;
}

const EventLog& World::event_log() const noexcept {
    return event_log_;
}

Result<void> World::advance(GameTime::Tick ticks) {
    if (ticks == 0) {
        return make_error(ErrorCode::invalid_argument, "World::advance requires ticks > 0");
    }

    time_.advance(ticks);
    event_log_.append(time_.current_tick(), "world.advanced");
    return Result<void>{};
}

} // namespace clc
