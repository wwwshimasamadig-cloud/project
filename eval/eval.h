#ifndef EVAL_H
#define EVAL_H

#include <math.h>
#include "sheet.h"
#include "mathfuncs.h"

#define max 100
/*استراکچر حافظه*/
typedef struct
{
    int most;
    double data[max];
} memorystack;

/*ارورهای موجود در ورودی */
typedef enum
{
    devision_peroblem,
    domain_peroblem,
    function_peroblem,
    input_peroblem,
    succetion,
    action_peroblem,
    phraise_peroblem,
    allocation_peroblem,
} erors;

extern sheet *g_sheet_ctx;
extern ErrorCode g_eval_err;

/* ارزیابی */
double tokenvalue(const char *token_str, sheet *sh);
erors calculate(const char **input,  int countinput, double *output);

erors push(memorystack *sone, double value);
void inistialzationstack(memorystack *sone);
erors pop(memorystack *sone, double *value);

#endif