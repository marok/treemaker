#ifndef SPLINECROWN_H
#define	SPLINECROWN_H

#include "Point3d.h"
#include "Spline.h"
#include <vector>
#include <cmath>

class SplineCrown: public Subcrown {

	float *splineX, *splineY;
	int n;
	int points;

public:
	Spline *s;
	std::vector<Point2d *> crownMainPoints;


	SplineCrown(float x, float y, float z, MethodParameters *params) {
		std::vector<Point2d *> crownMainPoints;
		Point2d *p = new Point2d(0, 0);
		crownMainPoints.push_back(p);
		p = new Point2d(2, 5);
		crownMainPoints.push_back(p);
		p = new Point2d(4, 0);
		crownMainPoints.push_back(p);

		this->shape = SPLINE;
		this->x = x;
		this->y = y;
		this->z = z;
		this->crownMainPoints = crownMainPoints;
		this->n = crownMainPoints.size();
		splineX = new float[n];
		splineY = new float[n];

		for(int i=0; i<n; i++)
		{
			splineX[i] = crownMainPoints.at(i)->x;
			splineY[i] = crownMainPoints.at(i)->y;
		}

		srand(params->seed);

		s = new Spline(splineX,splineY,n);
		points = params->points;
	}

	~SplineCrown()
	{
		delete s;
		delete splineX;
		delete splineY;
	}

	//punkty losujemy w walcu opisanym na bryle obrotowej o wysokości h i promieniu r
	//nastepnie sprawdzamy czy dany punkt leży w bryle
	std::vector<Point3d *> generatePoints(int n)
	{
		int points_num = n;
		std::vector<Point3d *> result;
		result.clear();


		float h = splineX[this->n - 1] - splineX[0];
		float x_max, y_max;

		s->globalMax(&x_max, &y_max);

		//float r = sqrt(pow(x_max,2)+pow(y_max,2));
		float r = y_max;
#define RANDF() (float)random()/(float)RAND_MAX

		while(points_num)
		{
			float r_rand = sqrt(RANDF())*r;
			float h_rand = RANDF()*h;

			float r_max = s->getS(splineX[0]+h_rand);

			if(r_max >= r_rand)
			{
				float alfa = RANDF()*2.0*M_PI;
				Point3d *point = new Point3d(r_rand*sin(alfa) + x,r_rand*cos(alfa) + y,splineX[0] + h_rand + this->z);
				result.push_back(point);

				points_num--;
			}
		}
#undef RANDF
		return result;
	}
};


#endif

