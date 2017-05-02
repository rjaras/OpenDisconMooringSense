#define NINT(a) ((a) >= 0.0 ? (int) ((a)+0.5) : ((a)-0.5))

#include "ikWitcon.h"
#include "ikDiscon_EXPORT.h"

void ikDiscon_EXPORT DISCON(float *DATA, int FLAG, const char *INFILE, const char *OUTNAME, char *MESSAGE) {
	int err;
	static ikWitcon *con;
	ikWitconIn in;
	ikWitconOut out;
		
	if (NINT(DATA[0]) == 0) {
		con = ikWitcon_init();
	}
	
	in.start = 1;
	in.stop = 0;
	in.pitchBlade1 = (double) DATA[3]/3.1416*180.0; // rad to deg
	in.pitchBlade2 = (double) DATA[32]/3.1416*180.0; // rad to deg
	in.pitchBlade3 = (double) DATA[33]/3.1416*180.0; // rad to deg
	in.generatorTorque = (double) DATA[21]*1.0e-3; // Nm to kNm
	in.generatorSpeed = (double) DATA[19]; // rad/s
	in.maximumPower = (double) DATA[12]*1.0e-3; // W to kW
	in.reservePower = 0.0;
	
	ikWitcon_step(con, &in, &out);
	
	DATA[46] = (float) (out.torqueDemand*1.0e3); // kNm to Nm
	DATA[41] = (float) (out.pitchDemandBlade1/180.0*3.1416); // deg to rad
	DATA[42] = (float) (out.pitchDemandBlade2/180.0*3.1416); // deg to rad
	DATA[43] = (float) (out.pitchDemandBlade3/180.0*3.1416); // deg to rad
}	
