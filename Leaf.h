#ifndef _LEAF_H
#define _LEAF_H
#include <cstring>
class Leaf {

#define MAXPATHLEN		256
#define TEXPATH_DEFAULT		"textures/leaf0_256.bmp"
	void initDefaults() {
		strcpy(leafTexPath,TEXPATH_DEFAULT);
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
	unsigned int texId;	//opengl texture binding id
	unsigned int maskId;	//opengl mask texture binding id

	static const bool LEAFTEXINITIALIZED_DEFAULT=FALSE;
	static const bool ACTIVE_DEFAULT=FALSE;
	static const float LEAFSIZE_DEFAULT=1.0f;
	static const float SIZEDERIVIATION_DEFAULT=0.0f;
	static const float AMOUNT_DEFAULT=0.0f;

	Leaf() {
		initDefaults();
	}
	Leaf(char *path) {
		initDefaults();
		setLeafPath(path);
	}
	void setLeafPath(char *path) {
		assert(strlen(path)<MAXPATHLEN);
		strcpy(leafTexPath,path);
	}
	/* Returns texture name without dirs names*/
	char *getTexName() {
		char *p=strrchr(leafTexPath,'/');
		if(p==NULL)
			return p;
		return p+1;
	}

};
#undef MAXPATHLEN
#undef TEXPATH_DEFAULT

#endif