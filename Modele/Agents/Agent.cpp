#include "Agent.h"



Agent::Agent(Environment* env,int speed, int turnSpeed,int orientation, float viewDepth,float fov)
{
    //ctor
    this->env = env;
    retina = new Retina(viewDepth,fov);
    observationPredators = std::vector<bool>(retina->getNbSegments(),false);
    observationPreys = std::vector<bool>(retina->getNbSegments(),false);

}

Agent::~Agent()
{
    //dtor
    delete retina;
}

void Agent::moveForward()
{

}

void Agent::turnLeft()
{

}

void Agent::turnRight()
{

}

void Agent::observe()
{

}

bool Agent::run()
{
    bool exec = false;


    return exec;

}
