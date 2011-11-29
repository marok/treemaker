#ifndef _TRUNK_PARAMETERS_H
#define _TRUNK_PARAMETERS_H
#include <cstring>
#include <fstream>
#include <iostream>
class TrunkParameters {
public:
#define MAXPATHLEN 256
	float radiusFactor;
	
	//liczba punktów segmentu w wygenerowanym modelu
	int circlePoints;
	
	//liczba punktów segmentu aktualnie na suwaku
	//po wciśnięciu generate bądź refresh circlePoints <- circlePointsCurrentAdj
	int circlePointsCurrentAdj;
	
	float aValue;
	float mValue;
	char barkPath[MAXPATHLEN];		//path to bark texture
	bool barkTexInitialized;
	float kx;			//ile razy ma sie owinac tekstura wokol pnia
	float ky;			//ile jednostek tekstury na jednostke pnia

	
	static const float RADIUSFACTOR_DEFAULT=1.8;
	static const float AVALUE_DEFAULT=0;
	static const float MVALUE_DEFAULT=1;
	static const int CIRCLEPOINTS_DEFAULT=10;
	static const bool BARKTEXINITIALIZED_DEFAULT=FALSE;
	static const float KX_DEFAULT=3;
	static const float KY_DEFAULT=0.5; 

	TrunkParameters() {
		setDefault();
	}
	
	void setDefault()
	{
		char defaultPath[]="textures/bark0_256.bmp";
		strcpy(barkPath,defaultPath);
		radiusFactor=RADIUSFACTOR_DEFAULT;
		aValue=AVALUE_DEFAULT;
		mValue=MVALUE_DEFAULT;
		circlePointsCurrentAdj = CIRCLEPOINTS_DEFAULT;
		barkTexInitialized=BARKTEXINITIALIZED_DEFAULT;
		kx=KX_DEFAULT;
		ky=KY_DEFAULT;
	
	}
	
	void setBarkPath(char *path) {
		assert(strlen(path)<MAXPATHLEN);
		strcpy(barkPath,path);
	}
	
	void setCirclePointsValue()
	{
		circlePoints = circlePointsCurrentAdj;
	}
	
	/* Serialization methods */

	void save(ofstream &s)
	{
#define SAVE(param) s<<param<<endl;

		SAVE(radiusFactor);
		SAVE(circlePoints);
		SAVE(aValue);
		SAVE(mValue);
		SAVE(barkPath);
		SAVE(kx);
		SAVE(ky);
#undef SAVE
	}
	void load(ifstream &s)
	{
		barkTexInitialized=FALSE;
#define LOAD(param) s>>param;

		LOAD(radiusFactor);
		LOAD(circlePointsCurrentAdj);
		LOAD(aValue);
		LOAD(mValue);
		LOAD(barkPath);
		LOAD(kx);
		LOAD(ky);

#undef LOAD
	}
#undef MAXPATHLEN
};

#endif