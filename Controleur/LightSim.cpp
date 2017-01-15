#include <iostream>
#include <list>
#include <algorithm>

#include "LightSim.h"
#include "../Modele/Agents/Predator.h"
#include "../Modele/Agents/Prey.h"

LightSim::LightSim(int x,int y,int nbPredators,int nbPreys)
{
    //grid = new Agent* [sizex];

    env = new Environment(x,y,nbPredators,nbPreys);

    tick = 0;
}

LightSim::~LightSim()
{

    delete env;

    /*for (int i=0; i < sizex; i++){
        grid[i] = new Agent[ sizey ];
    }*/
}

bool LightSim::run(int nbTicks)
{

    for(tick = 0;tick<nbTicks;tick++){
        std::cout << "Tick n°" << tick << std::endl;

    }

    return true;
}

std::ostream& operator<<(std::ostream &strm, const LightSim &a) { //Affichage de l'environnement sous forme de grille pour débug


  //return strm << "A(" << a.i << ")" << std::endl;
  return strm;
}
