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

#include "ikMooringSenseWTConfig.h"
#include "OpenDiscon_EXPORT.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/*
// TNO observer defines
# define OBSFUN DISCONOBS
typedef float BFP; /* 32 bit, floating */
/*
extern void OBSFUN(
    float* bl_avrSwap, int* bl_aviFail,
    const char* bl_accInfile, const char* bl_avcOutname,
    char* bl_avcMsg);
*/

void OpenDiscon_EXPORT DISCON(float* DATA, int FLAG, const char* INFILE, const char* OUTNAME, char* MESSAGE) {
    int err;
    char* errStr;
    static ikMooringSenseWTCon con;
    static FILE* fileHandler;
    static FILE* textfileHandler;
    float outputValue = 0.0F;
    double outputValueDouble = 0.0;

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
    char* cplambda3File = (char*)calloc(256, sizeof(char));
    char* ctlambda2File = (char*)calloc(256, sizeof(char));
    char *thrustLimitSurfaceFile = (char *) calloc(256, sizeof(char));
    char* outputBinFilename = (char*)calloc(256, sizeof(char));
    char* outputTextFilename = (char*)calloc(256, sizeof(char));
    
    static float platformInitialHeading;
    static float windDirection;
    static int writeOutputFile = 1;
    static float nacelleRotorNode_X0;
    static float nacelleRotorNode_Y0;
    static float nacelleBackNode_X0;
    static float nacelleBackNode_Y0;

    /* TNO observer required variables 
    BFP avrSwap[256]; // not all Bladed defined records are actually used
    int aviFail;
    const char *accInfile = "DISCON.IN";
    const char *avcOutname = "loadcase";
    char avcMsg[255]; // number in brackets is the max. message length!
    int loop; */
    double estimatedThrust = 0.0; // Estimated thrust force

    if (NINT(DATA[0]) == 0) {
        float platform_X_Global;
        float platform_Y_Global;
        float turbine_X_Local;
        float turbine_Y_Local;
        float nacelleInitialYaw; 
        float nacelleRotorNode_X_Local;
        float nacelleRotorNode_Y_Local;
        float nacelleBackNode_X_Local;
        float nacelleBackNode_Y_Local;
        double samplingInterval = (double)DATA[2];

        fileHandler = fopen(INFILE, "r");
        if (fileHandler) {
            err = fscanf(fileHandler, "%256[^\n]\n", outputBinFilename);
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
            err = fscanf(fileHandler, "%256[^\n]\n", cplambda3File);
            err = fscanf(fileHandler, "%256[^\n]\n", ctlambda2File);
            err = fscanf(fileHandler, "%256[^\n]\n", thrustLimitSurfaceFile);
            err = fscanf(fileHandler, "%256[^\n]\n", outputTextFilename);
            fclose(fileHandler);
        }

        ikMooringSenseWTConParams param;
        ikMooringSenseWTCon_initParams(&param);
        setParams(&param, samplingInterval);
        param.tsrEstimator.cplambda3SurfaceFileName = cplambda3File;
        param.thurstLimiter.ctlambda2SurfaceFileName = ctlambda2File;
        param.thurstLimitEstimator.maxThrustSurfaceFileName = thrustLimitSurfaceFile;
        errStr = ikMooringSenseWTCon_init(&con, &param);
        // Nothing is done with the errors!!

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
    }

    /* Calling TNO observer
    // copy avrSwap = DATA
    for (loop = 0; loop < 256; loop++) { avrSwap[loop] = DATA[loop]; }
    avrSwap[49] = (float) strlen(accInfile); // No. of characters in the 'INFILE' argument ("DISCON.IN")
    avrSwap[50] = (float) strlen(avcOutname); // No. of characters in the 'OUTNAME' argument
    avrSwap[14] = (float) (avrSwap[14] * 1.0e3); // kW to W
    OBSFUN(avrSwap, &aviFail, accInfile, avcOutname, avcMsg);
    estimatedThrust = avrSwap[129]; */
    
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
    con.in.externalMaximumTorque = 230.0; // kNm
    con.in.externalMinimumTorque = 0.0; // kNm
    con.in.externalMaximumPitch = 90.0; // deg
    con.in.externalMinimumPitch = 0.0; // deg
    con.in.externalMaximumPitchRate = 2.5; // deg/s
    con.in.externalMinimumPitchRate = -2.5; // deg/s
    con.in.generatorSpeed = (double)DATA[19]; // rad/s
    con.in.maximumSpeed = 480.0 / 30 * 3.1416; // rpm to rad/s
    con.in.nacellePitch = nacellePitch; // rad
    con.in.collectivePitchAngle = (double)DATA[3] * 180.0 / 3.1416; // rad -> deg
    con.in.generatorTorque = (double)DATA[22] / 1000.0; // Nm -> kNm
    con.in.turretXPosition = (double)DATA[119]; /* Locking turret position X [m] */
    con.in.turretYPosition = (double)DATA[120]; /* Locking turret position Y [m] */
    con.in.nacelleYawAbs = nacelleAbsoluteYaw; // rad
    con.in.thrust = (double) estimatedThrust / 1000.0; // N -> kN
    con.in.externalMaximumThrust = 2.8E3; // kN (2.8MN)
    
    ikMooringSenseWTCon_step(&con);

    DATA[46] = (float)(con.out.torqueDemand * 1.0e3); /* kNm to Nm */
    DATA[41] = (float)(con.out.pitchDemand / 180.0 * 3.1416); // deg to rad 
    DATA[42] = (float)(con.out.pitchDemand / 180.0 * 3.1416); // deg to rad 
    DATA[43] = (float)(con.out.pitchDemand / 180.0 * 3.1416); // deg to rad 
    DATA[44] = (float)(con.out.pitchDemand / 180.0 * 3.1416); // deg to rad (collective pitch angle) 
}
