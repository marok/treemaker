#ifndef CROWN_H
#define	CROWN_H

#include "Point3d.h"
#include "Spline.h"
#include <vector>
#include <cmath>

class Crown {
	float *x, *y;
	int n;
	int points;
	
public:
	Spline *s;
	
	Crown(MethodParameters *params) {
		this->n = params->crownMainPoints.size();
		x = new float[n];
		y = new float[n];
		
		for(int i=0; i<n; i++)
		{
			x[i] = params->crownMainPoints.at(i)->x;
			y[i] = params->crownMainPoints.at(i)->y;
		}
		
		srand(params->seed);
		
		s = new Spline(x,y,n);
		points = params->points;
	}
	
	~Crown()
	{
		delete s;
		delete x;
		delete y;
	}
	
	//punkty losujemy w walcu opisanym na bryle obrotowej o wysokości h i promieniu r
	//nastepnie sprawdzamy czy dany punkt leży w bryle
	std::vector<Point3d *> generatePoints()
	{
		int points_num = points;
		std::vector<Point3d *> result;
		result.clear();
		
		
		float h = x[this->n - 1] - x[0];
		float x_max, y_max;
		
		s->globalMax(&x_max, &y_max);
		
		float r = sqrt(pow(x_max,2)+pow(y_max,2));
#define RANDF() (float)random()/(float)RAND_MAX
		
		while(points_num)
		{
			float r_rand = sqrt(RANDF())*r;
			float h_rand = RANDF()*h;
			
			float r_max = s->getS(x[0]+h_rand);
			
			if(r_max >= r_rand)
			{
				float alfa = RANDF()*2.0*M_PI;
				Point3d *point = new Point3d(r_rand*sin(alfa),r_rand*cos(alfa),x[0]+h_rand);
				result.push_back(point);
				
				points_num--;
			}
		}
#undef RANDF
		return result;
	}
};


#endif

