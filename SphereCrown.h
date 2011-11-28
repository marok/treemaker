#ifndef SPHERECROWN_H
#define	SPHERECROWN_H

#include <vector>
#include "Subcrown.h"

class SphereCrown: public Subcrown
{
public:
	float r;
	
	SphereCrown(float x, float y, float z, float r)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->r = r;
		shape = SPHERE;
	}
	
	SphereCrown()
	{
		shape = SPHERE;
	}
	
	std::vector<Point3d *> generatePoints(int n)
	{
		std::vector<Point3d *> result;
		Point3d p = Point3d(0,0,0);
#define RANDF (float)random()/(float)RAND_MAX
		while(n)
		{
			float x_rand = RANDF * 2.0 * r - r;
			float y_rand = RANDF * 2.0 * r - r;
			float z_rand = RANDF * 2.0 * r - r;
			
			if(Point3d(x_rand,y_rand, z_rand).getDistance(&p) <= r)
			{
				result.push_back(new Point3d(x_rand+x, y_rand+y, z_rand+z));
				n--;
			}
		}
#undef RANDF
		return result;
	}
	
	void save(ofstream &s)
	{
		Subcrown::save(s);
#define SAVE(x) s<<x<<endl;
		SAVE(r);
#undef SAVE
	}
	
	void load(ifstream &stream)
	{
		Subcrown::load(stream);
#define LOAD(x) stream>>x;
		LOAD(r);
#undef LOAD
	}
};


#endif	/* SPHERECROWN_H */

