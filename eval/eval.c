#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "eval.h"
#include "sheet.h"
#include "mathfuncs.h"

sheet *g_sheet_ctx =NULL;
ErrorCode g_eval_err= ERR_OK;

/* stack operation: درج و حذف  */
erors push(memorystack *sone, double value)
{
    if (sone->most >= max - 1)
        return action_peroblem;
    sone->data[++(sone->most)] = value;
    return succetion;
}
void inistialzationstack(memorystack *sone)
{
    sone->most = -1;
}

erors pop(memorystack *sone, double *value)
{
    if (sone->most < 0)
        return action_peroblem;
    *value = sone->data[(sone->most)--];
    return succetion;
}

/* تبدیل توکن به عدد */
double tokenvalue(const char *token_str, sheet *sh)
{
    char *endptr = NULL;
    double val = strtod(token_str, &endptr);
    if (endptr && *endptr == '\0' && endptr != token_str)
        return val;

    int row, col;
    if (addressToNum(token_str, &row, &col))
    {
        if (!sh || !sh->cells)
        {
            g_eval_err = ERR_BADREF;
            return NAN;
        }
        if (row < 0 || col < 0 || (size_t)row >= sh->rows || (size_t)col >= sh->cols)
        {
            g_eval_err = ERR_BADREF;
            return NAN;
        }

        Cell *c = &sh->cells[row][col];
        if (c->err != ERR_OK)
            return NAN;
        return c->value;
    }

    return NAN;
}

/*  postfix calculate محاسبه با الگوریتم */
erors calculate(const char **input, int countinput, double *output)
{
    memorystack stack;
    inistialzationstack(&stack);

    for (int i = 0; i < countinput; ++i)
    {
        const char *took = input[i];
        if (!took || took[0] == '\0')
            return input_peroblem;

        if (strcmp(took, "-u") == 0)
        {
            double a;
            if (pop(&stack, &a) != succetion)
                return action_peroblem;
            if (push(&stack, -a) != succetion)
                return action_peroblem;
            continue;
        }

        /* عدد یا سلول */

        {
            double v = tokenvalue(took, g_sheet_ctx);
            if (!isnan(v))
            {
                if (push(&stack, v) != succetion)
                    return action_peroblem;
                continue;
            }
        }

        /* operators */
        /*دقت کنید در این قسمت قصد این را داریم که توکن را به عملگر تبدیل کنیم*/
        if (!strcmp(took, "+") || !strcmp(took, "-") || !strcmp(took, "*") || !strcmp(took, "/") || !strcmp(took, "^"))
        {
            double b, a;
            if (pop(&stack, &b) != succetion)
                return action_peroblem;
            if (pop(&stack, &a) != succetion)
                return action_peroblem;

            if (!strcmp(took, "+"))
            {
                if (push(&stack, a + b) != succetion)
                    return action_peroblem;
            }
            else if (!strcmp(took, "-"))
            {
                if (push(&stack, a - b) != succetion)
                    return action_peroblem;
            }
            else if (!strcmp(took, "*"))
            {
                if (push(&stack, a * b) != succetion)
                    return action_peroblem;
            }
            else if (!strcmp(took, "/"))
            {
                if (fabs(b) < 1e-15)
                    return devision_peroblem;
                if (push(&stack, a / b) != succetion)
                    return action_peroblem;
            }
            else
            {
                double r = pow(a, b);
                if (isnan(r) || isinf(r))
                    return domain_peroblem;
                if (push(&stack, r) != succetion)
                    return action_peroblem;
            }
            continue;
        }

        /* برای تابع توان  */
        if (!strcmp(took, "pow"))
        {
            double b, a;
            if (pop(&stack, &b) != succetion)
                return action_peroblem;
            if (pop(&stack, &a) != succetion)
                return action_peroblem;
            double r = pow(a, b);
            if (isnan(r) || isinf(r))
                return domain_peroblem;
            if (push(&stack, r) != succetion)
                return action_peroblem;
            continue;
        }

        /* نوابع تک عملگر ریاضی*/
        {
            funmath *mf = findfun(took);
            if (mf)
            {
                double a;
                if (pop(&stack, &a) != succetion)
                    return action_peroblem;
                double r = mf->fun1(a);
                if (isnan(r) || isinf(r))
                    return domain_peroblem;
                if (push(&stack, r) != succetion)
                    return action_peroblem;
                continue;
            }
        }

        return input_peroblem;
    }

    if (stack.most != 0)
        return action_peroblem;
    if (pop(&stack, output) != succetion)
        return phraise_peroblem;
    if (isnan(*output) || isinf(*output))
        return domain_peroblem;
    return succetion;
}