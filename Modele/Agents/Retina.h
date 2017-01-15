#ifndef RETINA_H
#define RETINA_H


#include "../Environnement/Environment.h"

class Environment;

class Retina
{
    public:
        Retina(int seg,int fov) ;
        virtual ~Retina();
        int getNbSegments(){return nbSegments;}
    protected:

    private:
        Environment* env;
        int nbSegments;
        int fieldOfView; //Angle in °
        int depth;

};

#endif // RETINA_H
