# self file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
# please see LICENSE.txt for more details and licensing issues
# copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include "DeviceRevert.h"
#include "OpticalDevice.h"

#include <cassert>

########################################
#swap surfaces and change sign of curvature and ticks
def revert(self, pDevice, iFirstSurface, iLastSurface):
	assert(pDevice != 0)
	opdOld = *pDevice

	for (iSurf = iFirstSurface; iSurf <= iLastSurface; iSurf++)
		#inverse each surface profile
		pDevice.set(iSurf, RADIUS_CURVATURE, -opdOld.get(iLastSurface - iSurf , RADIUS_CURVATURE))
		pDevice.set(iSurf, R4, -opdOld.get(iLastSurface - iSurf, R4))
		pDevice.set(iSurf, R6, -opdOld.get(iLastSurface - iSurf, R6))
		pDevice.set(iSurf, R8, -opdOld.get(iLastSurface - iSurf, R8))
		pDevice.set(iSurf, R10, -opdOld.get(iLastSurface - iSurf, R10))
		pDevice.set(iSurf, CONIC, -opdOld.get(iLastSurface - iSurf, CONIC))

		#inverse diameters
		pDevice.set(iSurf, DIAMETER, opdOld.get(iLastSurface - iSurf, DIAMETER))
		pDevice.set(iSurf, INNER_DIAMETER, opdOld.get(iLastSurface - iSurf, INNER_DIAMETER))


	for (iSurf = iFirstSurface+1; iSurf < iLastSurface; iSurf++)
		#inverse ticks
		pDevice.set(iSurf, Z, opdOld.get(iLastSurface , Z)- opdOld.get(iLastSurface - iSurf, Z)+ opdOld.get(iFirstSurface, Z))


	for (iSurf = iFirstSurface ; iSurf < iLastSurface; iSurf++)
		#inverse type
		pDevice.set_type(iSurf, opdOld.type(iLastSurface- iSurf-1))


    return True

#######################################
