// shima samadi 
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#define max 100
typedef struct{
    double (*fun1)(double);
    const char *name1;
} funmath;
typedef struct{
    int most;
    double data[max];}
memorystack;
typedef enum{
    devision_peroblem,
    domain_peroblem,
     function_peroblem,
    input_peroblem,
    succetion,
    action_peroblem,
    phraise_peroblem,
    allocation_peroblem,
}   erors;
erors calculate(const char **input,int countinput,double *output);
double truth_log(double n);
double truth_ln(double n); 
double truth_sqrt(double n);
 double truth_abs(double n); 
double truth_cos(double n);
double truth_sin(double n);
double truth_tan(double n);
double truth_cot(double n);
    double truth_exp(double n);
double truth_cosh(double n);
double truth_sinh(double n);
double truth_tanh(double n);
double truth_sin(double n){
    return sin(n);}
double truth_cos(double n){
    return cos(n);}
double truth_tan(double n)
    {return tan(n);}
double truth_exp(double n){
    return exp(n);}
double truth_abs(double n){ 
    return fabs(n);}
 double truth_sinh(double n){
    return sinh(n);}
double truth_cosh(double n) {
    return cosh(n);}
double truth_tanh(double n){
    return tanh(n);}
erors push(memorystack *sone, double value){
    if (sone->most >= max-1) 
    {return action_peroblem;
    }
    sone->data[++(sone->most)] =value;
    return succetion;
}
void inistialzationstack(memorystack *sone){
    sone->most=-1;
}
erors pop(memorystack *sone, double *value){
    if (sone->most<0)
    {return action_peroblem;
    }
    *value = sone->data[(sone->most)--];
    return succetion;}
 double truth_log(double n){
    if (n<=0)
    {return NAN; 
    }
    return log10(n);
}
double safe_ln(double n){
    if (n<=0)
    {return NAN;}
    return log(n);}
double truth_sqrt(double n){
    if(n<0)
    {return NAN;
    }
    return sqrt(n)
;}
    double truth_cot(double x){
    double tan_val=tan(x);
    if (fabs(tan_val)<1e-15)
    {return NAN;
    }
    return 1.0/tan_val;
}
funmath math_functions[]={
    {"sin", truth_sin},
    {"cos", truth_cos},
{"tan", truth_tan},
    {"cot", truth_cot},
    {"sqrt", truth_sqrt},
    {"log", truth_log},
        {"ln", truth_ln},
    {"exp", truth_exp},
    {"abs", truth_abs},
{"sinh", truth_sinh},
    {"cosh", truth_cosh},
    {"tanh", truth_tanh},
    {NULL,NULL}
    };
