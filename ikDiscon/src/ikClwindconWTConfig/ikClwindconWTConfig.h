/*
 * (c) IKERLAN
 * All rights reserved
 */

/* 
 * File:   ikClwindconWTConfig.h
 * Author: ielorza
 *
 * Created on 19 de junio de 2017, 16:25
 */

/**
 * @file ikClwindconWTConfig.h
 * 
 * @brief CL-Windcon wind turbine controller configuration interface
 */

#ifndef IKCLWINDCONWTCONFIG_H
#define IKCLWINDCONWTCONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ikClwindconWTCon.h"  

	void setParams(ikClwindconWTConParams *param);
	
	void ikTuneDrivetrainDamper(ikConLoopParams *params, double T);
	
	void ikTuneOptimumTorqueCurve(ikConLoopParams *params);
	
	void ikTunePitchPI(ikConLoopParams *params);
	
	void ikTunePitchLowpassFilter(ikConLoopParams *params, double T);
	
	void ikTunePitchNotches(ikConLoopParams *params, double T);
	
	void ikTunePitchPI(ikConLoopParams *params, double T);
	
	void ikTuneTorqueLowpassFilter(ikConLoopParams *params, double T);
	
	void ikTuneTorqueNotches(ikConLoopParams *params, double T);
	
	void ikTuneTorquePI(ikConLoopParams *params, double T);

#ifdef __cplusplus
}
#endif

#endif /* IKCLWINDCONWTCONFIG_H */

