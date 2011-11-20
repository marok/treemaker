#ifndef ISUBCROWN_H
#define	ISUBCROWN_H

#include<vector>
#define CYLINDER_CROWN(sub) ((CylinderCrown*)sub)
#define SPLINE_CROWN(sub) ((SplineCrown*)sub)

enum CrownShape
{
	SPLINE,
	CYLINDER,
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
	virtual void save (ofstream &s) = 0;
	virtual void load(ifstream &s) = 0;
};


#endif	/* ISUBCROWN_H */

