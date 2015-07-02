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

//evaluates fitness function script
double Genova::scriptScore(Genome g)
{
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
        for(int j=0; j<populationSize; j++)
        {
            //copy if elite
            if(j>populationSize - eliterange)
                populationB.replace(j, populationA.at(j));
            //selection and crossover
            else
            {
                populationB.replace(j, crossover(selection(populationA, populationScore), selection(populationA, populationScore)));
                mutate(populationB[j]);
                if(fileName=="")
                    populationB[j].fitnessScore = score(populationB[j]);

                else
                    populationB[j].fitnessScore = scriptScore(populationB[j]);

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


//---------------  Selection  --------------------------------------------------------------------------------
Genome Genova::selection(QVector<Genome>& population, int populationScore) const
{
    switch(selectionType)
    {
        case "Random": return Genova::randomSelection(population, populationScore); break;
        case "Roulette": return Genova::rouletteSelection(population, populationScore); break;
        case "RoulettePopulationAverage": return Genova::roulettePopulationAvgSelection(population, populationScore); break;
        case "RankLinear": return Genova::rankLinearSelection(population, populationScore); break;
        case "StochasticUniversal": return Genova::stochasticUniversalSelection(population, populationScore); break;
        case "Tournament": return Genova::tournamentSelection(population, kway, tourneyprob); break;
    default: return Genova::randomSelection(population, populationScore);
    }
}

Genome randomSelection(QVector<Genome>& population, int populationScore) const
{
    return population.at(qrand() % population.size());
}

Genome rouletteSelection(QVector<Genome>& population, int populationScore) const
{
    int idx;
    int rouletteValue = qrand() % populationScore;
    if(rouletteValue <= population.at(0).fitnessScore)
        idx = 0;
    else
        idx = roulette.indexOf(*std::upper_bound(roulette.begin(), roulette.end(), rouletteValue));
    return population.at(idx);
}

//TODO implement
Genome roulettePopulationAvgSelection(QVector<Genome>& population, int populationScore) const
{
    return population.at(qrand() % population.size());
}

Genome rankLinearSelection(QVector<Genome>& population, int populationScore) const
{
    int idx;
    int rouletteValue = qrand() % population.size();
    if(rouletteValue == 0)
        idx = 0;
    else
       idx = roulette.indexOf(*std::upper_bound(roulette.begin(), roulette.end(), rouletteValue));
   return population.at(idx);
}

Genome stochasticUniversalSelection(QVector<Genome>& population, int populationScore) const
{
    int idx;
    const int jumpsize = populationScore / population.size();
    static int jumpcounter = 0;
    if(jumpcounter = std::numeric_limits<int>::max()) //prevent overflow
        jumpcounter = 0;
    int rouletteValue = (jumpsize * jumpcounter) % populationScore;
    if(rouletteValue == 0)
        idx = 0;
    else
       idx = roulette.indexOf(*std::upper_bound(roulette.begin(), roulette.end(), rouletteValue));
    jumpcounter++;
   return population.at(idx);
}

//kway should always be even (set on GUI)
Genome Genova::tournamentSelection(QVector<Genome>& population, int kway, double probability) const
{
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

//-------------------------------------------------------------------------------------------------------------

//-----------------------------------------  Roulette  -------------------------------------------------------


void Genova::updateRoulette(QVector<Genome> &population)
{
    //does RankLinear need a different roulette design?
    if(selectionType=="Roulette" || selectionType=="StochasticUniversal" || selectionType=="RankLinear")
        _updateRoulette1(population);
    if(selectionType=="RoulettePopulationAverage")
         _updateRoulette2(population);
}

//for basic, stochasticU, (and rankLinear?)
void _updateRoulette1(QVector<Genome>& population)
{
    if(selectionType=="Roulette" || selectionType=="StochasticUniversal" || selectionType=="RankLinear")
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
}

//for RoulettePopulationAverage
void _updateRoulette2(QVector<Genome>& population)
{
    roulette.clear();
    double r = 0;
    int population_avg = 0;
    for(int i=0; i<population.size(); i++)
        population_avg += population.at(i).fitnessScore;
    population_avg /= population.size();

    //each element gets r whole copies in roulette + an additional copy with probability of the latter being (r mantissa)
    for(int i=0; i<population.size(); i++)
    {
        r = population.at(i).fitnessScore / population_avg;
        for(int j=0; j<floor(r); j++)
            roulette.append(i);
        r = (r - floor(r)) * 100; //get mantisaa as percentage
        if(qrand() % 100 <= r) roulette.append(i);
    }

}







