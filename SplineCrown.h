#ifndef SPLINECROWN_H
#define	SPLINECROWN_H

#include "Point3d.h"
#include "Spline.h"
#include <vector>
#include <algorithm>
#include <cmath>

class SplineCrown: public Subcrown {

	static bool cmpCrownMainPoints(Point2d* a, Point2d *b)
	{
		return a->x > b->x;
	}

public:
	Spline *s;
	std::vector<Point2d *> crownMainPoints;

	//punkt aktualnie edytowany
	int activePoint;
	
	SplineCrown(float x, float y, float z, MethodParameters *params) {
		std::vector<Point2d *> crownMainPoints;
		Point2d *p = new Point2d(0, 0);
		crownMainPoints.push_back(p);
		p = new Point2d(2, 5);
		crownMainPoints.push_back(p);
		p = new Point2d(4, 0);
		crownMainPoints.push_back(p);


		std::sort(crownMainPoints.begin(), crownMainPoints.end(), cmpCrownMainPoints);

		this->shape = SPLINE;
		this->x = x;
		this->y = y;
		this->z = z;
		this->crownMainPoints = crownMainPoints;

		srand(params->seed);

		s = new Spline(crownMainPoints);
		
		this->activePoint = -1;
	}

	~SplineCrown()
	{
		delete s;
	}

	void addCrownMainPoint(Point2d *p)
	{
		crownMainPoints.push_back(p);
		std::sort(crownMainPoints.begin(), crownMainPoints.end(), cmpCrownMainPoints);
		delete s;
		s = new Spline(crownMainPoints);
	}

	void deleteCrownMainPoint(std::vector< Point2d* >::iterator i)
	{
		crownMainPoints.erase(i);
		delete s;
		s = new Spline(crownMainPoints);
	}

	void changeCrownMainPoint(int id, float x, float y)
	{
		crownMainPoints[id]->x = x;
		crownMainPoints[id]->y = y;
		delete s;
		s = new Spline(crownMainPoints);
	}

	//punkty losujemy w walcu opisanym na bryle obrotowej o wysokości h i promieniu r
	//nastepnie sprawdzamy czy dany punkt leży w bryle
	std::vector<Point3d *> generatePoints(int n)
	{
		int points_num = n;
		std::vector<Point3d *> result;
		result.clear();


		float h = crownMainPoints[crownMainPoints.size() - 1]->x - crownMainPoints[0]->x;
		float x_max, y_max;

		s->globalMax(&x_max, &y_max);

		//float r = sqrt(pow(x_max,2)+pow(y_max,2));
		float r = y_max;
#define RANDF() (float)random()/(float)RAND_MAX

		while(points_num)
		{
			float r_rand = sqrt(RANDF())*r;
			float h_rand = RANDF()*h;

			float r_max = s->getS(crownMainPoints[0]->x+h_rand);

			if(r_max >= r_rand)
			{
				float alfa = RANDF()*2.0*M_PI;
				Point3d *point = new Point3d(r_rand*sin(alfa) + x,r_rand*cos(alfa) + y,crownMainPoints[0]->x + h_rand + this->z);
				result.push_back(point);

				points_num--;
			}
		}
#undef RANDF
		return result;
	}
};


#endif

