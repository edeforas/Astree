import math

def revert(pDevice, iFirstSurface, iLastSurface):
    assert pDevice is not None
    opdOld = pDevice.copy()

    for iSurf in range(iFirstSurface, iLastSurface+1):
        pDevice.set(iSurf, 'RADIUS_CURVATURE', -opdOld.get(iLastSurface - iSurf, 'RADIUS_CURVATURE'))
        pDevice.set(iSurf, 'R4', -opdOld.get(iLastSurface - iSurf, 'R4'))
        pDevice.set(iSurf, 'R6', -opdOld.get(iLastSurface - iSurf, 'R6'))
        pDevice.set(iSurf, 'R8', -opdOld.get(iLastSurface - iSurf, 'R8'))
        pDevice.set(iSurf, 'R10', -opdOld.get(iLastSurface - iSurf, 'R10'))
        pDevice.set(iSurf, 'CONIC', -opdOld.get(iLastSurface - iSurf, 'CONIC'))

        pDevice.set(iSurf, 'DIAMETER', opdOld.get(iLastSurface - iSurf, 'DIAMETER'))
        pDevice.set(iSurf, 'INNER_DIAMETER', opdOld.get(iLastSurface - iSurf, 'INNER_DIAMETER'))

    for iSurf in range(iFirstSurface+1, iLastSurface):
        pDevice.set(iSurf, 'Z', opdOld.get(iLastSurface, 'Z') - opdOld.get(iLastSurface - iSurf, 'Z') + opdOld.get(iFirstSurface, 'Z'))

    for iSurf in range(iFirstSurface, iLastSurface):
        pDevice.set_type(iSurf, opdOld.type(iLastSurface - iSurf - 1))

    return True