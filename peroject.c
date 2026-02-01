// shima samadi 
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#define max 256
// ساختار ریاضی
typedef struct{
    double (*fun1)(double);
    const char *name1;
} funmath;
// ساختار حافظه
typedef struct{
    int most;
    double data[max];}
memorystack;
// خطاها
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
// توابع ریاضی
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
//تابع مقداردهی استک
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
// برای تابع لگاریتم بر مبنا عدد ای
double truth_ln(double n){
    if (n<=0)
    {return NAN;}
    return log(n);}
// برای تابع جذر
double truth_sqrt(double n){
    if(n<0)
    {return NAN;
    }
    return sqrt(n)
;}
// برای تابع کتانژانت
    double truth_cot(double x){
    double tan_val=tan(x);
    if (fabs(tan_val)<1e-15)
    {return NAN;
    }
    return 1.0/tan_val;
} 
funmath math_functions[]={
    {truth_sin , "sin"},
    {truth_cos,"cos"},
{ truth_tan,"tan"},
    { truth_cot,"cot"},
    {truth_sqrt,"sqrt"},
    { truth_log,"log"},
        { truth_ln, "ln"},
    {truth_exp,"exp"},
    {truth_abs,"abs" },
{ truth_sinh,"sinh"},
    { truth_cosh ,"cosh"},
    {truth_tanh,"tanh"},
    {NULL,NULL}
    };
// تابع ریاضی
funmath* findfun(const char *name1){
    for (int i=0; math_functions[i].name1 !=NULL;++i)
        {if (strcmp(name1,math_functions[i].name1) == 0) 
            {return &math_functions[i];}}
    return NULL; }
// خطا محاسبه 
erors calculate(const char **input,int countinput,double *output){
    memorystack stack;
    inistialzationstack(&stack);
    erors status;
    double action1,action2,value;
    for (int i=0; i<countinput; ++i)
        {const char *took=input[i];
        char *endptr;
        value=strtod(input, &endptr);
        if (*endptr == '\0' && input!= endptr)
            {if (push(&stack, value) != succetion) 
                return action_peroblem;}
        else{
            if (strcmp(input, "+") == 0){
                if (pop(&stack, &action2)!=succetion) 
                    return action_peroblem;
                    if (pop(&stack, &action1)!= succetion)
                    return action_peroblem;
                if (push(&stack, action1 + action2)!=succetion)
                    return action_peroblem;
            } else if(strcmp(input, "-")==0){
                if (pop(&stack, &action2)!=succetion)
                    return action_peroblem;
                    if (pop(&stack, &action1)!=succetion)
                    return action_peroblem;
                if (push(&stack, action1 -action2)!=succetion)
                    return action_peroblem;
            } else if (strcmp(input, "*") == 0){
                if (pop(&stack, &action2)!=succetion)
                    return action_peroblem;
                    if (pop(&stack, &action1)  !=succetion)
                    return action_peroblem;
                if(push(&stack, action1 * action2)!=succetion)
                    return action_peroblem;}
            else if (strcmp(input, "/") == 0){
                if (pop(&stack, &action2) != succetion)
                    return action_peroblem;
                if (pop(&stack, &action1) != succetion)
                    return action_peroblem;
                    if (fabs(action2) < 1e-15)
                    {return devision_peroblem;
                }
                if (push(&stack,action1/action2)!=succetion) return action_peroblem;
            } else
                {funmath *mf =findfun(input);
                if (mf != NULL)
                    {if (pop(&stack,&action1)!=succetion) 
                        return action_peroblem;
                    double fun_result = mf->fun1(action1);
                    if (isnan(fun_result)) {
                        {return domain_peroblem;
                    }
                    if (push(&stack,fun_result) !=succetion) return action_peroblem;
                } else {
                    return input_peroblem;
                }}}}
    if (stack.most!= 0)
        {return action_peroblem;
    }
    if (pop(&stack, output)!=succetion)
        {return phraise_peroblem;}
     if ( isnan(*output))
        {return domain_peroblem
    ;}
    return succetion;}}
