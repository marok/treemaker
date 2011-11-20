#ifndef CROWN_H
#define	CROWN_H

#include<vector>
#include"Subcrown.h"

class Crown
{
private:
	MethodParameters *params;
public:
	std::vector<Subcrown *> subcrowns;

	//aktualnie zaznaczona korona
	//-1 kiedy żadna nie jest zaznaczona
	int activeSubcrown;

	Crown(MethodParameters *params)
	{
		this->params = params;
		this->activeSubcrown = -1;
	}

	std::vector<Point3d *> generatePoints()
	{
		std::vector<Point3d *> result;
		for(unsigned int i=0; i<subcrowns.size(); i++)
		{
			Subcrown *scrown = subcrowns.at(i);
			std::vector<Point3d *> pts = scrown->generatePoints(params->points/subcrowns.size());
			for(unsigned int j=0; j<pts.size(); j++)
				result.push_back(pts.at(j));
		}

		return result;
	}
};

#endif	/* CROWN_H */

