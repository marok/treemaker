#ifndef _LEAVESPARAMETERS_H
#define _LEAVESPARAMETERS_H
#include <vector>
#include "Leaf.h"
using namespace std;
class LeavesParameters {

public:
	/* Lista rodzajow lisci:
	  trzymamy obiekt a nie wskaznik poniewaz:
	    - rodzajow lisci jest malo
	    - b?dziemy mieli z g?owy czyszczenie pami?ci
	*/
	vector<Leaf> leaves;
	int activeLeaf; //leaf visible on gui
	LeavesParameters() {
		activeLeaf=-1;
#define ADD_LEAF(name)  {Leaf def((char*)name); leaves.push_back(def);}
		ADD_LEAF("textures/leaf0_256.bmp");
		ADD_LEAF("textures/leaf1_512.bmp");
		ADD_LEAF("textures/leaf2_512.bmp");
#undef ADD_LEAF

	}
	int getId(char *texName) {
		int ret=-1;
		for(unsigned int i=0; i<leaves.size(); i++)
		{
			char *p=strrchr(leaves[i].leafTexPath,'/');
			if(p==NULL)
				p=leaves[i].leafTexPath;
			if(strcmp(p+1,texName)==0)
				ret=i;

		}
		return ret;
	}

};

#endif