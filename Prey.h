#ifndef PREY_H
#define PREY_H
#include <vector>
#include "Agent.h"
#include "Retina.h"


class Prey : public Agent
{
    public:
        Prey();
        virtual ~Prey();

    protected:

    private:
        std::vector<bool> mnPredators;
};

#endif // PREY_H
