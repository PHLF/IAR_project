#include "Prey.h"
#include<iostream>
#include<vector>
Prey::Prey()
{

    mnPredators = std::vector<bool>(retina.getNbSegments(),false);

}

Prey::~Prey()
{

}
