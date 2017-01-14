#ifndef RETINA_H
#define RETINA_H


class Retina
{
    public:
        Retina(int seg,int fov) ;
        virtual ~Retina();
        int getNbSegments(){return nbSegments;}
    protected:

    private:
        int nbSegments;
        int fieldOfView; //Angle in °
        int depth;

};

#endif // RETINA_H
