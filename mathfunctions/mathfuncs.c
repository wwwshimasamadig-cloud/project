#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "mathfuncs.h"

/*   توابع ریاضی */
double truth_sin(double n)
{
    return sin(n);
}
double truth_cos(double n)
{
    return cos(n);
}
double truth_tan(double n)
{
    return tan(n);
}
double truth_exp(double n)
{
    return exp(n);
}
double truth_abs(double n)
{
    return fabs(n);
}
double truth_sinh(double n)
{
    return sinh(n);
}
double truth_cosh(double n)
{
    return cosh(n);
}
double truth_tanh(double n)
{
    return tanh(n);
}

/*شرط منطقی تابع لگاریتم در مبنا عدد ای*/
double truth_ln(double n)
{
    return (n <= 0) ? NAN : log(n);
}

/*شرط منطقی برای تابع لگاریتم*/
double truth_log(double n)
{
    return (n <= 0) ? NAN : log10(n);
}

/*شرط منطقی برای تابع جذر*/
double truth_sqrt(double n)
{
    return (n < 0) ? NAN : sqrt(n);
}

/*شرط منطقی برای تابع کتانژانت*/
double truth_cot(double x)
{
    double t = tan(x);
    if (fabs(t) < 1e-15)
        return NAN;
    return 1.0 / t;
}

/* توابع */
static funmath math_functions[] = {
    {truth_sin, "sin"},
    {truth_cos, "cos"},
    {truth_tan, "tan"},
    {truth_cot, "cot"},
    {truth_sqrt, "sqrt"},
    {truth_log, "log"},
    {truth_ln, "ln"},
    {truth_exp, "exp"},
    {truth_abs, "abs"},
    {truth_sinh, "sinh"},
    {truth_cosh, "cosh"},
    {truth_tanh, "tanh"},
    {NULL, NULL}};

funmath *findfun(const char *name1)
{
    for (int i = 0; math_functions[i].name1 != NULL; ++i)
    {
        if (strcmp(name1, math_functions[i].name1) == 0)
            return &math_functions[i];
    }
    return NULL;
}