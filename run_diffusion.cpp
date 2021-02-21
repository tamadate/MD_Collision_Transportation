//------------------------------------------------------------------------
#include "md.hpp"
//------------------------------------------------------------------------


/////////////////////////////////////////////////////////////////////
/*	
	- Diffusion coefficient calculation
	- MD simulation performed for (step_relax) steps as a thermal
	relaxation, and main calculation run for (Noftimestep) steps.
	- Among of two calculations, a calculation is re-initialized.
	- Properties (pressure, temperature, energy etc...) are observed
	each (OBSERVE) steps.
*/
/////////////////////////////////////////////////////////////////////
void
MD::run_diff(char** argv) {
 	const int OBSERVE = 500;
	for (int it=0; it < pp->step_relax; it++) {
		if (it%OBSERVE==0) {
			display(1);
			export_dump();
			vars->time+=(OBSERVE*dt);
		}
    	if (flags->cg==0) verlet();
    	if (flags->cg==1) verlet_cg();
	}
	reinitialization();
	setNVE();
	for (long int it=0; it<Noftimestep; it++) {
		if (it%OBSERVE==0) {
			display(0);
			analysis_gas();
			output();
			output_gas();
	    	if (flags->cg==0) export_dump();
	    	if (flags->cg==1) export_dump_CG();
			vars->time+=(OBSERVE*dt);
    	}
    	if (flags->cg==0) verlet();
    	if (flags->cg==1) verlet_cg();
		if (it%VELOCITY_DISTRIBUTION==0) {
			velocity_distribution();
			if (it%UPDATE_DISTRIBUTION==0) update_distribution();
		}
	}
}

