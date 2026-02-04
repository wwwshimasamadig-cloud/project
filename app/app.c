#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "app.h"
#include "sheet.h"
#include "tokenizer.h"
#include "eval.h"

static char *duplicateStr( const char *s);
static int is_plain_number(const char *s, double *out);
static int parse_assignment(const char *line , char out_addr[32], const char **out_rhs);

//این تابع کد خطا را از اینام ارور کد میگیرد و متن مناسب را برمیگرداند
const char *error_to_string(ErrorCode e)
{
    switch (e)
    {
    case ERR_OK:
    return "OK";

    case ERR_DIV0:
    return "Divide by zero";

    case ERR_BADREF:
    return "Invalid cell reference";

    case ERR_BADPAREN:
    return "Bad parentheses";

    case ERR_UNKNOWN_F:
    return "Unknown function";

    case ERR_DOMAIN:
    return "Math domain error";

    case ERR_PARSE:
    return "Parse error";

    case ERR_NOMEM:
    return "Out of memory";

    default:
    return "Unknown error";
    }
}

//این تابع یک کپی جدید از رشته ورودی میگیرد و ان را برمیگرداند
static char *duplicateStr(const char *s)
{
    if (!s)
        return NULL;
    size_t n = strlen(s);
    char *p = (char *)malloc(n + 1);
    if (!p)
        return NULL;
    memcpy(p, s, n + 1);
    return p;
}

//تابع چک میکند که ورودی فقط یک عدد باشد و  چیز اضافی مثل حرف یا ادرس  نداشته باشد
static int is_plain_number(const char *s, double *out){
    if (!s)
        return 0;
    while (isspace((unsigned char)*s))
        s++;
    if (*s == '\0')
        return 0;
    char *end = NULL;
    double v = strtod(s, &end);
    if (end == s)
    return 0;

    while (isspace((unsigned char)*end))
    end++;
    if (*end != '\0')
    return 0;
    if (out)
    *out = v;
    return 1;//موفق بود
}

//این تابع خطاهای داخلی ماشین حساب رو به خطاهای استاندارد تابع ارورکد تبدیل میکند
ErrorCode map_calc_error(erors e){
    if (e == succetion)
    return ERR_OK;

    if (e == devision_peroblem)
    return ERR_DIV0;

    if (e == domain_peroblem)
    return ERR_DOMAIN;

    if (e == function_peroblem)
    return ERR_UNKNOWN_F;

    return ERR_PARSE;
}

//فرمول را بررسی میکند و اگر مشکلی باشد کد خطای مناسب را برمیگرداند
ErrorCode evaluate_expression_to_value(sheet *sh, const char *expr, double *out_value)
{
    if (!sh || !expr || !out_value)
        return ERR_PARSE;

    g_sheet_ctx = sh;
    g_eval_err = ERR_OK;

    tokenize(expr);
    if (token_error)
        return ERR_PARSE;

    if (!valid_parentheses())
        return ERR_BADPAREN;

    postfix pf = POSTFIX(tokens, token_count);
    const char *post[token_max];
    for (int i = 0; i < pf.count; i++)
        post[i] = pf.output[i].string_value;

    double result = NAN;
    erors e = calculate(post, pf.count, &result);
    ErrorCode ec = map_calc_error(e);

    if (ec == ERR_PARSE && g_eval_err == ERR_BADREF)
        return ERR_BADREF;

    if (ec != ERR_OK)
        return ec;
    *out_value = result;
    return ERR_OK;
}

static int parse_assignment(const char *line, char out_addr[32], const char **out_rhs)
{
    const char *eq = strchr(line, '=');
    if (!eq)
        return 0;

    char left[32] = {0};
    int li = 0;
    for (const char *p = line; p < eq && li < 31; ++p)
    {
        if (!isspace((unsigned char)*p))
            left[li++] = *p;
    }
    left[li] = 0;

    int r, c;
    if (!addressToNum(left, &r, &c))
        return 0;

    strcpy(out_addr, left);

    const char *rhs = eq + 1;
    while (*rhs && isspace((unsigned char)*rhs))
        rhs++;
    *out_rhs = rhs;
    return 1;
}

ErrorCode set_cell_from_input(sheet *sh, const char *target_addr, const char *input_line){
    if (!sh || !target_addr || !input_line)
        return ERR_PARSE;

    char dest_addr[32] = {0};
    const char *rhs = input_line;
    if (parse_assignment(input_line, dest_addr, &rhs))
        target_addr = dest_addr;

    int r, c;
    if (!addressToNum(target_addr, &r, &c))
        return ERR_BADREF;

    Cell *cellp = sheetExpandOrGet(sh, r, c);
    if (!cellp)
        return ERR_NOMEM;

    double old_value = cellp->value;
    ErrorCode old_err = cellp->err;
    char *old_formula_copy = NULL;
    int old_formula_was_null = (cellp->formula == NULL);

    if (!old_formula_was_null)
    {
        old_formula_copy = duplicateStr(cellp->formula);
        if (!old_formula_copy)
            return ERR_NOMEM;
    }

    /* اگر عدد بود */
    double direct = 0.0;
    if (is_plain_number(rhs, &direct))
    {
        free(cellp->formula);
        cellp->formula = NULL;
        cellp->value = direct;
        cellp->err = ERR_OK;

        free(old_formula_copy);
        return ERR_OK;
    }

    /* فرمول را ذخیره و محاسبه میکند */
    ErrorCode ec = cellSetFormula(cellp, rhs);
    if (ec != ERR_OK)
        goto restore;

    double new_value = NAN;
    ec = evaluate_expression_to_value(sh, rhs, &new_value);
    if (ec == ERR_OK)
    {
        cellp->value = new_value;
        cellp->err = ERR_OK;
        free(old_formula_copy);
        return ERR_OK;
    }

restore:
    /* سلول را به حالت قبل برمیگرداند */
    free(cellp->formula);
    cellp->formula = NULL;

    if (!old_formula_was_null)
    {
        cellp->formula = old_formula_copy;
        old_formula_copy = NULL;
    }

    cellp->value = old_value;
    cellp->err = old_err;

    free(old_formula_copy);
    return ec;
}

//سلول هاییکه فرمول دارند را دوباره محاسبه میکند و
//  مقدار یا خطای ان ها را به روز رسانی میکند
void recalc_all(sheet *sh)
{
    if (!sh)
        return;
    for (size_t r = 0; r < sh->rows; r++)
    {
        for (size_t c = 0; c < sh->cols; c++)
        {
            Cell *cellp = &sh->cells[r][c];
            if (!cellp->formula || cellp->formula[0] == '\0')
                continue;

            double old_value = cellp->value;
            double v = NAN;
            ErrorCode ec = evaluate_expression_to_value(sh, cellp->formula, &v);

            if (ec == ERR_OK)
            {
                cellp->value = v;
                cellp->err = ERR_OK;
            }
            else
            {
                cellp->value = old_value;
                cellp->err = ec;
            }
        }
    }
}