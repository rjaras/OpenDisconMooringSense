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

#define NINT(a) ((a) >= 0.0 ? (int) ((a)+0.5) : ((a)-0.5))

#include "ikSimpleWTConfig.h"
#include "OpenDiscon_EXPORT.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void OpenDiscon_EXPORT DISCON(float* DATA, int FLAG, const char* INFILE, const char* OUTNAME, char* MESSAGE) {
    int err;
    static ikSimpleWTCon con;
    static double samplingInterval;

    /*Logging variables*/
    static FILE* fileHandler;
    char *ouputFile = (char *) calloc(256, sizeof(char));
    float outputValue = 0.0F;
    
    /*Computed signals*/
    double nacelleAbsoluteYaw = 0.0;
    double platformAbsoluteYaw = 0.0;
    double nacelleRotorNode_X = 0.0;
    double nacelleRotorNode_Y = 0.0;
    double nacelleBackNode_X = 0.0;
    double nacelleBackNode_Y = 0.0;
    double yawError;
    double nacellePitch;
        
    /*input file read parameters*/
    static float platformInitialHeading;
    static float windDirection;
    static int writeOutputFile = 1;
    static float nacelleRotorNode_X0;
    static float nacelleRotorNode_Y0;
    static float nacelleBackNode_X0;
    static float nacelleBackNode_Y0;
    float platform_X_Global;
    float platform_Y_Global;
    float turbine_X_Local;
    float turbine_Y_Local;
    float nacelleInitialYaw;
    float nacelleRotorNode_X_Local;
    float nacelleRotorNode_Y_Local;
    float nacelleBackNode_X_Local;
    float nacelleBackNode_Y_Local;

    if (NINT(DATA[0]) == 0) {
        samplingInterval = (double)DATA[2];
        
        ikSimpleWTConParams param;
        ikSimpleWTCon_initParams(&param);
        setParams(&param, samplingInterval);
        ikSimpleWTCon_init(&con, &param);

        fileHandler = fopen(INFILE, "r");
        err = fscanf(fileHandler, "%256[^\n]\n", ouputFile);
        err = fscanf(fileHandler, "%d%*[^\n]\n", &writeOutputFile);
        err = fscanf(fileHandler, "%f%*[^\n]\n", &platformInitialHeading);
        err = fscanf(fileHandler, "%f%*[^\n]\n", &nacelleInitialYaw);
        err = fscanf(fileHandler, "%f%*[^\n]\n", &windDirection);
        err = fscanf(fileHandler, "%f%*[^\n]\n", &platform_X_Global);
        err = fscanf(fileHandler, "%f%*[^\n]\n", &platform_Y_Global);
        err = fscanf(fileHandler, "%f%*[^\n]\n", &turbine_X_Local);
        err = fscanf(fileHandler, "%f%*[^\n]\n", &turbine_Y_Local);
        err = fscanf(fileHandler, "%f%*[^\n]\n", &nacelleRotorNode_X_Local);
        err = fscanf(fileHandler, "%f%*[^\n]\n", &nacelleRotorNode_Y_Local);
        err = fscanf(fileHandler, "%f%*[^\n]\n", &nacelleBackNode_X_Local);
        err = fscanf(fileHandler, "%f%*[^\n]\n", &nacelleBackNode_Y_Local);
        fclose(fileHandler);

        platformInitialHeading = platformInitialHeading * (float)(3.1416 / 180.0); /* deg to rad */
        nacelleInitialYaw = nacelleInitialYaw * (float)(3.1416 / 180.0); /* deg to rad */
        
        ikChangeCoordinateSystem(&nacelleRotorNode_X_Local, &nacelleRotorNode_Y_Local, nacelleInitialYaw, turbine_X_Local, turbine_Y_Local);
        ikChangeCoordinateSystem(&nacelleRotorNode_X_Local, &nacelleRotorNode_Y_Local, platformInitialHeading, platform_X_Global, platform_Y_Global);
        nacelleRotorNode_X0 = nacelleRotorNode_X_Local;
        nacelleRotorNode_Y0 = nacelleRotorNode_Y_Local;

        ikChangeCoordinateSystem(&nacelleBackNode_X_Local, &nacelleBackNode_Y_Local, nacelleInitialYaw, turbine_X_Local, turbine_Y_Local);
        ikChangeCoordinateSystem(&nacelleBackNode_X_Local, &nacelleBackNode_Y_Local, platformInitialHeading, platform_X_Global, platform_Y_Global);
        nacelleBackNode_X0 = nacelleBackNode_X_Local;
        nacelleBackNode_Y0 = nacelleBackNode_Y_Local;

        if (writeOutputFile != 0) {
            fileHandler = fopen(ouputFile, "wb");
        }
    }

    /* Compute input parameters */
    platformAbsoluteYaw = (double)DATA[122] + (double)platformInitialHeading; /* Platform yaw [rad] */
    platformAbsoluteYaw = platformAbsoluteYaw < -3.141593 ? platformAbsoluteYaw + 2 * 3.141593 : platformAbsoluteYaw;
    platformAbsoluteYaw = platformAbsoluteYaw > 3.141593 ? platformAbsoluteYaw - 2 * 3.141593 : platformAbsoluteYaw;
    nacelleRotorNode_X = (double)DATA[128] + nacelleRotorNode_X0; // Nacelle Point 1 X
    nacelleRotorNode_Y = (double)DATA[129] + nacelleRotorNode_Y0; // Nacelle Point 1 Y
    nacelleBackNode_X = (double)DATA[131] + nacelleBackNode_X0; // Nacelle Point 2 X 
    nacelleBackNode_Y = (double)DATA[132] + nacelleBackNode_Y0; // Nacelle Point 2 Y 
    nacelleAbsoluteYaw = atan2((nacelleBackNode_Y - nacelleRotorNode_Y), (nacelleBackNode_X - nacelleRotorNode_X));
    yawError = nacelleAbsoluteYaw * 180.0 / 3.141593 - windDirection;
    nacellePitch = -atan2(((double)DATA[133] - (double)DATA[130]), sqrt((nacelleBackNode_X - nacelleRotorNode_X) * (nacelleBackNode_X - nacelleRotorNode_X) + (nacelleBackNode_Y - nacelleRotorNode_Y) * (nacelleBackNode_Y - nacelleRotorNode_Y)));

    /* Set controller inputs */
    con.in.externalMaximumTorque = 230.0; /* kNm */
    con.in.externalMinimumTorque = 0.0; /* kNm */
    con.in.externalMaximumPitch = 90.0; /* deg */
    con.in.externalMinimumPitch = 0.0; /* deg */
    con.in.externalMaximumPitchRate = 2.5; /* deg/s */
    con.in.externalMinimumPitchRate = -2.5; /* deg/s */
    con.in.generatorSpeed = (double) DATA[19]; /* rad/s */
    con.in.maximumSpeed = 480.0/30*3.1416; /* rpm to rad/s */
    con.in.nacellePitch = nacellePitch; /* rad/s2 */
    ikSimpleWTCon_step(&con);

    DATA[46] = (float) (con.out.torqueDemand*1.0e3); /* kNm to Nm */
    DATA[41] = (float) (con.out.pitchDemand/180.0*3.1416); /* deg to rad */
    DATA[42] = (float) (con.out.pitchDemand/180.0*3.1416); /* deg to rad */
    DATA[43] = (float) (con.out.pitchDemand/180.0*3.1416); /* deg to rad */
    DATA[44] = (float) (con.out.pitchDemand/180.0*3.1416); /* deg to rad (collective pitch angle) */

    
    if (writeOutputFile && fileHandler != NULL) {
        if (NINT(DATA[0]) == -1) {
            fclose(fileHandler);
        }
    }
}
