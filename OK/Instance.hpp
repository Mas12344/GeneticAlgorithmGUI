#pragma once
#ifndef INSTANCE_H
#define INSTANCE_H
#include <vector>
#include <string>

class Instance {
public:
    Instance(const std::string& filepath);
    static Instance GenRandomInstance(int processors, int taskCount);
    inline std::vector<int>::const_iterator beginIter() const {return m_Tasks.cbegin();};
    inline std::vector<int>::const_iterator endIter() const {return m_Tasks.cend();};
    inline int GetTasksCount() const {return m_TasksCount;};
    inline int GetProcessorCount() const {return m_ProcessorsCount;};
    inline int GetTotalTime() const {return m_TotalTime;};
    void SaveToFile(const std::string& filepath);

private:
    Instance(int processors, int taskCount, std::vector<int> tasks);
    int m_TotalTime;
    int m_TasksCount;
    int m_ProcessorsCount;
    std::vector<int> m_Tasks;
};
#endif //!INSTANCE_H