#ifndef MN_H
#define MN_H

#include <Divers/Globals.h>

using namespace std;

class Mn
{
private:
    uint32_t _nb_actions;
    uint32_t _nb_sensors;
    //vector<vector<vector<vector<float>>>> _mn;
    float**** _mn;

public:
    Mn(uint32_t nbAct, uint32_t nbSens);
    ~Mn();
    bool loadGenomeFile(std::string name);
    void random_fill();
    void print_tirages();

};

#endif // MN_H
