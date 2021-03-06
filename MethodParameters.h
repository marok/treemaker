#ifndef _METHOD_PARAMETERS_H
#define	_METHOD_PARAMETERS_H
#include "Randomizer.h"
#include "Point2d.h"
#include <vector>
#include <fstream>
#include <iostream>
using namespace std;
class MethodParameters {
public:
	void setDefault() {
		seed=SEED_DEFAULT;
		points=POINTS_DEFAULT;
		D=D_DEFAULT;
		di=DI_DEFAULT;
		dk=DK_DEFAULT;
		maxRounds=MAXROUNDS_DEFAULT;
		ie=IE_DEFAULT;
		cd=CD_DEFAULT;
		attraction=ATTRACTION_DEFAULT;
		activeMethod=ACTIVEMETHOD_DEFAULT;
	}

	static const int SEED_DEFAULT=42;
	static const int POINTS_DEFAULT=60;
	static const float D_DEFAULT=0.2;
	static const float DI_DEFAULT=6.0;
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
	float D;		//node length
	Distribution distribution;//random generator distribution (uniform by default)
	int activeMethod;	//active generation method 0-colonization, 1-particle

	//colonization parameters
	float di;		//influence distance
	float dk;		//kill distance
	int maxRounds; 		//maximum number of colonization rounds

	//particle parameters
	float ie;		//initial energy
	float cd;		//combine distance
	float attraction;	//particle attraction (1 - on, 0 -off)

	MethodParameters() {
		setDefault();
	}
	/* Serialization methods */
	void save(ofstream &s)
	{
#define SAVE(param) s<<param<<endl;

		SAVE(seed);
		SAVE(points);
		SAVE(crownRadius);
		SAVE(D);
		SAVE(activeMethod);
		SAVE(di);
		SAVE(dk);
		SAVE(maxRounds);
		SAVE(ie);
		SAVE(cd);
		SAVE(attraction);

#undef SAVE
	}
	void load(ifstream &s)
	{
#define LOAD(param) s>>param;

		LOAD(seed);
		LOAD(points);
		LOAD(crownRadius);
		LOAD(D);
		LOAD(activeMethod);
		LOAD(di);
		LOAD(dk);
		LOAD(maxRounds);
		LOAD(ie);
		LOAD(cd);
		LOAD(attraction);
#undef LOAD
	}
};

#endif