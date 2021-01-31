from Astree import *
import matplotlib.pyplot as plt

#simple newton (spherical mirror)
diameter=200.
focal=1200.
d=Device()
#d.set_field_of_view(1.)
#d.add_surface(Ticks(focal))
d.add_surface(Reflect(diameter=diameter,rc=2.*focal)) # spherical
#d.add_surface(Image())

# plot results
ir=d.compute()
l0=ir.lights[0]

# plt.scatter(l0.x,l0.y)
# plt.gca().set_aspect('equal')

r=np.sqrt(l0.x**2+l0.y**2)
z=l0.z
plt.plot(r,z)

plt.show()