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
     * Instances of this type are power managers of the sort used in
     * pitch-controlled variable speed wind turbine control. They take the
     * maximum power command, the maximum speed command and power reserve command,
     * and generate the maximum torque command for the torque-pitch manager, as
     * well as configure the preferred torque and minimum pitch lookup tables.
     * 
     * @par Inputs
     * @li maximum power: maximum power command in kW
     * @li maximum speed: maximum generator speed command in rad/s
     * @li reserve power: reserve power command in kW
     * 
     * @par Outputs
     * @li maximum torque: maximum torque command in kNm
     * 
     * @par Unit block
     * 
     * @image html ikPowman_unit_block.svg
     * @image latex ikPowman_unit_block.pdf
     * 
     * @par Block diagram
     * 
     * @image html ikPowman_block_diagram.svg
     * @image latex ikPowman_block_diagram.pdf
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
        double powLim;
        double maxPower;
        double maxSpeed;
        double resPower;
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
     * @param resPower reserve power command in kW
     * @return maximum torque in kN
     */
    double ikPowman_step(ikPowman *self, double maxPower, double maxSpeed, double resPower);
    
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

