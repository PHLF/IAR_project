#ifndef AGENT_H
#define AGENT_H
#include <vector>
#include "Retina.h"
#include "../Environnement/utils.h"

class Agent
{
    public:
        Agent();
        virtual ~Agent();
        virtual void moveForward();
        virtual void turnLeft();
        virtual void turnRight();
        virtual void observe();
        virtual bool run();


    protected:
        float speed;
        float orientation; //Angle in °
        coord_s coord;
        Retina retina = Retina(10,180);
        std::vector<bool> observationPredators;
        std::vector<bool> observationPreys;

    private:


};

#endif // AGENT_H
