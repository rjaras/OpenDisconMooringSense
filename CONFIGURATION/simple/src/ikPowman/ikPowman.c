/*
  Copyright (C) 2017 IK4-IKERLAN

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
 * @file ikPowman.c
 * 
 * @brief Class ikPowman implementation
 */

/* @cond */

#include <stdlib.h>
#include <string.h>

#include "ikPowman.h"

int ikPowman_init(ikPowman *self, const ikPowmanParams *params) {
    int err;
        
    /* register rated power */
    self->ratedPower = params->ratedPower;
        
    /* register efficiency */
    if (0 == params->efficiency) return -1;
    self->efficiency = params->efficiency;
        
    /* initialise look-up tables */
    ikLutbl_init(&(self->lutblKopt));
    err = ikLutbl_setPoints(&(self->lutblKopt), params->belowRatedTorqueGainTableN, params->belowRatedTorqueGainTableX, params->belowRatedTorqueGainTableY);
    if (err) return -2;
        
    ikLutbl_init(&(self->lutblPitchDerating));
    err = ikLutbl_setPoints(&(self->lutblPitchDerating), params->minimumPitchDeratingTableN, params->minimumPitchDeratingTableX, params->minimumPitchDeratingTableY);
    if (err) return -3;
    
    ikLutbl_init(&(self->lutblPitchGenSpeed));
    err = ikLutbl_setPoints(&(self->lutblPitchGenSpeed), params->minimumPitchGenSpeedTableN, params->minimumPitchGenSpeedTableX, params->minimumPitchGenSpeedTableY);
    if (err) return -4;

    /* register samplingInterval */
    self->samplingInterval = params->samplingInterval;

    self->minimumPitchGenSpeedMinRate = params->minimumPitchGenSpeedMinRate;
    self->minimumPitchGenSpeedMaxRate = params->minimumPitchGenSpeedMaxRate;

    return 0;
}

void ikPowman_initParams(ikPowmanParams *params) {
    /* set power to 0 */
    params->ratedPower = 0.0;
        
    /* set efficiency to 1 */
    params->efficiency = 1.0;
        
    /* make the below rated torque gain 0 */
    params->belowRatedTorqueGainTableN = 1;
    params->belowRatedTorqueGainTableX[0] = 0.0;
    params->belowRatedTorqueGainTableY[0] = 0.0;
        
    /* make the minimum pitch 0 */
    params->minimumPitchDeratingTableN = 1;
    params->minimumPitchDeratingTableX[0] = 0.0;
    params->minimumPitchDeratingTableY[0] = 0.0;

    /* Initialize the generator speed - minimum pitch table */
    params->minimumPitchGenSpeedTableN = 1;
    params->minimumPitchGenSpeedTableX[0] = 0.0;
    params->minimumPitchGenSpeedTableY[0] = 0.0;

    /* set sampling interval to 0.01s */
    params->samplingInterval = 0.01;
}

double ikPowman_step(ikPowman *self, double deratingRatio, double maxSpeed, double measuredSpeed) {
    /* register inputs */
    self->deratingRatio = deratingRatio;
    self->maxSpeed = maxSpeed;
    self->measuredSpeed = measuredSpeed;
        
    /* calculate maximum torque */  
    self->maximumTorque = (1-deratingRatio)*self->ratedPower/maxSpeed/self->efficiency;
        
    /* calculate below rated torque */
    self->belowRatedTorque = ikLutbl_eval(&(self->lutblKopt), deratingRatio)*measuredSpeed*measuredSpeed;
        
    /* calculate minimum pitch value caused by derating ratio */
    double minimumPitchDerating = ikLutbl_eval(&(self->lutblPitchDerating), deratingRatio);

    /* calculate minimum pitch value caused by generator speed */
    static double minimumPitchGenSpeed;
    double newMinimumPitchGenSpeed = ikLutbl_eval(&(self->lutblPitchGenSpeed), measuredSpeed);
    double pitchChange = newMinimumPitchGenSpeed - minimumPitchGenSpeed;
    if (pitchChange > self->minimumPitchGenSpeedMaxRate * self->samplingInterval) {
        minimumPitchGenSpeed = minimumPitchGenSpeed + self->minimumPitchGenSpeedMaxRate * self->samplingInterval;
    } 
    else if (pitchChange < self->minimumPitchGenSpeedMinRate * self->samplingInterval) {
        minimumPitchGenSpeed = minimumPitchGenSpeed + self->minimumPitchGenSpeedMinRate * self->samplingInterval;
    }
    else {
        minimumPitchGenSpeed = newMinimumPitchGenSpeed;
    }

    /* calculate minimum pitch (total) */
    self->minimumPitch = max(minimumPitchDerating, minimumPitchGenSpeed);

    /* return the maximum torque */
    return self->maximumTorque;
}

int ikPowman_getOutput(const ikPowman *self, double *output, const char *name) {
    /* pick up the signal names */
    if (!strcmp(name, "derating ratio")) {
        *output = self->deratingRatio;
        return 0;
    }
    if (!strcmp(name, "maximum speed")) {
        *output = self->maxSpeed;
        return 0;
    }
    if (!strcmp(name, "measured speed")) {
        *output = self->measuredSpeed;
        return 0;
    }
    if (!strcmp(name, "maximum torque")) {
        *output = self->maximumTorque;
        return 0;
    }
    if (!strcmp(name, "below rated torque")) {
        *output = self->belowRatedTorque;
        return 0;
    }
    if (!strcmp(name, "minimum pitch")) {
        *output = self->minimumPitch;
        return 0;
    }
        
    return -1;
}

/* @endcond */
