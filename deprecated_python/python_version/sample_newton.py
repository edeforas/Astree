from optical_device import OpticalDevice
import numpy as np
import matplotlib.pyplot as plt

# This sample shows how the image quality changes with the conic constant of a simple concave mirror.
# The focal length is fixed at 1000 mm and the diameter is fixed at 200 mm. 
# The conic constant is varied from -1.5 to 0.5 in steps of 0.05, and the spot size is recorded at the center, middle edge, and edge of the field of view.

od = OpticalDevice()
od.set_nb_angles(3)
od.set_half_field_of_view(0.5)
dDiameter = 200.0
dFocal = 1000.0

# surface0 = mirror
od.insert_surface(0)
od.set_type(0, "reflect")
od.set(0, OpticalDevice.DIAMETER, dDiameter)
od.set(0, OpticalDevice.RADIUS_CURVATURE, -2.0 * dFocal)
od.set(0, OpticalDevice.THICK, -dFocal)

# surface1 = image
od.insert_surface(1)
od.set_type(1, "image")
od.set(1, OpticalDevice.AUTO_DIAMETER, 1)

conics = np.arange(-1.5, 0.5, 0.05)
all_spot_size_center=[]
all_spot_size_midedge=[]
all_spot_size_edge=[]
for conic in conics:
    od.set(0, OpticalDevice.CONIC, conic)
    iq = od.get_image_quality()
    all_spot_size_center.append(iq.spot_size[0])
    all_spot_size_midedge.append(iq.spot_size[1])
    all_spot_size_edge.append(iq.spot_size[2])

plt.figure()
plt.plot(conics, all_spot_size_center, label="Spot Size at Center")
plt.plot(conics, all_spot_size_midedge, label="Spot Size at Middle Edge")
plt.plot(conics, all_spot_size_edge, label="Spot Size at Edge")
plt.xlabel("Conic")
plt.ylabel("Spot Size (mm)")
plt.title("Image Quality vs Conic at Different Points of the Field")
plt.legend()
plt.grid()
plt.show()