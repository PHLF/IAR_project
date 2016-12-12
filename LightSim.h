#ifndef LIGHTSIM_H
#define LIGHTSIM_H
#include <iostream>
#include <list>
#include "Agent.h"

class LightSim
{
    public:
        LightSim(int ,int );
        virtual ~LightSim();


    protected:

    private:
        int tick;
        std::list<Agent*> predatorsList;
        std::list<Agent*> preysList;
};

#endif // LIGHTSIM_H
