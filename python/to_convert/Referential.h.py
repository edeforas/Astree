# self file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
# please see LICENSE.txt for more details and licensing issues
# copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef Referential_
#define Referential_

class Referential
public:
    Referential()
    virtual ~Referential()

    void setYawPitchRoll(double Yaw, Pitch, Roll)
    void setTranslation(double x, y, z)

    void transform(double& x, y, z)
    void unTransform(double& x, y, z)

private:
    double _rotMat[9]; #rotation
    double _x,_y,_z; #translation


#endif
