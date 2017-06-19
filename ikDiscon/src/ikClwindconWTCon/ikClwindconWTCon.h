/*
 * (c) IKERLAN
 * All rights reserved
 */

/* 
 * File:   ikClwindconWTCon.h
 * Author: ielorza
 *
 * Created on 19 de junio de 2017, 15:37
 */

/**
 * @file ikClwindconWTCon.h
 * 
 * @brief Class ikClwindconWTCon interface
 */

#ifndef IKCLWINDCONWTCON_H
#define IKCLWINDCONWTCON_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ikConLoop.h"
#include "ikTpman.h"
#include "ikPowman.h"

    /**
     * @struct ikClwindconWTConInputs
     * @brief controller inputs
     */
    typedef struct ikClwindconWTConInputs {
        double pitchBlade1; /**<blade 1 pitch angle in degrees*/
        double pitchBlade2; /**<blade 2 pitch angle in degrees*/
        double pitchBlade3; /**<blade 3 pitch angle in degrees*/
        double generatorTorque; /**<generator torque in kNm*/
        double generatorSpeed; /**<generator speed in rad/s*/
        double maximumPower; /**<maximum power setpoint in kW*/
    } ikClwindconWTConInputs;

    /**
     * @struct ikClwindconWTConOutputs
     * @brief controller outputs
     */
    typedef struct ikClwindconWTConOutputs {
        double torqueDemand; /**<torque demand in kNm*/
        double pitchDemandBlade1; /**<pitch demand for blade 1 in degrees*/
        double pitchDemandBlade2; /**<pitch demand for blade 2 in degrees*/
        double pitchDemandBlade3; /**<pitch demand for blade 3 in degrees*/
    } ikClwindconWTConOutputs;

    // @cond

    typedef struct ikClwindconWTConPrivate {
        //ikSsproc ssproc;
        ikPowman  powerManager;
        ikTpman   tpManager;
        ikConLoop dtdamper;
        ikConLoop torquecon;
        ikConLoop colpitchcon;
        double collectivePitch;
        double maxPitch;
        double minPitch;
        double maxSpeed;
        double maxTorqueFromPowerMan;
        double maxTorque;
        int tpManState;
        double maxPitchFromSpeed;
        double minPitchFromSpeed;
        double minTorque;
        double torqueFromDtdamper;
        double torqueFromTorqueCon;
        double collectivePitchFromPitchControl;
        double collectivePitchDemand;
    } ikClwindconWTConPrivate;
    // @endcond

    /**
     * @struct ikClwindconWTCon
     * @brief Main controller class
     * 
     * Instances of this type are variable pitch, variable speed wind turbine
     * controllers, which take generator speed and power measurements, as well
     * as measurements from other turbine sensors, and produce generator torque
     * and blade pitch demands.
     * 
     * @par Inputs
     * @li pitch angle at blade 1: current pitch angle at blade 1, specify via @link ikClwindconWTConInputs.pitchBlade1 @endlink at @link in @endlink
     * @li pitch angle at blade 2: current pitch angle at blade 2, specify via @link ikClwindconWTConInputs.pitchBlade2 @endlink at @link in @endlink
     * @li pitch angle at blade 3: current pitch angle at blade 3, specify via @link ikClwindconWTConInputs.pitchBlade3 @endlink at @link in @endlink
     * @li generator torque: current generator torque, specify via @link ikClwindconWTConInputs.generatorTorque @endlink at @link in @endlink
     * @li generator speed: current generator speed, specify via @link ikClwindconWTConInputs.generatorSpeed @endlink at @link in @endlink
     * @li maximum power: maximum power setpoint, specify via @link ikClwindconWTConInputs.maximumPower @endlink at @link in @endlink
     * 
     * @par Outputs
     * @li torque demand: in kNm, get via @link ikClwindconWTConOutputs.torqueDemand @endlink at @link out @endlink
     * @li pitch demand for blade 1: in degrees, get via @link ikClwindconWTConOutputs.pitchDemandBlade1 @endlink at @link out @endlink
     * @li pitch demand for blade 2: in degrees, get via @link ikClwindconWTConOutputs.pitchDemandBlade2 @endlink at @link out @endlink
     * @li pitch demand for blade 3: in degrees, get via @link ikClwindconWTConOutputs.pitchDemandBlade3 @endlink at @link out @endlink
     * 
     * @par Unit block
     * 
     * @image html ikClwindconWTCon_unit_block.svg
     * @image latex ikClwindconWTCon_unit_block.pdf
     * 
     * @par Block diagram
     * 
     * @image html ikClwindconWTCon_block_diagram.svg
     * @image latex ikClwindconWTCon_block_diagram.pdf
     * 
     * @par Public members
     * @li @link in @endlink inputs
     * @li @link out @endlink outputs
     * 
     * @par Methods
     * @li @link ikClwindconWTCon_initParams @endlink initialise initialisation parameter structure
     * @li @link ikClwindconWTCon_init @endlink initialise an instance
     * @li @link ikClwindconWTCon_step @endlink execute periodic calculations
     * @li @link ikClwindconWTCon_getOutput @endlink get output value
     * 
     */
    typedef struct ikClwindconWTCon {
        ikClwindconWTConInputs in; /**<inputs*/
        ikClwindconWTConOutputs out; /**<outputs*/
        // @cond
        ikClwindconWTConPrivate priv;
        // @endcond
    } ikClwindconWTCon;

    /**
     * @struct ikClwindconWTConParams
     * @brief controller initialisation parameters
     */
    typedef struct ikClwindconWTConParams {
        ikConLoopParams drivetrainDamper; /**<drivetrain damper initialisation parameters*/
        ikConLoopParams torqueControl; /**<torque control initialisation parameters*/
        ikConLoopParams collectivePitchControl; /**<collective pitch control initialisation parameters*/
        ikTpmanParams torquePitchManager; /**<torque-pitch manager inintialisation parameters*/
        ikPowmanParams powerManager; /**<power manager initialisation parameters*/
    } ikClwindconWTConParams;

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
     */
    int ikClwindconWTCon_init(ikClwindconWTCon *self, const ikClwindconWTConParams *params);

    /**
     * Initialise initialisation parameter structure
     * @param params initialisation parameter structure
     */
    void ikClwindconWTCon_initParams(ikClwindconWTConParams *params);

    /**
     * Execute periodic calculations
     * @param self controller instance
     * @return status
     */
    int ikClwindconWTCon_step(ikClwindconWTCon *self);

    /**
     * Get output value by name. All signals named on the block diagram of
     * @link ikClwindconWTCon @endlink are accessible, except for inputs and outputs,
     * which are available at @link ikClwindconWTCon.in @endlink and
     * @link ikClwindconWTCon.out @endlink, respectively. To refer to sub-block
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
    int ikClwindconWTCon_getOutput(const ikClwindconWTCon *self, double *output, const char *name);



#ifdef __cplusplus
}
#endif

#endif /* IKCLWINDCONWTCON_H */

