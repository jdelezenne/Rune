#pragma once

#include "Rune/Core/Assert.hpp"
#include "Rune/Core/SourceLocation.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/Core/StringView.hpp"
#include "Rune/Core/Variant.hpp"
#include "Rune/Core/Void.hpp"

namespace rune
{
    /// Source-aware error value used by Result.
    struct SourceError final
    {
        String message;          ///< Human-readable error message.
        SourceLocation location; ///< Source location associated with the error.
        String id;               ///< Stable diagnostic id (optional).

        /// Constructs a source error.
        /// @param message Error message.
        /// @param location Source location associated with the error.
        /// @param diagnosticId Stable diagnostic id (empty when unknown).
        SourceError(String message, SourceLocation const& location, String diagnosticId = {})
            : message(std::move(message))
            , location(location)
            , id(std::move(diagnosticId))
        {
        }
    };

    /// Holds either a successful value of type T or a SourceError.
    /// @tparam T Successful result value type.
    template <typename T = Void>
    class Result final
    {
        template <typename U>
        friend class Result;

    private:
        using Storage = Variant<T, SourceError>; ///< Internal value-or-error storage.
        Storage storage;                         ///< Stored value.

    public:
        /// Constructs a successful Result with a default T value.
        Result()
            : storage(Void{})
        {
        }

        /// Constructs a successful Result from a value.
        /// @param value Successful value.
        Result(T value)
            : storage(std::move(value))
        {
        }

        /// Constructs a failed Result from a source error.
        /// @param error Error value.
        Result(SourceError error)
            : storage(std::move(error))
        {
        }

        /// Converts a Result<U> rvalue into Result<T> when U is convertible to T.
        /// @tparam U Source successful value type.
        /// @param other Source result.
        template <typename U>
        Result(Result<U>&& other)
        requires std::convertible_to<U, T>
            : storage(std::visit(
                  [](auto&& value) -> Storage
                  {
                      using ValueType = std::decay_t<decltype(value)>;

                      if constexpr (std::is_same_v<ValueType, SourceError>)
                      {
                          return std::move(value);
                      }
                      else
                      {
                          return T(std::move(value));
                      }
                  },
                  std::move(other.storage)))
        {
        }

        /// Converts a Result<U> const lvalue into Result<T> when U is convertible to T.
        /// @tparam U Source successful value type.
        /// @param other Source result.
        template <typename U>
        Result(Result<U> const& other)
        requires std::convertible_to<U, T>
            : storage(std::visit(
                  [](auto const& value) -> Storage
                  {
                      using ValueType = std::decay_t<decltype(value)>;

                      if constexpr (std::is_same_v<ValueType, SourceError>)
                      {
                          return value;
                      }
                      else
                      {
                          return T(value);
                      }
                  },
                  other.storage))
        {
        }

        /// Returns true if this Result contains a successful value.
        bool isOk() const
        {
            return std::holds_alternative<T>(storage);
        }

        /// Returns true if this Result contains an error.
        bool isError() const
        {
            return std::holds_alternative<SourceError>(storage);
        }

        /// Returns the successful value.
        /// @return Const reference to the successful value.
        T const& value() const
        {
            RUNE_ASSERT(isOk());

            return std::get<T>(storage);
        }

        /// Returns the successful value.
        /// @return Mutable reference to the successful value.
        T& value()
        {
            RUNE_ASSERT(isOk());

            return std::get<T>(storage);
        }

        /// Returns the successful value, or defaultValue when this Result is an error.
        /// @param defaultValue Value returned when this Result is an error.
        /// @return Stored value or defaultValue.
        T valueOr(T defaultValue) const
        {
            if (isOk())
            {
                return std::get<T>(storage);
            }

            return defaultValue;
        }

        /// Returns the error value.
        /// @return Const reference to the stored SourceError.
        SourceError const& error() const
        {
            RUNE_ASSERT(isError());

            return std::get<SourceError>(storage);
        }

        /// Moves out and returns the successful value.
        /// @return Moved successful value.
        T unwrap() &&
        {
            RUNE_ASSERT(isOk());

            return std::move(std::get<T>(storage));
        }

        /// Moves out and returns the error value.
        /// @return Moved SourceError value.
        SourceError const& unwrapError() const&
        {
            RUNE_ASSERT(isError());

            return std::get<SourceError>(storage);
        }

        SourceError& unwrapError() &
        {
            RUNE_ASSERT(isError());

            return std::get<SourceError>(storage);
        }

        SourceError unwrapError() &&
        {
            RUNE_ASSERT(isError());

            return std::move(std::get<SourceError>(storage));
        }

        /// Rewraps this error Result as Result<NewT>.
        /// @tparam NewT Target successful value type.
        /// @return Error Result with unchanged SourceError value.
        template <typename NewT>
        Result<NewT> asError() const&
        {
            RUNE_ASSERT(isError());

            if (isError())
            {
                return Result<NewT>(error());
            }
            else
            {
                // Trying to create error result from successful result
                rune::terminate();
            }
        }

        /// Rewraps this error Result as Result<NewT> by moving the error value.
        /// @tparam NewT Target successful value type.
        /// @return Error Result with moved SourceError value.
        template <typename NewT>
        Result<NewT> asError() &&
        {
            RUNE_ASSERT(isError());

            if (isError())
            {
                return Result<NewT>(std::move(*this).unwrapError());
            }
            else
            {
                rune::terminate();
            }
        }

        /// Creates Result<T> from another Result<U> known to contain an error.
        /// @tparam U Source successful value type.
        /// @param other Source error result.
        /// @return Error Result<T> carrying the same SourceError value.
        template <typename U>
        static Result<T> fromError(Result<U> const& other)
        {
            RUNE_ASSERT(other.isError());

            if (other.isError())
            {
                return Result<T>(other).error();
            }
            else
            {
                rune::terminate();
            }
        }

        /// Creates Result<T> from another Result<U> rvalue known to contain an error.
        /// @tparam U Source successful value type.
        /// @param other Source error result.
        /// @return Error Result<T> carrying the moved SourceError value.
        template <typename U>
        static Result<T> fromError(Result<U>&& other)
        {
            RUNE_ASSERT(other.isError());

            if (other.isError())
            {
                return Result<T>(std::move(other).unwrapError());
            }
            else
            {
                rune::terminate();
            }
        }
    };

    /// Creates a successful Result<Void>.
    /// @return Successful empty Result.
    inline Result<Void> Ok()
    {
        return Result<Void>();
    }

    /// Creates a successful Result<T> from a value.
    /// @tparam T Successful value type.
    /// @param value Successful value.
    /// @return Successful Result containing value.
    template <typename T>
    Result<T> Ok(T value)
    {
        return Result<T>(std::move(value));
    }

    /// Creates an error Result<T> from message and source location.
    /// @tparam T Successful value type.
    /// @param message Error message.
    /// @param location Error source location.
    /// @return Error Result containing SourceError.
    template <typename T = Void>
    Result<T> Error(String message, SourceLocation const& location)
    {
        return Result<T>(SourceError(std::move(message), location));
    }

    /// Creates an error Result<T> from message, source location, and diagnostic id.
    template <typename T = Void>
    Result<T> Error(String message, SourceLocation const& location, String diagnosticId)
    {
        return Result<T>(SourceError(std::move(message), location, std::move(diagnosticId)));
    }

    template <typename T = Void>
    Result<T> Error(String message, SourceLocation const& location, StringView diagnosticId)
    {
        return Result<T>(SourceError(std::move(message), location, String(diagnosticId)));
    }

    /// Creates an error Result<T> from message and file position.
    /// @tparam T Successful value type.
    /// @param message Error message.
    /// @param fileName Source file name.
    /// @param line 1-based line number.
    /// @param column 1-based column number.
    /// @return Error Result containing SourceError.
    template <typename T = Void>
    Result<T> Error(String message, String fileName, uint32 line, uint32 column)
    {
        return Result<T>(SourceError(std::move(message), {std::move(fileName), line, column}));
    }
}
