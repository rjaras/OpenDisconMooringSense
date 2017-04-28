#define NINT(a) ((a) >= 0.0 ? (int) ((a)+0.5) : ((a)-0.5))

#include "ikVpvsController.h"
#include "ikDisconConfig.h"
#include "ikDiscon_EXPORT.h"

void ikDiscon_EXPORT DISCON(float *DATA, int FLAG, const char *INFILE, const char *OUTNAME, char *MESSAGE) {
	int err;
	ikVpvsController con;
	ikVpvsControllerParams param;
		
	if (NINT(DATA[0]) == 0) {
//		setParams(&param);
		err = ikVpvsController_init(&con, &param);
	}
	
	con.in.start = 1;
	con.in.stop = 0;
	con.in.pitchBlade1 = (double) DATA[3]/3.1416*180.0; // rad to deg
	con.in.pitchBlade2 = (double) DATA[32]/3.1416*180.0; // rad to deg
	con.in.pitchBlade3 = (double) DATA[33]/3.1416*180.0; // rad to deg
	con.in.generatorTorque = (double) DATA[21]*1.0e-3; // Nm to kNm
	con.in.generatorSpeed = (double) DATA[19];
	con.in.maximumPower = (double) DATA[12]*1.0e-3;
	con.in.reservePower = 0.0;
	
	ikVpvsController_step(&con);
	
	DATA[46] = (float) (con.out.torqueDemand*1.0e3); // kNm to Nm
	DATA[41] = (float) (con.out.pitchDemandBlade1/180.0*3.1416); // deg to rad
	DATA[42] = (float) (con.out.pitchDemandBlade2/180.0*3.1416); // deg to rad
	DATA[43] = (float) (con.out.pitchDemandBlade3/180.0*3.1416); // deg to rad
}	
