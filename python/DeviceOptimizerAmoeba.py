"""
This file is covered by the GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
please see LICENSE.txt for more details and licensing issues
copyright Etienne de Foras (the author) mailto: etienne.deforas@gmail.com
"""

import numpy as np
from copy import deepcopy
import DeviceOptimizer

class DeviceOptimizerAmoeba(DeviceOptimizer):
    SPOT_SIZE_INFINITY = float('inf')
    AMOEBA_MAX_ITER = 1000
    
    def __init__(self, device, parameters):
        """
        Initialize the optimizer with a device and parameters to optimize.
        
        Args:
            device: The optical device to optimize (must have compute_demerit method)
            parameters: List of dictionaries with keys:
                - dValue: current value
                - dMin: minimum value
                - dMax: maximum value
                - dResolution: resolution for stopping criteria
        """
        self._pDevice = device
        self._parameters = deepcopy(parameters)
        
    def optimize(self):
        """
        Perform optimization using Nelder-Mead method.
        
        Returns:
            str: Optimization result ('eBetterSolutionFound' or 'eNoBetterSolution')
        """
        # see algorithm at: https://en.wikipedia.org/wiki/Nelder%E2%80%93Mead_method
        assert self._pDevice is not None
        
        dMeritOrig = self.compute_demerit()
        deviceOrig = deepcopy(self._pDevice)
        
        if not self._parameters:
            return 'eNothingToOptimize'
        
        # init simplex with the center of the definition domain
        n_params = len(self._parameters)
        simplex = [deepcopy(self._parameters) for _ in range(n_params + 1)]
        
        # set center point
        for i in range(n_params):
            simplex[0][i]['dValue'] = (simplex[0][i]['dMin'] + simplex[0][i]['dMax']) * 0.5
        
        # init simplex with the semi edge of the definition domain
        for i in range(1, len(simplex)):
            simplex[i] = deepcopy(simplex[0])
            # expand the i-1 axis
            simplex[i][i-1]['dValue'] += (simplex[i][i-1]['dMax'] - simplex[i][i-1]['dMin']) / 8.
        
        # compute solution at each param
        vdDemerit = [0.] * len(simplex)
        for i in range(len(simplex)):
            self.apply_parameter(simplex[i])
            vdDemerit[i] = self.compute_demerit()
        
        iIter = 0
        bStopCriteria = False
        iBest = 0
        dBestMerit = vdDemerit[0]
        
        while iIter < self.AMOEBA_MAX_ITER and not bStopCriteria:
            # get the best, worse and secondworse solution index
            dBestMerit = vdDemerit[0]
            iBest = 0
            iWorse = 0
            dWorse = vdDemerit[0]
            iSecondWorse = 0
            dSecondWorse = vdDemerit[0]
            
            for i in range(1, len(simplex)):
                if vdDemerit[i] < dBestMerit:
                    iBest = i
                    dBestMerit = vdDemerit[i]
                
                if vdDemerit[i] > dWorse:
                    iWorse = i
                    dWorse = vdDemerit[i]
            
            # find the second worse
            for i in range(1, len(simplex)):
                if i != iWorse and vdDemerit[i] > dSecondWorse:
                    dSecondWorse = vdDemerit[i]
            
            assert dBestMerit <= dSecondWorse
            assert dSecondWorse <= dWorse
            
            # search the best solution on the line (worse,mean)
            paramWorse = simplex[iWorse]
            
            # compute param mean
            paramMean = deepcopy(paramWorse)
            for i in range(len(paramMean)):
                paramMean[i]['dValue'] = 0.
            
            for iS in range(len(simplex)):
                if iS == iWorse:
                    continue
                
                param = simplex[iS]
                for i in range(len(param)):
                    paramMean[i]['dValue'] += param[i]['dValue']
            
            for i in range(len(paramMean)):
                paramMean[i]['dValue'] /= (len(simplex) - 1)
            
            bFound = False
            
            # reflection case
            paramMirror = deepcopy(paramMean)
            bOutOfDomain = False
            for i in range(len(paramMirror)):
                paramMirror[i]['dValue'] += (paramMean[i]['dValue'] - paramWorse[i]['dValue'])
                if (paramMirror[i]['dValue'] < paramMirror[i]['dMin'] or 
                    paramMirror[i]['dValue'] > paramMirror[i]['dMax']):
                    bOutOfDomain = True
            
            dMirror = self.SPOT_SIZE_INFINITY
            if not bOutOfDomain:
                self.apply_parameter(paramMirror)
                dMirror = self.compute_demerit()
            
            if dBestMerit <= dMirror < dSecondWorse:
                # replace Worst by mirror
                simplex[iWorse] = paramMirror
                vdDemerit[iWorse] = dMirror
                bFound = True
            
            # expansion
            if not bFound and dMirror < dBestMerit:
                # compute paramMirrorFar
                paramMirrorFar = deepcopy(paramMean)
                bOutOfDomain2 = False
                for i in range(len(paramMirrorFar)):
                    paramMirrorFar[i]['dValue'] += 2 * (paramMirror[i]['dValue'] - paramMean[i]['dValue'])
                    if (paramMirrorFar[i]['dValue'] < paramMirrorFar[i]['dMin'] or 
                        paramMirrorFar[i]['dValue'] > paramMirrorFar[i]['dMax']):
                        bOutOfDomain2 = True
                
                dMirrorFar = self.SPOT_SIZE_INFINITY
                if not bOutOfDomain2:
                    self.apply_parameter(paramMirrorFar)
                    dMirrorFar = self.compute_demerit()
                
                if dMirrorFar < dBestMerit:
                    simplex[iWorse] = paramMirrorFar
                    vdDemerit[iWorse] = dMirrorFar
                    bFound = True
                else:
                    simplex[iWorse] = paramMirror
                    vdDemerit[iWorse] = dMirror
                    bFound = True
            
            # contraction
            if not bFound:
                assert dMirror >= dSecondWorse
                paramMirrorContracted = deepcopy(paramMean)
                for i in range(len(paramMirrorContracted)):
                    paramMirrorContracted[i]['dValue'] += 0.5 * (paramWorse[i]['dValue'] - paramMean[i]['dValue'])
                
                self.apply_parameter(paramMirrorContracted)
                dContracted = self.compute_demerit()
                if dContracted < dWorse:
                    simplex[iWorse] = paramMirrorContracted
                    vdDemerit[iWorse] = dContracted
                    bFound = True
            
            # reduction
            if not bFound:
                for i in range(len(simplex)):
                    paramBest = simplex[iBest]
                    if i != iBest:
                        paramReducted = simplex[i]
                        for j in range(len(paramReducted)):
                            paramReducted[j]['dValue'] = (paramBest[j]['dValue'] + 
                                                        0.5 * (paramReducted[j]['dValue'] - paramBest[j]['dValue']))
                        
                        self.apply_parameter(paramReducted)
                        vdDemerit[i] = self.compute_demerit()
                bFound = True  # keep for debug
            
            assert bFound
            
            # compute the parameter range
            # stop criteria based on the simplex size
            vdMinParam = [self.SPOT_SIZE_INFINITY] * len(self._parameters)
            vdMaxParam = [-self.SPOT_SIZE_INFINITY] * len(self._parameters)
            
            for i in range(len(simplex)):
                param = simplex[i]
                for j in range(len(param)):
                    if param[j]['dValue'] < vdMinParam[j]:
                        vdMinParam[j] = param[j]['dValue']
                    if param[j]['dValue'] > vdMaxParam[j]:
                        vdMaxParam[j] = param[j]['dValue']
            
            bStopCriteria = True
            for i in range(len(vdMinParam)):
                if vdMaxParam[i] - vdMinParam[i] > self._parameters[i]['dResolution']:
                    bStopCriteria = False
            
            iIter += 1
        
        if dBestMerit < self.SPOT_SIZE_INFINITY / 2:
            if dBestMerit < dMeritOrig and iIter >= 0:
                self._parameters = deepcopy(simplex[iBest])
                self.apply_parameter(self._parameters)
                return 'eBetterSolutionFound'
        
        # restore device original settings
        self._pDevice = deviceOrig
        return DeviceOptimizer.eNoBetterSolution
    
    def compute_demerit(self):
        """Compute the demerit (error) value for the current device state."""
        return self._pDevice.compute_demerit()
    
    def apply_parameter(self, parameters):
        """Apply the given parameters to the device."""
        for i, param in enumerate(parameters):
            # This assumes the device has a way to set parameters by index
            # You may need to modify this based on your actual device implementation
            self._pDevice.set_parameter(i, param['dValue'])
