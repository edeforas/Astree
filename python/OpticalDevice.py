import numpy as np

#################################################################################
class Light:
    def init(self,angle):
        pass

class Surface:
    def stop(self,l):
        pass

    def receive(self,l):
        pass

class Reflect(Surface):
    def __init__(self,RCurv, Conic, Diameter):
        self.RCurv=RCurv
        self.Conic=Conic
        self.Diameter=Diameter

    def receive(self,l):
        self.stop(l)

class Image(Surface):
    def receive(self,l):
        self.stop(l)

    def quality(self):
        return 0

#################################################################################
class OpticalDevice:
    def __init__(self):
        self.surfaces = list()
        self.light =Light()
        self.bMustCompute=True;

    def set_field_of_view(self,FieldOfView,NbAngles=5):
        self.FieldOfView=FieldOfView
        self.NbAngles=NbAngles
        self.bMustCompute=True;

    def add(self,s):
        self.surfaces.append(s)
        self.bMustCompute=True;

    def compute(self):
        if(self.bMustCompute==True):
            self.imageQuality=list()
            for iAngle in range(0,self.NbAngles):
                angle=iAngle/self.NbAngles*self.FieldOfView
                self.light.init(angle)
                for surf in self.surfaces:
                    surf.receive(self.light)
                self.imageQuality.append(self.surfaces[-1].quality())
            self.bMustCompute=False;
    
    def image_quality(self):
        self.compute()
        return self.imageQuality
