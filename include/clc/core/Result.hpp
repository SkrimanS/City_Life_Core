#pragma once

#include <string>
#include <utility>
#include <variant>

namespace clc {

enum class ErrorCode {
    none = 0,
    invalid_argument,
    invalid_state,
    not_found,
    io_error,
    internal_error,
};

struct Error final {
    ErrorCode code{ErrorCode::none};
    std::string message{};
};

template <typename T>
class Result final {
public:
    Result(T value) : data_(std::move(value)) {}
    Result(Error error) : data_(std::move(error)) {}

    [[nodiscard]] bool ok() const noexcept {
        return std::holds_alternative<T>(data_);
    }

    [[nodiscard]] explicit operator bool() const noexcept {
        return ok();
    }

    [[nodiscard]] const T& value() const {
        return std::get<T>(data_);
    }

    [[nodiscard]] T& value() {
        return std::get<T>(data_);
    }

    [[nodiscard]] const Error& error() const {
        return std::get<Error>(data_);
    }

private:
    std::variant<T, Error> data_;
};

template <>
class Result<void> final {
public:
    Result() = default;
    Result(Error error) : error_(std::move(error)), ok_(false) {}

    [[nodiscard]] bool ok() const noexcept {
        return ok_;
    }

    [[nodiscard]] explicit operator bool() const noexcept {
        return ok();
    }

    [[nodiscard]] const Error& error() const noexcept {
        return error_;
    }

private:
    Error error_{};
    bool ok_{true};
};

[[nodiscard]] inline Error make_error(ErrorCode code, std::string message) {
    return Error{code, std::move(message)};
}

} // namespace clc
