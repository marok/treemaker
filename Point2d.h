#ifndef POINT2D_H
#define	POINT2D_H
#include <fstream>
#include <iostream>
using namespace std;

class Point2d
{
public:
	float x, y;
	Point2d() {
		x=y=0;
	}

	Point2d(float x, float y)
	{
		this->x = x;
		this->y = y;
	}
	void save(ofstream &s)
	{
		s<<x<<" "<<y<<endl;
	}
	void load(ifstream &s)
	{
		s>>x>>y;
	}
};


#endif

