#include "Random.h"
#include <cstdlib>
class Random

{
public:
	static void Init() {

	}

	static float Float(float from, float to) {
		return (from + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (to - from))));
	}

};