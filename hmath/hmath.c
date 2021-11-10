#include "hmath.h"

int sign(double x) {
	if (x < 0) return -1;
	else if (x > 0) return 1;
	else return 0;
}

double sqrt(double x) {
	double guess = x / 2;
	double oldguess;
	do {
		oldguess = guess;
		guess = 0.5 * (oldguess + x / oldguess);
	} while (guess != oldguess);
	return guess;
}

double nsolve(double (*func)(double), double min, double max) {
	double minval = func(min);
	double maxval = func(max);
	while (1) {
		double mid = (min + max) / 2;
		double midval = func(mid);
		if (midval == 0) {
			return mid;
		}
		else if (min == max || min == mid || max == mid) {
			return mid;
		}
		else if (sign(minval) == sign(midval)) {
			min = mid;
			minval = midval;
		}
		else {
			max = mid;
			maxval = midval;
		}
	}
}
