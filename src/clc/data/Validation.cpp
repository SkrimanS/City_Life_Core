#include "clc/data/Validation.hpp"

#include <algorithm>
#include <utility>

namespace clc::data {

void ValidationReport::add_error(std::string path, std::string message) {
    messages_.push_back(ValidationMessage{ValidationSeverity::error, std::move(path), std::move(message)});
}

void ValidationReport::add_warning(std::string path, std::string message) {
    messages_.push_back(ValidationMessage{ValidationSeverity::warning, std::move(path), std::move(message)});
}

bool ValidationReport::ok() const noexcept {
    return error_count() == 0;
}

std::size_t ValidationReport::error_count() const noexcept {
    return static_cast<std::size_t>(std::count_if(messages_.begin(), messages_.end(), [](const ValidationMessage& message) {
        return message.severity == ValidationSeverity::error;
    }));
}

std::size_t ValidationReport::warning_count() const noexcept {
    return static_cast<std::size_t>(std::count_if(messages_.begin(), messages_.end(), [](const ValidationMessage& message) {
        return message.severity == ValidationSeverity::warning;
    }));
}

const std::vector<ValidationMessage>& ValidationReport::messages() const noexcept {
    return messages_;
}

} // namespace clc::data
