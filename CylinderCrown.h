#ifndef CYLINDERCROWN_H
#define	CYLINDERCROWN_H

#include "Subcrown.h"
#include <cmath>

class CylinderCrown: public Subcrown
{
public:
	float r, h;

	CylinderCrown(float x, float y, float z, float r, float h)
	{
		this->shape = CYLINDER;
		this->x = x;
		this->y = y;
		this->z = z;
		this->r = r;
		this->h = h;

	}
	CylinderCrown() {
		this->shape=CYLINDER;
	}
	std::vector<Point3d *> generatePoints(int n) {
		int points_num = n;
		std::vector<Point3d *> result;
		result.clear();

#define RANDF() (float)random()/(float)RAND_MAX

		while (points_num) {
			float r_rand = sqrt(RANDF()) * r;
			float h_rand = RANDF() * h;


			float alfa = RANDF()*2.0 * M_PI;
			Point3d *point = new Point3d(r_rand * sin(alfa)+x, r_rand * cos(alfa)+y, z + h_rand);
			result.push_back(point);

			points_num--;
		}
#undef RANDF
		return result;
	}
	void save(ofstream &s)
	{
#define SAVE(x) s<<x<<endl;
		SAVE(x);
		SAVE(y);
		SAVE(z);
		SAVE(r);
		SAVE(h);
#undef SAVE
	}
	void load(ifstream &s)
	{
#define LOAD(x) s>>x;
		LOAD(x);
		LOAD(y);
		LOAD(z);
		LOAD(r);
		LOAD(h);
#undef LOAD
	}
};

#endif	/* CYLINDERCROWN_H */

