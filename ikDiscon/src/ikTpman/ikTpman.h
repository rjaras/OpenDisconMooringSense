/*
 * (c) IKERLAN
 * All rights reserved
 */

/* 
 * File:   ikTpman.h
 * Author: ielorza
 *
 * Created on 16 de febrero de 2017, 10:43
 */

/**
 * @file ikTpman.h
 * 
 * @brief Class ikTpman interface
 */

#ifndef IKTPMAN_H
#define IKTPMAN_H

#ifdef __cplusplus
extern "C" {
#endif
    
#include "../ikLutbl/ikLutbl.h"
    
    /**
     * @struct ikTpman
     * @brief Torque-pitch manager
     * 
     * Instances of this type are torque-pitch managers of the sort used in
     * pitch-controlled variable speed wind turbine control. They take the
     * generator torque, the collective pitch due to the speed error and the
     * collective pitch introduced for platform stabilisation, and generate the
     * torque and pitch limits for the speed regulation loops.
     * 
     * @par Inputs
     * @li torque: generator torque in kN, specify via @link ikTpman_step @endlink
     * @li maximum torque: upper torque limit for speed regulation, in kN, specify via @link ikTpman_step @endlink
     * @li external minimum torque: externally imposed lower torque limit for speed regulation, in kN, specify via @link ikTpman_step @endlink
     * @li pitch from speed: pitch angle due to speed regulation loop, in degrees, specify via @link ikTpman_step @endlink
     * @li pitch from platform: pitch angle due to platform stabiliser loop, in degrees, specify via @link ikTpman_step @endlink
     * @li maximum pitch: upper overall pitch angle limit, in degrees, specify via @link ikTpman_step @endlink
     * @li external minimum pitch: externally imposed lower overall pitch angle limit, in degrees, specify via @link ikTpman_step @endlink
     * 
     * @par Outputs
     * @li maximum pitch from speed: upper pitch limit for speed regulation, in degrees, get via @link ikTpman_getOutput @endlink
     * @li minimum pitch: lower overall pitch angle limit, in degrees, get via @link ikTpman_getOutput @endlink
     * @li maximum pitch from platform: upper pitch limit for platform stabilisation, in degrees, get via @link ikTpman_getOutput @endlink
     * @li minimum pitch from platform: lower pitch limit for platform stabilisation, in degrees, get via @link ikTpman_getOutput @endlink
     * @li minimum torque: lower torque limit for speed regulation, in kN, get via @link ikTpman_getOutput @endlink
     * 
     * @par Unit block
     * 
     * @image html ikTpman_unit_block.svg
     * @image latex ikTpman_unit_block.pdf
     * 
     * @par Block diagram
     * 
     * @image html ikTpman_block_diagram.svg
     * @image latex ikTpman_block_diagram.pdf
     * 
     * @par State machine
     * 
     * @image html ikTpman_state_machine.svg
     * @image latex ikTpman_state_machine.pdf
     * 
     * @par Methods
     * @li @link ikTpman_initParams @endlink initialise initialisation parameter structure
     * @li @link ikTpman_init @endlink initialise an instance
     * @li @link ikTpman_step @endlink execute periodic calculations
     * @li @link ikTpman_getOutput @endlink get output value
     */
    typedef struct ikTpman {
        /**
         * Private members
         */
        // @cond
        int state;
        double minTorque;
        double minPitch;
        double maxPitchPltfrm;
        double minPitchPltfrm;
        double maxPitchSpeed;
        ikLutbl * minPitchTbl;
        double maxPitch;
        double minPitchExt;
        double torque;
        double pitchSpeed;
        double pitchPlatform;
        double minTorqueExt;
        double maxTorque;
        // @endcond
    } ikTpman;
    
    /**
     * @struct ikTpmanParams
     * @brief Torque-pitch manager initialisation parameters
     */
    typedef struct ikTpmanParams {
        ikLutbl * minPitchTbl; /**< pointer to minimum pitch table.
                                * If set to NULL, minimum pitch equals
                                * external minimum pitch.*/
    } ikTpmanParams;
    
    /**
     * Initialise an instance
     * @param self instance
     * @param params initialisation parameters
     * @return error code:
     * @li 0: no error
     */
    int ikTpman_init(ikTpman *self, const ikTpmanParams *params);
    
    /**
     * Initialise initialisation parameter structure
     * @param params initialisation parameter structure
     */
    void ikTpman_initParams(ikTpmanParams *params);
    
    /**
     * Execute periodic calculations
     * @param self torque-pitch manager instance
     * @param torque generator torque in kN
     * @param maxTorque upper torque limit for speed regulation, in kN
     * @param minTorqueExt externally imposed lower torque limit for speed regulation, in kN
     * @param pitchSpeed pitch angle due to speed regulation loop, in degrees
     * @param pitchPlatform pitch angle due to platform stabiliser loop, in degrees
     * @param maxPitch upper overall pitch angle limit, in degrees
     * @param minPitchExt externally imposed lower overall pitch angle limit, in degrees
     * @return state
     * @li 0: below rated
     * @li 1: above rated
     */
    int ikTpman_step(ikTpman *self, double torque, double maxTorque, double minTorqueExt, double pitchSpeed, double pitchPlatform, double maxPitch, double minPitchExt);
    
    /**
     * Get output value by name. All signals named on the block diagram of
     * @link ikTpman @endlink are accessible.
     * @param self torque-pitch manager instance
     * @param output output value
     * @param name output name
     * @return error code:
     * @li 0: no error
     * @li -1: invalid signal name
     * @li -2: invalid block name
     */
    int ikTpman_getOutput(const ikTpman *self, double *output, const char *name);


#ifdef __cplusplus
}
#endif

#endif /* IKTPMAN_H */

