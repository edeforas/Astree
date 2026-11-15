// this file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef Referential_
#define Referential_

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
