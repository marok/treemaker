#ifndef ISUBCROWN_H
#define	ISUBCROWN_H

#include<vector>
#define TRUNCATEDCONE_CROWN(sub) ((TruncatedConeCrown*)sub)
#define SPLINE_CROWN(sub) ((SplineCrown*)sub)
#define SPHERE_CROWN(sub)((SphereCrown*)sub)

enum CrownShape
{
	SPLINE=0,
	TRUNCATEDCONE=1,
	SPHERE=2,
	CROWNSHAPE_N=3
};


class Subcrown
{
public:
	Subcrown(){
	  negative=false;
	}
	enum CrownShape shape;
	float x,y,z;
	bool negative;
	virtual std::vector<Point3d *> generatePoints(int n) = 0;
	/* Serialization methods */
	virtual void save (ofstream &s)
	{
#define SAVE(x) s<<x<<endl;
		SAVE(x);
		SAVE(y);
		SAVE(z);
		SAVE(negative);
		SAVE(shape);
#undef SAVE
	}
	virtual void load(ifstream &s)
	{
#define LOAD(x) s>>x;
		LOAD(x);
		LOAD(y);
		LOAD(z);
		LOAD(negative);
		int w;
		s>>w;
		switch(w){
		  case 0: shape=SPLINE; break;
		  case 1: shape=TRUNCATEDCONE; break; 
		  case 2: shape=SPHERE; break; 
		  case 3: shape=CROWNSHAPE_N; break; 
		}
#undef LOAD
	}
};


#endif	/* ISUBCROWN_H */

