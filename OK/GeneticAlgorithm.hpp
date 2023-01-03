#pragma once
#ifndef GENETICALGORITHM_H
#define GENETICALGORITHM_H

#include "Chromosome.hpp"
#include "Instance.hpp"
#include <future>
#include <optional>


class GeneticAlgorithm{
public:
    GeneticAlgorithm(const std::string& filepath, int populationSize, float crossoverProb, float mutationProb, int maxGenNumber);
    int Run(bool timeLimit);
    void PrintBestChromosome();
    void RunInAnotherThread(bool timeLimit);
    std::optional<int> GetValue();
    float Progress = 0.0f;
private:
    std::vector<Chromosome> m_CurrentGeneration;
    int m_PopulationSize;
    Instance m_Instance;
    float m_CrossoverProb;
    float m_MutationProb;
    int m_MaxGenNumber;
    Chromosome m_BestChromosome;
    int m_BestChromosomeGenIndex;
    void RemoveDuplicates();
    std::vector<int> Duplicates();
    std::future<int> m_Value;
};


#endif //!GENETICALGORITHM_H