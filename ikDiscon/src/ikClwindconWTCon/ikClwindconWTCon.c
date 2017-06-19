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
    self->priv.collectivePitchFromPitchControl = 0.0;
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

    // calculate collective pitch
    self->priv.collectivePitch = (self->in.pitchBlade1 + self->in.pitchBlade2 + self->in.pitchBlade3) / 3.0;

    // run power manager
    self->priv.maxTorqueFromPowerMan = ikPowman_step(&(self->priv.powerManager), self->in.maximumPower, self->priv.maxSpeed, self->in.reservePower);

    // calculate maximum torque
    self->priv.maxTorque = self->priv.maxTorqueFromPowerMan < self->priv.maxTorqueFromStartStop ? self->priv.maxTorqueFromPowerMan : self->priv.maxTorqueFromStartStop;

    // run torque-pitch manager
    self->priv.tpManState = ikTpman_step(&(self->priv.tpManager), self->priv.torqueFromTorqueCon, self->priv.maxTorque, self->priv.minTorqueFromStartStop, self->priv.collectivePitchFromPitchControl, self->priv.collectivePitchFromAeroStab, self->priv.maxPitch, self->priv.minPitch);
    ikTpman_getOutput(&(self->priv.tpManager), &(self->priv.maxPitchFromSpeed), "maximum pitch from speed");
    ikTpman_getOutput(&(self->priv.tpManager), &(self->priv.minPitchFromSpeed), "minimum pitch");
    ikTpman_getOutput(&(self->priv.tpManager), &(self->priv.maxPitchFromPltfrm), "maximum pitch from platform");
    ikTpman_getOutput(&(self->priv.tpManager), &(self->priv.minPitchFromPltfrm), "minimum pitch from platform");
    ikTpman_getOutput(&(self->priv.tpManager), &(self->priv.minTorque), "minimum torque");

    // run drivetrain damper
    self->priv.torqueFromDtdamper = ikConLoop_step(&(self->priv.dtdamper), 0.0, self->in.generatorSpeed, -(self->priv.maxTorque), self->priv.maxTorque);

    // run torque control
    self->priv.torqueFromTorqueCon = ikConLoop_step(&(self->priv.torquecon), self->priv.maxSpeed, self->in.generatorSpeed, self->priv.minTorque, self->priv.maxTorque);

    // calculate torque demand
    self->out.torqueDemand = self->priv.torqueFromDtdamper + self->priv.torqueFromTorqueCon;

    // run collective pitch control
    self->priv.collectivePitchFromPitchControl = ikConLoop_step(&(self->priv.colpitchcon), self->priv.maxSpeed, self->in.generatorSpeed, self->priv.minPitchFromSpeed, self->priv.maxPitchFromSpeed);
    
    // calculate collective pitch demand
    self->priv.collectivePitchDemand = self->priv.collectivePitchFromPitchControl;
    
    // run IPC
    self->out.pitchDemandBlade1 = self->priv.collectivePitchDemand;
    self->out.pitchDemandBlade2 = self->priv.collectivePitchDemand;
    self->out.pitchDemandBlade3 = self->priv.collectivePitchDemand;

    return self->priv.status;
}

int ikClwindconWTCon_getOutput(const ikClwindconWTCon *self, double *output, const char *name) {
    int err;
    char *sep;
    // pick up the signal names
    if (!strcmp(name, "collective pitch demand from pitch control")) {
        *output = self->priv.collectivePitchFromPitchControl;
        return 0;
    }
    if (!strcmp(name, "torque demand from torque control")) {
        *output = self->priv.torqueFromTorqueCon;
        return 0;
    }
    if (!strcmp(name, "torque demand from drivetrain damper")) {
        *output = self->priv.torqueFromDtdamper;
        return 0;
    }
    if (!strcmp(name, "minimum pitch from speed")) {
        *output = self->priv.minPitchFromSpeed;
        return 0;
    }
    if (!strcmp(name, "collective pitch")) {
        *output = self->priv.collectivePitch;
        return 0;
    }
    if (!strcmp(name, "maximum pitch")) {
        *output = self->priv.maxPitch;
        return 0;
    }
    if (!strcmp(name, "minimum pitch")) {
        *output = self->priv.minPitch;
        return 0;
    }
    if (!strcmp(name, "maximum speed")) {
        *output = self->priv.maxSpeed;
        return 0;
    }
    if (!strcmp(name, "minimum speed")) {
        *output = self->priv.minSpeed;
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
    if (!strcmp(name, "maximum pitch from speed")) {
        *output = self->priv.maxPitchFromSpeed;
        return 0;
    }
    if (!strcmp(name, "collective pitch demand")) {
        *output = self->priv.collectivePitchDemand;
        return 0;
    }

    // pick up the block names
    sep = strstr(name, ">");
    if (NULL == sep) return -1;


    return -2;
}

