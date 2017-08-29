/*
 * (c) IKERLAN
 * All rights reserved
 */

/* 
 * File:   ikTpman.c
 * Author: ielorza
 *
 * Created on 16 de febrero de 2017, 14:32
 */

/**
 * @file ikTpman.c
 * 
 * @brief Class ikTpman implementation
 */

// @cond

#include <stdlib.h>
#include <string.h>

#include "../ikTpman/ikTpman.h"

int ikTpman_init(ikTpman *self, const ikTpmanParams *params) {
    // register lookup table
    self->minPitchTbl = params->minPitchTbl;

    // set state to 0
    self->state = 0;

    return 0;
}

void ikTpman_initParams(ikTpmanParams *params) {
    params->minPitchTbl = NULL;
}

int ikTpman_step(ikTpman *self, double torque, double maxTorque, double minTorqueExt, double pitch, double maxPitchExt, double minPitchExt) {
    // save inputs
    self->maxPitchExt = maxPitchExt;
    self->minPitchExt = minPitchExt;
    self->torque = torque;
    self->pitch = pitch;
    self->minTorqueExt = minTorqueExt;
    self->maxTorque = maxTorque;

    // apply lookup table and external minimum pitch
    if (NULL != self->minPitchTbl) {
        self->minPitch = ikLutbl_eval(self->minPitchTbl, torque);
        self->minPitch = self->minPitch > minPitchExt ? self->minPitch : minPitchExt;
    } else {
        self->minPitch = minPitchExt;
    }

    // transition between states if necessary
    switch (self->state) {
        case 0:
            if ((torque >= maxTorque) || (pitch > self->minPitch)) self->state = 1;
            break;
        case 1:
            if (pitch <= self->minPitch) self->state = 0;
            break;
    }

    // calculate limits depending on state
    switch (self->state) {
        case 0:
            self->maxPitch = pitch;
            self->maxPitch = self->maxPitch < maxPitchExt ? self->maxPitch : maxPitchExt;
            self->maxPitch = self->maxPitch > self->minPitch ? self->maxPitch : self->minPitch;
            self->minTorque = minTorqueExt;
            break;
        case 1:
            self->maxPitch = maxPitchExt;
            self->minTorque = torque;
            self->minTorque = self->minTorque < maxTorque ? self->minTorque : maxTorque;
            self->minTorque = self->minTorque > minTorqueExt ? self->minTorque : minTorqueExt;
            break;
    }

    return self->state;
}

int ikTpman_getOutput(const ikTpman *self, double *output, const char *name) {
    const char *sep;

    // pick up the signal names
    if (!strcmp(name, "minimum pitch")) {
        *output = self->minPitch;
        return 0;
    }
    if (!strcmp(name, "maximum pitch")) {
        *output = self->maxPitch;
        return 0;
    }
    if (!strcmp(name, "minimum torque")) {
        *output = self->minTorque;
        return 0;
    }
    if (!strcmp(name, "external maximum pitch")) {
        *output = self->maxPitchExt;
        return 0;
    }
    if (!strcmp(name, "external minimum pitch")) {
        *output = self->minPitchExt;
        return 0;
    }
    if (!strcmp(name, "torque")) {
        *output = self->torque;
        return 0;
    }
    if (!strcmp(name, "pitch")) {
        *output = self->pitch;
        return 0;
    }
    if (!strcmp(name, "external minimum torque")) {
        *output = self->minTorqueExt;
        return 0;
    }
    if (!strcmp(name, "maximum torque")) {
        *output = self->maxTorque;
        return 0;
    }

    // pick up the block names
    sep = strstr(name, ">");
    if (NULL == sep) return -1;

    return -2;
}

// @endcond



