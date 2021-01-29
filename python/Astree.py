import numpy as np

INFINITE=9.e999
NB_LIGHT=3 # from center to edge
YELLOW=560.e-9# 560 nanometers

#################################################################################################

class Device:
    surfaces=[]
    fov=0.
    must_recompute=True
    iq=None

    def add_surface(self,surf_type,ticks=0,rcurv=INFINITE,diameter=INFINITE,conic=0.):
        s=Surface(surf_type,rcurv,diameter)
        self.surfaces.append(s)
        self.must_recompute=True

    def set_field_of_view(self,fov):
        self.fov=fov
        self.must_recompute=True

    def image_quality(self):
        if self.must_recompute:
            self.compute_quality()
            self.must_recompute=False
        return self.iq

    def compute_quality(self):
        if len(self.surfaces)==0:
            self.lf=-1
            return

        l=Light(self.surfaces[0].diameter)

        for s in self.surfaces:
            s.receive(l)

        self.lf=l.lf()

    def lf(self):
        self.compute_quality()
        return self.lf


#################################################################################################
class Light:
    def __init__(self,diameter, nb_photons=31): # no tilt for now
        nb_total=nb_photons*nb_photons
        self.dx=np.zeros(nb_total)
        self.dy=np.zeros(nb_total)
        self.dz=np.ones(nb_total)
        
        xd=np.linspace(-diameter,diameter,nb_photons)
        yd=np.linspace(-diameter,diameter,nb_photons)
        self.x,self.y=np.meshgrid(xd,yd)
        self.z=np.zeros(nb_total)
        self.valid=np.ones(nb_total)
        self.wavelenght=np.ones(nb_total)*YELLOW
        self.index=np.ones(nb_total)

    def lf(self):
        return -1.
#################################################################################################

class Surface:
    type="stop"
    
    def __init__(self,surf_type,rcurv,diameter):
        self.type=surf_type
        self.rcurv=rcurv
        self.diameter=diameter

    def receive(self,light):
        if self.type=="stop" :
            self.stop(light)

        if self.type=="reflect" :
            self.reflect(light)

        if self.type=="transmit" :
            self.transmit(light)

    def stop(self,light):
        pass

    def reflect(self,light):
        self.stop(light)
        nrm=self.compute_normals(light)
        pass

    def transmit(self,light):
        self.stop(light)
        nrm=self.compute_normals(light)
        pass

    def compute_normals(self,light):
        pass


#################################################################################################
class ImageQuality:
    maxlfro=INFINITE
    pass

#################################################################################################
