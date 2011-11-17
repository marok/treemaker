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
	void init(int seed, unsigned int ceil, Distribution d) {
		distribution=d;
		this->seed=seed;
		this->ceil=ceil;
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
		assert(0);
		return 0;
	}

};
#endif