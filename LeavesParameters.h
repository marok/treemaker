#ifndef _LEAVESPARAMETERS_H
#define _LEAVESPARAMETERS_H

class LeavesParameters {
public:
#define MAXPATHLEN 256
	char leavesPath[MAXPATHLEN];		//path to leaves texture
	bool leavesTexInitialized;

	static const bool LEAVESTEXINITIALIZED_DEFAULT=FALSE;

	LeavesParameters() {
		char defaultPath[]="textures/leaf0_256.bmp";
		strcpy(leavesPath,defaultPath);
		leavesTexInitialized=LEAVESTEXINITIALIZED_DEFAULT;
	}
	void setLeavesPath(char *path) {
		assert(strlen(path)<MAXPATHLEN);
		strcpy(leavesPath,path);
	}
#undef MAXPATHLEN
};

#endif