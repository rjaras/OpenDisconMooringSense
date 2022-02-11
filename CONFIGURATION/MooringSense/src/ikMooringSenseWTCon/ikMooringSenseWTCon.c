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
 * @file ikMooringSenseWTCon.c
 * 
 * @brief Class ikMooringSenseWTCon implementation
 */

#include <stdlib.h>
#include <string.h>
#include "ikMooringSenseWTCon.h"

char* ikMooringSenseWTCon_init(ikMooringSenseWTCon *self, const ikMooringSenseWTConParams *params) {
    int err;
    char* errStr;
    char* outputErrorMessage = malloc(140 * sizeof(char));
    ikMooringSenseWTConParams params_ = *params;

    /* pass reference to collective pitch demand for use in gain scheduling */
    params_.collectivePitchControl.linearController.gainShedXVal = &(self->out.pitchDemand);

    /* pass references to external pitch rate limits for use in collective pitch controller */
    params_.collectivePitchControl.linearController.maxPostGainValue = &(self->in.externalMaximumPitchRate);
    params_.collectivePitchControl.linearController.minPostGainValue = &(self->in.externalMinimumPitchRate);

    /* pass reference to preferred torque for use in torque control */
    params_.torqueControl.setpointGenerator.preferredControlAction = &(self->priv.belowRatedTorque);

    /* pass on the member parameters */
    err = ikConLoop_init(&(self->priv.dtdamper), &(params_.drivetrainDamper));
    if (err) {
        snprintf(outputErrorMessage, 140, "Error initializing drivetrain damper: %d", err);
        return outputErrorMessage;
    }
    err = ikConLoop_init(&(self->priv.torquecon), &(params_.torqueControl));
    if (err) {
        snprintf(outputErrorMessage, 140, "Error initializing torque control: %d", err);
        return outputErrorMessage;
    }
    err = ikConLoop_init(&(self->priv.colpitchcon), &(params_.collectivePitchControl));
    if (err) {
        snprintf(outputErrorMessage, 140, "Error initializing collective pitch control: %d", err);
        return outputErrorMessage;
    }
    err = ikTpman_init(&(self->priv.tpManager), &(params_.torquePitchManager));
    if (err) {
        snprintf(outputErrorMessage, 140, "Error initializing torque pitch manager: %d", err);
        return outputErrorMessage;
    }
    err = ikPowman_init(&(self->priv.powerManager), &(params_.powerManager));
    if (err) {
        snprintf(outputErrorMessage, 140, "Error initializing power manager: %d", err);
        return outputErrorMessage;
    }
    err = ikTfList_init(&(self->priv.torqueFromHubPitchTf), &(params_.torqueFromHubPitchTf));
    if (err) {
        snprintf(outputErrorMessage, 140, "Error initializing torque corrector: %d", err);
        return outputErrorMessage;
    }
    errStr = ikTsrEst_init(&(self->priv.tsrEstimator), &(params_.tsrEstimator));
    if (strlen(errStr)) {
        snprintf(outputErrorMessage, 140, "Error initializing TSR estimator: %s", errStr);
        return outputErrorMessage;
    }
    errStr = ikThrustLim_init(&(self->priv.thurstLimiter), &(params_.thurstLimiter));
    if (strlen(errStr)) {
        snprintf(outputErrorMessage, 140, "Error initializing thrust limiter: %s", errStr);
        return outputErrorMessage;
    }
    errStr = ikThrustLimEst_init(&(self->priv.thurstLimitEstimator), &(params_.thurstLimitEstimator));
    if (strlen(errStr)) {
        snprintf(outputErrorMessage, 140, "Error initializing thrust limit estimator: %s", errStr);
        return outputErrorMessage;
    }

    /* initialise feedback signals */
    self->priv.torqueFromTorqueCon = 0.0;
    self->out.pitchDemand = 0.0;

    return "";
}

void ikMooringSenseWTCon_initParams(ikMooringSenseWTConParams *params) {
    /* pass on the member parameters */
    ikConLoop_initParams(&(params->collectivePitchControl));
    ikConLoop_initParams(&(params->drivetrainDamper));
    ikConLoop_initParams(&(params->torqueControl));
    ikTpman_initParams(&(params->torquePitchManager));
    ikPowman_initParams(&(params->powerManager));
    ikTfList_initParams(&(params->torqueFromHubPitchTf));
    ikThrustLim_initParams(&(params->thurstLimiter));
    ikTsrEst_initParams(&(params->tsrEstimator));
    ikThrustLimEst_initParams(&(params->thurstLimitEstimator));
}

int ikMooringSenseWTCon_step(ikMooringSenseWTCon *self) {
    /* run tip-speed ratio estimator */
    self->priv.tipSpeedRatio = ikTsrEst_step(&(self->priv.tsrEstimator), self->in.generatorSpeed, self->in.generatorTorque, self->in.collectivePitchAngle);
    ikTsrEst_getOutput(&(self->priv.tsrEstimator), &(self->priv.rotorSpeedFiltered), "rotor speed");
    
    /* run thrust limit estimator */
    self->priv.thrustLimit = ikThrustLimEst_eval(&(self->priv.thurstLimitEstimator), self->in.turretXPosition, self->in.turretYPosition, self->in.nacelleYawAbs, self->in.thrust);
    self->priv.thrustLimit = self->priv.thrustLimit < self->in.externalMaximumThrust ? self->priv.thrustLimit : self->in.externalMaximumThrust;

    /* run thrust limiter */
    self->priv.minPitchFromThrustLim = ikThrustLim_step(&(self->priv.thurstLimiter), self->priv.tipSpeedRatio, self->priv.rotorSpeedFiltered, self->priv.thrustLimit);
    
    /* run power manager */
    self->priv.maxTorqueFromPowman = ikPowman_step(&(self->priv.powerManager), 0.0, self->in.maximumSpeed, self->in.generatorSpeed);
    ikPowman_getOutput(&(self->priv.powerManager), &(self->priv.minPitchFromPowman), "minimum pitch");
    ikPowman_getOutput(&(self->priv.powerManager), &(self->priv.belowRatedTorque), "below rated torque");

    /* calculate minimum pitch */
    self->priv.minPitch = self->priv.minPitchFromPowman > self->in.externalMinimumPitch ? self->priv.minPitchFromPowman : self->in.externalMinimumPitch;
    self->priv.minPitch = self->priv.minPitchFromThrustLim > self->priv.minPitch ? self->priv.minPitchFromThrustLim : self->priv.minPitch;
        
    /* calculate maximum torque */
    self->priv.maxTorque = self->priv.maxTorqueFromPowman < self->in.externalMaximumTorque ? self->priv.maxTorqueFromPowman : self->in.externalMaximumTorque;

    /* run torque-pitch manager */
    self->priv.tpManState = ikTpman_step(&(self->priv.tpManager), self->priv.torqueFromTorqueCon, self->priv.maxTorque, self->in.externalMinimumTorque, self->out.pitchDemand, self->in.externalMaximumPitch, self->priv.minPitch);
    ikTpman_getOutput(&(self->priv.tpManager), &(self->priv.maxPitch), "maximum pitch");
    ikTpman_getOutput(&(self->priv.tpManager), &(self->priv.minTorque), "minimum torque");

    /* run drivetrain damper */
    self->priv.torqueFromDtdamper = ikConLoop_step(&(self->priv.dtdamper), 0.0, self->in.generatorSpeed, -(self->in.externalMaximumTorque), self->in.externalMaximumTorque);

    /* run torque control */
    self->priv.torqueFromTorqueCon = ikConLoop_step(&(self->priv.torquecon), self->in.maximumSpeed, self->in.generatorSpeed, self->priv.minTorque, self->priv.maxTorque);
    
    /* get torque related to nacelle nodding/pitch */
    self->priv.torqueFromHubPitch = ikTfList_step(&(self->priv.torqueFromHubPitchTf), self->in.nacellePitch);
    
    /* Torque related to nacelle nodding/pitch limitation (limitted using percentage of torque control)*/
    self->priv.torqueFromHubPitch = self->priv.torqueFromHubPitch > 0.6 * self->priv.torqueFromTorqueCon ? 0.6 * self->priv.torqueFromTorqueCon : self->priv.torqueFromHubPitch;
    self->priv.torqueFromHubPitch = self->priv.torqueFromHubPitch < -0.6 * self->priv.torqueFromTorqueCon ? -0.6 * self->priv.torqueFromTorqueCon : self->priv.torqueFromHubPitch;
    
    /* calculate torque demand */
    self->out.torqueDemand = self->priv.torqueFromDtdamper + self->priv.torqueFromTorqueCon - self->priv.torqueFromHubPitch;

    /* run collective pitch control */
    self->out.pitchDemand = ikConLoop_step(&(self->priv.colpitchcon), self->in.maximumSpeed, self->in.generatorSpeed, self->priv.minPitch, self->priv.maxPitch);

    return self->priv.tpManState;
}

int ikMooringSenseWTCon_getOutput(const ikMooringSenseWTCon *self, double *output, const char *name) {
    int err;
    const char *sep;
    
    /* pick up the signal names */
    if (!strcmp(name, "torque demand from torque control")) {
        *output = self->priv.torqueFromTorqueCon;
        return 0;
    }
    if (!strcmp(name, "torque demand from drivetrain damper")) {
        *output = self->priv.torqueFromDtdamper;
        return 0;
    }
    if (!strcmp(name, "minimum pitch")) {
        *output = self->priv.minPitch;
        return 0;
    }
    if (!strcmp(name, "maximum pitch")) {
        *output = self->priv.maxPitch;
        return 0;
    }
    if (!strcmp(name, "maximum torque")) {
        *output = self->priv.maxTorque;
        return 0;
    }
    if (!strcmp(name, "minimum torque")) {
        *output = self->priv.minTorque;
        return 0;
    }
    if (!strcmp(name, "maximum torque from power manager")) {
        *output = self->priv.maxTorqueFromPowman;
        return 0;
    }
    if (!strcmp(name, "minimum pitch from power manager")) {
        *output = self->priv.minPitchFromPowman;
        return 0;
    }
    if (!strcmp(name, "torque from nacelle pitch")) {
        *output = self->priv.torqueFromHubPitch;
        return 0;
    }
    if (!strcmp(name, "thrust limit")) {
        *output = self->priv.thrustLimit;
        return 0;
    }

    /* pick up the block names */
    sep = strstr(name, ">");
    if (NULL == sep) return -1;
    if (!strncmp(name, "power manager", strlen(name) - strlen(sep))) {
        err = ikPowman_getOutput(&(self->priv.powerManager), output, sep + 1);
        if (err) return -1;
        else return 0;
    }
    if (!strncmp(name, "torque-pitch manager", strlen(name) - strlen(sep))) {
        err = ikTpman_getOutput(&(self->priv.tpManager), output, sep + 1);
        if (err) return -1;
        else return 0;
    }
    if (!strncmp(name, "drivetrain damper", strlen(name) - strlen(sep))) {
        err = ikConLoop_getOutput(&(self->priv.dtdamper), output, sep + 1);
        if (err) return -1;
        else return 0;
    }
    if (!strncmp(name, "torque control", strlen(name) - strlen(sep))) {
        err = ikConLoop_getOutput(&(self->priv.torquecon), output, sep + 1);
        if (err) return -1;
        else return 0;
    }
    if (!strncmp(name, "collective pitch control", strlen(name) - strlen(sep))) {
        err = ikConLoop_getOutput(&(self->priv.colpitchcon), output, sep + 1);
        if (err) return -1;
        else return 0;
    }
    if (!strncmp(name, "tip-speed ratio estimator", strlen(name) - strlen(sep))) {
        err = ikTsrEst_getOutput(&(self->priv.tsrEstimator), output, sep + 1);
        if (err) return -1;
        else return 0;
    }
    if (!strncmp(name, "thrust limiter", strlen(name) - strlen(sep))) {
        err = ikThrustLim_getOutput(&(self->priv.thurstLimiter), output, sep + 1);
        if (err) return -1;
        else return 0;
    }
    if (!strncmp(name, "thrust limit estimator", strlen(name) - strlen(sep))) {
        err = ikThrustLimEst_getOutput(&(self->priv.thurstLimitEstimator), output, sep + 1);
        if (err) return -1;
        else return 0;
    }

    return -2;
}

