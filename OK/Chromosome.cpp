#include "Chromosome.hpp"
#include <iostream>
#include <algorithm>
#include <numeric>
#include "Instance.hpp"

std::random_device Chromosome::m_RandomDevice{};
std::mt19937 Chromosome::m_Generator{ Chromosome::m_RandomDevice()};

Chromosome::Chromosome(Instance& instance)
	:m_Instance(std::make_shared<Instance>(instance)), m_Hash(0), m_CachedFitness(INT_MAX)
{
	m_Genes.resize(m_Instance->GetTasksCount());
	std::iota(m_Genes.begin(), m_Genes.end(), 0);
	m_FitnessProcessors.resize(m_Instance->GetProcessorCount());
}

Chromosome::Chromosome(Instance& instance, std::vector<int> Genes)
	:m_Genes(Genes), m_Instance(std::make_shared<Instance>(instance)), m_CachedFitness(INT_MAX)
{
	m_FitnessProcessors.resize(m_Instance->GetProcessorCount());
	UpdateHash();
}

void Chromosome::Mutate(Chromosome& other)
{
	std::uniform_int_distribution<> distr(0, m_Instance->GetTasksCount()-1);
	int f = distr(m_Generator);
	int s = distr(m_Generator);
	int tmp = m_Genes[f]; 
	m_Genes[f] = other.m_Genes[s];
	other.m_Genes[s] = tmp;
	ClearDuplicates(other);
	UpdateHash();
	other.UpdateHash();
	m_CachedFitness = CalculateFitness();
	other.m_CachedFitness = other.CalculateFitness();
}

std::vector<std::pair<int, int>> Chromosome::Duplicates() {
	std::vector<std::pair<int, int>> duplicates;	
	const int chromosomeSize = m_Genes.size();
	std::vector<int> genesCopy(chromosomeSize);
	for (int i = 0; i < chromosomeSize; i++) {
		if (genesCopy.at(m_Genes.at(i))++ != 0) {
			duplicates.push_back({i, m_Genes.at(i)});
		}
	}
	return duplicates;
}

void Chromosome::ClearDuplicates(Chromosome& other)
{
	auto print_duplicates = [](std::vector<std::pair<int, int>> x){
		std::cout<<"Duplikaty:\n";
		for(auto& p : x) {
			std::cout << "\tindex: " << p.first << " value: " << p.second << std::endl;
		}
		std::cout<< std::endl;
	};
	
	auto my_duplicates = Duplicates();
	auto other_duplicates = other.Duplicates();

	//print_duplicates(my_duplicates);
	//print_duplicates(other_duplicates);

	if(my_duplicates.size()>0){
		for(int i = 0; i < my_duplicates.size(); i++){
			m_Genes.at(my_duplicates[i].first) = other_duplicates[i].second;
			other.m_Genes.at(other_duplicates[i].first) = my_duplicates[i].second;
		}
	}


}

int Chromosome::CalculateFitness()
{
	std::fill(m_FitnessProcessors.begin(), m_FitnessProcessors.end(), 0);

    for(auto& i : m_Genes) {
		int smallest = INT_MAX;
        int smallest_idx = -1;
        for(int proc = 0; proc < m_FitnessProcessors.size(); ++proc){
            if(m_FitnessProcessors.at(proc) < smallest){
                smallest = m_FitnessProcessors.at(proc);
                smallest_idx = proc;
            }
        }
        m_FitnessProcessors.at(smallest_idx) += (*(m_Instance->beginIter()+i));
    }

    int biggest = -1;

    for(int i = 0; i < m_FitnessProcessors.size(); ++i){
        if(m_FitnessProcessors.at(i) > biggest){
            biggest = m_FitnessProcessors.at(i);
        }
    }

    return biggest;
}

std::vector<int> Chromosome::GenRandomChromosome(int ChromosomeSize)
{
	std::vector<int> genes(ChromosomeSize);
	std::iota(genes.begin(), genes.end(), 0);
	std::shuffle(genes.begin(), genes.end(), m_Generator);
	return genes;
}

void Chromosome::PrintGenes(){
	auto copy = std::vector<int>(m_Genes);
	//std::sort(copy.begin(), copy.end());
	std::cout << "Hash: " << m_Hash << " Genes: ";
	for(int i = 0; i < copy.size(); i++){
		std::cout << copy[i] << " ";	
	}
	std::cout << std::endl;
}

void Chromosome::CrossOver(Chromosome& other, float percentage){
	int pivot = (m_Genes.size() + 1 ) * percentage;

	for(int i = pivot; i < m_Genes.size(); i++) {
		int tmp = m_Genes[i];
		m_Genes[i] = other.m_Genes[i];
		other.m_Genes[i] = tmp;
	}
	ClearDuplicates(other);
	UpdateHash();
	other.UpdateHash();
	m_CachedFitness = CalculateFitness();
	other.m_CachedFitness = other.CalculateFitness();
}

bool Chromosome::operator < (const Chromosome& rhs) {
	return m_CachedFitness < rhs.m_CachedFitness;
}

