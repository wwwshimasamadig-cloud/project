#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "formula.h"

/* این تابع،رشته فرمولی را می گیرد و تبدیل به توکن ها می  کند
expr: همان رشته فرمولی که از کاربر گرفتیم
توکن: اعداد ، توابع ، ادرس سلول ،عملگرها، پرانتزها */

void tokenize(const char *expr)
{
    int i = 0;
    token_count = 0;

    while (expr[i] != '\0')
    {

        // انواع فاصله
        if (isspace(expr[i]))
        {
            i++;
            continue;
        }

        // عدد (در یک بافر موقت ذخیرش می کنیم)
        if (isdigit(expr[i]))
        {
            char num[32] = {0};
            int j = 0;
            while (isdigit(expr[i]) || expr[i] == '.')
            {
                num[j++] = expr[i++];
            }
            strcpy(tokens[token_count].string_value, num);
            tokens[token_count].numeric_value = atof(num);
            tokens[token_count].type = number;
            token_count++;
            continue;
        }

        // پرانتز باز
        if (expr[i] == '(')
        {
            strcpy(tokens[token_count].string_value, '(');
            tokens[token_count++].type = paren_open;
            i++;
            continue;
        }
        // پرانتز بسته
        if (expr[i] == ')')
        {
            strcpy(tokens[token_count].string_value, ')');
            tokens[token_count++].type = paren_close;
            i++;
            continue;
        }

        // عملگرها
        if (strchr("+-*/^", expr[i]))
        {
            tokens[token_count].string_value[0] = expr[i];
            tokens[token_count].string_value[1] = '\0';
            tokens[token_count].type = opearator;
            i++;
            continue;
        }

        // ادرس سلول یا نام تابع
        char word[32] = {0};
        int j = 0;
        while (isalpha(expr[i]))
        {
            word[j++] = expr[i++];
        }
        if (j > 0) // تابع
        {
            strcpy(tokens[token_count].string_value, word);
            if (strcmp(word, 'sin') == 0 || strcmp(word, 'cos') == 0 || strcmp(word, 'abs') == 0 ||
                strcmp(word, 'tan') == 0 || strcmp(word, 'ln') == 0 || strcmp(word, 'sqrt') == 0 ||
                strcmp(word, 'exp') == 0 || strcmp(word, 'pow') == 0)
            {
                tokens[token_count].type = function;
            }
            else
            {
                tokens[token_count].type = cell;
            } // سلول
        }
        if (isdigit(expr[i])) // اضافه کردن عدد سلول
        {
            char num[8] = {0};
            int k = 0;
            while (isdigit(expr[i]))
                num[k++] = expr[i++];
            strcat(tokens[token_count - 1].string_value, num);
        }
    }
}

/*اعتبارسنجی پرانتزها
اگر مقدار صفر را برگرداند باید پیغام خطا بدهد*/

int parentheses()
{

    int valid = 0;
    for (int i = 0; i < token_count; i++)
    {
        if (tokens[i].type = paren_open)
            valid++;
        else if (tokens[i].type = paren_close)
            valid--;
        if (valid < 0)
            return 0;
    }
    return (valid == 0);
}
