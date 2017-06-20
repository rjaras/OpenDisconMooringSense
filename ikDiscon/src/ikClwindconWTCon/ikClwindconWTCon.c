/*
 * (c) IKERLAN
 * All rights reserved
 */

/* 
 * File:   ikClwindconWTCon.c
 * Author: ielorza
 *
 * Created on 19 de junio de 2017, 15:49
 */

/**
 * @file ikClwindconWTCon.c
 * 
 * @brief Class ikClwindconWTCon implementation
 */

#include <stdlib.h>
#include <string.h>
#include "ikClwindconWTCon.h"

int ikClwindconWTCon_init(ikClwindconWTCon *self, const ikClwindconWTConParams *params) {
    int err;
	ikClwindconWTConParams params_ = params;
	
	// pass reference to collective pitch demand for use in gain scheduling
	params_->collectivePitchControl.linearController.gainShedXVal = &(self->priv.collectivePitchDemand);

    // pass on the member parameters
    err = ikConLoop_init(&(self->priv.dtdamper), &(params_->drivetrainDamper));
    if (err) return -1;
    err = ikConLoop_init(&(self->priv.torquecon), &(params_->torqueControl));
    if (err) return -2;
    err = ikConLoop_init(&(self->priv.colpitchcon), &(params_->collectivePitchControl));
    if (err) return -3;
    err = ikTpman_init(&(self->priv.tpManager), &(params_->torquePitchManager));
    if (err) return -5;
    
    // initialise feedback signals
    self->priv.torqueFromTorqueCon = 0.0;
	self->priv.collectivePitchDemand = 0.0;

    return 0;
}

void ikClwindconWTCon_initParams(ikClwindconWTConParams *params) {
    // pass on the member parameters
    ikConLoop_initParams(&(params->collectivePitchControl));
    ikConLoop_initParams(&(params->drivetrainDamper));
    ikPowman_initParams(&(params->powerManager));
    ikConLoop_initParams(&(params->torqueControl));
    ikTpman_initParams(&(params->torquePitchManager));
}

int ikClwindconWTCon_step(ikClwindconWTCon *self) {

    // run power manager
    self->priv.maxTorqueFromPowerMan = ikPowman_step(&(self->priv.powerManager), self->in.maximumPower, self->priv.maxSpeed);

    // calculate maximum torque
    self->priv.maxTorque = self->priv.maxTorqueFromPowerMan < self->in.externalMaximumTorque ? self->priv.maxTorqueFromPowerMan : self->in.externalMaximumTorque;

    // run torque-pitch manager
    self->priv.tpManState = ikTpman_step(&(self->priv.tpManager), self->priv.torqueFromTorqueCon, self->priv.maxTorque, self->in.externalMinimumTorque, self->priv.collectivePitchDemand, self->in.externalMaximumPitch, self->in.externalMinimumPitch);
    ikTpman_getOutput(&(self->priv.tpManager), &(self->priv.maxPitch), "maximum pitch");
    ikTpman_getOutput(&(self->priv.tpManager), &(self->priv.minPitch), "minimum pitch");
    ikTpman_getOutput(&(self->priv.tpManager), &(self->priv.minTorque), "minimum torque");

    // run drivetrain damper
    self->priv.torqueFromDtdamper = ikConLoop_step(&(self->priv.dtdamper), 0.0, self->in.generatorSpeed, -(self->priv.maxTorque), self->priv.maxTorque);

    // run torque control
    self->priv.torqueFromTorqueCon = ikConLoop_step(&(self->priv.torquecon), self->in.maximumSpeed, self->in.generatorSpeed, self->priv.minTorque, self->priv.maxTorque);

    // calculate torque demand
    self->out.torqueDemand = self->priv.torqueFromDtdamper + self->priv.torqueFromTorqueCon;

    // run collective pitch control
    self->priv.collectivePitchDemand = ikConLoop_step(&(self->priv.colpitchcon), self->in.maximumSpeed, self->in.generatorSpeed, self->priv.minPitch, self->priv.maxPitch);
    
    // run IPC
    self->out.pitchDemandBlade1 = self->priv.collectivePitchDemand;
    self->out.pitchDemandBlade2 = self->priv.collectivePitchDemand;
    self->out.pitchDemandBlade3 = self->priv.collectivePitchDemand;

    return self->priv.tpManState;
}

int ikClwindconWTCon_getOutput(const ikClwindconWTCon *self, double *output, const char *name) {
    int err;
    char *sep;
    
	// pick up the signal names
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
    if (!strcmp(name, "maximum torque from power manager")) {
        *output = self->priv.maxTorqueFromPowerMan;
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
    if (!strcmp(name, "collective pitch demand")) {
        *output = self->priv.collectivePitchDemand;
        return 0;
    }

    // pick up the block names
    sep = strstr(name, ">");
    if (NULL == sep) return -1;
	if (!strncmp(name, "power manager", strlen(name) - strlen(sep))) {
        err = ikStpgen_getOutput(&(self->powerManager), output, sep + 1);
        if (err) return -1;
        else return 0;
    }
	if (!strncmp(name, "torque-pitch manager", strlen(name) - strlen(sep))) {
        err = ikStpgen_getOutput(&(self->tpManager), output, sep + 1);
        if (err) return -1;
        else return 0;
    }
	if (!strncmp(name, "drivetrain damper", strlen(name) - strlen(sep))) {
        err = ikStpgen_getOutput(&(self->dtdamper), output, sep + 1);
        if (err) return -1;
        else return 0;
    }
	if (!strncmp(name, "torque control", strlen(name) - strlen(sep))) {
        err = ikStpgen_getOutput(&(self->torquecon), output, sep + 1);
        if (err) return -1;
        else return 0;
    }
	if (!strncmp(name, "collective pitch control", strlen(name) - strlen(sep))) {
        err = ikStpgen_getOutput(&(self->colpitchcon), output, sep + 1);
        if (err) return -1;
        else return 0;
    }


    return -2;
}

