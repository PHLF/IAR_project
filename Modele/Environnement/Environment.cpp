#include "Environment.h"
#include "../Agents/Predator.h"
#include "../Agents/Prey.h"

Environment::Environment(uint32_t x,uint32_t y,uint32_t nbPredators,uint32_t nbPreys)
{
    //ctor
    sizex = x;
    sizey = y;

    /*for (uint32_t i=0; i < sizex; i++){
        grid[i] = new Agent[ sizey ];
    }*/

    for(uint32_t i = 0;i < nbPredators;i++){
        predatorsList.push_back(new Predator(this));
    }
    for(uint32_t i = 0;i < nbPreys;i++){
        preysList.push_back(new Prey(this));
    }
}

Environment::~Environment()
{
    //dtor
    for(std::list<Agent*>::iterator i=predatorsList.begin();i!=predatorsList.end();predatorsList.erase(i++)){delete *i;}
    for(std::list<Agent*>::iterator i=preysList.begin();i!=preysList.end();preysList.erase(i++)){delete *i;}
}
