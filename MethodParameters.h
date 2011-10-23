#ifndef _METHOD_PARAMETERS_H
#define	_METHOD_PARAMETERS_H
#include "Randomizer.h"
#include "Point2d.h"
#include <vector>

class MethodParameters {
protected:
	void init() {
		seed=SEED_DEFAULT;
		points=POINTS_DEFAULT;
		showEnvelope=SHOWENVELOPE_DEFAULT;
		D=D_DEFAULT;
		di=DI_DEFAULT;
		dk=DK_DEFAULT;
		maxRounds=MAXROUNDS_DEFAULT;
		ie=IE_DEFAULT;
		cd=CD_DEFAULT;
		attraction=ATTRACTION_DEFAULT;
		activeMethod=ACTIVEMETHOD_DEFAULT;
		
		Point2d *p = new Point2d(5, 0);
		crownMainPoints.push_back(p);
		p = new Point2d(6, 4);
		crownMainPoints.push_back(p);
		p = new Point2d(9, 3);
		crownMainPoints.push_back(p);
		p = new Point2d(13, 8);
		crownMainPoints.push_back(p);
		p = new Point2d(20, 0);
		crownMainPoints.push_back(p);
		
	}
public:
	static const int SEED_DEFAULT=42;
	static const int POINTS_DEFAULT=60;
	static const int SHOWENVELOPE_DEFAULT=0;
	static const float D_DEFAULT=0.2;
	static const float DI_DEFAULT=5.0;
	static const float DK_DEFAULT=0.1;
	static const int MAXROUNDS_DEFAULT=1000;
	static const float IE_DEFAULT=1.0;
	static const float CD_DEFAULT=0.2;
	static const float ATTRACTION_DEFAULT=0.0;
	static const int ACTIVEMETHOD_DEFAULT=0;

	//common parameters
	int seed;		//seed for srand()
	int points;     	//attraction points
	float crownRadius;	//radius of crown envelope
	int showEnvelope;	//show points on GUI
	float D;		//node length
	Distribution distribution;//random generator distribution (uniform by default)
	int activeMethod;	//active generation method 0-colonization, 1-particle
	
	//crown params
	std::vector<Point2d *> crownMainPoints;

	//colonization parameters
	float di;		//influence distance
	float dk;		//kill distance
	int maxRounds; 		//maximum number of colonization rounds

	//particle parameters
	float ie;		//initial energy
	float cd;		//combine distance
	float attraction;	//particle attraction (1 - on, 0 -off)

	MethodParameters() {
		init();
	}
};

#endif