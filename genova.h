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
    int kway;

    double tourneyprob;
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

    QList<double> roulette;

    QScriptEngine scriptengine;
    QScriptProgram script;
    //QFile file;
    QString fileName;
    QString scriptString;
    QString selectionType;
    QString crossoverType;

public:
//     Genova();
//    ~Genova();

    void initialize(int, int, int, int, double, double, double, double, bool, bool, bool, QString, QString, QString, int, double);
    void populate();
    Genome tournamentSelection(QVector<Genome>&, int, double) const;
    Genome selection(QVector<Genome>&, int) const;
    void updateRoulette(QVector<Genome> &);
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
