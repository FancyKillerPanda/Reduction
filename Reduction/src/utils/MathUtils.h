#pragma once

#define _USE_MATH_DEFINES
#include <cmath>


inline double toRadians(double value)
{
	return value * (M_PI / 180);
}

inline double toDegrees(double value)
{
	return value * (180 / M_PI);
}
