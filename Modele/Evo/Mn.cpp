#include "Mn.h"

Mn::Mn(uint32_t nbAct, uint32_t nbSens):_nb_actions(nbAct),_nb_sensors(nbSens)
{

    uint32_t nb_combi_sensors = pow(2,_nb_sensors);
    //Compromis mémoire utilisabilité sympa, reste assez lourd 2.5Go par instance. Pas vraiment du c++ btw.

    //uint32_t debugNbCases = 0;
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

    //_mn[3][4095][4095][3] = 0.2;
    //cout << _mn[3][4095][4095][3] << " "<< debugNbCases << endl;
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
