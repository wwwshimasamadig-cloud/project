//  به نام خدا
// مه یاس جواهری  ۱۱/۱۱/۱۴۰۴
#ifndef FORMULA_H
#define FORMULA_H

// تشخیص نوع توکن
typedef enum
{
    number,
    opearator,
    cell,
    paren_open,
    paren_close,
    function
} tokentype;

typedef struct
{
    tokentype type;
    char string_value[32];
    double numeric_value;
} token;

#define token_max 256
token tokens[token_max];
int token_count = 0;

// توابع
void tokenize(const char *expr);
int parentheses();

#endif