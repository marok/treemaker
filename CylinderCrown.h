#ifndef CYLINDERCROWN_H
#define	CYLINDERCROWN_H

#include "Subcrown.h"
#include <cmath>

class CylinderCrown: public Subcrown
{
public:
	float x,y;
	float r, downZ, upZ;
	
	CylinderCrown(float x, float y, float r, float downZ, float upZ)
	{
		this->shape = CYLINDER;
		this->x = x;
		this->y = y;
		this->r = r;
		this->downZ = downZ;
		this->upZ = upZ;
	}

	std::vector<Point3d *> generatePoints(int n) {
		int points_num = n;
		std::vector<Point3d *> result;
		result.clear();


		float h = upZ - downZ;

#define RANDF() (float)random()/(float)RAND_MAX

		while (points_num) {
			float r_rand = sqrt(RANDF()) * r;
			float h_rand = RANDF() * h;


			float alfa = RANDF()*2.0 * M_PI;
			Point3d *point = new Point3d(r_rand * sin(alfa)+x, r_rand * cos(alfa)+y, downZ + h_rand);
			result.push_back(point);

			points_num--;
		}
#undef RANDF
		return result;
	}
};

#endif	/* CYLINDERCROWN_H */

