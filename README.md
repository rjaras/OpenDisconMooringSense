# OpenDiscon
Open source implementation of popular wind turbine controller interfaces.

Currently available distributions are for:
- The legacy GHBladed external controller interface (typically called DISCON, hence the name OpenDiscon).
- Level-2 MATLAB S-Function for Simulink.

Currently available configurations are for:
- MooringSense (tuned for DTU 10MW wind turbine with SAITEC SATH floating offshore platform models used in the 
MooringSense EU project) (only DISCON distribution available for this configuration).
- Simple (tuned for the DTU 10MW model)
- CL-Windcon (DTU 10MW model).

This uses [IKERLAN](http://www.ikerlan.es/en/)'s library OpenWitcon, included as a submodule.
Documentation is provided in Doxygen format. To generate it, run Doxygen on ./doc/Doxyfile.

For compilation, run cmake here and choose the desired CONFIGURATION and DISTRIBUTION.
This will generate the VS solution, makefiles or MATLAB function for straightforward compilation, depending on your 
toolchain.
