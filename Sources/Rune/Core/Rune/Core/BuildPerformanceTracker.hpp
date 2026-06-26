#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/String.hpp"

namespace rune
{
    struct ModulePerformanceStatistics final
    {
        String moduleName;
        String backendName;
        bool isExecutable{false};
        double lexingMs{0.0};
        double parsingMs{0.0};
        double semanticAnalysisMs{0.0};
        double moduleMs{0.0};
        double rilMs{0.0};
        double backendMs{0.0};
        double linkingMs{0.0};
        double totalMs{0.0};
    };

    class BuildPerformanceTracker final
    {
    public:
        static BuildPerformanceTracker& getInstance()
        {
            static BuildPerformanceTracker instance;
            return instance;
        }

        void startModule(String name)
        {
            activeModuleName = std::move(name);
            currentStats = ModulePerformanceStatistics{};
            currentStats.moduleName = activeModuleName;
        }

        void addLexingTime(double ms)
        {
            currentStats.lexingMs += ms;
        }

        void addParsingTime(double ms)
        {
            currentStats.parsingMs += ms;
        }

        void addSemanticAnalysisTime(double ms)
        {
            currentStats.semanticAnalysisMs += ms;
        }

        void addModuleTime(double ms)
        {
            currentStats.moduleMs += ms;
        }

        void setBackendName(String name)
        {
            currentStats.backendName = std::move(name);
        }

        void setIsExecutable(bool value)
        {
            currentStats.isExecutable = value;
        }

        void addRILTime(double ms)
        {
            currentStats.rilMs += ms;
        }

        void addBackendTime(double ms)
        {
            currentStats.backendMs += ms;
        }

        void addLinkingTime(double ms)
        {
            currentStats.linkingMs += ms;
        }

        void setTotalTime(double ms)
        {
            currentStats.totalMs = ms;
        }

        void addPreparationTime(double ms)
        {
            preparationMs += ms;
        }

        double getPreparationTime() const
        {
            return preparationMs;
        }

        void commitModule()
        {
            moduleStats.push_back(currentStats);
        }

        Array<ModulePerformanceStatistics> const& getModuleStats() const
        {
            return moduleStats;
        }

        void clear()
        {
            moduleStats.clear();
            activeModuleName.clear();
            currentStats = {};
            preparationMs = 0.0;
        }

    private:
        BuildPerformanceTracker() = default;
        ~BuildPerformanceTracker() = default;
        BuildPerformanceTracker(BuildPerformanceTracker const&) = delete;
        BuildPerformanceTracker& operator=(BuildPerformanceTracker const&) = delete;

        String activeModuleName;
        ModulePerformanceStatistics currentStats;
        Array<ModulePerformanceStatistics> moduleStats;
        double preparationMs{0.0};
    };
}
