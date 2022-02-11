/*
  Copyright (C) 2021-2022 IKERLAN

  This file is part of OpenDiscon.

  OpenDiscon is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  OpenDiscon is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with OpenDiscon. If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file ikThrustLimEst.c
 * 
 * @brief Class ikThrustLimEst implementation
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "ikThrustLimEst.h"

char* ikThrustLimEst_init(ikThrustLimEst* self, const ikThrustLimEstParams* params) {
    const char* errStr;
    char* outputErrorMessage = malloc(100 * sizeof(char));

    /* pass parameters on */
    
	
    /*construct maximum thrust surface*/
    errStr = ik2DLutbl_init(&(self->surfMaxThurst), params->maxThrustSurfaceFileName);
    if (strlen(errStr)) {
        snprintf(outputErrorMessage, 100, "Error initializing max thrust surface: %s", errStr);
        return outputErrorMessage;
    }

    return "";
}

void ikThrustLimEst_initParams(ikThrustLimEstParams *params) {
    params->maxThrustSurfaceFileName = "thrustLimitSurface.bin";
}

double ikThrustLimEst_eval(ikThrustLimEst* self, double xPosition, double yPosition, double nacelleYawAbs, double thrust) {
    double tableInX, tableInY, turretDisplacement;
    // double thrustDiff, thrustDiffPercentage;

    turretDisplacement = sqrt(xPosition * xPosition + yPosition * yPosition);
    tableInX = (double)turretDisplacement * cos(nacelleYawAbs);
    tableInY = (double)turretDisplacement * sin(nacelleYawAbs);
    self->maximumThrust = ik2DLutbl_eval(self->surfMaxThurst, tableInX, tableInY) * 1E-3 * 1.1; // N to kN
    
    /*
    if (thrust != 0.0) {
        thrustDiff = thrust - self->maximumThrust;
        thrustDiffPercentage = thrustDiff / thrust * 100.0;
        if (thrustDiffPercentage > 50.0) {
            self->maximumThrust = (self->maximumThrust + thrust) / 2;
        }
    }
    */

    return self->maximumThrust;
}

int ikThrustLimEst_getOutput(const ikThrustLimEst *self, double *output, const char *name) {
    /* pick up the signal names */
    if (!strcmp(name, "maximum thrust")) {
        *output = self->maximumThrust;
        return 0;
    }
    
    return -1;
}

void ikThrustLimEst_delete(ikThrustLimEst *self) {
    ik2DLutbl_delete(self->surfMaxThurst);
}
