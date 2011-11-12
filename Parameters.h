#ifndef _PARAMETERS_H
#define _PARAMETERS_H
#include "RenderingParameters.h"
#include "TrunkParameters.h"
#include "LeavesParameters.h"
class Parameters {
public:

	RenderingParameters	*rp;
	TrunkParameters		*tp;
	LeavesParameters	*lp;


	Parameters() {
		rp=new RenderingParameters();
		tp=new TrunkParameters();
		lp=new LeavesParameters();
	}
};
#endif