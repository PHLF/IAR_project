#include <Controleur/LightSim.h>

LightSim::LightSim(uint32_t x,
                   uint32_t y,
                   uint32_t nbPredators,
                   uint32_t nbPreys)
    : _env(new Environment(x, y, nbPredators, nbPreys)) {
  // grid = new Agent* [sizex];
}

LightSim::~LightSim() {
  /*for (uint32_t i=0; i < sizex; i++){
      grid[i] = new Agent[ sizey ];
  }*/
}


float LightSim::random_x(){

    std::uniform_real_distribution<float> distrib_x(0.0,_env->get_size_x()-1);

    return distrib_x(generator);
}

float LightSim::random_y(){

    std::uniform_real_distribution<float> distrib_y(0.0,_env->get_size_y()-1);

    return distrib_y(generator);
}

uint32_t LightSim::random_orientation(){

    std::uniform_int_distribution<uint32_t> distrib_ori(0,359);

    return distrib_ori(generator);
}

void LightSim::print_agents(std::vector<std::unique_ptr<Agent>>& agents){
    for(std::unique_ptr<Agent>& agent : agents){
        std::cout << *agent << std::endl ;
    }
}

void LightSim::move_agents(std::vector<std::unique_ptr<Agent>>& agents){
    for(std::unique_ptr<Agent>& agent : agents){
        agent->moveForward();
    }
}

void LightSim::observe_agents(std::vector<std::unique_ptr<Agent>>& agents){
    for(std::unique_ptr<Agent>& agent : agents){

    }
}

void LightSim::setup_agents(std::vector<std::unique_ptr<Agent>>& agents){
    for(std::unique_ptr<Agent>& agent : agents){
        agent->set_coord({random_x(),random_y()});
        agent->set_orientation(random_orientation());
    }
}

bool LightSim::run(uint32_t nbTicks) {
  generator.seed(std::chrono::system_clock::now().time_since_epoch().count());

  std::vector<std::unique_ptr<Agent>>& preds = _env->get_predators();
  std::vector<std::unique_ptr<Agent>>& preys = _env->get_preys();

  setup_agents(preds);
  setup_agents(preys);
print_agents(preys);
  for (_tick = 0; _tick < nbTicks; ++_tick) {

    std::cout << "Tick n°" << _tick << std::endl;
    move_agents(preds);
    print_agents(preds);
  }

  return true;
}

/**
 * @brief operator << Affichage de l'environnement sous forme de grille pour
 * débug
 * @param strm
 * @param a
 * @return
 */
std::ostream& operator<<(std::ostream& strm, const LightSim& a) {
  // return strm << "A(" << a.i << ")" << std::endl;
  return strm;
}
