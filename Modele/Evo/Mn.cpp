#include "Mn.h"
#include <memory>

Mn::Mn(uint32_t nbAct, uint32_t nbSens):_nb_actions(nbAct),_nb_sensors(nbSens)
{

    uint32_t nb_combi_sensors = pow(2,_nb_sensors);

    _mn = vector<vector<vector<vector<float>>>>(_nb_actions,vector<vector<vector<float>>>(nb_combi_sensors,vector<vector<float>>(_nb_sensors,vector<float>(_nb_actions))));

}

Mn::~Mn(){

}

void Mn::save_as_file(string id){
    ofstream myfile;
    myfile.open (id);
    myfile << _nb_actions << endl << _nb_sensors << endl;

    for(auto& actions : _mn){
        for(auto& sensor_prey : actions){
            for(auto& sensor_predator : sensor_prey){
                for(auto& action : sensor_predator){
                    myfile << action << endl;
                }
            }
        }
    }

    myfile.close();
}

void Mn::load_file(string id){
    ifstream myfile;
    myfile.open (id);
    string line;
    uint32_t nb_sensors;
    uint32_t nb_actions;


    if (myfile.is_open())
    {
        getline (myfile,line);
        nb_actions = stof(line);
        getline (myfile,line);
        nb_sensors = stof(line);
        if(nb_actions == _nb_actions && nb_sensors == _nb_sensors){
            for(auto& actions : _mn){
                for(auto& sensor_prey : actions){
                    for(auto& sensor_predator : sensor_prey){
                        for(auto& action : sensor_predator){
                                getline (myfile,line);
                                action = std::stof(line);
                            }
                        }
                    }
                }
        }else{
            cout << "Mn non compatible avec le fichier fourni, vérifier le nombre d'actions et de senseurs" << endl;
            exit(1);
        }
        myfile.close();
    }
}


void Mn::random_fill(){

    using namespace std::chrono;
    std::default_random_engine gen;
    gen.seed(system_clock::now().time_since_epoch().count());

    for(auto& actions : _mn){
        for(auto& sensor_prey : actions){
            for(auto& sensor_predator : sensor_prey){
                float total = 0.0;
                float tirage = 0.0;
                for(auto& action : sensor_predator){
                    std::uniform_real_distribution<double> distrib_norm(0.0, 1.0);
                    tirage = distrib_norm(gen);
                    total += tirage;
                    action = tirage;
                }
                for(auto& action : sensor_predator){
                    action /= total;
                }
            }
        }
    }
}

void Mn::print_tirages(){
    //Debug verif
    uint32_t nb_combi_sensors = pow(2,_nb_sensors);
    float mean[_nb_actions];

    for(auto& actions : _mn){
        for(auto& sensor_prey : actions){
            for(auto& sensor_predator : sensor_prey){
                float total = 0.0;
                uint32_t cpt = 0;
                for(auto& action : sensor_predator){
                    total += action;
                    cout  << action << " ";
                    mean[cpt++] += action;
                }
                cout  << total << endl;
            }
        }
    }

    for(uint32_t l = 0; l < _nb_actions;l++){
        cout << " mean" << l <<" : " << mean[l]/(_nb_sensors*nb_combi_sensors*_nb_actions);

    }
}

void Mn::print_p_actions(uint32_t old_action,vector<bool>& sensor1,vector<bool>& sensor2){
    vector<float> p_actions = _mn[old_action][convert_vbool_to_uint32(sensor1)][get_vbool_pos(sensor2)];
    //float* p_actions = _mn[old_action][convert_vbool_to_uint32(sensor1)][convert_vbool_to_uint32(sensor2)];
    for(uint32_t l = 0; l < _nb_actions;l++){
        cout << "action" << l << " : "  << p_actions[l] << " ";
    }
    cout << endl;
}

void Mn::print_p_cum_actions(uint32_t old_action,vector<bool>& sensor1,vector<bool>& sensor2){
    vector<float> p_actions = _mn[old_action][convert_vbool_to_uint32(sensor1)][get_vbool_pos(sensor2)];

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
    vector<float> p_actions = _mn[old_action][convert_vbool_to_uint32(sensor1)][get_vbool_pos(sensor2)];
    float p_cum = 0.0;
    for(uint32_t l = 0; l < _nb_actions;l++){
        p_cum += p_actions[l];
        if( tirage < p_cum )
            return l;
    }
    return 0;
}

