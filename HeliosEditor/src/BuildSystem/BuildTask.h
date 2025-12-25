#pragma once

#include <pch.h>
#include <string>
#include <future>

namespace Helios
{
    struct BuildStep
    {
        struct ProgressCallback {
            
            void operator()(const std::string& name, float progress) { name_and_progress_callback(name, progress); }
            void operator()(float progress) { progress_callback(progress); }
        private:
            ProgressCallback(std::function<void(const std::string&, float)> name_and_progress_callback, std::function<void(float)> progress_callback) : name_and_progress_callback(name_and_progress_callback), progress_callback(progress_callback) {}

            std::function<void(const std::string&, float)> name_and_progress_callback;
            std::function<void(float)> progress_callback;

            friend class BuildTask;
        };
        using Action = std::function<bool(ProgressCallback)>;

        std::string name;
        Action action;
    };
    
    class BuildTask
    {
        public:
            struct Progress
            {
                std::string step;
                std::string name;
                float progress;

                void reset() { step = name = ""; progress = 0.0f; }
            };
        
            BuildTask(const std::string& name) : m_Name(name) {}
            BuildTask(const std::string& name, std::initializer_list<BuildStep> steps) : m_Name(name) {
                for (const auto& step : steps)
                    m_Steps.push_back(CreateUnique<BuildStep>(step));
            }
        
            void AddStep(const std::string& name, BuildStep::Action action) { assert(m_IsRunning == false && "BuildTask is running"); m_Steps.push_back(CreateUnique<BuildStep>(BuildStep { .name = name, .action = action })); }
            void AddStep(const BuildStep& step) { assert(m_IsRunning == false && "BuildTask is running"); m_Steps.push_back(CreateUnique<BuildStep>(BuildStep { .name = step.name, .action = step.action })); }
            void AddStep(Unique<BuildStep> step) { assert(m_IsRunning == false && "BuildTask is running"); m_Steps.push_back(std::move(step)); }

            void RemoveStep(std::size_t index) { assert(m_IsRunning == false && "BuildTask is running"); m_Steps.erase(m_Steps.begin() + index); }
            void RemoveStep(const BuildStep& step) { RemoveStep(step.name); }
            void RemoveStep(Unique<BuildStep> step) { assert(m_IsRunning == false && "BuildTask is running"); m_Steps.erase(std::remove(m_Steps.begin(), m_Steps.end(), step), m_Steps.end()); }
            void RemoveStep(const std::string& name) { assert(m_IsRunning == false && "BuildTask is running"); m_Steps.erase(std::remove_if(m_Steps.begin(), m_Steps.end(), [&name](const Unique<BuildStep>& step) { return step->name == name; }), m_Steps.end()); }

            std::vector<Unique<BuildStep>>& GetSteps() { return m_Steps; }

            void ClearSteps() { assert(m_IsRunning == false && "BuildTask is running"); m_Steps.clear(); }
    
            bool Run() 
            { 
                {
                    std::lock_guard<std::mutex> lock(m_RunInfoMutex);
                    m_Progress.reset();
                }
                m_Failed = false;
                m_IsRunning = true;
                bool result = std::all_of(m_Steps.begin(), m_Steps.end(), [this](const Unique<BuildStep>& step) 
                { 
                    m_Progress.step = step->name; 
                    return step->action(m_ProgressCallback); 
                });
                m_IsRunning = false;
                m_Failed = !result;
                return result;
            }
            std::future<bool> RunAsync() { return std::async(std::launch::async, &BuildTask::Run, this); }
            void RunAsync(std::function<void(bool)> callback) { 
                std::thread([callback, this]() { callback(Run()); }).detach();
            }

            inline std::string_view GetName() const { return m_Name; }
            const Progress GetProgress() { // Returns a copy to prevent race conditions
                std::lock_guard<std::mutex> lock(m_RunInfoMutex);
                return m_Progress;
            }
            const bool IsRunning() { return m_IsRunning.load(); }
            const bool Failed() { return m_Failed.load(); }
        private:
            std::string m_Name;
            std::vector<Unique<BuildStep>> m_Steps;
            std::mutex m_RunInfoMutex;
            std::atomic<bool> m_IsRunning = false;
            std::atomic<bool> m_Failed = false;

            void ActionProgressCallback(const std::string& name, float progress) 
            {
                std::lock_guard<std::mutex> lock(m_RunInfoMutex);
                m_Progress.name = name;
                m_Progress.progress = progress;
            }

            void ActionProgressCallbackPOnly(float progress) 
            {
                std::lock_guard<std::mutex> lock(m_RunInfoMutex);
                m_Progress.progress = progress;
            }

            Progress m_Progress;

            BuildStep::ProgressCallback m_ProgressCallback = BuildStep::ProgressCallback(
                std::bind(&BuildTask::ActionProgressCallback, this, std::placeholders::_1, std::placeholders::_2),
                std::bind(&BuildTask::ActionProgressCallbackPOnly, this, std::placeholders::_1)
            );
    };
}