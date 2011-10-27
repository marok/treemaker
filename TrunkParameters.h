#ifndef _TRUNK_PARAMETERS_H
#define _TRUNK_PARAMETERS_H
#include <cstring>

class TrunkParameters {
public:
	float radiusFactor;
	int circlePoints;
	float aValue;
	float mValue;
	char barkPath[256];		//path to bark texture

	static const float RADIUSFACTOR_DEFAULT=1.8;
	static const float AVALUE_DEFAULT=0;
	static const float MVALUE_DEFAULT=1;
	static const int CIRCLEPOINTS_DEFAULT=10;

	TrunkParameters() {
		char defaultPath[]="textures/bark0_256.bmp";
		memcpy(barkPath,defaultPath,strlen(defaultPath)+1);
		radiusFactor=RADIUSFACTOR_DEFAULT;
		aValue=AVALUE_DEFAULT;
		mValue=MVALUE_DEFAULT;
		circlePoints=CIRCLEPOINTS_DEFAULT;
	}
};

#endif