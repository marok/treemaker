#ifndef _RANDIMIZER_H
#define _RANDOMIZER_H
#include <cstdlib>
#include <cassert>
enum Distribution { UNIFORM, NORMAL };

class Randomizer {
	Distribution distribution;
	int seed;
	unsigned int ceil;
public:
	void init(int _seed, unsigned int _ceil, Distribution d) {
		distribution=d;
		seed=_seed;
		ceil=_ceil;
		srand(seed);
		assert(ceil%2==0);
	}

	/** Returns random int from 0 to ceil **/
	unsigned int getInt() {
		if(distribution==UNIFORM) {
			return random()%(ceil+1);
		}
		if(distribution==NORMAL) {
			unsigned int a=random()%(ceil/2);
			unsigned int b=random()%(ceil/2);
			return a+b;
		}
	}

};


#endif