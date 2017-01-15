#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <iostream>
#include <list>
#include <algorithm>
#include <random>

#include "../Agents/Agent.h"

class Agent;

class Environment
{
    public:
        Environment(int x,int y,int nbPredators,int nbPreys);
        virtual ~Environment();
        void moveForward();
        void turnLeft();
        void turnRight();
        void observe();
        //coord_s randomCoord();

    protected:

    private:
        int sizex;
        int sizey;
        std::list<Agent*> predatorsList;
        std::list<Agent*> preysList;
};
#endif // ENVIRONMENT_H
