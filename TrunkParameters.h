#ifndef _TRUNK_PARAMETERS_H
#define _TRUNK_PARAMETERS_H

class TrunkParameters {
public:
	float radiusFactor;
	int circlePoints;
	float aValue;
	float mValue;

	static const float RADIUSFACTOR_DEFAULT=1.8;
	static const float AVALUE_DEFAULT=0;
	static const float MVALUE_DEFAULT=1;
	static const int CIRCLEPOINTS_DEFAULT=10;

	TrunkParameters() {
		radiusFactor=RADIUSFACTOR_DEFAULT;
		aValue=AVALUE_DEFAULT;
		mValue=MVALUE_DEFAULT;
		circlePoints=CIRCLEPOINTS_DEFAULT;
	}
};

#endif