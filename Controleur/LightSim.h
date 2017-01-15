#ifndef LIGHTSIM_H
#define LIGHTSIM_H
#include <iostream>
#include <list>
#include "../Modele/Agents/Agent.h"

class LightSim
{
    public:
        LightSim(int x, int y,int nbPredators,int nbPreys);
        virtual ~LightSim();
        bool run(int );



    protected:

    private:
        int tick;
        Environment* env;

};
#endif // LIGHTSIM_H
