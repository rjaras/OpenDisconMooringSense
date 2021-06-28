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
 * @file ikPowman.h
 * 
 * @brief Class ikPowman interface
 */

#ifndef IKPOWMAN_H
#define IKPOWMAN_H

#ifdef __cplusplus
extern "C" {
#endif
    
#include "ikLutbl.h"
#include "ikTfList.h"
    
    /**
     * @struct ikPowman
     * @brief Power manager
     * 
     * This is the above-below rated state machine.
     * 
     * @par Inputs
     * @li derating ratio: proportion of producible power to be deducted from production, non-dimensional, specify via @link ikPowman_step @endlink
     * @li maximum speed: maximum generator speed setpoint, in rad/s, specify via @link ikPowman_step @endlink
     * @li measured speed: measured generator speed, in rad/s, specify via @link ikPowman_step @endlink
     * 
     * @par Outputs
     * @li maximum torque: upper torque limit, in kNm, get via @link ikPowman_getOutput @endlink
     * @li below rated torque: preferred torque below rated wind speed, in kNm, get via @link ikPowman_getOutput @endlink
     * @li minimum pitch: lower pitch angle limit, in degrees, get via @link ikPowman_getOutput @endlink
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
        /* @cond */
        double ratedPower;
        double efficiency;
        ikLutbl lutblKopt;
        ikLutbl lutblPitchDerating;
        ikLutbl lutblPitchGenSpeed;
        double deratingRatio;
        double maxSpeed;
        double measuredSpeed;
        double maximumTorque;
        double belowRatedTorque;
        double minimumPitch;
        double samplingInterval;
        double minimumPitchGenSpeedMaxRate;
        double minimumPitchGenSpeedMinRate;
        ikTfList generatorSpeedFilter;
        double filteredGeneratorSpeed;
        /* @endcond */
    } ikPowman;
    
    /**
     * @struct ikPowmanParams
     * @brief Power manager initialisation parameters
     */
    typedef struct ikPowmanParams {
        double              ratedPower; /**<rated power, in kW*/
        double              efficiency; /**<drivetrain and generator efficiency, non-dimensional*/
        int                 belowRatedTorqueGainTableN; /**<number of points defining the below rated torque gain table. The default value is 1.*/
        double              belowRatedTorqueGainTableX [IKLUTBL_MAXPOINTS]; /**<derating ratios defining the below rated torque gain table, non-dimensional. The default value is {0.0, 0.0, ...}*/
        double              belowRatedTorqueGainTableY [IKLUTBL_MAXPOINTS]; /**<gains defining the below rated torque gain table, in kNm*s^2/rad^2. The default value is {0.0, 0.0, ...}*/
        int                 minimumPitchDeratingTableN; /**<number of points defining the derating ratio - minimum pitch table. The default value is 1.*/
        double              minimumPitchDeratingTableX [IKLUTBL_MAXPOINTS]; /**<derating ratios defining the derating ratio - minimum pitch table, non-dimensional. The default value is {0.0, 0.0, ...}*/
        double              minimumPitchDeratingTableY [IKLUTBL_MAXPOINTS]; /**<pitch angles defining the derating ratio - minimum pitch table, in degrees. The default value is {0.0, 0.0, ...}*/
        int                 minimumPitchGenSpeedTableN; /**<number of points defining the generator speed - minimum pitch table. The default value is 1.*/
        double              minimumPitchGenSpeedTableX[IKLUTBL_MAXPOINTS]; /**<generator speeds defining the generator speed - minimum pitch table, non-dimensional. The default value is {0.0, 0.0, ...}*/
        double              minimumPitchGenSpeedTableY[IKLUTBL_MAXPOINTS]; /**<pitch angles defining the generator speed - minimum pitch table, in degrees. The default value is {0.0, 0.0, ...}*/
        double              samplingInterval; /**<sampling period for the controller execution, in s*/
        double              minimumPitchGenSpeedMaxRate; /**<minimum pitch from platform-nacelle yaw difference maximum variation rate, in deg/s*/
        double              minimumPitchGenSpeedMinRate; /**<minimum pitch from platform-nacelle yaw difference minimum variation rate, in deg/s*/
        ikTfListParams      generatorSpeedFilter; /**<generator speed filtering transfer function*/
    } ikPowmanParams;
    
    /**
     * Initialise an instance
     * @param self instance
     * @param params initialisation parameters
     * @return error code:
     * @li 0: no error
     * @li -1: invalid efficiency value, must be non-zero
     * @li -2: invalid below rated speed-torque curve gain look-up table initialisation parameters
     * @li -3: invalid derating minimum pitch look-up table initialisation parameters
     * @li -4: invalid generator speed minimum pitch look-up table initialisation parameters
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
     * @param deratingRatio derating ratio (non-dimensional)
     * @param maxSpeed maximum speed, in rad/s
     * @param measuredSpeed measured speed, in rad/s
     * @return maximum torque, in kNm
     */
    double ikPowman_step(ikPowman *self, double deratingRatio, double maxSpeed, double measuredSpeed);
    
    /**
     * Get output value by name. All signals named on the block diagram of
     * @link ikPowman @endlink are accessible.
     * @param self power manager instance
     * @param output output value
     * @param name output name
     * @return error code:
     * @li 0: no error
     * @li -1: invalid signal name
     */
    int ikPowman_getOutput(const ikPowman *self, double *output, const char *name);


#ifdef __cplusplus
}
#endif

#endif /* IKPOWMAN_H */

