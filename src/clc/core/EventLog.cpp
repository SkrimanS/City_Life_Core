#include "clc/core/EventLog.hpp"

#include <utility>

namespace clc {

EventId EventLog::append(std::uint64_t tick, std::string type, std::string payload) {
    const EventId id{next_id_++};
    events_.push_back(Event{id, tick, std::move(type), std::move(payload)});
    return id;
}

const std::vector<Event>& EventLog::events() const noexcept {
    return events_;
}

std::size_t EventLog::size() const noexcept {
    return events_.size();
}

} // namespace clc
