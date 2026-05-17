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

} // namespace clc
