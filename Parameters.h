#ifndef _PARAMETERS_H
#define _PARAMETERS_H
#include "RenderingParameters.h"
#include "TrunkParameters.h"
#include "LeavesParameters.h"
#include "Crown.h"
#include "SplineCrown.h"
#include "TruncatedConeCrown.h"
#define TREEMAKER_VERSION "treemaker-ver0.1"
class Parameters {
public:

	RenderingParameters	*rp;
	TrunkParameters		*tp;
	LeavesParameters	*lp;
	Crown                   *crown;
	MethodParameters 	*methodParams;

	void clean() {
	}
	void setDefaults() {
		clean();
		rp=new RenderingParameters();
		tp=new TrunkParameters();
		lp=new LeavesParameters();
		methodParams = new MethodParameters();
		crown = new Crown(methodParams);
	}
	Parameters() {
		setDefaults();
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