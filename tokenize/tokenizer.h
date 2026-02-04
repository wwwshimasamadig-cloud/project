#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stddef.h>

// انواع توکن
typedef enum
{
    number,
    t_operator,
    cell,
    paren_open,
    paren_close,
    function,
    invalid
} tokentype;

// ساختار توکن ها
typedef struct
{
    tokentype type;
    char string_value[32];
    double numeric_value;
} token;

// ارایه ای از توکن ها
#define token_max 256
extern token tokens[token_max];
extern int token_count;

// ارور هندلینگ : توکن نامعتبر -> 1
extern int token_error;
extern char token_error_msg[256];

typedef struct
{
    token output[token_max];
    int count;
} postfix;

void tokenize(const char *expr);
int valid_parentheses(void );
postfix POSTFIX (token *tokens_in, int token_count_in);

#endif