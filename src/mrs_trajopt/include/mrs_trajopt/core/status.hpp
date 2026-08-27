// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_TRAJOPT_CORE_STATUS_HPP
#define MRS_TRAJOPT_CORE_STATUS_HPP

#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

/// @file status.hpp
/// @brief Error-handling vocabulary (CN-13/CN-14/CN-15).
///
/// 🔴 TODO(integration): `mrs_core` package is absent in this checkout (only
/// `mrs_msgs` and `prototype` exist). The canon (`multibot3/CLAUDE.md` CN-13~15)
/// places `Status`/`Result<T,E>`/`ContractViolation` in `mrs_core`. Until that
/// package exists we define the vocabulary locally, in the `mrs_trajopt::core`
/// namespace, so the pure core builds without ROS. When `mrs_core` lands, this
/// header is replaced by an include of `mrs_core/status.hpp` and the local
/// definitions are deleted. The names and semantics are kept identical so the
/// swap is textual. See 20d_impl_trajopt.md "알려진 한계".

namespace mrs_trajopt::core
{

/// @brief Outcome of a value-less operation (CN-13): adapter / check / apply.
enum class Status
{
    kOk = 0,    ///< Success.
    kError = 1  ///< Failure without an accompanying value.
};

/// @brief Is @p s a success?
/// @param s Status to test.
/// @return true iff @p s == Status::kOk.
inline bool ok(Status s)
{
    return s == Status::kOk;
}

/// @brief Result carrying a value on success and an error tag on failure (CN-14).
///
/// `Result<void, E>` is forbidden by CN-14 — use Status for value-less outcomes.
/// The static_assert enforces the half of that rule that a type can enforce.
/// @tparam T Success value type (must not be void).
/// @tparam E Error tag type.
template<typename T, typename E>
class Result
{
    static_assert(!std::is_void_v<T>, "Result<void,E> is forbidden (CN-14); use Status.");

public:
    /// @brief Construct a success result.
    /// @param value The success value (moved in).
    /// @return A Result holding @p value.
    static Result ok_value(T value)
    {
        Result r;
        r.has_value_ = true;
        r.value_ = std::move(value);
        return r;
    }

    /// @brief Construct a failure result.
    /// @param error The error tag (moved in).
    /// @return A Result holding @p error.
    static Result err(E error)
    {
        Result r;
        r.has_value_ = false;
        r.error_ = std::move(error);
        return r;
    }

    /// @brief Did the operation succeed?
    /// @return true iff a value is held.
    bool ok() const
    {
        return has_value_;
    }

    /// @brief Access the success value.
    /// @return const reference to the held value.
    /// @throws std::logic_error if no value is held.
    const T& value() const
    {
        if (!has_value_)
        {
            throw std::logic_error("Result::value() on an error result");
        }
        return value_;
    }

    /// @brief Access the error tag.
    /// @return const reference to the held error.
    /// @throws std::logic_error if a value is held.
    const E& error() const
    {
        if (has_value_)
        {
            throw std::logic_error("Result::error() on an ok result");
        }
        return error_;
    }

private:
    bool has_value_ = false;
    T value_{};
    E error_{};
};

/// @brief Contract / invariant violation (CN-15). Thrown only on the startup path.
///
/// Never thrown inside a control-period tick — CN-16 requires tick-path
/// violations to become a fatal latch + counter + safe-stop declaration instead.
class ContractViolation : public std::runtime_error
{
public:
    /// @brief Construct with a human-readable message.
    /// @param what Description of the violated contract.
    explicit ContractViolation(const std::string& what) : std::runtime_error(what)
    {
    }
};

}  // namespace mrs_trajopt::core

#endif  // MRS_TRAJOPT_CORE_STATUS_HPP
