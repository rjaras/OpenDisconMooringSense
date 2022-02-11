/*
  Copyright (C) 2017, 2021 IKERLAN

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
 * @file ikMooringSenseWTCon.h
 * 
 * @brief Class ikMooringSenseWTCon interface
 */

#ifndef IKSIMPLEWTCON_H
#define IKSIMPLEWTCON_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ikConLoop.h"
#include "ikTpman.h"
#include "ikPowman.h"
#include "ikThrustLim.h"
#include "ikTsrEst.h"
#include "ikThrustLimEst.h"

    /**
     * @struct ikMooringSenseWTConInputs
     * @brief controller inputs
     */
    typedef struct ikMooringSenseWTConInputs {
        double externalMaximumTorque; /**<external maximum torque in kNm*/
        double externalMinimumTorque; /**<external minimum torque in kNm*/
        double externalMaximumPitch; /**<external maximum pitch in degrees*/
        double externalMinimumPitch; /**<external minimum pitch in degrees*/
        double externalMaximumPitchRate; /**<external maximum pitch rate in degrees per second*/
        double externalMinimumPitchRate; /**<external minimum pitch rate in degrees per second*/
        double maximumSpeed; /**<maximum generator speed setpoing in rad/s*/
        double generatorSpeed; /**<generator speed in rad/s*/
        double nacellePitch; /**<nacelle pitch angle in rad*/
        double generatorTorque; /**<measured generator torque in kNm*/
        double externalMaximumThrust;  /**<minimum allowed thrust in kN*/
        double turretXPosition; /**<turret X position in m*/
        double turretYPosition; /**<turret Y position in m*/
        double nacelleYawAbs; /**<nacelle absolute yaw angle (in global XY axis) in rad*/
        double thrust; /**<estimated thrust in kN*/
        double collectivePitchAngle; /**<collective blade pitch angle in degrees*/
    } ikMooringSenseWTConInputs;

    /**
     * @struct ikMooringSenseWTConOutputs
     * @brief controller outputs
     */
    typedef struct ikMooringSenseWTConOutputs {
        double torqueDemand; /**<torque demand in kNm*/
        double pitchDemand; /**<pitch demand in degrees*/
    } ikMooringSenseWTConOutputs;

    /* @cond */

    typedef struct ikMooringSenseWTConPrivate {
        ikPowman powerManager;
        ikTpman   tpManager;
        ikConLoop dtdamper;
        ikConLoop torquecon;
        ikConLoop colpitchcon;
        ikTfList torqueFromHubPitchTf;
        ikThrustLim thurstLimiter;
        ikTsrEst tsrEstimator;
        ikThrustLimEst thurstLimitEstimator;
        double maxPitch;
        double minPitch;
        double maxSpeed;
        int tpManState;
        double maxTorque;
        double minTorque;
        double torqueFromDtdamper;
        double torqueFromTorqueCon;
        double belowRatedTorque;
        double minPitchFromPowman;
        double maxTorqueFromPowman;
        double torqueFromHubPitch;
        double tipSpeedRatio;
        double rotorSpeedFiltered;
        double minPitchFromThrustLim;
        double thrustLimit;
    } ikMooringSenseWTConPrivate;
    /* @endcond */

    /**
     * @struct ikMooringSenseWTCon
     * @brief Main controller class
     * 
     * This is a simple wind turbine controller
     * 
     * @par Inputs
     * @li external maximum torque: externally set upper torque limit, in kNm, specify via @link ikMooringSenseWTConInputs.externalMaximumTorque @endlink at @link in @endlink
     * @li external minimum torque: externally set lower torque limit, in kNm, specify via @link ikMooringSenseWTConInputs.externalMinimumTorque @endlink at @link in @endlink
     * @li external maximum pitch: externally set upper pitch limit, in degrees, specify via @link ikMooringSenseWTConInputs.externalMaximumPitch @endlink at @link in @endlink
     * @li external minimum pitch: externally set lower pitch limit, in degrees, specify via @link ikMooringSenseWTConInputs.externalMinimumPitch @endlink at @link in @endlink
     * @li external maximum pitch rate: externally set upper pitch rate limit, in degrees per second, specify via @link ikMooringSenseWTConInputs.externalMaximumPitchRate @endlink at @link in @endlink
     * @li external minimum pitch rate: externally set lower pitch rate limit, in degrees per second, specify via @link ikMooringSenseWTConInputs.externalMinimumPitchRate @endlink at @link in @endlink
     * @li maximum speed: maximum generator speed setpoint, in rad/s, specify via @link ikMooringSenseWTConInputs.maximumSpeed @endlink at @link in @endlink
     * @li generator speed: current generator speed, in rad/s, specify via @link ikMooringSenseWTConInputs.generatorSpeed @endlink at @link in @endlink
     * @li nacelle pitch: current nacelle pitch angle, in rad, specify via @link ikMooringSenseWTConInputs.nacellePitch @endlink at @link in @endlink
     * @li external maximum thrust: current generator torque, in kNm, specify via @link ikMooringSenseWTConInputs.generatorTorque @endlink at @link in @endlink
     * @li turret X position: current turret X position, in m, specify via @link ikMooringSenseWTConInputs.turretXPosition @endlink at @link in @endlink
     * @li turret Y position: current turret Y position, in m, specify via @link ikMooringSenseWTConInputs.turretYPosition @endlink at @link in @endlink
     * @li thrust: current thrust force estimated by the observer, in kN, specify via @link ikMooringSenseWTConInputs.thrust @endlink at @link in @endlink
     * 
     * @par Outputs
     * @li torque demand: in kNm, get via @link ikMooringSenseWTConOutputs.torqueDemand @endlink at @link out @endlink
     * @li pitch demand in degrees, get via @link ikMooringSenseWTConOutputs.pitchDemand @endlink at @link out @endlink
     * 
     * @par Unit block
     * 
     * @image html ikMooringSenseWTCon_unit_block.svg
     * 
     * @par Block diagram
     * 
     * @image html ikMooringSenseWTCon_block_diagram.svg
     * 
     * @par Public members
     * @li @link in @endlink inputs
     * @li @link out @endlink outputs
     * 
     * @par Methods
     * @li @link ikMooringSenseWTCon_initParams @endlink initialise initialisation parameter structure
     * @li @link ikMooringSenseWTCon_init @endlink initialise an instance
     * @li @link ikMooringSenseWTCon_step @endlink execute periodic calculations
     * @li @link ikMooringSenseWTCon_getOutput @endlink get output value
     * 
     */
    typedef struct ikMooringSenseWTCon {
        ikMooringSenseWTConInputs in; /**<inputs*/
        ikMooringSenseWTConOutputs out; /**<outputs*/
        /* @cond */
        ikMooringSenseWTConPrivate priv;
        /* @endcond */
    } ikMooringSenseWTCon;

    /**
     * @struct ikMooringSenseWTConParams
     * @brief controller initialisation parameters
     */
    typedef struct ikMooringSenseWTConParams {
        ikConLoopParams drivetrainDamper; /**<drivetrain damper initialisation parameters*/
        ikConLoopParams torqueControl; /**<torque control initialisation parameters*/
        ikConLoopParams collectivePitchControl; /**<collective pitch control initialisation parameters*/
        ikTpmanParams torquePitchManager; /**<torque-pitch manager inintialisation parameters*/
        ikPowmanParams powerManager; /**<power manager initialisation parameters*/
        ikTfListParams torqueFromHubPitchTf; /**<torque value related to platform pitch transfer function parameters*/
        ikThrustLimParams thurstLimiter; /**<thrust limiter initialisation parameters*/
        ikTsrEstParams tsrEstimator; /**<tip-speed ratio estimator initialisation parameters*/
        ikThrustLimEstParams thurstLimitEstimator; /**<thrust limiter initialisation parameters*/
    } ikMooringSenseWTConParams;

    /**
     * Initialise a controller instance
     * @param self instance
     * @param params initialisation parameters
     * @return error code:
     * @li 0: no error
     * @li -1: drivetrain damper initialisation failed
     * @li -2: torque control initialisation failed
     * @li -3: collective pitch control initialisation failed
     * @li -5: torque-pitch manager initialisation failed
     * @li -6: power manager initialisation failed
     * @li -7: torque component from nacelle pitch transfer function initialisation failed
     * @li -8: tip-speed ratio estimator initialisation failed
     * @li -9: thrust limiter initialisation failed
     * @li -10: thrust limit estimator initialisation failed
     */
    char* ikMooringSenseWTCon_init(ikMooringSenseWTCon *self, const ikMooringSenseWTConParams *params);

    /**
     * Initialise initialisation parameter structure
     * @param params initialisation parameter structure
     */
    void ikMooringSenseWTCon_initParams(ikMooringSenseWTConParams *params);

    /**
     * Execute periodic calculations
     * @param self controller instance
     * @return state
     * @li 0: below rated
     * @li 1: above rated
     */
    int ikMooringSenseWTCon_step(ikMooringSenseWTCon *self);

    /**
     * Get output value by name. All signals named on the block diagram of
     * @link ikMooringSenseWTCon @endlink are accessible, except for inputs and outputs,
     * which are available at @link ikMooringSenseWTCon.in @endlink and
     * @link ikMooringSenseWTCon.out @endlink, respectively. To refer to sub-block
     * signals, use the sub-block name followed by a ">" character and the
     * signal name. For example:
     * @li to access the torque demand from the drivetrain damper, use "torque demand from drivetrain damper"
     * @li to access the torque control control action, use "torque control>control action"
     * 
     * @param self controller instance
     * @param output output value
     * @param name output name, NULL terminated string
     * @return error code:
     * @li 0: no error
     * @li -1: invalid signal name
     * @li -2: invalid block name
     */
    int ikMooringSenseWTCon_getOutput(const ikMooringSenseWTCon *self, double *output, const char *name);



#ifdef __cplusplus
}
#endif

#endif /* IKSIMPLEWTCON_H */

