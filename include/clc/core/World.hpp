#pragma once

#include "clc/core/EventLog.hpp"
#include "clc/core/Ids.hpp"
#include "clc/core/Result.hpp"
#include "clc/core/Time.hpp"

#include <cstdint>
#include <string>

namespace clc {

struct WorldConfig final {
    std::string name{"City Life World"};
    std::uint64_t seed{1};
};

class World final {
public:
    explicit World(WorldConfig config);

    [[nodiscard]] const WorldConfig& config() const noexcept;
    [[nodiscard]] const GameTime& time() const noexcept;
    [[nodiscard]] const EventLog& event_log() const noexcept;

    Result<void> advance(GameTime::Tick ticks);

private:
    WorldConfig config_{};
    GameTime time_{};
    EventLog event_log_{};
};

} // namespace clc
