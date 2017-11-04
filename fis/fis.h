#ifndef FIS_LOW_TARIFF_H_INCLUDED
#define FIS_LOW_TARIFF_H_INCLUDED

#ifdef BUILD_DLL // the dll exports
//#define BUILD_DLL
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

#include <stdbool.h>

typedef float (*_mffloat2)(float,float,float);
typedef float (*_mffloat3)(float,float,float,float);
typedef float (*_mffloat4)(float,float,float,float,float);
typedef double (*_mfdouble2)(double,double,double);
typedef double (*_mfdouble3)(double,double,double,double);
typedef double (*_mfdouble4)(double,double,double,double,double);

/************************************************************************
 * A Fast, Compact Approximation of the Exponential Function
 * - Nicol N. Schraudolph
 * IDSIA, Lugano, Switzerland
************************************************************************/
#define M_LN2 0.693147180559945309417
union {
    double d;
    struct {
        int j,i;
    } n;
} _eco;
#define EXP_A (1048576/M_LN2)
#define EXP_C 60801
#define _exp(y) (_eco.n.i = EXP_A * (y) + (1072693248 - EXP_C), _eco.d)

EXPORT float fis(float rate, float level, float* params, bool is_low_tariff);
#endif
