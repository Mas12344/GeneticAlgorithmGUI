#pragma once
#ifndef CHROMOSOME_H
#define CHROMOSOME_H
#include <vector>
#include <random>
#include <memory>
#include <zlib.h>

class Instance;

class Chromosome {
public:
	Chromosome(Instance& instance);
	Chromosome(Instance& instance, std::vector<int> Genes);
	void Mutate(Chromosome& other);
	void CrossOver(Chromosome& other, float percentage);
	void ClearDuplicates(Chromosome& other);
	int CalculateFitness();
	static std::vector<int> GenRandomChromosome(int ChromosomeSize);
	void PrintGenes();
	bool operator < (const Chromosome& rhs);
	inline unsigned long GetHash() { return m_Hash;	}
private:
	unsigned long m_Hash;
	int m_CachedFitness;
	std::vector<int> m_Genes;
	static std::random_device m_RandomDevice;
	static std::mt19937 m_Generator;
	std::shared_ptr<Instance> m_Instance;
	std::vector<int> m_FitnessProcessors;
	std::vector<std::pair<int, int>> Duplicates();
	inline void UpdateHash() {
		unsigned long  crc = crc32(0L, 0, 0);
		m_Hash = crc32(crc, (const unsigned char*)m_Genes.data(), m_Genes.size()*(sizeof(int) / sizeof(unsigned char)));
	}
};

#endif // !CHROMOSOME_H
