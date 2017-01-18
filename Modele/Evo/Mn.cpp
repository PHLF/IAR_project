#include "Mn.h"

Mn::Mn(uint32_t nbAct, uint32_t nbSens):_nb_actions(nbAct),_nb_sensors(nbSens)
{

    uint32_t nb_combi_sensors = pow(2,_nb_sensors);
    //Compromis mémoire utilisabilité sympa, reste assez lourd 2.5Go par instance. Pas vraiment du c++ btw.

    //uint32_t debugNbCases = 0;

    //Methode1
    _mn = new float***[_nb_actions];
    for(uint32_t i = 0; i < _nb_actions;i++){
        _mn[i] = new float**[nb_combi_sensors];
        for(uint32_t j = 0; j < nb_combi_sensors;j++){
            _mn[i][j] = new float*[nb_combi_sensors];
            for(uint32_t k = 0; k < nb_combi_sensors;k++){
                _mn[i][j][k] = new float[_nb_actions];

            }
        }
    }

    //Methode 2
    /*_mn = new vector<float*>(_nb_actions*nb_combi_sensors*nb_combi_sensors,new float[_nb_actions]);
    cout << "size : "<< _mn->size() << endl;
    (*_mn)[10][3] = 0.2;
    cout << (*_mn)[10][3] << endl;*/

    //Methode 3 marche pas
    /*_mn = new vector<vector<vector<float*>*>*>(
                _nb_actions,new vector<vector<float*>*>(
                    nb_combi_sensors,new vector<float*>(
                        nb_combi_sensors,new float[_nb_actions])));
    (*_mn)[2][2][2][2] = 0.2;

    cout << (*_mn)[2][2][2][2] << endl;*/
}

Mn::~Mn(){

    uint32_t nb_combi_sensors = pow(2,_nb_sensors);
    for(uint32_t i = 0; i < _nb_actions;i++){
        for(uint32_t j = 0; j < nb_combi_sensors;j++){
            for(uint32_t k = 0; k < nb_combi_sensors;k++){
                delete[] _mn[i][j][k];

            }
            delete[] _mn[i][j];
        }
        delete[] _mn[i];
    }
    delete[] _mn;
}

void Mn::random_fill(){
    uint32_t nb_combi_sensors = pow(2,_nb_sensors);

    using namespace std::chrono;
    std::default_random_engine gen;
    gen.seed(system_clock::now().time_since_epoch().count());


    for(uint32_t i = 0; i < _nb_actions;i++){
        for(uint32_t j = 0; j < nb_combi_sensors;j++){
            for(uint32_t k = 0; k < nb_combi_sensors;k++){
                    float total = 0.0;
                    float tirage = 0.0;
                    for(uint32_t l = 0; l < _nb_actions;l++){
                        std::uniform_real_distribution<double> distrib_norm(0.0, 1.0);
                        tirage = distrib_norm(gen);
                        total += tirage;
                        _mn[i][j][k][l] = tirage;
                    }
                    for(uint32_t l = 0; l < _nb_actions;l++){
                        _mn[i][j][k][l] /= total;
                    }
            }
        }
    }
}

void Mn::print_tirages(){
    //Debug verif
    uint32_t nb_combi_sensors = pow(2,_nb_sensors);

    float mean[_nb_actions];

    for(uint32_t i = 0; i < _nb_actions;i++){
        for(uint32_t j = 0; j < nb_combi_sensors;j++){
            for(uint32_t k = 0; k < nb_combi_sensors;k++){
                float total = 0.0;
                for(uint32_t l = 0; l < _nb_actions;l++){
                    total += _mn[i][j][k][l];
                    cout  << _mn[i][j][k][l] << " ";
                    mean[l] += _mn[i][j][k][l];
                }

                cout  << total << endl;
            }
        }
    }
    for(uint32_t l = 0; l < _nb_actions;l++){
        cout << " mean" << l <<" : " << mean[l]/(nb_combi_sensors*nb_combi_sensors*_nb_actions);

    }

    cout << endl;
}

void Mn::print_p_actions(uint32_t old_action,vector<bool>& sensor1,vector<bool>& sensor2){
    float* p_actions = _mn[old_action][convert_vbool_to_uint32(sensor1)][convert_vbool_to_uint32(sensor2)];
    for(uint32_t l = 0; l < _nb_actions;l++){
        cout << "action" << l << " : "  << p_actions[l] << " ";
    }
    cout << endl;
}

void Mn::print_p_cum_actions(uint32_t old_action,vector<bool>& sensor1,vector<bool>& sensor2){
    float* p_actions = _mn[old_action][convert_vbool_to_uint32(sensor1)][convert_vbool_to_uint32(sensor2)];
    float p_cum = 0.0;
    for(uint32_t l = 0; l < _nb_actions;l++){
        p_cum+=p_actions[l];
        cout << "action" << l << " : "  << p_cum << " ";
    }
    cout << endl;
}

uint32_t Mn::choose_action(uint32_t old_action,vector<bool>& sensor1,vector<bool>& sensor2){

    using namespace std::chrono;
    std::default_random_engine gen;
    gen.seed(system_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<double> distrib_norm(0.0, 1.0);
    float tirage = distrib_norm(gen);
    cout << "tirage : " << tirage << endl;
    float* p_actions = _mn[old_action][convert_vbool_to_uint32(sensor1)][convert_vbool_to_uint32(sensor2)];
    float p_cum = 0.0;
    for(uint32_t l = 0; l < _nb_actions;l++){
        p_cum += p_actions[l];
        if( tirage < p_cum )
            return l;
    }
    return 0;
}

