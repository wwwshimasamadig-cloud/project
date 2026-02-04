#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tokenizer.h"

// ارایه ای از توکن ها
token tokens[token_max];
int token_count = 0;

// ارور هندلینگ : توکن نامعتبر -> 1
int token_error = 0;
char token_error_msg[256] = {0};

//  tokenize توابع کمکی برای تشخیص توابع ریاضی در
static int mathfunction(const char *word)
{
    if (!word)
        return 0;
    const char *list[] = {"sin", "cos", "tan", "cot", "sinh", "cosh", "tanh", "sqrt", "abs", "ln", "log", "exp", "pow"};
    int n = (int)(sizeof(list) / sizeof(list[0]));
    for (int i = 0; i < n; i++)
        if (strcmp(word, list[i]) == 0)
            return 1;
    return 0;
}

// تشخیص اولویت عملگرها
static int precedence(char op)
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
    tokentype last_token_type = (tokentype)-1;
    token_count = 0;
    token_error = 0;
    token_error_msg[0] = '\0';

    while (expr[i] != '\0' && token_count < token_max)
    {

        if (isspace((unsigned char)expr[i]))
        {
            i++;
            continue;
        }

        // اعداد (اعشاری و صحیح )
        if (isdigit((unsigned char)expr[i]) || (expr[i] == '.' && isdigit((unsigned char)expr[i + 1])))
        {
            char num[32] = {0};
            int j = 0, dot_count = 0;

            while ((isdigit((unsigned char)expr[i]) || expr[i] == '.') && j < 31)
            {
                if (expr[i] == '.')
                {
                    dot_count++;
                    if (dot_count > 1)
                    {
                        token_error = 1;
                        snprintf(token_error_msg, sizeof(token_error_msg), "error : invalid number");
                        return;
                    }
                }
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

        //  pow (a,b) کاما برای
        if (expr[i] == ',')
        {
            tokens[token_count].string_value[0] = ',';
            tokens[token_count].string_value[1] = '\0';
            tokens[token_count].type = t_operator;
            token_count++;
            i++;
            last_token_type = t_operator;
            continue;
        }

        // عملگرها
        if (strchr("+-*/^", expr[i]) != NULL)
        {
            if (expr[i] == '-')
            {
                if (token_count == 0 || last_token_type == paren_open || last_token_type == function || last_token_type == t_operator)
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
            tokens[token_count].type = t_operator;
            token_count++;
            i++;
            last_token_type = t_operator;
            continue;
        }

        // ادرس سلول یا نام توابع ریاضی
        if (isalpha((unsigned char)expr[i]))
        {
            char word[32] = {0};
            int j = 0;

            while (isalpha((unsigned char)expr[i]) && j < 31)
                word[j++] = expr[i++];
            word[j] = '\0';

            // اگر به دنبالش عدد باشه : ادرس سلول
            if (isdigit((unsigned char)expr[i]))
            {
                while (isdigit((unsigned char)expr[i]) && j < 31)
                    word[j++] = expr[i++];
                word[j] = '\0';

                strcpy(tokens[token_count].string_value, word);
                tokens[token_count].type = cell;
                token_count++;
                last_token_type = cell;
                continue;
            }

            // اگر تابع کمکی خروجی غیر صفر بدهد : تابع ریاضی
            if (mathfunction(word))
            {
                strcpy(tokens[token_count].string_value, word);
                tokens[token_count].type = function;
                token_count++;
                last_token_type = function;
                continue;
            }

            // عبارتی بجز این دو: پیغام خطا
            token_error = 1;
            snprintf(token_error_msg, sizeof(token_error_msg), "error: '%s' is an invalid input.", word);
            return;
        }

        // کارکترهای ناشناخته
        token_error = 1;
        snprintf(token_error_msg, sizeof(token_error_msg),
                 "error : '%c' is an invalid input. ", expr[i]);
        return;
    }

    // تعداد بیش از حد توکن ها
    if (token_count >= token_max)
    {
        token_error = 1;
        snprintf(token_error_msg, sizeof(token_error_msg), "error: Too much inputs. ");
        return;
    }
}

// اعتبارسنجی پرانتز ها
// خروجی : 0 -> پیغام خطا
int valid_parentheses(void)
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

// این تابع توکن ها را به ترتیبی قرار میدهد که قابل محاسبه باشند
postfix POSTFIX(token *tokens, int token_count)
{
    token stack[token_max];
    int top = -1; // اندیس بالاترین عنصر: خالی است
    postfix result;
    result.count = 0;

    for (int i = 0; i < token_count; i++)
    {
        token current = tokens[i];

        switch (current.type)
        {

        // ادرس سلول یا اعداد مستقیما به خروجی منتقل میشن
        case number:
        case cell:
            result.output[result.count++] = current;
            break;

        // توابع به پشته ارسال میشن
        case function:
            stack[++top] = current;
            break;

        case t_operator:
        {

            if (current.string_value[0] == ',')
            {
                while (top >= 0 && stack[top].type != paren_open)
                {
                    result.output[result.count++] = stack[top--];
                }
                break;
            }

            while (top >= 0 && stack[top].type == t_operator &&
                   precedence(stack[top].string_value[0]) >= precedence(current.string_value[0]))
            {
                result.output[result.count++] = stack[top--];
            }
            stack[++top] = current;
            break; // وقتی عملگر با الویت بالاتر قبلش هست؛ قبلی،خارج میشه و بعدش به پشته منتقل میشه
        }

        // پرانتز باز به پشته منتقل می شود
        case paren_open:
            stack[++top] = current;
            break;

        // عملگرها تا زمانی که به پرانتز باز برسیم؛ خارج می شوند
        case paren_close:
            while (top >= 0 && stack[top].type != paren_open)
            {
                result.output[result.count++] = stack[top--];
            }
            if (top >= 0 && stack[top].type == paren_open)
                top--; // پرانتز باز هم از پشته خارج می کنیم
            if (top >= 0 && stack[top].type == function)
            {
                result.output[result.count++] = stack[top--];
            } // اگر تابعی بلافاصله بعد از پرانتز بیایید ان هم باید به خروجی منتقل شود: sin(A1+3)
            break;

        default:
            break;
        }
    }

    // هرچه در پشته باقیمانده به خروجی اضافه می کنیم
    while (top >= 0)
        result.output[result.count++] = stack[top--];
    return result;
}