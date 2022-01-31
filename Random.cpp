#include "Random.h"
#include <cstdlib>


	void Random::Init() {

	}

	float Random::Float(float from, float to) {
		return (from + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (to - from))));
	}

