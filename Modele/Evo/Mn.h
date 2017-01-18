#ifndef MN_H
#define MN_H

#include <Divers/Globals.h>
#include <Divers/Utils.h>
#include <fstream>

using namespace std;

class Mn
{
private:
    uint32_t _nb_actions;
    uint32_t _nb_sensors;
    //vector<vector<vector<vector<float>>>> _mn; //Très lourd

    //float**** _mn; // Methode 1
    vector<vector<vector<vector<float>>>> _mn;

    //vector<float*>* _mn; //Methode 2 : Moins pratique à utiliser
    //vector<vector<vector<float*>*>*>* _mn; //Methode 3 marche pas

public:
    Mn(uint32_t nbAct, uint32_t nbSens);
    ~Mn();
    void random_fill();
    void print_tirages();
    uint32_t choose_action(uint32_t old_action,vector<bool>& sensor1,vector<bool>& sensor2);
    void print_p_actions(uint32_t old_action,vector<bool>& sensor1,vector<bool>& sensor2);
    void print_p_cum_actions(uint32_t old_action,vector<bool>& sensor1,vector<bool>& sensor2);
    vector<vector<vector<vector<float>>>> get_mn(){return _mn;};
    void save_as_file(string id);
    void load_file(string id);
    void gaussian_random_mutation(float alpha);
};

#endif // MN_H
