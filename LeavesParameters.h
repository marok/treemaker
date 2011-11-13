#ifndef _LEAVESPARAMETERS_H
#define _LEAVESPARAMETERS_H

class LeavesParameters {

public:
	/*	char leavesPath[MAXPATHLEN];		//path to leaves texture
		bool leavesTexInitialized;
		float leavesSize;
		float sizeDeriviation;			//maximum absolute leaf size deriviation

		static const bool LEAVESTEXINITIALIZED_DEFAULT=FALSE;
		static const float LEAVESSIZE_DEFAULT=1.0f;
		static const float SIZEDERIVIATION_DEFAULT=0.2;
	*/
	LeavesParameters() {
		/*		char defaultPath[]="textures/leaf0_256.bmp";
				strcpy(leavesPath,defaultPath);
				leavesTexInitialized=LEAVESTEXINITIALIZED_DEFAULT;
				leavesSize=LEAVESSIZE_DEFAULT;
		*/
	}
	void setLeavesPath(char *path) {
//		assert(strlen(path)<MAXPATHLEN);
//		strcpy(leavesPath,path);
	}
};

#endif