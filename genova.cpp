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
    bool isGenomeLengthFixed, bool isChromosomeLengthFixed, bool isPopulationFixed, QString fileName,
    QString selectionType, QString crossoverType, int kway, double tourneyprob)
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
    this->selectionType = selectionType;
    this->crossoverType = crossoverType;
    this->kway = kway;
    this->tourneyprob = tourneyprob;

    mutationRange = this->mutationPercentage * RAND_MAX;

    populationA.fill(Genome(genomeLength), populationSize);
    populationB.fill(Genome(genomeLength), populationSize);

    roulette.reserve(populationSize);

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
  //single-point crossover producing one child
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

//default fitness function
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
    int populationScore = 0; //sum of all scores in population

    //init and score 1st generation
    populate();
    for(int i=0; i<populationA.size(); i++)
        if(fileName=="") //assign default scoring function
        {
            populationA[i].fitnessScore = score(populationA[i]);
            populationScore += populationA[i].fitnessScore;
        }
        else
        {
            populationA[i].fitnessScore = scriptScore(populationA[i]);
            populationScore += populationA[i].fitnessScore;
        }
    if(selectionType=="Roulette") updateRoulette(populationA);

    std::sort(populationA.begin(), populationA.end());

    //v1.0: just run specified number of generations
    for(int i=0; i<maxGenerations; i++)
    {
        //currently selection for parent A is in order and parent B is random unless parent A is elite
        //TODO add selection method with both parents are selected the same way
        for(int j=0; j<populationSize; j++)
        {
            //copy if elite
            if(j>populationSize - eliterange)
                populationB.replace(j, populationA.at(j));
            //selection and crossover
            else
            {
              //  populationB.replace(j, crossover(populationA.at(j), populationA.at(qrand() % populationSize))); //original
                populationB.replace(j, crossover(selection(populationA, populationScore), selection(populationA, populationScore)));
                mutate(populationB[j]);
                if(fileName=="")
                {
                    populationB[j].fitnessScore = score(populationB[j]);
                   // populationScore += populationB[j].fitnessScore;
                }
                else
                {
                    populationB[j].fitnessScore = scriptScore(populationB[j]);
                   // populationScore += populationB[j].fitnessScore;
                }
            }
       }
        populationScore = 0;

        for(int i=0; i<populationB.size(); i++)
            populationScore =+ populationB.at(i).fitnessScore;
        populationA = populationB;

        std::sort(populationA.begin(), populationA.end());
        if(selectionType=="Roulette") updateRoulette(populationA);
    }
   emit sendReport(report());
}

//selection
Genome Genova::selection(QVector<Genome>& population, int populationScore) const
{
   // QMessageBox::information(0,"error", QString::number(population.size()));
    //TODO get rid of "magic value" strings
    if(selectionType == "Random")
        return population.at(qrand() % population.size());

    if(selectionType == "Roulette")
    {
        int idx;
        int rouletteValue = qrand() % populationScore;
       //  QMessageBox::information(0,"error", QString::number(population.size()));
        if(rouletteValue <= population.at(0).fitnessScore)
            idx = 0;
        else
            idx = roulette.indexOf(*std::upper_bound(roulette.begin(), roulette.end(), rouletteValue));
        return population.at(idx);
    }

    if(selectionType == "Tournament")
        return Genova::tournamentSelection(population, kway, tourneyprob);
}

void Genova::updateRoulette(QVector<Genome> &population)
{
    int runningTotal = 0;

    if(roulette.size() < population.size())
    {
            roulette.reserve(population.size());
            for(int i=0; i<population.size(); i++)
                roulette.push_back(0);;
    }

    for(int i=0; i<population.size(); i++)
    {
        runningTotal += population.at(i).fitnessScore;
        roulette[i] += runningTotal;
    }
}

//kway should always be even (set on GUI)
Genome Genova::tournamentSelection(QVector<Genome>& population, int kway, double probability) const
{
    //QList<int> competitors;
    //QList<double> competitorScores;

    QList<Genome> competitors;
    double chance;

    //select candidates
    for(int i=0; i<kway; i++)
        competitors.append(qrand() % population.size());

    std::sort(competitors.begin(), competitors.end());

    //compete
    for(int i=0; i<kway; i++)
    {
        chance = (qrand() % 100)/100; //normalize qrand() to range 0-1
        if(chance <= (probability*pow((1-probability),i))) return competitors.at(i);
    }

    //if no winner from above code, return weakest
    return competitors.at(0);
}

/*selection implementations
 *
 * Roulette (fitness proportionate) implementation:  pool is array with size of population, and values of running total.
 * Index of first item bigger than rolled value is index of selected individual in population.
 * Use binary search std::binary_search on a list
 *
 * If more than one genome assigned to same percentage value,
 * pick one randomly.
 *
 * Population Average roulette: take ratio r =(individual score)/(population average score)
 * Build static selection pool that gets updated each generation.
 * Each individual gets copies in the pool as follows:
 * a number of copies equal to the real part of r
 * a chance of an additional copy equal to the mantissa of r
 * So, an individual with a relative score of 2.3 gets 2 copies and a 30% chance of an additional one
 *
 * Rank-Based Roulette: individual's probability of selection is proportional to its rank.
 * Not influenced by super individuals or spreading fitness like standard (proportional) roulette wheel
 * can avoid premature convergence. Usually better than tourney for big problems
 *
 * Tournament selection: also requires a mating pool. Select k individuals at random.
 * Pick the best with probablility p
 * second best with probaility p*(1-p)
 * third best p = p*((1-p)^2)
 * Deterministic tournament selection selects the best individual (when p = 1) in any tournament.
 *  A 1-way tournament (k = 1) selection is equivalent to random selection.
 * (info from wikipedia)
 * Low susceptinbility to takeover, no need to sort or scale fitness. Simple and efficient.
 * Usually good for small problems
 *
 * */



