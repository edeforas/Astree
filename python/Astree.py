import numpy as np

INFINITE=9.e999
NB_LIGHT=3 # from center to edge

#################################################################################################

class Device:
    surfaces=list()
    fov=0
    must_recompute=True
    iq=None

    def add_surface(self,type,ticks=0,rcurv=INFINITE,diameter=INFINITE,conic=0):
        self.must_recompute=True

    def set_field_of_view(self,fov):
        self.must_recompute=True
        self.fov=fov

    def image_quality(self):
        if self.must_recompute:
            self.compute_quality()
            self.must_recompute=False
        return self.iq

    def compute_quality(self):
        self.iq=ImageQuality()

#################################################################################################

class Light:
    photons_pos=None
    photons_dir=None
    photons_lambda=None
    photons_valid=None

    def init(self,diameter,tilt):
        pass

#################################################################################################

class Surface:
    type="stop"
    
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
