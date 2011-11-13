#ifndef _LEAF_H
#define _LEAF_H

class Leaf {

#define MAXPATHLEN		256
#define TEXTPATH_DEFAULT	"textures/leaf0_256.bmp"
	void initDefaults() {
		strcpy(leavesPath,TEXPATH_DEFAULT);
		leafTexInitialized=LEAFTEXINITIALIZED_DEFAULT;
		active=ACTIVE_DEFAULT;
		leafSize=LEAFSIZE_DEFAULT;
		sizeDeriviation=SIZEDERIVIATION_DEFAULT;
		amount=AMOUNT_DEFAULT;
	}
public:
	char leafTexPath[MAXPATHLEN];
	bool leafTexInitialized;
	bool active;
	float leafSize;
	float sizeDeriviation;
	float amount;

	static const bool LEAFTEXINITIALIZED_DEFAULT=FALSE;
	static const bool ACTIVE_DEFAULT=FALSE;
	static const float LEAFSIZE_DEFAULT=1.0f;
	static const float SIZEDERIVIATION_DEFAULT=0.0f;
	static const float AMOUNT_DEFAULT=1.0;

	Leaf(char *path) {
		initDefaults();
		setLeafPath(path);
	}
	void setLeafPath(char *path) {
		assert(strlen(path)<MAXPATHLEN);
		strcpy(leavesPath,path);
	}

};
#undef MAXPATHLEN
#undef TEXPATH_DEFAULT

#endif