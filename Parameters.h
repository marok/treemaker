#ifndef _PARAMETERS_H
#define _PARAMETERS_H
#include "RenderingParameters.h"
#include "TrunkParameters.h"
#include "LeavesParameters.h"
#include "Crown.h"
#include "SplineCrown.h"
#include "CylinderCrown.h"
#define TREEMAKER_VERSION "treemaker-ver0.1"
class Parameters {
public:

	RenderingParameters	*rp;
	TrunkParameters		*tp;
	LeavesParameters	*lp;
	Crown                   *crown;
	MethodParameters 	*methodParams;

	void clean() {
		//clean previous settings
		/*if(rp!=NULL) delete rp;
		if(tp!=NULL) delete tp;
		if(lp!=NULL) {
		//  lp->clear();
		  delete lp;
		}
		if(crown!=NULL){
		//  crown->clear();
		  delete crown;
		}
		if(methodParams!=NULL) delete methodParams;
		*/
	}
	void init() {
		clean();
		rp=new RenderingParameters();
		tp=new TrunkParameters();
		lp=new LeavesParameters();
		methodParams = new MethodParameters();
		crown = new Crown(methodParams);
		crown->subcrowns.push_back(new CylinderCrown(0,0,0,3,5));

		//crown->subcrowns.push_back(new SplineCrown(-3,-3, 4, methodParams));
		//crown->subcrowns.push_back(new SplineCrown(3, 3, 2, methodParams));
		//crown->subcrowns.push_back(new SplineCrown(0, 0, 6, methodParams));
	}
	Parameters() {
		init();
	}
	/* Serialization methods */
	void save(ofstream &s)
	{
		s<<TREEMAKER_VERSION<<endl;
		rp->save(s);
		tp->save(s);
		lp->save(s);
		methodParams->save(s);
		crown->save(s);
	}
	void load(ifstream &s)
	{
		string version;
		s>>version;
		if(version.compare(TREEMAKER_VERSION)!=0)
			return;
		rp->load(s);
		tp->load(s);
		lp->load(s);
		methodParams->load(s);
		crown->load(s);
	}
};
#endif