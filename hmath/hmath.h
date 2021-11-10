#ifndef HMATH_H
#define HMATH_H

#ifdef __cplusplus
extern "C" {
#endif

double nsolve(double (*func)(double), double min, double max);

int sign(double);

double sqrt(double);

#ifdef __cplusplus
}
#endif

#endif
