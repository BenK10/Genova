#include "genova.h"
#include <algorithm>
#include <QTextStream>
#include <QMessageBox>

//Genova::Genova(QMainWindow *ui)
//{
//    this->ui = ui;
//}

//Genova::~Genova()
//{
//    delete ui;
//}



void Genova::initialize(int genomeLength, int chromosomeLength, int populationSize, int maxGenerations,
    double elitismPercentage, double mutationPercentage, double resurrectionPercentage, double cullingThreshold,
    bool isGenomeLengthFixed, bool isChromosomeLengthFixed, bool isPopulationFixed, QString fileName)
{
    this->genomeLength = genomeLength,
    this->chromosomeLength = chromosomeLength;
    this->populationSize = populationSize;
    this->maxGenerations = maxGenerations;
    this->elitismPercentage = elitismPercentage;
    this->mutationPercentage = mutationPercentage;
    this->resurrectionPercentage = resurrectionPercentage;
    this->cullingThreshold = cullingThreshold;
    this->isGenomeLengthFixed = isGenomeLengthFixed;
    this->isChromosomeLengthFixed = isChromosomeLengthFixed;
    this->isPopulationFixed = isPopulationFixed;
    this->fileName = fileName;

    mutationRange = this->mutationPercentage * RAND_MAX;

    populationA.fill(Genome(genomeLength), populationSize);
    populationB.fill(Genome(genomeLength), populationSize);

    QFile file =(fileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) QMessageBox::information(0,"error",file.errorString());
    QTextStream in(&file);
    this->scriptString = in.readAll();
    file.close();

    this->script = QScriptProgram(scriptString);
}

void Genova::populate()
{
   for(int i=0; i<populationA.size(); i++)
    {
        for(int j=0; j<populationA.at(i).genes.size(); j++)
            if(qrand()%2 == 0)
            populationA[i].genes.clearBit(j);
            else
            populationA[i].genes.setBit(j);
    }
}

Genome Genova::crossover(Genome parentA, Genome parentB)
{
  //single-point crossover with one child
  int crosspoint = qrand() % genomeLength;
  Genome child(genomeLength);

  for(int i=0; i<crosspoint; i++)
      if(parentA.genes.testBit(i))
          child.genes.setBit(i);

  for(int i=crosspoint; i<genomeLength; i++)
        if(parentB.genes.testBit(i))
            child.genes.setBit(i);

  return child;
}

//flips one random bit in mutant
void Genova::mutate(Genome g)
{
    if(qrand() < mutationRange)
    {
        g.genes.toggleBit(qrand()%g.genes.size());
    }
    else return;

}

//not implemented in v1.0
void Genova::splice()
{

}

double Genova::score(Genome g)
{
    double score = 0;
    for(int i=0; i<g.genes.size(); i++)
        if(g.genes.testBit(i))
            score++;

    return score;
}

double Genova::scriptScore(Genome g)
{
   // return scriptengine.evaluate(script).toNumber();
    //test
    scriptengine.evaluate(scriptString);
    QScriptValue global = scriptengine.globalObject();
    QScriptValue fitness = global.property("fitness");
    QScriptValueList args;

    args << g.genes.size();
    for(int i=0; i<g.genes.size(); i++)
        args << g.genes.at(i);

    return fitness.call(QScriptValue(), args).toNumber();
}

QString Genova::report()
{
    QString result = "top scoring genome: ";
    for(int i=0; i<genomeLength; i++)
    {
        result.append(QString::number(populationA.back().genes.at(i)));
        result.append(" ");
    }
    result.append("score: ");
    result.append(QString::number(populationA.back().fitnessScore));

    return result;
}

void Genova::run()
{
    int eliterange = populationSize * elitismPercentage;

    populate();
    for(int i=0; i<populationA.size(); i++)
        populationA[i].fitnessScore = scriptScore(populationA[i]);

    std::sort(populationA.begin(), populationA.end());

    //v1.0: just run specified number of generations
    for(int i=0; i<maxGenerations; i++)
    {
        //currently selection for parent A is in order and parent B is random unless parent A is elite
        for(int j=0; j<populationSize; j++)
        {
            if(j>populationSize - eliterange)
                populationB.replace(j, populationA.at(j));
            else
            {
            populationB.replace(j, crossover(populationA.at(j), populationA.at(qrand() % genomeLength)));
            mutate(populationB[j]);
            populationB[j].fitnessScore = scriptScore(populationB[j]);
            }
        }

        populationA = populationB;
        std::sort(populationA.begin(), populationA.end());
    }
   emit sendReport(report());
}



