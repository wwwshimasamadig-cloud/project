#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

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
static token tokens[token_max];
static int token_count = 0;

// ارور هندلینگ : توکن نامعتبر -> 1
static int token_error = 0;
static char token_error_msg[256] = {0};

typedef struct
{
    token output[token_max];
    int count;
} postfix;

/*استراکچر توابع*/
typedef struct
{
    double (*fun1)(double);
    const char *name1;
} funmath;

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

//کد های خطا که در پروژه استفاده میشود:
typedef enum{
    ERR_OK= 0, //خطا ندارد
    ERR_DIV0,//خطای تقسیم بر صفر دارد
    ERR_BADREF, //خطای ارجاع نامعتبر
    ERR_BADPAREN,//خطای پرانتزگذاری نامعتبر یا مطابق نباشد
    ERR_UNKNOWN_F,//خطای تابع ناشناخته
    ERR_DOMAIN,//خطای دامنه
    ERR_PARSE ,//خطای مشکل ساختاری یا نوشتاری
    ERR_NOMEM,//کمبود حافظه یا خطای تخصیص حافظه
}ErrorCode;

//تعریف ساختار هر سلول:
typedef struct{
    int row; //شماره سطر سلول
    int col; //شماره ستون
    char address[16];//نگه داری ادرس متنی سلول
    double value; //مقدار عددی نهایی سلول
    char *formula; //اگر سلول فرمول داشته باشد محل ذخیره آن اینجاست
    ErrorCode err;//مشخص میکند سلول خطا دارد یا خیر و نوع خطا
}Cell;

typedef struct{
    size_t rows;//تعداد
    size_t cols;
    Cell  **cells;//یک ارایه از اشاره گر به سطرها
    // هر سطر خود یک ارایه از cell
}sheet;

static sheet *g_sheet_ctx =NULL;
static ErrorCode g_eval_err= ERR_OK;

//پروتوتایپ ها
static void numToAddress (int row, int col, char out[16] );
static const char *error_to_string (ErrorCode e);
static char *duplicateStr( const char *s);
static int is_plain_number(const char *s, double *out);
static int parse_assignment(const char *line , char out_addr[32], const char **out_rhs);
static void recalc_all (sheet *sh);
static ErrorCode map_calc_error (erors e);
static ErrorCode evaluate_expression_to_value(sheet *sh , const char *expr, double *out_value);
static ErrorCode set_cell_from_input(sheet *sh, const char *target_addr, const char *input_line);


static int rowExpand(sheet *s, size_t expandedR);
static int colsExpand(sheet *s,  size_t expandedC);
static int read_line(FILE *f, char *buf, size_t cap);


int addressToNum(const char *address, int *resultRow, int *resultCol);
Cell cellMake (int row, int col);
void cellFree(Cell *c );
ErrorCode cellSetFormula(Cell *c, const char *formula);

int createsheet(sheet *s, size_t rows, size_t cols);
void freesheet (sheet *s);
Cell *sheetFindCell(sheet *s, int row, int col);
Cell *sheetExpandOrGet(sheet *s, int row , int col);


void tokenize(const char *expr);
int valid_parentheses(void );
postfix POSTFIX (token *tokens_in, int token_count_in);


erors push(memorystack *sone, double value);
void inistialzationstack(memorystack *sone);
erors pop(memorystack *sone, double *value);

/*برای ورودی توابع ریاضی*/
funmath *findfun(const char *name1);

/* prototypes   */
double truth_log(double n);
double truth_ln(double n);
double truth_sqrt(double n);
double truth_abs(double  n);
double truth_cos(double n);
double truth_sin(double n);
double truth_tan(double n);
double truth_cot(double n);
double truth_exp(double n);
double truth_cosh(double n);
double truth_sinh(double n);
double truth_tanh(double n);

/* ارزیابی */
double tokenvalue(const char *token_str, sheet *sh);
erors calculate(const char **input,  int countinput, double *output);

void savesheet(sheet *s, const char name[]);
void loadsheet(sheet *s, const char name[]);

//تابع main
int main()
{
    sheet sh;
    int rows, cols;
    int choice = 0;

    printf("Enter number of rows: ");
    if (scanf("%d", &rows) != 1)
        return 0;
    printf("Enter number of cols: ");
    if (scanf("%d", &cols) != 1)
        return 0;
    getchar();

    if (!createsheet(&sh, (size_t)rows, (size_t)cols))
    {
        printf("Allocation failed.\n");
        return 0;
    }
    g_sheet_ctx = &sh;

    while (1)
    {
        printf("\nMenu:\n"
               "1-Set cell value/formula\n"
               "2-Calculate formula (no store)\n"
               "3-Show cell value\n"
               "4-Save sheet\n"
               "5-Load sheet\n"
               "6-Extend sheet (add rows/cols)\n"
               "7-Recalculate all formulas\n"
               "8-Exit\n"
               "Choice: ");

        if (scanf("%d", &choice) != 1)
            break;
        getchar();

        if (choice == 8)
            break;

        switch (choice)
        {
        case 1:
        {
            char addr[32];
            char input[512];

            printf("Enter cell address (e.g., A1): ");
            if (scanf("%31s", addr) != 1)
                break;
            getchar();

            printf("Enter value or formula (you may type 'B2 = sin(A1)+2'):\n> ");
            if (!fgets(input, sizeof(input), stdin))
                break;
            input[strcspn(input, "\r\n")] = 0;

            ErrorCode ec = set_cell_from_input(&sh, addr, input);

            if (token_error)
            {
                printf("%s\n", token_error_msg);
                token_error = 0;
                token_error_msg[0] = '\0';
                break;
            }

            if (ec == ERR_OK)
            {
                int r, c;
                if (addressToNum(addr, &r, &c))
                {
                    Cell *cellp = sheetFindCell(&sh, r, c);
                    if (cellp)
                        printf("OK. %s = %.10g\n", addr, cellp->value);
                    else
                        printf("OK.\n");
                }
                else
                {
                    printf("OK.\n");
                }
            }
            else
            {
                printf("Error: %s (cell NOT changed)\n", error_to_string(ec));
            }
            break;
        }

        case 2:
        {
            char input[512];
            printf("Enter formula to calculate: ");
            if (!fgets(input, sizeof(input), stdin))
                break;
            input[strcspn(input, "\r\n")] = 0;

            tokenize(input);
            if (token_error)
            {
                printf("%s\n", token_error_msg);
                token_error = 0;
                token_error_msg[0] = '\0';
                break;
            }

            if (!valid_parentheses())
            {
                printf("Error: Bad parentheses\n");
                break;
            }

            postfix pf = POSTFIX(tokens, token_count);
            const char *expr[token_max];
            for (int i = 0; i < pf.count; i++)
                expr[i] = pf.output[i].string_value;

            double result = NAN;
            g_eval_err = ERR_OK;
            erors e = calculate(expr, pf.count, &result);
            if (e == succetion)
                printf("Result = %.10g\n", result);
            else
            {
                ErrorCode ec = map_calc_error(e);
                if (ec == ERR_PARSE && g_eval_err == ERR_BADREF)
                    ec = ERR_BADREF;
                printf("Error: %s\n", error_to_string(ec));
            }
            break;
        }

        case 3:
        {
            char addr[32];
            int r, c;

            printf("Enter cell address: ");
            if (scanf("%31s", addr) != 1)
                break;
            getchar();

            if (!addressToNum(addr, &r, &c))
            {
                printf("Invalid address\n");
                break;
            }

            Cell *cellp = sheetFindCell(&sh, r, c);
            if (!cellp)
            {
                printf("Cell not found\n");
                break;
            }

            if (cellp->err != ERR_OK)
            {
                printf("Cell %s ERROR: %s (code=%d)\n", addr, error_to_string(cellp->err), cellp->err);
                printf("Stored value (unchanged on error) = %.10g\n", cellp->value);
            }
            else
            {
                printf("Cell %s value = %.10g\n", addr, cellp->value);
            }

            if (cellp->formula)
                printf("Formula: %s\n", cellp->formula);
            break;
        }

        case 4:
        {
            char filename[256];
            printf("Enter filename to save: ");
            if (!fgets(filename, sizeof(filename), stdin))
                break;
            filename[strcspn(filename, "\r\n")] = 0;
            if (filename[0] == '\0')
                break;
            savesheet(&sh, filename);
            break;
        }

        case 5:
        {
            char filename[256];
            printf("Enter filename to load: ");
            if (!fgets(filename, sizeof(filename), stdin))
                break;
            filename[strcspn(filename, "\r\n")] = 0;
            if (filename[0] == '\0')
                break;
            loadsheet(&sh, filename);
            g_sheet_ctx = &sh;
            break;
        }

        case 6:
        {
            int addR = 0, addC = 0;
            printf("Add how many rows? ");
            if (scanf("%d", &addR) != 1)
            {
                getchar();
                break;
            }
            printf("Add how many cols? ");
            if (scanf("%d", &addC) != 1)
            {
                getchar();
                break;
            }
            getchar();

            if (addR < 0)
                addR = 0;
            if (addC < 0)
                addC = 0;

            size_t newR = sh.rows + (size_t)addR;
            size_t newC = sh.cols + (size_t)addC;

            if (!rowExpand(&sh, newR))
            {
                printf("Row expand failed.\n");
                break;
            }
            if (!colsExpand(&sh, newC))
            {
                printf("Col expand failed.\n");
                break;
            }

            printf("Expanded to %zu rows, %zu cols.\n", sh.rows, sh.cols);
            break;
        }

        case 7:
            recalc_all(&sh);
            if (token_error)
            {
                printf("%s\n", token_error_msg);
                token_error = 0;
                token_error_msg[0] = '\0';
            }
            else
            {
                printf("Recalculated.\n");
            }
            break;

        default:
            printf("Invalid choice\n");
            break;
        }
    }

    freesheet(&sh);
    printf("Program ended.\n");
    return 0;
}

//تعاریف توابع

//این تابع کد خطا را از اینام ارور کد میگیرد و متن مناسب را برمیگرداند
static const char *error_to_string(ErrorCode e)
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

//این تابع شماره سطر و ستون را به ادرس اکسل تبدیل میکند
static void numToAddress(int row, int col, char out[16]){
    char tmp[16];
    int ti = 0;
    int c = col + 1;
    while (c > 0 && ti < 10){
        int rem = (c-1) % 26;
        tmp[ti++] = (char)('A' + rem);
        c = (c - 1) / 26;
    }
    int oi = 0;
    for (int i = ti - 1; i >= 0 && oi < 12; --i)
        out[oi++] = tmp[i];
    snprintf(out + oi, 16 - oi, "%d", row + 1);
}


//تبدیل یک ادرس به شماره سطر و ستون 
int addressToNum(const char *address, int *resultRow, int *resultCol){
    if (!address || !resultRow || !resultCol)
    return 0;

    size_t i = 0;
    if (!isalpha((unsigned char)address[i]))
    return 0;//اگر اولین کاراکتر ادرس حرف نبود نامعتبر است.

    int col_acc = 0;
    while (isalpha((unsigned char)address[i])){//تا وقتی حرف میبینی ادامه بده{
        char ch = (char)toupper((unsigned char)address[i]);
        //فرقی بین حروف بزرگ و گوچک نباشد
        if (ch < 'A' || ch > 'Z')
        return 0; //نامعتبر
        col_acc = col_acc * 26 + (ch - 'A' + 1);//تبدیل حرف به عدد
        i++;
    }
    if(col_acc <= 0)//نامعتبر
    return 0;
    int col = col_acc - 1;//شمارش ستون ها از صفر شروع میشود

    if (!isdigit((unsigned char)address[i]))
    return 0;//اگر بعد از حروف عدد نبامد ممعتبر نیست

    int row = 0;
    while (isdigit((unsigned char)address[i])){//تا رقم میبینی ادامه بده
        row=row * 10 + (address[i] - '0');//تبدیل رقم به عدد
        i++;
    }

    if (address[i] != '\0')
        return 0;//اگر بعد از عددها کاراکتر باشد معتبر نیست
    if (row <= 0)
        return 0;

    *resultRow = row - 1;//اولین سطر سظر ضفر است
    *resultCol = col;
    return 1;
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

/*   توابع ریاضی */
double truth_sin(double n)
{
    return sin(n);
}
double truth_cos(double n)
{
    return cos(n);
}
double truth_tan(double n)
{
    return tan(n);
}
double truth_exp(double n)
{
    return exp(n);
}
double truth_abs(double n)
{
    return fabs(n);
}
double truth_sinh(double n)
{
    return sinh(n);
}
double truth_cosh(double n)
{
    return cosh(n);
}
double truth_tanh(double n)
{
    return tanh(n);
}

/*شرط منطقی تابع لگاریتم در مبنا عدد ای*/
double truth_ln(double n)
{
    return (n <= 0) ? NAN : log(n);
}

/*شرط منطقی برای تابع لگاریتم*/
double truth_log(double n)
{
    return (n <= 0) ? NAN : log10(n);
}

/*شرط منطقی برای تابع جذر*/
double truth_sqrt(double n)
{
    return (n < 0) ? NAN : sqrt(n);
}

/*شرط منطقی برای تابع کتانژانت*/
double truth_cot(double x)
{
    double t = tan(x);
    if (fabs(t) < 1e-15)
        return NAN;
    return 1.0 / t;
}

/* توابع */
static funmath math_functions[] = {
    {truth_sin, "sin"},
    {truth_cos, "cos"},
    {truth_tan, "tan"},
    {truth_cot, "cot"},
    {truth_sqrt, "sqrt"},
    {truth_log, "log"},
    {truth_ln, "ln"},
    {truth_exp, "exp"},
    {truth_abs, "abs"},
    {truth_sinh, "sinh"},
    {truth_cosh, "cosh"},
    {truth_tanh, "tanh"},
    {NULL, NULL}};

funmath *findfun(const char *name1)
{
    for (int i = 0; math_functions[i].name1 != NULL; ++i)
    {
        if (strcmp(name1, math_functions[i].name1) == 0)
            return &math_functions[i];
    }
    return NULL;
}

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
            tokens[token_count].type = t_operator ;
            token_count++;
            i++;
            last_token_type = t_operator  ;
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

        case t_operator :
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


//توابع ایجاد و افزایش جدول
//ساخت جدول با اندازه های اولیه سطرو ستون
int createsheet(sheet *s, size_t rows, size_t cols){
    if (!s || rows==0 || cols==0)
    return 0;

    s->rows = 0;
    s->cols = 0;
    s->cells = NULL;

    Cell **ptrs = (Cell **)malloc(rows * sizeof(Cell *));
    if (!ptrs)
        return 0;

    for (size_t r = 0; r < rows; r++)
        ptrs[r] = NULL;

    for (size_t r=0; r<rows; r++){
        ptrs[r] = (Cell *)malloc(cols * sizeof(Cell));
        if (!ptrs[r]){
            for (size_t k = 0; k < r; k++){
                for (size_t c = 0; c < cols; c++)
                    cellFree(&ptrs[k][c]);
                free(ptrs[k]);
            }
            free(ptrs);
            return 0;
        }
        //مقداردهی اولیه
        for(size_t c = 0; c < cols; c++)
            ptrs[r][c] = cellMake((int)r, (int)c);
    }

    s->cells = ptrs;
    s->rows = rows;
    s->cols = cols;
    return 1;
}

//حافظه ای که برای شیت گرفته شده راازاد میکند تا نشت حافظه رخ ندهد
//ازاد سازی در سه مرحله ی سلول، سطر و ارایه سطر ها انجام یدهد
void freesheet(sheet *s){
    if (!s || !s->cells)
    return;

    for (size_t r = 0; r < s->rows; r++){
        if (!s->cells[r])
            continue;
        for (size_t c = 0; c < s->cols; c++)
            cellFree(&s->cells[r][c]);
        free(s->cells[r]);
        s->cells[r] = NULL;
    }
    free(s->cells);
    s->cells = NULL;
    s->rows = 0;
    s->cols = 0;
}

//برای دسترسی به سلولی در سطری بزرگتر از اندازه کنونی جدول
static int rowExpand(sheet *s, size_t expandedR){
    if(!s || !s->cells)
        return 0;
    if(expandedR <= s->rows)
        return 1;//نیازی به افزایش نیست
    Cell **newPtrs = (Cell **)malloc(expandedR * sizeof(Cell *));
    if(!newPtrs)
        return 0;
    for(size_t r = 0; r < s->rows; r++)
        newPtrs[r] = s->cells[r];
    //سطر های جدید را به NULLپیش فرض میکنیم
    for(size_t r = s->rows; r < expandedR; r++)
        newPtrs[r] = NULL;

    //سطرهای جدید
    for(size_t r = s->rows; r < expandedR; r++){
        newPtrs[r] = (Cell *)malloc(s->cols * sizeof(Cell));
        if(!newPtrs[r]){
            for(size_t k = s->rows; k < r; k++){
                for(size_t c = 0; c < s->cols; c++)
                    cellFree(&newPtrs[k][c]);
                free(newPtrs[k]);
            }
            free(newPtrs);
            return 0;
        }
        for(size_t c = 0; c < s->cols; c++)
        newPtrs[r][c] = cellMake((int)r, (int)c);
    }
    free(s->cells);
    s->cells=newPtrs;
    s->rows=expandedR;
    return 1;
}

//دسترسی به ستونی بزرگ تر از تعداد ستون های فعلی جدول
static int colsExpand(sheet *s, size_t expandedC){
    if (!s || !s->cells)
    return 0;
    if (expandedC <= s->cols)//ستون های فعلی کافیست
    return 1;//افزایش لازم نیست

    size_t oldC = s->cols;//تعداد ستون های قبلی را نگهداری میکند

    /* allocate new rows arrays first; do NOT touch old on failure */
    Cell **newRows = (Cell **)malloc(s->rows * sizeof(Cell *));
    if(!newRows)
    return 0;
    for(size_t r = 0; r < s->rows; r++)
        newRows[r] = NULL;
    for(size_t r = 0; r < s->rows; r++){
        newRows[r] = (Cell *)malloc(expandedC * sizeof(Cell));
        if (!newRows[r]){
            for (size_t k = 0; k < r; k++)
                free(newRows[k]);
            free(newRows);
            return 0;
        }

        /* copy existing cells (shallow copy ok: formula pointers move to new row) */
        for (size_t c = 0; c < oldC; c++)
            newRows[r][c] = s->cells[r][c];

        /* create new cells beyond oldC */
        for (size_t c = oldC; c < expandedC; c++)
            newRows[r][c]=cellMake((int)r, (int)c);
    }

    /* commit: free ONLY old row arrays (do not cellFree: formulas are now owned by newRows copies) */
    for (size_t r = 0; r < s->rows; r++)
        free(s->cells[r]);

    /* replace pointers to new rows */
    for (size_t r = 0; r < s->rows; r++)
        s->cells[r] = newRows[r];
    free(newRows);

    s->cols = expandedC;
    return 1;
}

//اگر سلول داخل محدوده جدول باشد ادرس ان را برمیگرداند
//در غیر این صورت NULL برمیگرداند
Cell *sheetFindCell(sheet *s, int row, int col){
    if (!s || !s->cells)
        return NULL;
    if (row < 0 || col < 0)
        return NULL;
    if ((size_t)row >= s->rows || (size_t)col >= s->cols)
        return NULL;
    return &s->cells[row][col];
}

//در این تابع اگر سلول خارج از حدوده باشد ابتدا جدول را بزرگ میکند
//سپس ادرس سلول را برمیگرداند
Cell *sheetExpandOrGet(sheet *s, int row, int col){
    if (!s || !s->cells)
        return NULL;
    if (row < 0 || col < 0)
        return NULL;
  //تعداد سطر و ستون لازم:
    size_t Rneeded = (size_t)row + 1;
    size_t Cneeded = (size_t)col + 1;

    if (Rneeded > s->rows)
        if (!rowExpand(s, Rneeded))
            return NULL;
    if (Cneeded > s->cols)
        if (!colsExpand(s, Cneeded))
            return NULL;

    numToAddress(row, col, s->cells[row][col].address);
    return &s->cells[row][col];
}

//این تابع سلول جدید را میسازد ئ مقدار دهی اولیه را انجام میدهد
Cell cellMake(int row, int col){
    Cell c;
    c.row=row;
    c.col=col;
    numToAddress(row, col, c.address);
    c.value=0.0;
    c.formula=NULL;
    c.err=ERR_OK;
    return c;
}

//این تابع حافظه ای را که قبلا برای رشته ی فرمول اشغال شده را آزاد میکند
//و اشارهگر ان را NULL میگذارد تا نشت حافظه رخ ندهد
void cellFree(Cell *c){
    if (!c)
        return;
    free(c->formula);
    c->formula = NULL;
}

//این تابع با کنترل خطا و بدون نشت حافظه فرمول سلول را تغییر میدهد
ErrorCode cellSetFormula(Cell *c, const char *formula){
    if (!c)
    return ERR_PARSE;//تابع اشتباه صدا زده شده

    free(c->formula);//ازاد کردن فرمول قبلی
    c->formula = NULL;

    if (!formula || formula[0] == '\0') // فرمول جدید خالی باشد و فرمول قبلی نیز حذف شده
    return ERR_OK;

    c->formula = duplicateStr(formula);//کپی کردن فرمول جدید 
    if (!c->formula)
    return ERR_NOMEM;

    return ERR_OK;
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

//این تابع خطاهای داخلی ماشین حساب رو به خطاهای استاندارد تابع ارورکد تبدیل میکند
static ErrorCode map_calc_error(erors e){
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
static ErrorCode evaluate_expression_to_value(sheet *sh, const char *expr, double *out_value)
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

static ErrorCode set_cell_from_input(sheet *sh, const char *target_addr, const char *input_line){
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
static void recalc_all(sheet *sh)
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

/* ===================== save/load ===================== */
void savesheet(sheet *s, const char name[])
{
    FILE *f = fopen(name, "wb");
    if (!f)
    {
        printf("Unable to open the file\n");
        return;
    }

    fprintf(f, "%zu %zu\n", s->rows, s->cols);

    for (size_t r = 0; r < s->rows; r++)
    {
        for (size_t c = 0; c < s->cols; c++)
        {
            Cell *cellp = &s->cells[r][c];
            if (cellp->formula && cellp->formula[0])
            {
                size_t len = strlen(cellp->formula);
                fprintf(f, "F %zu ", len);
                fwrite(cellp->formula, 1, len, f);
                fputc('\n', f);
            }
            else
            {
                fprintf(f, "V %.17g\n", cellp->value);
            }
        }
    }

    fclose(f);
    printf("Saved to %s\n", name);
}

static int read_line(FILE *f, char *buf, size_t cap)
{
    if (!fgets(buf, (int)cap, f))
        return 0;
    size_t n = strlen(buf);
    while (n && (buf[n - 1] == '\n' || buf[n - 1] == '\r'))
        buf[--n] = 0;
    return 1;
}

void loadsheet(sheet *s, const char name[])
{
    FILE *f = fopen(name, "rb");
    if (!f)
    {
        printf("Unable to open the file\n");
        return;
    }

    size_t rows = 0, cols = 0;
    if (fscanf(f, "%zu %zu", &rows, &cols) != 2 || rows == 0 || cols == 0)
    {
        fclose(f);
        printf("Bad file format\n");
        return;
    }
    int ch;
    while ((ch = fgetc(f)) != '\n' && ch != EOF)
    {
    }

    freesheet(s);
    if (!createsheet(s, rows, cols))
    {
        fclose(f);
        printf("Allocation failed\n");
        return;
    }

    char line[4096];
    for (size_t r = 0; r < rows; r++)
    {
        for (size_t c = 0; c < cols; c++)
        {
            if (!read_line(f, line, sizeof(line)))
            {
                fclose(f);
                printf("Unexpected EOF\n");
                return;
            }

            Cell *cellp = &s->cells[r][c];
            cellp->err = ERR_OK;

            if (line[0] == 'V' && line[1] == ' ')
            {
                double v = 0.0;
                sscanf(line + 2, "%lf", &v);
                free(cellp->formula);
                cellp->formula = NULL;
                cellp->value = v;
            }
            else if (line[0] == 'F' && line[1] == ' ')
            {
                const char *p = line + 2;
                (void)strtoull(p, (char **)&p, 10);
                while (*p == ' ')
                    p++;
                cellSetFormula(cellp, p);
            }
            else
            {
                cellp->err = ERR_PARSE;
            }
        }
    }
    fclose(f);

    recalc_all(s);
    printf("Loaded from %s\n", name);
}