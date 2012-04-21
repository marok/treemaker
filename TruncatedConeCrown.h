#ifndef TRUNCATEDCONECROWN_H
#define	TRUNCATEDCONECROWN_H

#include "Subcrown.h"
#include <cmath>

class TruncatedConeCrown: public Subcrown
{
public:
	float r_down, r_up, h;

	TruncatedConeCrown(float x, float y, float z, float r_down, float r_up, float h)
	{
		this->shape = TRUNCATEDCONE;
		this->x = x;
		this->y = y;
		this->z = z;
		this->r_down = r_down;
		this->r_up = r_up;
		this->h = h;

	}
	TruncatedConeCrown() {
		this->shape=TRUNCATEDCONE;
	}

	std::vector<Point3d *> generatePoints(int n) {
		int points_num = n;
		float r_max = r_up > r_down? r_up: r_down;
		std::vector<Point3d *> result;
		result.clear();

#define RANDF() (float)random()/(float)RAND_MAX

		while (points_num) {
			float r_rand = sqrt(RANDF()) * r_max;
			float h_rand = RANDF() * h;

			if((r_up-r_rand)*(0-h_rand)-(h-h_rand)*(r_down-r_rand) > 0)
				continue;


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
		Subcrown::save(s);
#define SAVE(x) s<<x<<endl;
		SAVE(r_down);
		SAVE(r_up);
		SAVE(h);
#undef SAVE
	}
	void load(ifstream &s)
	{
		Subcrown::load(s);
#define LOAD(x) s>>x;
		LOAD(r_down);
		LOAD(r_up);
		LOAD(h);
#undef LOAD
	}
};

#endif	/* CYLINDERCROWN_H */

