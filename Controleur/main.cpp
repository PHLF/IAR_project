#include <Controleur/LightSim.h>
#include <cmath>
#include <Modele/Evo/Mn.h>

using namespace std;

int main() {
  //sim::LightSim lightSim(768,768,512, 512, 10, 10);
  //lightSim.run(3e2);

   Mn mn(8,6);
   mn.random_fill();
   //mn.print_tirages();

  // cout << sizeof(float) << endl;


    vector<bool> A = {1,0,1,0,1,1};
    mn.print_p_cum_actions(0,A,A);

    //cout << mn.get_mn()[0][43][43][0] << " " << mn.get_mn()[0][43][43][1] << " " << mn.get_mn()[0][43][43][2] << " " << mn.get_mn()[0][43][43][3] << " " << endl;
    cout << mn.choose_action(0,A,A) << endl;

  return EXIT_SUCCESS;
}
