#ifndef AGENT_H
#define AGENT_H
#include <vector>

#include "Retina.h"
#include "../Environnement/utils.h"

class Environment;
class Retina;

class Agent
{
    public:
        Agent(Environment* env,int speed = 1,int turnSpeed = 8,int orientation = 0,float viewDepth = 100,float fov = 180);
        virtual ~Agent();
        virtual void moveForward();
        virtual void turnLeft();
        virtual void turnRight();
        virtual void observe();
        virtual bool run();


    protected:
        Environment* env;
        int speed;
        int turnSpeed;
        int orientation; //Angle in °
        coord_s coord;
        Retina* retina;
        std::vector<bool> observationPredators;
        std::vector<bool> observationPreys;

    private:


};

#endif // AGENT_H
