// this file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include "DeviceRevert.h"
#include "OpticalDevice.h"

#include <cassert>

////////////////////////////////////////////////////////////////////////////////
//swap surfaces and change sign of curvature and ticks
bool DeviceRevert::revert(OpticalDevice* pDevice, int iFirstSurface, int iLastSurface)
{
	assert(pDevice != 0);
	OpticalDevice opdOld = *pDevice;

	for (int iSurf = iFirstSurface; iSurf <= iLastSurface; iSurf++)
	{
		Surface s2 = opdOld.surface(iSurf);
		
		//invert profile
		s2.set_radius_curvature( -s2.radius_curvature());
		s2.set_R4(-s2.R4());
		s2.set_R6(-s2.R6());
		s2.set_R8(-s2.R8());
		s2.set_R10(-s2.R10());

		pDevice->surface(iLastSurface - iSurf) = s2;
	}

	//permute all ticks but the last
	for (int iSurf = iFirstSurface; iSurf <= iLastSurface; iSurf++)
	{
		pDevice->set(iSurf, Z, opdOld.get(iLastSurface - iSurf - 1, Z));
	}
	//pDevice->set(iLastSurface, THICK, opdOld.get(iLastSurface, THICK));

    return true;
}
//////////////////////////////////////////////////////////////////////////////
