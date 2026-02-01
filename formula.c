#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "formula.h"

token tokens[token_max];
int token_count = 0;

// تابع کمکی برای تشخیص توابع ریاضی
int mathfunction(const char *word)
{
    if (!word)
        return 0;
    const char *math_functions[] = {"sin", "cos", "tan", "cot", "sinh", "cosh", "tanh", "sqrt", "abs", "ln", "log", "exp", "pow"};
    int functions_count = sizeof(math_functions) / sizeof(math_functions[0]);
    for (int i = 0; i < functions_count; i++)
    {
        if (strcmp(word, math_functions[i]) == 0)
            return 1;
    }
    return 0;
}

// تشخیص اولویت عملگرها
int precedence(char op)
{
    switch (op)
    {
    case '^':
        return 4;
    case '*':
    case '/':
        return 3;
    case '+':
    case '-':
        return 2;
    default:
        return 1;
    }
}

/* این تابع،رشته فرمولی را می گیرد و تبدیل به توکن ها می  کند
expr: همان رشته فرمولی که از کاربر گرفتیم
توکن: اعداد ، توابع ، ادرس سلول ،عملگرها، پرانتزها */

void tokenize(const char *expr)
{
    int i = 0;
    tokentype last_token_type = -1;
    token_count = 0;

    while (expr[i] != '\0')
    {

        // رد کردن فاصله ها
        if (isspace(expr[i]))
        {
            i++;
            continue;
        }

        // عدد(صحیح و اعشاری )
        if (isdigit(expr[i]) || (expr[i] == '.' && isdigit(expr[i + 1])))
        {
            char num[32] = {0};
            int j = 0, dot_count = 0;

            while (isdigit(expr[i]) || expr[i] == '.')
            {
                if (expr[i] == '.')
                {
                    dot_count++;
                    if (dot_count > 1)
                    {
                        break;
                    }
                } // تعداد ممیزها غیرمجاز: پیغام خطا
                num[j++] = expr[i++];
            }
            num[j] = '\0';
            strcpy(tokens[token_count].string_value, num);
            tokens[token_count].numeric_value = atof(num);
            tokens[token_count].type = number;
            token_count++;
            last_token_type = number;
            continue;
        }

        // پرانتز باز
        if (expr[i] == '(')
        {
            tokens[token_count].string_value[0] = '(';
            tokens[token_count].string_value[1] = '\0';
            tokens[token_count].type = paren_open;
            token_count++;
            i++;
            last_token_type = paren_open;
            continue;
        }
        // پرانتز بسته
        if (expr[i] == ')')
        {
            tokens[token_count].string_value[0] = ')';
            tokens[token_count].string_value[1] = '\0';
            tokens[token_count].type = paren_close;
            token_count++;
            i++;
            last_token_type = paren_close;
            continue;
        }

        // عملگرها
        if (strchr("+-*/^", expr[i]) != NULL)
        {
            if (expr[i] == '-')
            {
                if (token_count == 0 || last_token_type == paren_open || last_token_type == function || last_token_type == operator)
                {
                    tokens[token_count].string_value[0] = '-';
                    tokens[token_count].string_value[1] = 'u';
                    tokens[token_count].string_value[2] = '\0';
                }
                else
                {
                    tokens[token_count].string_value[0] = '-';
                    tokens[token_count].string_value[1] = '\0';
                }
            }
            else
            {
                tokens[token_count].string_value[0] = expr[i];
                tokens[token_count].string_value[1] = '\0';
            }
            tokens[token_count].type = operator;
            last_token_type = operator;
            token_count++;
            i++;
            continue;
        }

        // ادرس سلول یا نام تابع
        if (isalpha(expr[i]))
        {
            char word[32] = {0};
            int j = 0;
            while (isalpha(expr[i]))
            {
                word[j++] = expr[i++];
            }
            word[j] = '\0';
            if (isdigit(expr[i]))
            {

                while (isdigit(expr[i]))
                {
                    if (j < 31)
                        word[j++] = expr[i++];
                    else
                        i++;
                }
                word[j] = '\0';

                strcpy(tokens[token_count].string_value, word);
                tokens[token_count].type = cell; // سلول
                token_count++;
                last_token_type = cell;
            }
            else if (mathfunction(word))
            {
                strcpy(tokens[token_count].string_value, word);
                tokens[token_count].type = function; // تابع
                token_count++;
                last_token_type = function;
            }
            else
            {
                strcpy(tokens[token_count].string_value, word);
                tokens[token_count].type = invalid; // یک عبارت غیر مجاز
                token_count++;
            }
            continue;
        }
        // کارکتر ناشناخته
        i++;
    }
}

/*اعتبارسنجی پرانتزها
اگر مقدار صفر را برگرداند باید پیغام خطا بدهد*/

int valid_parentheses()
{
    int valid = 0;
    for (int i = 0; i < token_count; i++)
    {
        if (tokens[i].type == paren_open)
            valid++;
        else if (tokens[i].type == paren_close)
            valid--;
        if (valid < 0)
            return 0;
    }
    return (valid == 0);
}
// دو تابع ارور هندلینگ در اینجا: چک کردن دو عملگر پشت سرهم و چک کردن سلول معتبر
// در این تابع توکن ها به ترتیب قرار میگیرند تا قابل محاسبه باشند
postfix POSTFIX(token *tokens, int token_count)
{
    token stack[token_max];
    int top = -1; // اندیس بالاترین عنصر: خالی است
    postfix result = {.count = 0};

    for (int i = 0; i < token_count; i++)
    {
        token current = tokens[i];
        switch (current.type)
        {

        // سلول یا عدد مستقیم به خروجی منتقل میشن.
        case number:
        case cell:
            result.output[result.count++] = current;
            break;

        // توابع به پشته ارسال میشن
        case function:
            stack[++top] = tokens[i];
            break;

        case operator:
            // وقتی عملگر با الویت بالاتر قبلش است. خارج میشه و به خروجی منتقل میشه
            while (top >= 0 && stack[top].type == operator && precedence(stack[top].string_value[0]) >= precedence(current.string_value[0]))
            {
                result.output[result.count++] = stack[top--];
            }
            // عملگر فعلی به پشته وارد میشود
            stack[++top] = current;
            break;

        // پرانتز باز به پشته منتقل می شود
        case paren_open:
            stack[++top] = current;
            break;

        case paren_close:
            // عملگرها تا زمانی که به پرانتز باز برسیم به خروجی منتقل میشوند.
            while (top >= 0 && stack[top].type != paren_open)
            {
                result.output[result.count++] = stack[top--];
            }
            // پرانتز باز را از پشته حذف می کنیم
            if (top >= 0 && stack[top].type == paren_open)
                top--;
            // اگر تابعی بلافاصله بعد از پرانتز بیایید ان هم باید به خروجی منتقل شود: sin(A1+3)
            if (top >= 0 && stack[top].type == function)
                result.output[result.count++] = stack[top--];
            break;
        }
    }
    // هرچه در پشته باقیمانده به خروجی اضافه می کنیم
    while ((top >= 0))
    {
        result.output[result.count++] = stack[top--];
    }

    return result;
}