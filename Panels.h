#ifndef PANELS_H
#define	PANELS_H

#include "IPanel.h"
#include <vector>
class Panels
{
	MethodParametersPanel *mpp;
	CrownParametersPanel *cpp;
	RenderingParametersPanel *rpp;
	TrunkParametersPanel *tpp;
	LeavesParametersPanel *lpp;
	ExportParametersPanel *xpp;
	EditPanel *ep;

	std::vector<IPanel*> panels;
public:
	Panels(MethodParametersPanel *mpp,
	       CrownParametersPanel *cpp,
	       RenderingParametersPanel *rpp,
	       TrunkParametersPanel *tpp,
	       LeavesParametersPanel *lpp,
	       EditPanel *ep,
	       ExportParametersPanel *xpp)
	{
		this->mpp = mpp;
		panels.push_back(mpp);

		this->cpp = cpp;
		panels.push_back(cpp);

		this->rpp = rpp;
		panels.push_back(rpp);

		this->tpp = tpp;
		panels.push_back(tpp);

		this->lpp = lpp;
		panels.push_back(lpp);

		this->ep = ep;
		panels.push_back(ep);

		this->xpp=xpp;
		panels.push_back(xpp);
	}

	MethodParametersPanel* getMPP()
	{
		return mpp;
	}

	CrownParametersPanel* getCPP()
	{
		return cpp;
	}

	RenderingParametersPanel* getRPP()
	{
		return rpp;
	}

	TrunkParametersPanel* getTPP()
	{
		return tpp;
	}

	LeavesParametersPanel* getLPP()
	{
		return lpp;
	}

	EditPanel *getEP()
	{
		return ep;
	}

	ExportParametersPanel *getXP()
	{
		return xpp;
	}

	void updatePanels()
	{
		for(unsigned int i=0; i<panels.size(); i++)
			panels[i]->updatePanel();
	}
};


#endif	/* PANELS_H */

