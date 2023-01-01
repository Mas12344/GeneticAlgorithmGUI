#include "Instance.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cassert>
#include <random>

Instance::Instance(const std::string& filepath)
    : m_ProcessorsCount(0), m_TasksCount(0)
{
    std::ifstream data(filepath);
    std::stringstream ss;
    ss << data.rdbuf();
    std::string line;
    std::getline(ss, line, '\n');
    m_ProcessorsCount = std::stoi(line);
    std::getline(ss, line, '\n');
    m_TasksCount = std::stoi(line);

    while(std::getline(ss, line, '\n')){
        m_Tasks.push_back(std::stoi(line));
    }
    
    assert(m_TasksCount == m_Tasks.size() && "nieoczekiwana liczba zadan");
}

Instance Instance::GenRandomInstance(int processors, int taskCount){
    std::random_device dv;
    std::mt19937 gen(dv());
    std::uniform_int_distribution<> rand_vals(30, 100);

    std::vector<int> tasks;

    for(int i = 0; i < taskCount; ++i){
        int tmp = rand_vals(gen);
        tasks.push_back(tmp);
    }

    return Instance(processors, taskCount, tasks);
}

Instance::Instance(int processors, int taskCount, std::vector<int> tasks)
    : m_ProcessorsCount(processors), m_TasksCount(taskCount), m_Tasks(tasks)
{
}

void Instance::SaveToFile(const std::string& filepath){
    std::ofstream file(filepath);

    file << m_ProcessorsCount << std::endl;
    file << m_TasksCount << std::endl;

    for(int i = 0; i < m_TasksCount; ++i){
        file << m_Tasks.at(i) << std::endl;
    }

    file.close();
}