#ifndef IKWITCON_H
#define IKWITCON_H

#ifdef __cplusplus
extern "C" {
#endif
  
	typedef struct ikWitcon ikWitcon;

	typedef struct ikWitconIn {
		int start; /**<start command*/
		int stop; /**<stop command*/
		double pitchBlade1; /**<blade 1 pitch angle in degrees*/
		double pitchBlade2; /**<blade 2 pitch angle in degrees*/
		double pitchBlade3; /**<blade 3 pitch angle in degrees*/
		double generatorTorque; /**<generator torque in kNm*/
		double generatorSpeed; /**<generator speed in rad/s*/
		double maximumPower; /**<maximum power setpoint in kW*/
		double reservePower; /**<reserve power setpoint in kW*/	
	} ikWitconIn;

	typedef struct ikWitconOut {
        double torqueDemand; /**<torque demand in kNm*/
        double pitchDemandBlade1; /**<pitch demand for blade 1 in degrees*/
        double pitchDemandBlade2; /**<pitch demand for blade 2 in degrees*/
        double pitchDemandBlade3; /**<pitch demand for blade 3 in degrees*/
        int status; /**<status code as in @link ikSsproc @endlink*/
    } ikWitconOut;

	ikWitcon * ikWitcon_init();

	void ikWitcon_step(ikWitcon *con, const ikWitconIn *in, ikWitconOut *out);

#ifdef __cplusplus
}
#endif

#endif /* IKWITCON_H */
