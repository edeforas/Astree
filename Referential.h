#ifndef _Referential_
#define _Referential_

class Referential
{
public:
    Referential();
    virtual ~Referential();

    void setYawPitchRoll(double Yaw,double Pitch,double Roll);
    void setTranslation(double x,double y, double z);

    void transform(double& x,double& y,double& z) const;
    void unTransform(double& x,double& y,double& z) const;

private:
    double _rotMat[9]; //rotation
    double _x,_y,_z; //translation
};

#endif
