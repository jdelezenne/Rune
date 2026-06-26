#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/Assert.hpp"
#include "Rune/Core/Macros.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/Core/StringView.hpp"
#include "Rune/Core/Types.hpp"

#include "Rune/Core/SharedPtr.hpp"
#include "Rune/Core/Types.hpp"

#include <atomic>
#include <format>
#include <mutex>

namespace rune
{
    /// Severity levels for log messages.
    enum class LogLevel
    {
        Debug,   ///< Diagnostic detail intended for development.
        Info,    ///< Informational message.
        Warning, ///< Recoverable issue or unexpected condition.
        Error,   ///< Error condition.
    };

    /// Converts a log level to its display name.
    /// @param level Log level to convert.
    /// @return Human-readable level name.
    inline StringView logLevelToString(LogLevel level)
    {
        switch (level)
        {
            case LogLevel::Debug:
                return "Debug";

            case LogLevel::Info:
                return "Info";

            case LogLevel::Warning:
                return "Warning";

            case LogLevel::Error:
                return "Error";
        }

        RUNE_UNREACHABLE();
    }

    /// Abstract log output sink with configurable minimum level.
    class LogOutput
    {
    private:
        std::atomic<LogLevel> level; ///< Minimum level emitted by this sink.

    public:
        LogOutput();
        virtual ~LogOutput() = default;

        /// Writes a formatted log message when level meets the sink threshold.
        /// @param level Message severity.
        /// @param message Message text.
        virtual void log(LogLevel level, StringView message) = 0;

        /// Returns the current minimum log level for this sink.
        LogLevel getLevel() const;

        /// Sets the minimum log level for this sink.
        /// @param value New minimum level.
        void setLevel(LogLevel value);
    };

    /// Log output sink that writes to standard output.
    class StdoutLogOutput final : public LogOutput
    {
    private:
        std::mutex mutex; ///< Serializes concurrent writes.

    public:
        StdoutLogOutput() = default;

        virtual void log(LogLevel level, StringView message) override;
    };

    /// Static logger that fan-outs messages to registered outputs.
    class Logger final
    {
        RUNE_INTERFACE(Logger)
        RUNE_NOT_COPYABLE(Logger)
        RUNE_NOT_MOVABLE(Logger)

    private:
        static std::mutex mutex;                    ///< Protects the output list.
        static Array<SharedPtr<LogOutput>> outputs; ///< Registered output sinks.

    public:
        /// Registers an output sink.
        /// @param output Sink to add.
        static void addOutput(SharedPtr<LogOutput> output);

        /// Unregisters a previously added output sink.
        /// @param output Sink to remove.
        static void removeOutput(SharedPtr<LogOutput> output);

        /// Writes a message to all registered outputs.
        /// @param level Message severity.
        /// @param message Message text.
        static void log(LogLevel level, StringView message);

        /// Writes a preformatted message to all registered outputs.
        /// @param level Message severity.
        /// @param message Message text.
        static void logFormat(LogLevel level, StringView message)
        {
            log(level, message);
        }

        /// Formats and writes a message to all registered outputs.
        /// @param level Message severity.
        /// @param message std::format-style format string.
        /// @param args Format arguments.
        template <typename... Args>
        static void logFormat(LogLevel level, StringView message, Args const&... args)
        {
            String output = std::vformat(StringView(message.data(), message.size()), std::make_format_args(args...));

            log(level, output);
        }

        /// Writes a debug-level message.
        /// @param message Message text.
        static void debug(StringView message)
        {
            log(LogLevel::Debug, message);
        }

        /// Formats and writes a debug-level message.
        /// @param message std::format-style format string.
        /// @param args Format arguments.
        template <typename... Args>
        static void debug(StringView message, Args const&... args)
        {
            logFormat(LogLevel::Debug, message, args...);
        }

        /// Writes an info-level message.
        /// @param message Message text.
        static void info(StringView message)
        {
            log(LogLevel::Info, message);
        }

        /// Formats and writes an info-level message.
        /// @param message std::format-style format string.
        /// @param args Format arguments.
        template <typename... Args>
        static void info(StringView message, Args const&... args)
        {
            logFormat(LogLevel::Info, message, args...);
        }

        /// Writes a warning-level message.
        /// @param message Message text.
        static void warn(StringView message)
        {
            log(LogLevel::Warning, message);
        }

        /// Formats and writes a warning-level message.
        /// @param message std::format-style format string.
        /// @param args Format arguments.
        template <typename... Args>
        static void warn(StringView message, Args const&... args)
        {
            logFormat(LogLevel::Warning, message, args...);
        }

        /// Writes an error-level message.
        /// @param message Message text.
        static void error(StringView message)
        {
            log(LogLevel::Error, message);
        }

        /// Formats and writes an error-level message.
        /// @param message std::format-style format string.
        /// @param args Format arguments.
        template <typename... Args>
        static void error(StringView message, Args const&... args)
        {
            logFormat(LogLevel::Error, message, args...);
        }
    };
}
