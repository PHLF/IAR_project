#include <iostream>
#include <list>
#include <algorithm>

#include "LightSim.h"
#include "Predator.h"
#include "Prey.h"

LightSim::LightSim(int nbPredators,int nbPreys)
{

    for(int i = 0;i < nbPredators;i++){
        predatorsList.push_back(new Predator());
    }
    for(int i = 0;i < nbPreys;i++){
        preysList.push_back(new Prey());
    }

    tick = 0;
}

LightSim::~LightSim()
{

    for(std::list<Agent*>::iterator i=predatorsList.begin();i!=predatorsList.end();predatorsList.erase(i++)){delete *i;}
    for(std::list<Agent*>::iterator i=preysList.begin();i!=preysList.end();preysList.erase(i++)){delete *i;}

}

bool LightSim::run(int nbTicks)
{
    int cptTicks = 0;

    for(int cptTicks = 0;cptTicks<nbTicks;cptTicks++){
        std::cout << "Tick n°" << cptTicks << std::endl;

    }

    return true;
}

std::ostream& operator<<(std::ostream &strm, const LightSim &a) { //Affichage de l'environnement sous forme de grille pour débug


  //return strm << "A(" << a.i << ")" << std::endl;
  return strm;
}
