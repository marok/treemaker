#ifndef _COLONIZATION_PARAMETERS_H
#define	_COLONIZATION_PARAMETERS_H

class ColonizationParameters {
public:
        int seed;
	float di;	//influence distance
	float dk;	//kill distance
	float D;	//node length
	int points;//attraction points
        
        ColonizationParameters()
        {
                setDefaults();
        }
        
private:
        void setDefaults(){
	 seed=42;
	 di=5.0;
	 dk=0.1;
	 D=0.2;
	 points=60;
	}
};
#endif