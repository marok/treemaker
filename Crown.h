#ifndef CROWN_H
#define	CROWN_H

#include<vector>

#include "Subcrown.h"
#include "SplineCrown.h"
#include "CylinderCrown.h"

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
		setDefault();
	}
	
	void setDefault()
	{
		subcrowns.clear();
		subcrowns.push_back(new CylinderCrown(0,0,2,5,3,8));
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
	void clear() {
		for(unsigned int i=0; i<subcrowns.size(); i++)
			delete subcrowns[i];
		subcrowns.clear();
	}
	/* Serialization methods */
	void save(ofstream &s)
	{
		s<<subcrowns.size()<<endl;
		for(unsigned int i=0; i<subcrowns.size(); i++) {
			int shape=subcrowns[i]->shape;
			s<<shape<<endl;
			subcrowns[i]->save(s);
		}
	}
	void load(ifstream &s)
	{
		clear();
		unsigned int size=0;
		s>>size;
		for(unsigned int i=0; i<size; i++) {
			int shape;
			s>>shape;
			Subcrown *sub;
			switch(shape)
			{
			case SPLINE:
				sub=new SplineCrown();
				break;
			case CYLINDER:
				sub=new CylinderCrown();
				break;
			default:
				printf("Nieprawidlowy typ korony podczas wczytywania pliku ustawien");
				assert(0);
			}
			sub->load(s);
			subcrowns.push_back(sub);
		}
	}
};

#endif	/* CROWN_H */

