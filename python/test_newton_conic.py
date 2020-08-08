import OpticalDevice as od

opd=od.OpticalDevice();
opd.set_field_of_view(1.);
opd.add(od.Reflect(2000,-1,200))
opd.add(od.Image())

iq=opd.image_quality()
print(iq)