#include "Agent.h"

Agent::Agent()
{
    //ctor
    observationPredators = std::vector<bool>(retina.getNbSegments(),false);
    observationPreys = std::vector<bool>(retina.getNbSegments(),false);

}

Agent::~Agent()
{
    //dtor
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
