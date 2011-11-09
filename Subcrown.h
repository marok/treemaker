#ifndef ISUBCROWN_H
#define	ISUBCROWN_H

#include<vector>

enum CrownShape
{
	SPLINE,
	CYLINDER,
	SPHERE
};

class Subcrown
{
public:
    enum CrownShape shape;
    virtual std::vector<Point3d *> generatePoints(int n) = 0;
};


#endif	/* ISUBCROWN_H */

