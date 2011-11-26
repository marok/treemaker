#ifndef _LEAVESPARAMETERS_H
#define _LEAVESPARAMETERS_H
#include <vector>
#include <fstream>
#include <iostream>
#include "Leaf.h"
using namespace std;
class LeavesParameters {

	float sumAmounts() {
		float ret=0.0f;
		for(unsigned int i=0; i<leaves.size(); i++)
			ret+=leaves[i].amount;
		return ret;
	}
public:
	/* Lista rodzajow lisci:
	  trzymamy obiekt a nie wskaznik poniewaz:
	    - rodzajow lisci jest malo
	    - będziemy mieli z głowy czyszczenie pamięci
	*/
	vector<Leaf> leaves;
	vector<pair<int,float > > types;//types of possible leaves (texture,size)
	int activeLeaf; //leaf visible on gui


	LeavesParameters() {
		activeLeaf=-1;
#define ADD_LEAF(name)  {Leaf def((char*)name); leaves.push_back(def);}
		ADD_LEAF("textures/leaf0_256.bmp");
		ADD_LEAF("textures/leaf1_512.bmp");
		ADD_LEAF("textures/leaf2_512.bmp");
#undef ADD_LEAF
		generateTypes();

	}
	void clear() {
		leaves.clear();
		types.clear();
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
	void generateTypes() {
		types.clear();
		const unsigned int TYPES=1000;//Różne pary (tekstura,rozmiar)
		float sum=sumAmounts();
		if(sum==0) return;
		Randomizer r;
		r.init(42,1000,UNIFORM);
		vector<float> limits;
		for(unsigned int i=0; i<leaves.size(); i++)
		{
			float prev=0;
			if(i!=0) prev=limits[i-1];
			limits.push_back(leaves[i].amount*1000.0f/sum+prev);
		}

		for(unsigned int i=0; i<TYPES; i++) {
			//wylosuj typ liscia
			unsigned int type=r.getInt();
			float prev=0;
			for(unsigned int j=0; j<leaves.size(); j++) {
				if(type>=prev&&type<limits[j]) {
					//wylosuj rozmiar
					float s=1;
					if(r.getInt()%2) s=-1;;
					float dr=leaves[j].sizeDeriviation*r.getInt()/1000.0f;
					dr*=s;
					types.push_back(pair<int,float>(j,leaves[j].leafSize+dr));
					break;
				}
			}
		}
	}

	/* Serialization methods */
	void save(ofstream &s)
	{
		s<<leaves.size()<<endl;
		for(unsigned int i=0; i<leaves.size(); i++)
			leaves[i].save(s);
	}
	void load(ifstream &s)
	{
		activeLeaf=-1;
		unsigned int x;
		s>>x;
		for(unsigned int i=0; i<x; i++)
			leaves[i].load(s);
		generateTypes();
	}
};

#endif