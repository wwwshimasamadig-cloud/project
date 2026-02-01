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

//توکن ها با ترتیبی قابل محاسبه
typedef struct {
    token output[token_max];
    int count;
}postfix;

// توابع
void tokenize(const char *expr);
int parentheses();
postfix POSTFIX(token *tokens , int count);

#endif