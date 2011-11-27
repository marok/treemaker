#ifndef _POINT3D_H
#define _POINT3D_H

#include <cstdio>
#include <cmath>

class Point3d
{
public:
	float x, y, z;

	Point3d ()
	{
		x = y = z = 0.0f;
	} Point3d (float _x, float _y, float _z):x (_x), y (_y), z (_z)
	{
	}
	Point3d (Point3d * p):x (p->x), y (p->y), z (p->z) {
	}
	void set (float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
	float getDistance (Point3d * b)
	{
		float sum = (x - b->x) * (x - b->x);

		sum += (y - b->y) * (y - b->y);
		sum += (z - b->z) * (z - b->z);
		return sqrt (sum);
	}
	bool isEqual (Point3d * p)
	{
		if (x == p->x && y == p->y && z == p->z)
			return 1;
		return 0;
	}

	void sub(const Point3d p)
	{
		x -= p.x;
		y -= p.y;
		z -= p.z;
	}

	void add(const Point3d p)
	{
		x += p.x;
		y += p.y;
		z += p.z;
	}

	void print ()
	{
		g_print ("p %f %f %f\n", x, y, z);
	}
};
#endif
