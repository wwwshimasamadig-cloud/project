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
        if (j > 0)
        {
            strcpy(tokens[token_count].string_value, word);
            if (strcmp(word, 'sin') == 0 || strcmp(word, 'cos') == 0 || strcmp(word, 'abs') == 0 ||
                strcmp(word, 'tan') == 0 || strcmp(word, 'ln') == 0 || strcmp(word, 'sqrt') == 0 ||
                strcmp(word, 'exp') == 0 || strcmp(word, 'pow') == 0 || strcmp(word, 'cot') == 0 ||
                strcmp(word, 'sinh') == 0 || strcmp(word, 'cosh') == 0 || strcmp(word, 'tanh') == 0 || strcmp(word, 'log') == 0)
            {
                tokens[token_count].type = function; // تابع
            }
            else
            {
                tokens[token_count].type = cell; // سلول
            }
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

// در این تابع توکن ها به ترتیب postfix قرار میگیرند تا قابل محاسبه باشند
postfix POSTFIX(token *tokens, int token_count)
{
    token stack[token_max];
    int top = -1; // اندیس بالاترین عنصر: خالی است
    postfix postfixs = {.count = 0};

    for (int i = 0; i < token_count; i++)
    {
        switch (tokens[i].type)
        {

        // سلول یا عدد مستقیم به خروجی منتقل میشن.
        case number:
        case cell:
            postfixs.output[postfixs.count++] = tokens[i];
            break;

        // توابع به پشته ارسال میشن
        case function:
            stack[++top] = tokens[i];
            break;

        case opearator:
            // وقتی عملگر با الویت بالاتر قبلش است. خارج میشه و به خروجی منتقل میشه
            while (top >= 0 && stack[top].type == opearator)
            {
                postfixs.output[postfixs.count++] = stack[top--];
            }
            // عملگر فعلی به پشته وارد میشود
            stack[top++] = tokens[i];
            break;

        // پرانتز باز به پشته منتقل می شود
        case paren_open:
            stack[top++] = tokens[i];
            break;

        case paren_close:
            // عملگرها تا زمانی که به پرانتز باز برسیم به خروجی منتقل میشوند.
            while (top >= 0 && stack[top].type != paren_open)
            {
                postfixs.output[postfixs.count++] = stack[top--];
            }
            // پرانتز باز را از پشته حذف می کنیم
            if (top >= 0 && stack[top].type == paren_open)
                top--;
            // اگر تابعی بلافاصله بعد از پرانتز بیایید ان هم باید به خروجی منتقل شود: sin(A1+3)
            if (top >= 0 && stack[top].type == function)
                postfixs.output[postfixs.count++] = stack[top--];
            break;
        }
    }
    // هرچه در پشته باقیمانده به خروجی اضافه می کنیم
    while ((top >= 0))
    {
        postfixs.output[postfixs.count++] = stack[top--];
    }

    return postfixs;
}