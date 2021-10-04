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
 * @file ikSimpleWTConfig.c
 *
 * @brief Simple wind turbine controller configuration implementation
 */

#include "ikSimpleWTConfig.h"

#include <math.h>

void setParams(ikSimpleWTConParams *param, double samplingInterval) {
    /*! [Sampling interval] */
    /*
      ####################################################################
      Sampling interval

      Set sampling interval in discon.c
      ####################################################################
    */
    /*! [Sampling interval] */

    ikTuneDrivetrainDamper(&(param->drivetrainDamper), samplingInterval);
    ikTuneSpeedRange(&(param->torqueControl));
    ikTunePowerSettings(&(param->powerManager));
    ikTuneDeratingTorqueStrategy(&(param->powerManager));
    ikTuneDeratingPitchStrategy(&(param->powerManager));
    ikTuneGenSpeedPitchStrategy(&(param->powerManager), samplingInterval);
    ikTunePitchPIGainSchedule(&(param->collectivePitchControl));
    ikTunePitchLowpassFilter(&(param->collectivePitchControl), samplingInterval);
    ikTunePitchNotches(&(param->collectivePitchControl), samplingInterval);
    ikTunePitchPI(&(param->collectivePitchControl), samplingInterval);
    ikTuneTorqueLowpassFilter(&(param->torqueControl), samplingInterval);
    ikTuneTorqueNotches(&(param->torqueControl), samplingInterval);
    ikTuneTorquePI(&(param->torqueControl), samplingInterval);
    ikTuneTorqueFromHubPitch(&(param->torqueFromHubPitchTf), samplingInterval);
}

void ikTuneDrivetrainDamper(ikConLoopParams *params, double T) {

    /*! [Drivetrain damper] */
    /*
      ####################################################################
      Drivetrain damper

      Transfer function:

      D(s) = G*s*w^2/(s^2 + 2*d*w*s + w^2)

      The sampling time is given by function parameter T.

      Set parameters here:
    */
    const double G = 0.0382; /* [kNm s^2/rad] 4 Nm s/rpm */
    const double d = 0.1; /* [-] */
    const double w = 21.1; /* [rad/s] */
    /*
      ####################################################################
    */
    /*! [Drivetrain damper] */


    /*
      tune the drivetrain damper to this tf:
      z^2 - 1
      D(z) = G*T/2*w^2 -------------------------------------------------------------------------------
      (1 + T*d*w + T^2*w^2/4)*z^2 -2*(1 - T^2*w^2/4)*z + (1 - T*d*w + T^2*w^2/4)
      rad/s --> kNm
    */
    params->linearController.errorTfs.tfParams[0].enable = 1;
    params->linearController.errorTfs.tfParams[0].b[0] = 1.0;
    params->linearController.errorTfs.tfParams[0].b[1] = 0.0;
    params->linearController.errorTfs.tfParams[0].b[2] = -1.0;
    params->linearController.errorTfs.tfParams[0].a[0] = 1.0 + T*d*w + T*T*w*w/4.0;
    params->linearController.errorTfs.tfParams[0].a[1] = -2.0*(1.0 - T*T*w*w/4.0);
    params->linearController.errorTfs.tfParams[0].a[2] = (1.0 - T*d*w + T*T*w*w/4.0);
    params->linearController.errorTfs.tfParams[1].enable = 1;
    params->linearController.errorTfs.tfParams[1].b[0] = -G*T/2.0*w*w;

}

void ikTuneSpeedRange(ikConLoopParams *params) {

    /*
      ####################################################################
      Variable generator speed range
                                         
      Set parameters here:
    */
    const double Wmin = 31.4159265358979; /* [rad/s] 300 rpm */
    const double Wmax = 50.2654824574367; /* [rad/s] 480 rpm */
    /*
      ####################################################################
    */

    params->setpointGenerator.nzones = 1;
    params->setpointGenerator.setpoints[0][0] = Wmin;
    params->setpointGenerator.setpoints[1][0] = Wmax;

}
        
void ikTunePowerSettings(ikPowmanParams *params) {
        
    /*
      ####################################################################
      Power settings
                                         
      Set parameters here:
    */
    const double Pn = 10.0e3; /* kW */
    //const double eff = 1; /* - */
    const double eff = 0.94; /* - */
    /*
      ####################################################################
    */

    params->ratedPower = Pn;
    params->efficiency = eff;
}

void ikTuneDeratingTorqueStrategy(ikPowmanParams *params) {
    /*
      This is an original implementation of derating strategy 3a as described by ECN in deliverable D2.1 of H2020 project CL-Windcon.
    */

    int i;
        
    /*! [Optimum torque] */
    /*
      ####################################################################
      Below rated speed-torque curve

      Curve:

      Q = Kopt(dr) * w^2

      Set parameters here:
    */
    const int n = 1; /* number of points in the lookup table */
    const double dr[] = {0.00}; /* - */
    const double Kopt[] = { 103.278666 }; /* Nm*s^2/rad^2 */
    /*
      ####################################################################
    */
    /*! [Optimum torque] */

    params->belowRatedTorqueGainTableN = n;
    for (i = 0; i < n; i++) {
        params->belowRatedTorqueGainTableX[i] = dr[i];
        params->belowRatedTorqueGainTableY[i] = Kopt[i]/1.0e3;
    }           
}

void ikTuneDeratingPitchStrategy(ikPowmanParams *params) {
    /*
      This is an original implementation of derating strategy 3a as described by ECN in deliverable D2.1 of H2020 project CL-Windcon.
    */

    int i;
        
    /*! [Minimum pitch] */
    /*
      ####################################################################
      Minimum pitch

      Set parameters here:
    */
    const int n = 1; /* number of points in the lookup table */
    const double dr[] = {0.00}; /* - */
    const double pitch[] = {0.00}; /* rad */
    /*
      ####################################################################
    */
    /*! [Minimum pitch] */

    params->minimumPitchDeratingTableN = n;
    for (i = 0; i < n; i++) {
        params->minimumPitchDeratingTableX[i] = dr[i];
        params->minimumPitchDeratingTableY[i] = pitch[i]/3.1416*180.0;
    }           
}

void ikTuneGenSpeedPitchStrategy(ikPowmanParams* params, double T) {
    /*
      @rjaras implementation of pitch minimum value taking into account generator speed.
    */

    int i;

    /*! [Minimum pitch] */
    /*
      ####################################################################
      Minimum pitch

      Set parameters here:
    */

    // Generator Speed - minimum pitch table
    const int n = 2; /* number of points in the lookup table */
    const double genSpeed[] = { 45.919427, 50.2656 }; /* rad/s */
    //const double pitch[] = { 0.00,  0.05236 }; /* rad -> 3deg*/
    //const double pitch[] = { 0.00,  0.10472 }; /* rad -> 6deg*/
    //const double pitch[] = { 0.00,  5.0 * 3.1416 / 180.0 }; /* rad -> 5deg GOOD PERFORMANCE */ 
    const double pitch[] = { 0.00,  0.00 }; /* rad -> DEACTIVATED*/ 

    // Minimum pitch maximum allowed rates
    const double minimumPitchGenSpeedMinRate = -0.01; /* deg/s */
    const double minimumPitchGenSpeedMaxRate = 1.5; /* deg/s */

    // Generator speed low pass filtering parameters
    const double w = 1.0 * 2.0 * 3.1416; /* [rad/s] */
    const double d = 0.5; /* [-] */

    /*
      ####################################################################
    */
    /*! [Minimum pitch] */

    params->minimumPitchGenSpeedTableN = n;
    for (i = 0; i < n; i++) {
        params->minimumPitchGenSpeedTableX[i] = genSpeed[i];
        params->minimumPitchGenSpeedTableY[i] = pitch[i] / 3.1416 * 180.0;
    }

    params->samplingInterval = T;
    params->minimumPitchGenSpeedMinRate = minimumPitchGenSpeedMinRate;
    params->minimumPitchGenSpeedMaxRate = minimumPitchGenSpeedMaxRate;


    /*! [Generator Speed lowpass filter] */
    /*
      ####################################################################
      Transfer function (to be done twice as we want a 4th order filter):
      H(s) = w^2 / (s^2 + 2*d*w*s + w^2)

      Passing to the z-domain:
                    (0.5*T*w)^2                                                                   z^2 + 2z + 1
      H(z) =  --------------------------- -----------------------------------------------------------------------------------------------------------------
               1 + T*d*w + (0.5*T*w)^2     z^2 - 2*(1 - (0.5*T*w)^2) / (1 + T*d*w + (0.5*T*w)^2)z + (1 - T*d*w + (0.5*T*w)^2) / (1 + T*d*w + (0.5*T*w)^2)
    */
    params->generatorSpeedFilter.tfParams[0].enable = 1;
    params->generatorSpeedFilter.tfParams[0].b[0] = 1.0;
    params->generatorSpeedFilter.tfParams[0].b[1] = 2.0;
    params->generatorSpeedFilter.tfParams[0].b[2] = 1.0;
    params->generatorSpeedFilter.tfParams[0].a[0] = 1.0;
    params->generatorSpeedFilter.tfParams[0].a[1] = -2 * (1 - (0.5 * T * w) * (0.5 * T * w)) / (1 + T * d * w + (0.5 * T * w) * (0.5 * T * w));
    params->generatorSpeedFilter.tfParams[0].a[2] = (1 - T * d * w + (0.5 * T * w) * (0.5 * T * w)) / (1 + T * d * w + (0.5 * T * w) * (0.5 * T * w));

    params->generatorSpeedFilter.tfParams[1].enable = 1;
    params->generatorSpeedFilter.tfParams[1].b[0] = 1.0;
    params->generatorSpeedFilter.tfParams[1].b[1] = 2.0;
    params->generatorSpeedFilter.tfParams[1].b[2] = 1.0;
    params->generatorSpeedFilter.tfParams[1].a[0] = 1.0;
    params->generatorSpeedFilter.tfParams[1].a[1] = -2 * (1 - (0.5 * T * w) * (0.5 * T * w)) / (1 + T * d * w + (0.5 * T * w) * (0.5 * T * w));
    params->generatorSpeedFilter.tfParams[1].a[2] = (1 - T * d * w + (0.5 * T * w) * (0.5 * T * w)) / (1 + T * d * w + (0.5 * T * w) * (0.5 * T * w));

    params->generatorSpeedFilter.tfParams[2].enable = 1;
    params->generatorSpeedFilter.tfParams[2].b[0] = ((0.5 * T * w) * (0.5 * T * w) / (1 + T * d * w + (0.5 * T * w) * (0.5 * T * w))) * ((0.5 * T * w) * (0.5 * T * w) / (1 + T * d * w + (0.5 * T * w) * (0.5 * T * w)));
}


void ikTunePitchPIGainSchedule(ikConLoopParams *params) {
    int i;
        
    /*! [Gain schedule] */
    /*
      ####################################################################
      Pitch Gain Schedule

      Set parameters here:
    */
    const int n = 11; /* number of points in the lookup table */
    const double pitch[] = { 0.0,   3.8424, 5.6505, 8.1091, 11.6797, 14.5687, 17.1140, 19.4472, 21.6249, 23.6774, 25.000 }; /* degrees */
    const double gain[] = { 2.1000, 2.1000, 2.0727, 1.7182, 1.51820, 1.35450, 1.26360, 1.19090, 1.11820, 1.05450, 1.0545 }; /* - ORIGINAL ONSHORE GAINS */
    
    /*
      ####################################################################
    */
    /*! [Gain schedule] */

    params->linearController.gainSchedN = n;

    for (i = 0; i < n; i++) {
        params->linearController.gainSchedX[i] = pitch[i];
        params->linearController.gainSchedY[i] = gain[i];
    }   
}

void ikTunePitchLowpassFilter(ikConLoopParams *params, double T) {

    /*! [Pitch lowpass filter] */
    /*
      ####################################################################
      Speed feedback low pass filter

      Transfer function (to be done twice - we want a 4th order filter):
      H(s) = w^2 / (s^2 + 2*d*w*s + w^2)

      The sampling time is given by function parameter T.

      Set parameters here:
    */
    const double w = 5.6; /* [rad/s] */
    const double d = 0.5; /* [-] */
    /*
      ####################################################################
    */
    /*! [Pitch lowpass filter] */

    /*
      tune the pitch control feedback filter to this tf (twice, mind you):
      (0.5*T*w)^2                                                                     z^2 + 2z + 1
      H(z) =  -----------------------------   ------------------------------------------------------------------------------------------------------------------------
      1 + T*d*w +  (0.5*T*w)^2    z^2 - 2*(1 - (0.5*T*w)^2) / (1 + T*d*w +  (0.5*T*w)^2)z +  (1 - T*d*w +  (0.5*T*w)^2) / (1 + T*d*w +  (0.5*T*w)^2)
    */
    params->linearController.measurementTfs.tfParams[1].enable = 1;
    params->linearController.measurementTfs.tfParams[1].b[0] = 1.0;
    params->linearController.measurementTfs.tfParams[1].b[1] = 2.0;
    params->linearController.measurementTfs.tfParams[1].b[2] = 1.0;
    params->linearController.measurementTfs.tfParams[1].a[0] = 1.0;
    params->linearController.measurementTfs.tfParams[1].a[1] = -2 * (1 - (0.5*T*w)*(0.5*T*w)) / (1 + T*d*w + (0.5*T*w)*(0.5*T*w));
    params->linearController.measurementTfs.tfParams[1].a[2] = (1 - T*d*w + (0.5*T*w)*(0.5*T*w)) / (1 + T*d*w + (0.5*T*w)*(0.5*T*w));

    params->linearController.measurementTfs.tfParams[2].enable = 1;
    params->linearController.measurementTfs.tfParams[2].b[0] = 1.0;
    params->linearController.measurementTfs.tfParams[2].b[1] = 2.0;
    params->linearController.measurementTfs.tfParams[2].b[2] = 1.0;
    params->linearController.measurementTfs.tfParams[2].a[0] = 1.0;
    params->linearController.measurementTfs.tfParams[2].a[1] = -2 * (1 - (0.5*T*w)*(0.5*T*w)) / (1 + T*d*w + (0.5*T*w)*(0.5*T*w));
    params->linearController.measurementTfs.tfParams[2].a[2] = (1 - T*d*w + (0.5*T*w)*(0.5*T*w)) / (1 + T*d*w + (0.5*T*w)*(0.5*T*w));

    params->linearController.measurementTfs.tfParams[3].enable = 1;
    params->linearController.measurementTfs.tfParams[3].b[0] = ((0.5*T*w)*(0.5*T*w) / (1 + T*d*w + (0.5*T*w)*(0.5*T*w))) * ((0.5*T*w)*(0.5*T*w) / (1 + T*d*w + (0.5*T*w)*(0.5*T*w)));

}

void ikTunePitchNotches(ikConLoopParams *params, double T) {

    /*! [1st fore-aft tower mode filter] */
    /*
      ####################################################################
      1st fore-aft tower mode filter

      Transfer function:
      H(s) = (s^2 + 2*dnum*w*s + w^2) / (s^2 + 2*dden*w*s + w^2)

      The sampling time is given by function parameter T.

      Set parameters here:
    */
    const double wfa = 1.59; /* [rad/s] */
    const double dnumfa = 0.01; /* [-] */
    const double ddenfa = 0.2; /* [-] */
    /*
      ####################################################################
    */
    /*! [1st fore-aft tower mode filter] */

    /*! [3p filter] */
    /*
      ####################################################################
      3p filter

      Transfer function:
      H(s) = (s^2 + 2*dnum*w*s + w^2) / (s^2 + 2*dden*w*s + w^2)

      The sampling time is given by function parameter T.

      Set parameters here:
    */
    const double w3p = 3.016; /* [rad/s] */
    const double dnum3p = 0.01; /* [-] */
    const double dden3p = 0.2; /* [-] */
    /*
      ####################################################################
    */
    /*! [1st fore-aft tower mode filter] */

    params->linearController.measurementNotches.dT = T;
    params->linearController.measurementNotches.notchParams[0].enable = 1;
    params->linearController.measurementNotches.notchParams[0].freq = wfa;
    params->linearController.measurementNotches.notchParams[0].dampNum = dnumfa;
    params->linearController.measurementNotches.notchParams[0].dampDen = ddenfa;

    params->linearController.measurementNotches.notchParams[1].enable = 1;
    params->linearController.measurementNotches.notchParams[1].freq = w3p;
    params->linearController.measurementNotches.notchParams[1].dampNum = dnum3p;
    params->linearController.measurementNotches.notchParams[1].dampDen = dden3p;

}

void ikTunePitchPI(ikConLoopParams *params, double T) {

    /*! [Pitch PI] */
    /*
      ####################################################################
      Pitch PI

      Transfer function:

      C(s) = (Kp*s + Ki)/s

      The sampling time is given by function parameter T.

      Set parameters here:
    */
    const double Kp = -0.3939; /* [degs/rad] 7.2e-4 rad/rpm */
    const double Ki = -0.1313; /* [deg/rad] 2.4e-4 rad/rpms */
    /*
      ####################################################################
    */
    /*! [Pitch PI] */


    /*
      tune the speed control to this tf:
             (Kp + Ki*T/2)z - (Kp - Ki*T/2)
      C(z) = ------------------------------
                      z - 1
      rad/s --> deg
    */
    params->linearController.errorTfs.tfParams[0].enable = 1;
    params->linearController.errorTfs.tfParams[0].b[0] = (Kp/T + Ki/2);
    params->linearController.errorTfs.tfParams[0].b[1] = -(Kp/T - Ki/2);
    params->linearController.errorTfs.tfParams[0].b[2] = 0.0;
    params->linearController.errorTfs.tfParams[0].a[0] = 1.0;
    params->linearController.errorTfs.tfParams[0].a[1] = 0.0;
    params->linearController.errorTfs.tfParams[0].a[2] = 0.0;

    params->linearController.postGainTfs.tfParams[0].enable = 1;
    params->linearController.postGainTfs.tfParams[0].b[0] = T;
    params->linearController.postGainTfs.tfParams[0].b[1] = 0.0;
    params->linearController.postGainTfs.tfParams[0].b[2] = 0.0;
    params->linearController.postGainTfs.tfParams[0].a[0] = 1.0;
    params->linearController.postGainTfs.tfParams[0].a[1] = -1.0;
    params->linearController.postGainTfs.tfParams[0].a[2] = 0.0;

}

void ikTuneTorqueLowpassFilter(ikConLoopParams *params, double T) {

    /*! [Torque lowpass filter] */
    /*
      ####################################################################
      Speed feedback low pass filter

      Transfer function (to be done twice - we want a 4th order filter):
      H(s) = w^2 / (s^2 + 2*d*w*s + w^2)

      The sampling time is given by function parameter T.

      Set parameters here:
    */
    const double w = 3.39; /* [rad/s] */
    const double d = 0.5; /* [-] */
    /*
      ####################################################################
    */
    /*! [Torque lowpass filter] */

    /*
      tune the torque control feedback filter to this tf (twice, mind you):
      (0.5*T*w)^2                                                                     z^2 + 2z + 1
      H(z) =  -----------------------------   ------------------------------------------------------------------------------------------------------------------------
      1 + T*d*w +  (0.5*T*w)^2    z^2 - 2*(1 - (0.5*T*w)^2) / (1 + T*d*w +  (0.5*T*w)^2)z +  (1 - T*d*w +  (0.5*T*w)^2) / (1 + T*d*w +  (0.5*T*w)^2)
    */
    params->linearController.measurementTfs.tfParams[0].enable = 1;
    params->linearController.measurementTfs.tfParams[0].b[0] = 1.0;
    params->linearController.measurementTfs.tfParams[0].b[1] = 2.0;
    params->linearController.measurementTfs.tfParams[0].b[2] = 1.0;
    params->linearController.measurementTfs.tfParams[0].a[0] = 1.0;
    params->linearController.measurementTfs.tfParams[0].a[1] = -2 * (1 - (0.5*T*w)*(0.5*T*w)) / (1 + T*d*w + (0.5*T*w)*(0.5*T*w));
    params->linearController.measurementTfs.tfParams[0].a[2] = (1 - T*d*w + (0.5*T*w)*(0.5*T*w)) / (1 + T*d*w + (0.5*T*w)*(0.5*T*w));

    params->linearController.measurementTfs.tfParams[1].enable = 1;
    params->linearController.measurementTfs.tfParams[1].b[0] = 1.0;
    params->linearController.measurementTfs.tfParams[1].b[1] = 2.0;
    params->linearController.measurementTfs.tfParams[1].b[2] = 1.0;
    params->linearController.measurementTfs.tfParams[1].a[0] = 1.0;
    params->linearController.measurementTfs.tfParams[1].a[1] = -2 * (1 - (0.5*T*w)*(0.5*T*w)) / (1 + T*d*w + (0.5*T*w)*(0.5*T*w));
    params->linearController.measurementTfs.tfParams[1].a[2] = (1 - T*d*w + (0.5*T*w)*(0.5*T*w)) / (1 + T*d*w + (0.5*T*w)*(0.5*T*w));

    params->linearController.measurementTfs.tfParams[2].enable = 1;
    params->linearController.measurementTfs.tfParams[2].b[0] = ((0.5*T*w)*(0.5*T*w) / (1 + T*d*w + (0.5*T*w)*(0.5*T*w))) * ((0.5*T*w)*(0.5*T*w) / (1 + T*d*w + (0.5*T*w)*(0.5*T*w)));

}

void ikTuneTorqueNotches(ikConLoopParams *params, double T) {

    /*! [1st side-side tower mode filter] */
    /*
      ####################################################################
      1st side-side tower mode filter

      Transfer function:
      H(s) = (s^2 + 2*dnum*w*s + w^2) / (s^2 + 2*dden*w*s + w^2)

      The sampling time is given by function parameter T.

      Set parameters here:
    */
    const double w = 1.59; /* [rad/s] */
    const double dnum = 0.01; /* [-] */
    const double dden = 0.2; /* [-] */
    /*
      ####################################################################
    */
    /*! [1st side-side tower mode filter] */

    params->linearController.measurementNotches.dT = T;
    params->linearController.measurementNotches.notchParams[0].enable = 1;
    params->linearController.measurementNotches.notchParams[0].freq = w;
    params->linearController.measurementNotches.notchParams[0].dampNum = dnum;
    params->linearController.measurementNotches.notchParams[0].dampDen = dden;

}

void ikTuneTorquePI(ikConLoopParams *params, double T) {

    /*! [Torque PI] */
    /*
      ####################################################################
      Torque PI

      Transfer function:

      C(s) = (Kp*s + Ki)/s

      The sampling time is given by function parameter T.

      Set parameters here:
    */
    const double Kp = -34.3775; /* [kNms/rad] 3600 Nm/rpm */
    const double Ki = -11.4592; /* [kNm/rad] 1200 Nm/rpms */
    /*
      ####################################################################
    */
    /*! [Torque PI] */

    /*
      tune the torque control to this tf:
      (Kp + Ki*T/2)z - (Kp - Ki*T/2)
      C(z) = ------------------------------
      z - 1
      rad/s --> kNm
    */
    params->linearController.errorTfs.tfParams[0].enable = 1;
    params->linearController.errorTfs.tfParams[0].b[0] = (Kp + Ki*T/2);
    params->linearController.errorTfs.tfParams[0].b[1] = -(Kp - Ki*T/2);
    params->linearController.errorTfs.tfParams[0].b[2] = 0.0;
    params->linearController.errorTfs.tfParams[0].a[0] = 1.0;
    params->linearController.errorTfs.tfParams[0].a[1] = 0.0;
    params->linearController.errorTfs.tfParams[0].a[2] = 0.0;

    params->linearController.postGainTfs.tfParams[0].enable = 1;
    params->linearController.postGainTfs.tfParams[0].b[0] = 1.0;
    params->linearController.postGainTfs.tfParams[0].b[1] = 0.0;
    params->linearController.postGainTfs.tfParams[0].b[2] = 0.0;
    params->linearController.postGainTfs.tfParams[0].a[0] = 1.0;
    params->linearController.postGainTfs.tfParams[0].a[1] = -1.0;
    params->linearController.postGainTfs.tfParams[0].a[2] = 0.0;

}


void ikTuneTorqueFromHubPitch(ikTfListParams *params, double T) {

    /*! [Torque from Platform pitch transfer function] */
    /*
      ####################################################################
      Torque from Platform pitch transfer function

      Transfer function:

      H(s) = Kev * s^2 * s/(s^2+2*d*w1*s+w1^2) * w2^2/(s^2 + 2*d*w2*s + w2^2)

      The sampling time is given by function parameter T.

      Set parameters here:
    */
    const double Kev = 4800; /* 4800 kNm/deg */

    const double w1 = 2 * 3.1416 / 2300; /* [rad/s] */
    const double d1 = 0.5; /* [-] */

    const double w2 = 2 * 3.1416; /* [rad/s] */
    const double d2 = 0.5; /* [-] */
    /*
      ####################################################################
    */
    /*! [Torque from Platform pitch] */

    /*
      tune the torque from platform pitch to this tf:
				  4/T^2*(z^-2-2*z^-1+1)						 T/2*(1-z^-2)											    		           (T/2*w2)^2*(z^-2 + 2*z^-1 + 1)
      H(z) = Kev ----------------------- ----------------------------------------------------------------------------- ----------------------------------------------------------------------------- 
                     (z^-2+2*z^-1+1)      (1-d1*T*w1+(w1*T/2)^2)*z^-2 + (2*(w1*T/2)^2-2)*z^-1 + (1+d1*T*w1+(w1*T/2)^2)  (1-d2*T*w2+(w2*T/2)^2)*z^-2 + (2*(w2*T/2)^2-2)*z^-1 + (1+d2*T*w2+(w2*T/2)^2)
    */
    params->tfParams[0].enable = 1;
    params->tfParams[0].b[0] = Kev;
    params->tfParams[0].a[0] = 1;
    
    params->tfParams[1].enable = 1;
    params->tfParams[1].b[0] = 4 / (T * T);
    params->tfParams[1].b[1] = -8 / (T * T);
    params->tfParams[1].b[2] = 4 / (T * T);
    params->tfParams[1].a[0] = 1;
    params->tfParams[1].a[1] = 2;
    params->tfParams[1].a[2] = 1;

    params->tfParams[2].enable = 1;
    params->tfParams[2].b[0] = T / 2;
    params->tfParams[2].b[1] = 0.0;
    params->tfParams[2].b[2] = -T / 2;
    params->tfParams[2].a[0] = 1 + d1 * T * w1 + (w1 * T / 2) * (w1 * T / 2);
    params->tfParams[2].a[1] = -2 + 2 * (w1 * T / 2) * (w1 * T / 2);
    params->tfParams[2].a[2] = 1 - d1 * T * w1 + (w1 * T / 2) * (w1 * T / 2);

    params->tfParams[3].enable = 1;
    params->tfParams[3].b[0] = T * T / 4 * w2 * w2;
    params->tfParams[3].b[1] = T * T / 2 * w2 * w2;
    params->tfParams[3].b[2] = T * T / 4 * w2 * w2;
    params->tfParams[3].a[0] = 1 + d2 * T * w2 + (w2 * T / 2) * (w2 * T / 2);
    params->tfParams[3].a[1] = -2 + 2 * (w2 * T / 2) * (w2 * T / 2);
    params->tfParams[3].a[2] = 1 - d2 * T * w2 + (w2 * T / 2) * (w2 * T / 2);
}


void ikChangeCoordinateSystem(float* x, float* y, float rotationAngle, float originDisplacement_x, float originDisplacement_y) {
    double x_final;
    double y_final;

    x_final = *x * cos(rotationAngle) - *y * sin(rotationAngle) + originDisplacement_x;
    y_final = *x * sin(rotationAngle) + *y * cos(rotationAngle) + originDisplacement_y;

    *x = x_final;
    *y = y_final;
}