#ifndef _COLONIZATION_PARAMETERS_H
#define	_COLONIZATION_PARAMETERS_H

class ColonizationParameters {


	void setDefaults() {
		seed=SEED_DEFAULT;
		di=DI_DEFAULT;
		dk=DK_DEFAULT;
		D=D_DEFAULT;
		points=POINTS_DEFAULT;
		maxRounds=MAXROUNDS_DEFAULT;
		crownRadius=CROWNRADIUS_DEFAULT;
		showEnvelope=SHOWENVELOPE_DEFAULT;
	}


public:
	static const int SEED_DEFAULT=42;
	static const float DI_DEFAULT=5.0;
	static const float DK_DEFAULT=0.1;
	static const float D_DEFAULT=0.2;
	static const int POINTS_DEFAULT=60;
	static const int MAXROUNDS_DEFAULT=1000;
	static const float CROWNRADIUS_DEFAULT=5.0;
	static const int SHOWENVELOPE_DEFAULT=0;

	int seed;	//seed for srand()
	float di;	//influence distance
	float dk;	//kill distance
	float D;	//node length
	int points;     //attraction points
	int maxRounds; 	//maximum number of colonization rounds
	float crownRadius;//radius of crown envelope
	int showEnvelope;//show points on GUI

	ColonizationParameters()
	{
		setDefaults();
	}
};
#endif