#ifndef _TRUNK_PARAMETERS_H
#define _TRUNK_PARAMETERS_H
#include <cstring>
#include <fstream>
#include <iostream>
class TrunkParameters {
public:
#define MAXPATHLEN 256
	float radiusFactor;
	int circlePoints;
	float aValue;
	float mValue;
	char barkPath[MAXPATHLEN];		//path to bark texture
	bool barkTexInitialized;

	static const float RADIUSFACTOR_DEFAULT=1.8;
	static const float AVALUE_DEFAULT=0;
	static const float MVALUE_DEFAULT=1;
	static const int CIRCLEPOINTS_DEFAULT=10;
	static const bool BARKTEXINITIALIZED_DEFAULT=FALSE;

	TrunkParameters() {
		char defaultPath[]="textures/bark0_256.bmp";
		strcpy(barkPath,defaultPath);
		radiusFactor=RADIUSFACTOR_DEFAULT;
		aValue=AVALUE_DEFAULT;
		mValue=MVALUE_DEFAULT;
		circlePoints=CIRCLEPOINTS_DEFAULT;
		barkTexInitialized=BARKTEXINITIALIZED_DEFAULT;
	}
	void setBarkPath(char *path) {
		assert(strlen(path)<MAXPATHLEN);
		strcpy(barkPath,path);
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

#undef SAVE
	}
	void load(ifstream &s)
	{
		barkTexInitialized=FALSE;
#define LOAD(param) s>>param;

		LOAD(radiusFactor);
		LOAD(circlePoints);
		LOAD(aValue);
		LOAD(mValue);
		LOAD(barkPath);

#undef LOAD
	}
#undef MAXPATHLEN
};

#endif