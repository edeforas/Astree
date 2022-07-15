# self file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
# please see LICENSE.txt for more details and licensing issues
# copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include "Referential.h"
#include <cmath>

#######################################
Referential.Referential()
    _rotMat[0]=1.; _rotMat[1]=0.;  _rotMat[2]=0.
    _rotMat[3]=0.; _rotMat[4]=1.;  _rotMat[5]=0.
    _rotMat[6]=0.; _rotMat[7]=0.;  _rotMat[8]=1.

    _x=0.; _y=0.; _z=0.

#######################################
Referential.~Referential()
{
#######################################
def setYawPitchRoll(self, Yaw, Pitch, Roll):
{ # from public domain www.developpez.com
    double A=cos(Roll)
    double B=sin(Roll)
    double C=cos(Pitch)
    double D=sin(Pitch)
    double E=cos(Yaw)
    double F=sin(Yaw)

    _rotMat[0]=C*E
    _rotMat[1]=-C*F
    _rotMat[2]=-D

    _rotMat[3]=-B*D*E + A*F
    _rotMat[4]= B*D*F + A*E
    _rotMat[5]=-B*C

    _rotMat[6]= A*D*E + B*F
    _rotMat[7]=-A*D*F + B*E
    _rotMat[8]= A*C

#######################################
def transform(self, x, y, z):
    double xt=x+_x
    double yt=y+_y
    double zt=z+_z

    double nx=_rotMat[0]*xt+_rotMat[1]*yt+_rotMat[2]*zt
    double ny=_rotMat[3]*xt+_rotMat[4]*yt+_rotMat[5]*zt
    double nz=_rotMat[6]*xt+_rotMat[7]*yt+_rotMat[8]*zt

    x=nx; y=ny; z=nz

#######################################
def unTransform(self, x, y, z):
    double nx=_rotMat[0]*x+_rotMat[3]*y+_rotMat[6]*z
    double ny=_rotMat[1]*x+_rotMat[4]*y+_rotMat[7]*z
    double nz=_rotMat[2]*x+_rotMat[5]*y+_rotMat[8]*z

    x=nx-_x; y=ny-_y; z=nz-_z

#######################################
def setTranslation(self, x, y, z):
    _x=x; _y=y;_z=z

#######################################
