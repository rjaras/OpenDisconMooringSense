#define NINT(a) ((a) >= 0.0 ? (int) ((a)+0.5) : ((a)-0.5))

#include "ikClwindconWTConfig.h"
#include "ikDiscon_EXPORT.h"

void ikDiscon_EXPORT DISCON(float *DATA, int FLAG, const char *INFILE, const char *OUTNAME, char *MESSAGE) {
	int err;
	static ikClwindconWTCon con;
		
	if (NINT(DATA[0]) == 0) {
		ikClwindconWTConParams param;
		ikClwindconWTCon_initParams(&param);
		setParams(&param);
		ikClwindconWTCon_init(&con, &param);
	}
	
	con.in.externalMaximumTorque = 0.0; // kNm
	con.in.externalMinimumTorque = 0.0; // 0.0
	con.in.externalMaximumPitch = 90.0; // deg
	con.in.externalMinimumPitch = 0.0; // deg
	con.in.generatorSpeed = (double) DATA[19]; // rad/s
	con.in.maximumPower = (double) DATA[12]*1.0e-3; // W to kW
	con.in.maximumSpeed = 0.0; // rad/s
	
	ikClwindconWTCon_step(&con);
	
	DATA[46] = (float) (con.out.torqueDemand*1.0e3); // kNm to Nm
	DATA[41] = (float) (con.out.pitchDemandBlade1/180.0*3.1416); // deg to rad
	DATA[42] = (float) (con.out.pitchDemandBlade2/180.0*3.1416); // deg to rad
	DATA[43] = (float) (con.out.pitchDemandBlade3/180.0*3.1416); // deg to rad
}	
