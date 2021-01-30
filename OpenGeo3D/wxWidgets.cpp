#include "wxWidgets.h"

bool IsZero(double v) {
	return abs(v) < ZERO_ERROR;
}

bool IsEqual(double a, double b) {
	return abs(a - b) < ZERO_ERROR;
}
