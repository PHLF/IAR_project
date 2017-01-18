#include <Controleur/LightSim.h>
#include <cmath>
#include <Modele/Evo/Mn.h>

using namespace std;

int main() {
  //sim::LightSim lightSim(768,768,512, 512, 10, 10);
  //lightSim.run(3e2);

   Mn mn(4,12);
   mn.random_fill();
   //mn.save_as_file("test");
   //mn.load_file("test");
   mn.print_tirages();

   mn.gaussian_random_mutation(0.001);
   mn.print_tirages();

   //mn.print_tirages();

   // cout << sizeof(float) << endl;

    //vector<vector<vector<vector<float>>>> _mn = vector<vector<vector<vector<float>>>>(4,vector<vector<vector<float>>>(4096,vector<vector<float>>(4096,vector<float>(4))));
    //cout << sizeof(_mn) << endl;
    /*vector<bool> A = {1,0,1,1,0,0};
    vector<bool> B = {0,0,0,1,0,0};
    mn.print_p_cum_actions(0,A,B);
    mn.print_p_actions(0,A,B);
    cout << mn.get_mn()[0][convert_vbool_to_uint32(A)][get_vbool_pos(B)][0] << " " << mn.get_mn()[0][convert_vbool_to_uint32(A)][get_vbool_pos(B)][1] << " " << mn.get_mn()[0][convert_vbool_to_uint32(A)][get_vbool_pos(B)][2] << " " << mn.get_mn()[0][convert_vbool_to_uint32(A)][get_vbool_pos(B)][3] << " " << endl;
    cout << mn.choose_action(0,A,B) << endl;*/

  return EXIT_SUCCESS;
}
