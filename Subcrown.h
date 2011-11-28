#ifndef ISUBCROWN_H
#define	ISUBCROWN_H

#include<vector>
#define TRUNCATEDCONE_CROWN(sub) ((TruncatedConeCrown*)sub)
#define SPLINE_CROWN(sub) ((SplineCrown*)sub)
#define SPHERE_CROWN(sub)((SphereCrown*)sub)

enum CrownShape
{
	SPLINE,
	TRUNCATEDCONE,
	SPHERE,
	CROWNSHAPE_N
};

class Subcrown
{
public:
	enum CrownShape shape;
	float x,y,z;
	virtual std::vector<Point3d *> generatePoints(int n) = 0;
	/* Serialization methods */
	virtual void save (ofstream &s)
	{
#define SAVE(x) s<<x<<endl;
		SAVE(x);
		SAVE(y);
		SAVE(z);
#undef SAVE
	}
	virtual void load(ifstream &s)
	{
#define LOAD(x) s>>x;
		LOAD(x);
		LOAD(y);
		LOAD(z);
#undef LOAD
	}
};


#endif	/* ISUBCROWN_H */

