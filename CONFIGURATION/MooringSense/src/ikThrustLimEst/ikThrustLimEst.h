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
 * @file ikThrustLimEst.h
 * 
 * @brief Class ikThrustLimEst interface
 */

#ifndef IKTHRUSTLIMEST_H
#define IKTHRUSTLIMEST_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ik2DLutbl.h"

    /**
     * @struct ikThrustLimEst
     * @brief Thrust limit estimator
     * 
     * Instances of this type are thrust limit estimators. They take the
     * turret position and the observer calculated thrust and calculate 
     * a maximum thrust command value.
     * 
     * @par Inputs
     * @li thrust from the observer in kN
     * @li turret X position in m
     * @li turret Y position in m
     * @li nacelle absolute yaw angle (in global XY axis) in deg
     * @li current estimated thrust from observer in kN
     * 
     * @par Outputs
     * @li maximum thrust command in kN
     * 
     * @par Unit block
     * 
     * @image html ikThrustLimEst_unit_block.svg
     * 
     * @par Block diagram
     * 
     * @image html ikThrustLimEst_block_diagram.svg
     * 
     * @par Methods
     * @li @link ikThrustLimEst_new @endlink construct new instance
     * @li @link ikThrustLimEst_eval @endlink evaluate maximum thrust
     * @li @link ikThrustLimEst_delete @endlink delete instance
     */
    typedef struct ikThrustLimEst {
            /**
             * Private members
             */
             /* @cond */
        ik2DLutbl *surfMaxThurst;  /**<surface giving maximum thrust value as a function of x and y position*/
        double maximumThrust;
            /* @endcond */
    } ikThrustLimEst;
    
    /**
     * @struct ikThrustLimEstParams
     * @brief Thrust limiter initialisation parameters
     */
    typedef struct ikThrustLimEstParams {
        const char* maxThrustSurfaceFileName; /**<name of a valid file for @link ikSurf_newf @endlink*/
    } ikThrustLimEstParams;
    
    /**
     * Initialise an instance
     * @param self instance
     * @param params initialisation parameters
     * @return error message
     * @li "": no error*
     * @li "...": error message stating the failing submodule (maximum thrust surface initialization error)
     */
    char* ikThrustLimEst_init(ikThrustLimEst *self, const ikThrustLimEstParams *params);
    
    /**
     * Initialise initialisation parameter structure
     * @param params initialisation parameter structure
     */
    void ikThrustLimEst_initParams(ikThrustLimEstParams *params);
    
    /**
     * Execute periodic estimation
     * @param self thrust limit estimator instance
     * @param xPosition turret x position in m
     * @param yPosition turret y position in m
     * @param nacelleYawAbs nacelle absolute yaw angle (in global XY axis) in deg
     * @param thurst current estimated thrust from observer in kN
     * @return maximum thrust
     */
    double ikThrustLimEst_eval(ikThrustLimEst *self, double xPosition, double yPosition, double nacelleYawAbs, double thrust);
    
    /**
     * Get output value by name.
     *
     * @param self thrust limit estimator instance
     * @param output output value
     * @param name output name
     * @return error code:
     * @li 0: no error
     * @li -1: invalid signal name
     */
    int ikThrustLimEst_getOutput(const ikThrustLimEst *self, double *output, const char *name);

    /**
     * Delete instance
     * @param self thrust limit estimator instance
     */
    void ikThrustLimEst_delete(ikThrustLimEst *self);


#ifdef __cplusplus
}
#endif

#endif /* IKTHRUSTLIMEST_H */

