# self file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
# please see LICENSE.txt for more details and licensing issues
# copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

import Photon
import math

# include "Light.h"
# include "Photon.h"
# include "GlassManager.h"

# include "Vector3D.h"

# define PI (3.1415926535897)
RAD_TO_DEG= (360./2./math.pi)
DEG_TO_RAD= (math.pi*2./360.)

# define RGB(r,g,b) ((r << 16) + (g <<8) + b)

# define RED RGB(255,0,0)
# define GREEN RGB(0,255,0)
# define BLUE RGB(0,0,255)
# define DARK_GREY RGB(32,32,32)
# define YELLOW 0xede500
# define BROWN 0x74572e
# define PURPLE 0x772176

# define LAMBDA_IR 1000.e-3
# define LAMBDA_RED 656.2725e-3
# define LAMBDA_YELLOW 587.5618e-3
# define LAMBDA_GREEN 546.0740e-3
# define LAMBDA_BLUE 486.1327e-3
# define LAMBDA_UV 300.e-3

#######################################
class Light:
    def __init__(self):
        self._iNbPhotons=0
        self._dTiltX=0.
        self._dTiltY=0.
        self._dSpotSize=0.

        self._dCenterX=0.
        self._dCenterY=0.

        self._dDiameter=0.
        self._dZ=0.
        self._dFD=1.e99

        self._bMustInit=True
        self._bYellowBlack=False

        # self._pFirstMaterial=0
        # self._pMaterial=_pFirstMaterial

        self._dVignetting=-1.
        self._iNbPhotonsInitVignetting=0
        self._bIsInfinite=False
        self._bIsValid=False

        self._iDimX=0
        self._iDimY=0

    #######################################
    def nb_photon(self):
        self.init()
        return self._iNbPhotons
    #######################################
    def get_photon(self, iPos):
        self.init()

        return self._vPhotons[iPos]
    #######################################
    def photons(self):
        self.init()
        return self._vPhotons

    #######################################
    def set_geometry(self, dZ, dDiameter):
        self._dZ=dZ
        self._dDiameter=dDiameter
        self._bMustInit=True

    #######################################
    def init(self):
        if self._bMustInit==False:
            return

        #light direction
        dTanX=-math.tan(self._dTiltX)
        dTanY=-math.tan(self._dTiltY)
        dIdentN=+1.

        # #TODO normalize dItentN?

        # vector<double> vdLambda
        # vector<int> viVisualColors
        # calc_colors(_sAllColors, vdLambda, viVisualColors)

        # _iNbPhotons=(int)(_iDimX*_iDimY*vdLambda.size())
        # _vPhotons.resize(_iNbPhotons)

        # int iPhoton=0
        # double dRadius=_dDiameter; #TODO
        # double dTranslateX=dTanX*dRadius
        # double dTranslateY=dTanY*dRadius

        # for (unsigned int iLambda=0;iLambda<vdLambda.size();iLambda++)
        #     double dLambda=vdLambda[iLambda]
        #     #int iVisualColor=viVisualColors[iLambda]
        #     int iNMx=_iDimX/2
        #     int iNMy=_iDimY/2
        #     # assert(_iDimX is odd)
        #     # assert(_iDimY is odd)

        #     for (int j=0; j<_iDimY;j++)
        #         double dPosY=(j-iNMy)*_dDiameter/(double)iNMy/2.
        #         if _iDimY==1:
        #             dPosY=0

        #         for (int i=0;i<_iDimX;i++)
        #             Photon& p=_vPhotons[iPhoton]
        #             p.set_lambda(dLambda)

        #             # can be optimised if needed
        #             double dPosX=(i-iNMx)*_dDiameter/(double)iNMx/2.

        #             p.x=dPosX-dTranslateX
        #             p.y=dPosY-dTranslateY
        #             p.z=_dZ-dRadius
        #             p.dx=dTanX
        #             p.dy=dTanY
        #             p.dz=dIdentN

        #             p.valid=True
        #             assert(p.is_valid())
        #             iPhoton++

        # GlassManager.singleton().destroy(_pFirstMaterial)
        # self._pFirstMaterial=GlassManager.singleton().create("Air")
        # self._pMaterial=_pFirstMaterial
        self._bIsInfinite=False
        self._bIsValid=True
        self._bMustInit=False

    #######################################
    def set_tilt(self, dTiltX, dTiltY):
        self._dTiltX=dTiltX*DEG_TO_RAD
        self._dTiltY=dTiltY*DEG_TO_RAD
        self._bMustInit=True

    #######################################
    def get_tilt(self):
        return self.dTiltX*RAD_TO_DEG, self._dTiltY*RAD_TO_DEG

    #######################################
    # def calc_colors(self, sColors, vdLambda, vdVisualColors):
    #     vdLambda.clear()
    #     vdVisualColors.clear()

    #     if sColors.find("IR.")!=string.npos:
    #         vdLambda.push_back(LAMBDA_IR)
    #         vdVisualColors.push_back(BROWN); #brown


    #     if sColors.find("Red.")!=string.npos:
    #         vdLambda.push_back(LAMBDA_RED)
    #         vdVisualColors.push_back(RED)


    #     if sColors.find("YellowBlack.")!=string.npos:
    #         vdLambda.push_back(LAMBDA_YELLOW)
    #         vdVisualColors.push_back(DARK_GREY)


    #     if sColors.find("Yellow.")!=string.npos:
    #         vdLambda.push_back(LAMBDA_YELLOW)
    #         vdVisualColors.push_back(YELLOW); #yellow


    #     if sColors.find("Green.")!=string.npos:
    #         vdLambda.push_back(LAMBDA_GREEN)
    #         vdVisualColors.push_back(GREEN)


    #     if sColors.find("Blue.")!=string.npos:
    #         vdLambda.push_back(LAMBDA_BLUE)
    #         vdVisualColors.push_back(BLUE)


    #     if sColors.find("UV.")!=string.npos:
    #         vdLambda.push_back(LAMBDA_UV)
    #         vdVisualColors.push_back(PURPLE)

    #######################################
    # def get_visual_color(self, dLambda):
    #     if dLambda==LAMBDA_IR:
    #         return BROWN

    #     if dLambda==LAMBDA_RED:
    #         return RED

    #     if dLambda==LAMBDA_YELLOW:
    #         if _bYellowBlack:
    #             return DARK_GREY
    #         else:
    #             return YELLOW


    #     if dLambda==LAMBDA_GREEN:
    #         return GREEN

    #     if dLambda==LAMBDA_BLUE:
    #         return BLUE

    #     if dLambda==LAMBDA_UV:
    #         return PURPLE

    #     return 0

    #######################################
    # def set_colors(self, & sAllColors):
    #     _sAllColors=sAllColors

    #     if _sAllColors.empty():
    #         _sAllColors="YellowBlack."

    #     _bYellowBlack=(_sAllColors=="YellowBlack.")
    #     _bMustInit=True

    #######################################
    def set_nb_photons(self, iNbX, iNbY):

        self._iDimX=iNbX
        self._iDimY=iNbY
        self._bMustInit=True

    #######################################
    # def material(self):
    #     self.init()
    #     return _pMaterial

    #######################################
    # def set_material(self, pM):
    #     self._pMaterial=pM

    #######################################
    def get_spot_center(self):
        return self._dCenterX, self._dCenterY

    #######################################
    def spot_size(self):
        return self._dSpotSize

    #######################################
    def is_image_infinite(self):
        return self._bIsInfinite

    #######################################
    def compute_spot_size(self,bInfinite): #todo remove bInfinite flag here
        dXS=0., dYS=0.
        dMinX=0.,dMaxX=0.,dMinY=0.,dMaxY=0.
        dMinXR=0.,dMaxXR=0.,dMinYR=0.,dMaxYR=0.; #rotated 45deg

        # double px,py

        dDxCentral=0.
        dDyCentral=0.
        dDzCentral=0.

        _bIsInfinite=bInfinite

        #compute bounding octogon (8 edges)
        iNbValidPhoton=0
        for i in range (self._iNbPhotons):
            p=self._vPhotons[i]

            if p.is_valid()==False:
                continue

            if bInfinite==True:
                #compute the x and y angle in deg, in px, py
                px=math.atan2(p.dx,p.dz)*180./math.pi
                py=math.atan2(p.dy,p.dz)*180./math.pi

                p.anglex=px
                p.angley=py

            else:
                #use intersection at focal plane
                px=p.x; py=p.y


            if (iNbValidPhoton==0): #first time tinit
                dMinX=px
                dMaxX=px
                dMinY=py
                dMaxY=py

                dMinXR=px-py
                dMaxXR=dMinXR
                dMinYR=px+py
                dMaxYR=dMinYR

            else:
                # bounding rectangle
                dMinX=min(dMinX,px)
                dMaxX=max(dMaxX,px)
                dMinY=min(dMinY,py)
                dMaxY=max(dMaxY,py)

                # bounding rotated(45deg) rectangle
                dMinXR=min(dMinXR,px-py)
                dMaxXR=max(dMaxXR,px-py)
                dMinYR=min(dMinYR,px+py)
                dMaxYR=max(dMaxYR,px+py)

            dXS+=px
            dYS+=py

            if not bInfinite:
                Vector3D.normalize(p.dx, p.dy, p.dz); #TODO remove using mean axis(max,min)
                dDxCentral+= p.dx
                dDyCentral+= p.dy
                dDzCentral+= p.dz

            iNbValidPhoton+=1

        if iNbValidPhoton==0:
            self._bIsValid=False
            self._dCenterX=0.
            self._dCenterY=0.
            self._dSpotSize=1.e99
            self._dFD=1.e99
            return


        if bInfinite:
            self._dFD=1.e99
        else:
            # compute F/D
            Vector3D.normalize(dDxCentral,dDyCentral,dDzCentral)

            #compute cos on mean axis
            dMinCos=3.; #cos . decreasing function . search for min
            for i in range(self._iNbPhotons):
                p=self._vPhotons[i]

                if p.is_valid()==False:
                    continue

                # project p on axis
                dCosP=p.dx*dDxCentral+p.dy*dDyCentral+p.dz*dDzCentral
                if dCosP<0.:
                    self._bIsValid=False
                    self._dCenterX=0.
                    self._dCenterY=0.
                    self._dSpotSize=1.e99
                    self._dFD=1.e99
                    return

                #dCosP = std.clamp(dCosP, 0., 1.); #c++17 function
                if dCosP>1.:
                    dCosP=1.
                elif dCosP<0.:
                    dCosP=0.

                dMinCos = min(dMinCos, dCosP)

            if dMinCos>1:
                self._bIsValid = False
                self._dCenterX = 0.
                self._dCenterY = 0.
                self._dSpotSize = 1.e99
                self._dFD = 1.e99
                return

            self._dFD=0.5/math.tan(math.acos(dMinCos))

            if self._dFD<0.01:
                self._bIsValid=False
                self._dCenterX=0.
                self._dCenterY=0.
                self._dSpotSize=1.e99
                self._dFD=1.e99
                return

        self._dCenterX=dXS/iNbValidPhoton
        self._dCenterY=dYS/iNbValidPhoton

        self._dSpotSize=max(dMaxX-dMinX,dMaxY-dMinY)
        dSpotSizeRotated=max(dMaxXR-dMinXR,dMaxYR-dMinYR)*0.7071067811865475; #sqrt(2)/2
        self._dSpotSize=max(self._dSpotSize,dSpotSizeRotated)

        _dVignetting=100.*iNbValidPhoton/self._iNbPhotonsInitVignetting
        self._bIsValid=True

    #######################################
    def get_FD(self):
        return self._dFD
    #######################################
    def spot_vs_airy(self):
        return self.spot_size()/2./self.airy_radius()
    #######################################
    def airy_radius(self):
        if self.is_image_infinite():
            return 0.02; # in deg ; for now ;  https:#en.wikipedia.org/wiki/Naked_eye
        else:
            return 1.22*self.get_FD()*(LAMBDA_YELLOW)*1.e-3; # use yellow light

    #######################################
    def vignetting(self):
        return self._dVignetting

    #######################################
    def init_vignetting(self):
        self._iNbPhotonsInitVignetting=0

        for i in range(self._iNbPhotons):
            if _vPhotons[i].is_valid():
                _iNbPhotonsInitVignetting+=1

    #######################################
    def is_valid(self):
        return self._bIsValid
    #######################################