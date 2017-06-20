/*
 * (c) IKERLAN
 * All rights reserved
 */

/* 
 * File:   ikPowman.h
 * Author: ielorza
 *
 * Created on 21 de febrero de 2017, 16:32
 */

/**
 * @file ikPowman.h
 * 
 * @brief Class ikPowman interface
 */

#ifndef IKPOWMAN_H
#define IKPOWMAN_H

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * @struct ikPowman
     * @brief Power manager
     * 
     * This is the power manager
     * 
     * @par Inputs
     * @li maximum power: maximum power command in kW
     * @li maximum speed: maximum generator speed command in rad/s
     * 
     * @par Outputs
     * @li maximum torque: maximum torque command in kNm
     * 
     * @par Unit block
     * 
     * @image html ikPowman_unit_block.svg
     * 
     * @par Block diagram
     * 
     * @image html ikPowman_block_diagram.svg
     * 
     * @par Methods
     * @li @link ikPowman_initParams @endlink initialise initialisation parameter structure
     * @li @link ikPowman_init @endlink initialise an instance
     * @li @link ikPowman_step @endlink execute periodic calculations
     * @li @link ikPowman_getOutput @endlink get output value
     */
    typedef struct ikPowman {
        /**
         * Private members
         */
        // @cond
        double maxPower;
        double maxSpeed;
        double maxTorque;
        // @endcond
    } ikPowman;
    
    /**
     * @struct ikPowmanParams
     * @brief Power manager initialisation parameters
     */
    typedef struct ikPowmanParams {
        // @cond
        int foo;
        // @endcond
    } ikPowmanParams;
    
    /**
     * Initialise an instance
     * @param self instance
     * @param params initialisation parameters
     * @return error code:
     * @li 0: no error
     */
    int ikPowman_init(ikPowman *self, const ikPowmanParams *params);
    
    /**
     * Initialise initialisation parameter structure
     * @param params initialisation parameter structure
     */
    void ikPowman_initParams(ikPowmanParams *params);
    
    /**
     * Execute periodic calculations
     * @param self power manager instance
     * @param maxPower maximum power command in kW
     * @param maxSpeed maximum generator speed command in rad/s
     * @return maximum torque in kN
     */
    double ikPowman_step(ikPowman *self, double maxPower, double maxSpeed);
    
    /**
     * Get output value by name. All signals named on the block diagram of
     * @link ikPowman @endlink are accessible.
     * @param self power manager instance
     * @param output output value
     * @param name output name
     * @return error code:
     * @li 0: no error
     * @li -1: invalid signal name
     * @li -2: invalid block name
     */
    int ikPowman_getOutput(const ikPowman *self, double *output, const char *name);


#ifdef __cplusplus
}
#endif

#endif /* IKPOWMAN_H */

