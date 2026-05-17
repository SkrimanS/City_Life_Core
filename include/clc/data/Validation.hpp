#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace clc::data {

enum class ValidationSeverity {
    warning,
    error,
};

struct ValidationMessage final {
    ValidationSeverity severity{ValidationSeverity::error};
    std::string path{};
    std::string message{};
};

class ValidationReport final {
public:
    void add_error(std::string path, std::string message);
    void add_warning(std::string path, std::string message);

    [[nodiscard]] bool ok() const noexcept;
    [[nodiscard]] std::size_t error_count() const noexcept;
    [[nodiscard]] std::size_t warning_count() const noexcept;
    [[nodiscard]] const std::vector<ValidationMessage>& messages() const noexcept;

private:
    std::vector<ValidationMessage> messages_{};
};

} // namespace clc::data
