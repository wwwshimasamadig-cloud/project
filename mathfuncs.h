#ifndef MATHFUNCS_H
#define MATHFUNCS_H

#include <math.h>

/*استراکچر توابع*/
typedef struct
{
    double (*fun1)(double);
    const char *name1;
} funmath;

/*برای ورودی توابع ریاضی*/
funmath *findfun(const char *name1);

/* prototypes   */
double truth_log(double n);
double truth_ln(double n);
double truth_sqrt(double n);
double truth_abs(double  n);
double truth_cos(double n);
double truth_sin(double n);
double truth_tan(double n);
double truth_cot(double n);
double truth_exp(double n);
double truth_cosh(double n);
double truth_sinh(double n);
double truth_tanh(double n);

#endif