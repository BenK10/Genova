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

//how to select/define a score function from GUI? Plugins?
//default scoring: the number of 1 bits, no penalty
//void Genome::score()
//{
//    double score = 0;
//    for(int i=0; i<genes.size(); i++)
//        if(genes.testBit(i))
//            score++;

//    this->fitnessScore = score;
//}
