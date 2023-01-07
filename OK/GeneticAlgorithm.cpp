#include <algorithm>
#include <chrono>
#include <iostream>
#include <unordered_map>

#include <execution>
#include <algorithm>

#include "GeneticAlgorithm.hpp"

#define NO_PRINT

GeneticAlgorithm::GeneticAlgorithm(
    const std::string& filepath,
    int populationSize,
    float crossoverProb,
    float mutationProb,
    int maxGenNumber
    ) : m_Instance(filepath), m_PopulationSize(populationSize),
        m_CrossoverProb(crossoverProb),
        m_MutationProb(mutationProb), m_MaxGenNumber(maxGenNumber),
        m_BestChromosome(Chromosome(m_Instance, Chromosome::GenRandomChromosome(m_Instance.GetTasksCount()))),
        m_BestChromosomeGenIndex(0),
        HistoricalBest()
{
    for(int i = 0; i < m_PopulationSize; i++){
       m_CurrentGeneration.push_back(Chromosome(m_Instance, Chromosome::GenRandomChromosome(m_Instance.GetTasksCount())));

    }
}

std::vector<int> GeneticAlgorithm::Duplicates() {
    std::vector<int> duplicates;
    int Size = m_CurrentGeneration.size();
    std::unordered_map<unsigned long, int> indexMap(Size);
    for (int i = 0; i < Size; i++) {
        auto hash = m_CurrentGeneration.at(i).GetHash();
        if (indexMap[hash]++ != 0) {
            duplicates.push_back(i);
        }
    }
    return duplicates;
}

void GeneticAlgorithm::RemoveDuplicates() {
    auto dup = Duplicates();
    for (auto& i : dup) {
        m_CurrentGeneration.at(i) = Chromosome(m_Instance, Chromosome::GenRandomChromosome(m_Instance.GetTasksCount()));
    }
}

int GeneticAlgorithm::Run(bool timeLimit){
    std::random_device RandomDevice{};
    std::mt19937 Generator{ RandomDevice() };
    std::uniform_real_distribution<> distr(0, 1);

    auto start = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point isEnd;
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(isEnd - start).count();
    HistoricalBest.clear();
    
    for(int gen = 0; gen < m_MaxGenNumber; gen++){
        Progress = (float)gen / m_MaxGenNumber;
        if(timeLimit){
            duration = std::chrono::duration_cast<std::chrono::milliseconds>(isEnd - start).count();
            if( duration > 5LL*60*1000){
                break;
            }
        }
        isEnd = std::chrono::steady_clock::now();
#ifndef NO_PRINT
        std::cout << "\b\b\b\b\b\b\b\b\b";
        std::cout.width(4);
        std::cout << gen;
#endif // !NO_PRINT
#if 0
        if (gen % int(m_MaxGenNumber/20) == 0 || gen > m_MaxGenNumber * (1.0 - 0.005) || gen < m_MaxGenNumber * 0.005) {
            std::sort(std::execution::par_unseq, m_CurrentGeneration.begin(), m_CurrentGeneration.end());
            std::copy(std::execution::par_unseq, m_CurrentGeneration.begin(), m_CurrentGeneration.begin()+m_CurrentGeneration.size()/2, m_CurrentGeneration.begin()+m_CurrentGeneration.size()/2);
        }
#else
        std::sort(std::execution::par_unseq, m_CurrentGeneration.begin(), m_CurrentGeneration.end());
        std::copy(std::execution::par_unseq, m_CurrentGeneration.begin(), m_CurrentGeneration.begin() + m_CurrentGeneration.size() / 2, m_CurrentGeneration.begin() + m_CurrentGeneration.size() / 2);

#endif
        if (m_CurrentGeneration[0].CalculateFitness() < m_BestChromosome.CalculateFitness()) {
            m_BestChromosome = m_CurrentGeneration[0];
            m_BestChromosomeGenIndex = gen;
        }
        
        for(auto it = m_CurrentGeneration.begin(); it <  m_CurrentGeneration.begin()+m_CurrentGeneration.size()/2; it+=2){
            //crossover
            it->CrossOver(*(it + 1), m_CrossoverProb);
            //mutation           
            if(distr(Generator) <= m_MutationProb){
                it->Mutate(*(it+1));
            }   
            
        }
        RemoveDuplicates();
        HistoricalBest.push_back(m_CurrentGeneration[0].GetCashedFitness());

    }
#ifndef NO_PRINT
    std::cout << std::endl << "Duration : " << duration / (60.0 * 1000.0)  << " minutes" << std::endl << "Gen index of best score: " << m_BestChromosomeGenIndex << std::endl;
#endif
    std::sort(m_CurrentGeneration.begin(), m_CurrentGeneration.end());
    m_BestChromosome = m_CurrentGeneration[0].GetCashedFitness() < m_BestChromosome.GetCashedFitness() ? m_CurrentGeneration[0] : m_BestChromosome;
    return m_BestChromosome.CalculateFitness();
}

void GeneticAlgorithm::PrintBestChromosome(){
    std::cout << "Best ";
    m_BestChromosome.PrintGenes();
    std::cout << std::endl;
}

void GeneticAlgorithm::RunInAnotherThread(bool timeLimit)
{
    m_Value = std::async(&GeneticAlgorithm::Run, this, timeLimit);
}

std::optional<int> GeneticAlgorithm::GetValue()
{
    using namespace std::chrono_literals;
    std::optional<int> val;
    if (m_Value.wait_for(1ms) == std::future_status::ready) {
        val = m_Value.get();
    }
    return val;
}
