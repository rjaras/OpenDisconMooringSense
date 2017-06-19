/*
 * (c) IKERLAN
 * All rights reserved
 */

/* 
 * File:   ikPowman.c
 * Author: ielorza
 *
 * Created on 21 de febrero de 2017, 17:13
 */

/**
 * @file ikPowman.c
 * 
 * @brief Class ikPowman implementation
 */

#include <stdlib.h>
#include <string.h>
#include "../ikPowman/ikPowman.h"

int ikPowman_init(ikPowman *self, const ikPowmanParams *params) {
    return 0;
}

void ikPowman_initParams(ikPowmanParams *params) {
    
}

double ikPowman_step(ikPowman *self, double maxPower, double maxSpeed, double resPower) {
    //save inputs
    self->maxPower = maxPower;
    self->maxSpeed = maxSpeed;
    self->resPower = resPower;
    
    //calculate power limit
    self->powLim = maxPower - resPower;
    
    if (0.0 < maxSpeed) {
        self->maxTorque = self->powLim / maxSpeed;
    } else {
        self->maxTorque = 0.0;
    }
    
    return self->maxTorque;
}

int ikPowman_getOutput(const ikPowman *self, double *output, const char *name) {
    char *sep;

    // pick up the signal names
    if (!strcmp(name, "power limit")) {
        *output = self->powLim;
        return 0;
    }
    if (!strcmp(name, "maximum power")) {
        *output = self->maxPower;
        return 0;
    }
    if (!strcmp(name, "maximum speed")) {
        *output = self->maxSpeed;
        return 0;
    }
    if (!strcmp(name, "reserve power")) {
        *output = self->resPower;
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

