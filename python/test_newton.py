import Astree as astree

n=astree.Device()
n.set_field_of_view(1.)

n.add_surface('reflect',1200,2400,200)
n.add_surface('image')

iq=n.image_quality()
print(iq.maxlfro)