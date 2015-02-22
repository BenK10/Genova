#ifndef GENOVA_H
#define GENOVA_H

#include "genome.h"
#include <QString>
#include <QObject>
#include <QScriptEngine>
#include <QScriptProgram>
#include <QFile>

 class Genova : public QObject
{
     Q_OBJECT
private:
    int genomeLength;
    int chromosomeLength;
    int populationSize;
    int maxGenerations;

    double elitismPercentage;
    double mutationPercentage;
    double mutationRange;
    double resurrectionPercentage;
    double cullingThreshold;

    bool isGenomeLengthFixed;
    bool isChromosomeLengthFixed;
    bool isPopulationFixed;

    QVector<Genome> populationA;
    QVector<Genome> populationB;

    QScriptEngine scriptengine;
    QScriptProgram script;
    //QFile file;
    QString fileName;
    QString scriptString;

public:
//     Genova();
//    ~Genova();

    void initialize(int, int, int, int, double, double, double, double, bool, bool, bool, QString);
    void populate();
    Genome crossover(Genome, Genome);
    void mutate(Genome);
    void splice();
    double score(Genome);
    double scriptScore(Genome g);
    QString report();
    void run();

signals:
    void sendReport(QString);
};

#endif // GENOVA_H
