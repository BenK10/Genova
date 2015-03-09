#include "genome.h"

 Genome::Genome()
{
    fitnessScore = 0.0;
    Genome::genes = QBitArray();
}

// Genome::~Genome()
//{}

Genome::Genome(int size)
{
    fitnessScore = 0.0;
    Genome::genes = QBitArray(size);
}

bool Genome::operator<(Genome other) const
{
    return (this->fitnessScore < other.fitnessScore);
}

