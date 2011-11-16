#ifndef _PARAMETERS_H
#define _PARAMETERS_H
#include "RenderingParameters.h"
#include "TrunkParameters.h"
#include "LeavesParameters.h"
#include "Crown.h"
#include "SplineCrown.h"
#include "CylinderCrown.h"

class Parameters {
public:

	RenderingParameters	*rp;
	TrunkParameters		*tp;
	LeavesParameters	*lp;
	Crown                   *crown;
	MethodParameters *methodParams;

	Parameters() {
		rp=new RenderingParameters();
		tp=new TrunkParameters();
		lp=new LeavesParameters();
		methodParams = new MethodParameters();
		crown = new Crown(methodParams);

		//tmp

		//SplineCrown *sCrown = new SplineCrown(-3,-3, 4, methodParams);
		//SplineCrown *sCrown2 = new SplineCrown(3, 3, 2, methodParams);

		SplineCrown *sCrown3 = new SplineCrown(0, 0, 6, methodParams);


		//CylinderCrown *cylinder = new CylinderCrown(0,0,0,6,15);
		//crown->subcrowns.push_back(cylinder);


		//crown->subcrowns.push_back(sCrown);
		//crown->subcrowns.push_back(sCrown2);
		crown->subcrowns.push_back(sCrown3);
	}
};
#endif