#ifndef GENE_H
#define GENE_H

#include <QVector>
#include <QString>
#include <QBitArray>

class Genome
{
private:
    double fitnessScore;
    QBitArray genes;
   // QString genes;
public:
    Genome();
   // ~Genome();

    Genome(int);

    bool operator<(Genome other) const;

    //void score();

    friend class Genova;
};

#endif // GENE_H
