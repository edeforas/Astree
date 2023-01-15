from optical_device import OpticalDevice
import numpy as np
import matplotlib.pyplot as plt


od = OpticalDevice()
dDiameter = 200.0  # primary diameter in mm
dFocal = 2400.0    # primary focal length in mm
dSecondaryMagnification = 5.0  # magnification ratio
dSecondaryDiameter = 60.0  # secondary diameter in mm

# Surface 0 = primary mirror (hyperbolic)
od.insert_surface(0)
od.set_type(0, "reflect")
od.set(0, OpticalDevice.DIAMETER, dDiameter)
dPrimaryRadius = 2.0 * dFocal
od.set(0, OpticalDevice.RADIUS_CURVATURE, -dPrimaryRadius)
od.set(0, OpticalDevice.CONIC, -1.0)  # hyperbolic for RC design
od.set(0, OpticalDevice.THICK, -dFocal)

# Surface 1 = secondary mirror (hyperbolic)
od.insert_surface(1)
od.set_type(1, "reflect")
od.set(1, OpticalDevice.DIAMETER, dSecondaryDiameter)
dSecondaryRadius = dPrimaryRadius / dSecondaryMagnification
od.set(1, OpticalDevice.RADIUS_CURVATURE, dSecondaryRadius)
od.set(1, OpticalDevice.CONIC, -1.0)  # hyperbolic for RC design
dSecondarySpacing = dFocal * (1.0 - 1.0/dSecondaryMagnification)
od.set(1, OpticalDevice.THICK, dSecondarySpacing)

# Surface 2 = image plane
od.insert_surface(2)
od.set_type(2, "image")
od.set(2, OpticalDevice.AUTO_DIAMETER, 1)

# Test 1: Spot size vs primary conic (RC design optimization)
print("Test 1: Spot size vs primary conic variation...")
all_spot_size = []
primary_conics = np.arange(-1.5, -0.5, 0.05)

for conic in primary_conics:
    od.set(0, OpticalDevice.CONIC, conic)
    iq = od.get_image_quality()
    all_spot_size.append(iq.spot_size[0])

plt.figure(figsize=(12, 4))

plt.subplot(1, 2, 1)
plt.plot(primary_conics, all_spot_size, 'b-', linewidth=2, label="Primary Conic Variation")
plt.axvline(-1.0, color='r', linestyle='--', label="RC Design (conic=-1.0)")
plt.xlabel("Primary Mirror Conic")
plt.ylabel("Spot Size (mm)")
plt.title("Ritchey-Chrétien: Image Quality vs Primary Conic")
plt.legend()
plt.grid()

# Test 2: Spot size vs secondary magnification
print("Test 2: Spot size vs secondary magnification...")
od.set(0, OpticalDevice.CONIC, -1.0)  # RC optimized

all_spot_sizes_mag = []
all_magnifications = np.arange(3.0, 8.0, 0.2)

for mag in all_magnifications:
    dSecondaryRadius = dPrimaryRadius / mag
    od.set(1, OpticalDevice.RADIUS_CURVATURE, dSecondaryRadius)
    dSecondarySpacing = dFocal * (1.0 - 1.0/mag)
    od.set(1, OpticalDevice.THICK, dSecondarySpacing)
    iq = od.get_image_quality()
    all_spot_sizes_mag.append(iq.spot_size[0])

plt.subplot(1, 2, 2)
plt.plot(all_magnifications, all_spot_sizes_mag, 'g-', linewidth=2, label="Magnification Variation")
plt.axvline(dSecondaryMagnification, color='r', linestyle='--', label=f"Current Design (mag={dSecondaryMagnification})")
plt.xlabel("Secondary Mirror Magnification")
plt.ylabel("Spot Size (mm)")
plt.title("Ritchey-Chrétien: Image Quality vs Secondary Magnification")
plt.legend()
plt.grid()

plt.tight_layout()
plt.show()

print(f"Ritchey-Chrétien Telescope Configuration:")
print(f"  Primary diameter: {dDiameter} mm")
print(f"  Primary focal length: {dFocal} mm")
print(f"  Primary radius of curvature: {-dPrimaryRadius} mm")
print(f"  Secondary diameter: {dSecondaryDiameter} mm")
print(f"  Secondary magnification: {dSecondaryMagnification}")
print(f"  Both mirrors: hyperbolic (conic = -1.0)")
