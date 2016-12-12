#ifndef AGENT_H
#define AGENT_H
#include <vector>
#include "Retina.h"
class Agent
{
    public:
        Agent();
        virtual ~Agent();
        virtual void moveForward();
        virtual void turnLeft();
        virtual void turnRight();


    protected:
        float speed;
        float orientation; //Angle in °
        Retina retina = Retina(10,180);
        std::vector<bool> mnPreys;

    private:


};

#endif // AGENT_H
