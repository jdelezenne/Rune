#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/HashMap.hpp"
#include "Rune/Core/Optional.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/Core/StringView.hpp"

namespace rune
{
    /// Holds all values extracted from a single command-line invocation.
    class CommandLineArguments final
    {
    public:
        bool getFlag(StringView name) const;
        Optional<String> getOption(StringView name) const;
        Array<String> const& getOptions(StringView name) const;
        Array<String> const& getPositionals() const;
        Array<String> const& getTrailingArguments() const;

    private:
        friend class CommandLineParser;

        void setFlag(String name);
        void setOption(String name, String value);
        void addOptionValue(String name, String value);
        void addPositional(String value);
        void addTrailingArgument(String value);

        HashMap<String, bool> flagValues;
        HashMap<String, String> singleOptionValues;
        HashMap<String, Array<String>> multiOptionValues;
        Array<String> positionalArguments;
        Array<String> trailingArguments;
    };

    struct CommandLineParseResult final
    {
        bool ok = false;
        bool helpRequested = false;
        Optional<String> command;
        Optional<String> subcommand;
        CommandLineArguments arguments;
    };

    enum class CommandLineOptionArity
    {
        Flag,
        Single,
        Multi,
    };

    struct CommandLineOptionSpecification final
    {
        String longFlag;
        String shortFlag;
        String valueName;
        String description;
        CommandLineOptionArity arity = CommandLineOptionArity::Flag;
    };

    struct CommandLinePositionalSpecification final
    {
        String name;
        String description;
        bool repeatable = false;
    };

    class CommandLineSubcommand;
    class CommandLineCommand;

    class CommandLineSubcommand final
    {
    public:
        CommandLineSubcommand& description(String text);

        CommandLineSubcommand& flag(String longFlag, String shortFlag, String description);
        CommandLineSubcommand& flag(String longFlag, String description);

        CommandLineSubcommand& option(String longFlag, String shortFlag, String valueName, String description);
        CommandLineSubcommand& option(String longFlag, String valueName, String description);

        CommandLineSubcommand& multiOption(
            String longFlag,
            String shortFlag,
            String valueName,
            String description);
        CommandLineSubcommand& multiOption(String longFlag, String valueName, String description);

        CommandLineSubcommand& positional(String name, String description);
        CommandLineSubcommand& positionals(String name, String description);

    private:
        friend class CommandLineCommand;
        friend class CommandLineParser;

        String subcommandName;
        String descriptionText;
        Array<CommandLineOptionSpecification> optionSpecifications;
        Array<CommandLinePositionalSpecification> positionalArgumentSpecifications;
    };

    class CommandLineCommand final
    {
    public:
        CommandLineCommand& description(String text);

        CommandLineCommand& flag(String longFlag, String shortFlag, String description);
        CommandLineCommand& flag(String longFlag, String description);

        CommandLineCommand& option(String longFlag, String shortFlag, String valueName, String description);
        CommandLineCommand& option(String longFlag, String valueName, String description);

        CommandLineCommand& multiOption(
            String longFlag,
            String shortFlag,
            String valueName,
            String description);
        CommandLineCommand& multiOption(String longFlag, String valueName, String description);

        CommandLineCommand& positional(String name, String description);
        CommandLineCommand& positionals(String name, String description);

        CommandLineSubcommand& subcommand(String name);

    private:
        friend class CommandLineParser;

        String commandName;
        String descriptionText;
        Array<CommandLineOptionSpecification> optionSpecifications;
        Array<CommandLinePositionalSpecification> positionalArgumentSpecifications;
        Array<CommandLineSubcommand> subcommandSpecifications;
    };

    class CommandLineParser final
    {
    public:
        explicit CommandLineParser(String programName, String description = {});

        CommandLineCommand& command(String name);

        CommandLineParser& flag(String longFlag, String shortFlag, String description);
        CommandLineParser& flag(String longFlag, String description);

        CommandLineParser& option(String longFlag, String shortFlag, String valueName, String description);
        CommandLineParser& multiOption(String longFlag, String shortFlag, String valueName, String description);

        CommandLineParser& defaultCommand(String name);

        CommandLineParseResult parse(int argc, char* argv[]) const;

        void printHelp() const;
        void printCommandHelp(StringView commandName, Optional<StringView> subcommandName = {}) const;

    private:
        String programNameText;
        String descriptionText;
        String defaultCommandName;
        Array<CommandLineCommand> commandSpecifications;
        Array<CommandLineOptionSpecification> globalOptionSpecifications;

        CommandLineCommand const* findCommand(StringView name) const;
        CommandLineSubcommand const* findSubcommand(CommandLineCommand const& command, StringView name) const;

        using OptionLookup = HashMap<String, CommandLineOptionSpecification const*>;
        OptionLookup buildOptionLookup(
            CommandLineCommand const* command,
            CommandLineSubcommand const* subcommand) const;

        static void printOptionTable(Array<CommandLineOptionSpecification> const& options);
        static void printPositionalTable(Array<CommandLinePositionalSpecification> const& positionals);
        static String formatOptionLabel(CommandLineOptionSpecification const& specification);

        void printSubcommandList(CommandLineCommand const& command) const;
    };
}
