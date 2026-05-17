#pragma once

#include "clc/core/Ids.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace clc {

struct Event final {
    EventId id{};
    std::uint64_t tick{};
    std::string type{};
    std::string payload{};
};

class EventLog final {
public:
    EventId append(std::uint64_t tick, std::string type, std::string payload = {});

    [[nodiscard]] const std::vector<Event>& events() const noexcept;
    [[nodiscard]] std::size_t size() const noexcept;

private:
    std::vector<Event> events_{};
    std::uint64_t next_id_{1};
};

} // namespace clc
