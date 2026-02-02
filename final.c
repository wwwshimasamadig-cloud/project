#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<math.h>



typedef enum { NUMBER,TEXT}CellType;
typedef struct{
    CellType type;
    union{
        double value;
        char text[1000];
    };
}ccell;


typedef struct
{
    int rows;
    int cols;
    ccell cells [100][100];
}Sheet;

typedef enum
{
    number,
    operator,
    cell,
    paren_open,
    paren_close,
    function,
    invalid
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

typedef struct {
    token output[token_max];
    int count;
}postfix;

typedef struct{
    double (*fun1)(double);
    const char *name1;
} funmath;
// ساختار حافظه
#define max 100
typedef struct{
    int most;
    double data[max];}
memorystack;
// خطاها
typedef enum{
    devision_peroblem,
    domain_peroblem,
     function_peroblem,
    input_peroblem,
    succetion,
    action_peroblem,
    phraise_peroblem,
    allocation_peroblem,
}   erors;
typedef enum{
    ERR_OK= 0, //خطا ندارد
    ERR_DIV0,//خطای تقسیم بر صفر دارد
    ERR_BADREF, //خطای ارجاع نامعتبر
    ERR_BADPAREN,//خطای پرانتزگذاری نامعتبر یا مطابق نباشد
    ERR_UNKNOWN_F,//خطای تابع ناشناخته
    ERR_DOMAIN,//خطای دامنه
    ERR_PARSE,//خطای مشکل ساختاری یا نوشتاری
    ERR_NOMEM,//کمبود حافظه یا خطای تخصیص حافظه
}ErrorCode;

typedef struct{
    int row; //شماره سطر سلول
    int col; //شماره ستون
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
erors calculate(const char **input,int countinput,double *output);
// توابع ریاضی
double truth_log(double n);
double truth_ln(double n); 
double truth_sqrt(double n);
double truth_abs(double n); 
double truth_cos(double n);
double truth_sin(double n);
double truth_tan(double n);
double truth_cot(double n);
double truth_exp(double n);
double truth_cosh(double n);
double truth_sinh(double n);
double truth_tanh(double n);

// توابع


void savesheet(Sheet *s,const char name[]);
void loadsheet(Sheet *s,const char name[]);
void Formula(Sheet *s,const char formula[]);
void inisheet(Sheet *s,int r,int c);
void tokenize(const char *expr);
int valid_parentheses();
postfix POSTFIX(token *tokens, int token_count);
erors push(memorystack *sone, double value);
void inistialzationstack(memorystack *sone);
erors pop(memorystack *sone, double *value);
double safe_ln(double n);
double truth_sqrt(double n);
double truth_cot(double x);
funmath* findfun(const char *name1);
erors calculate(const char **input,int countinput,double *output);
Cell cellMake(int row, int col);
void cellFree(Cell *c);
ErrorCode cellSetFormula (Cell *c, const char *formula);
int createsheet (sheet *s, size_t rows, size_t cols );
void freesheet (sheet *s);
int addressToNum(const char *address, int *resultRow, int *resultCol);
Cell* sheetFindCell(sheet *s, int row, int col );
Cell* sheetExpandOrGet (sheet *s, int row , int col);
//========================
int main() {
    sheet sh;
    int rows, cols;
    int choice = 0;
    printf("Enter number of rows: ");
    scanf("%d",&rows);
    printf("Enter number of cols: ");
    scanf("%d",&cols);
    getchar(); // consume newline

    createsheet(&sh, rows, cols);

    while(1) {
        printf("\nMenu:\n1-Set cell value/formula\n2-Calculate formula\n3-Show cell value\n4-Exit\nChoice: ");
        scanf("%d",&choice);
        getchar(); // consume newline

        if(choice==4) break;

        switch(choice) {
            case 1: {
                char addr[10];
                char formula[200];
                int r,c;
                printf("Enter cell address: ");
                scanf("%s", addr);
                getchar();
                if(!addressToNum(addr,&r,&c)) { printf("Invalid address\n"); break; }
                Cell *cell = sheetExpandOrGet(&sh,r,c);
                printf("Enter value or formula: ");
                fgets(formula, sizeof(formula), stdin);
                formula[strcspn(formula, "\n")] = 0;
                cellSetFormula(cell,formula);
                printf("Cell %s set successfully\n",addr);
                break;
            }
            case 2: {
                char input[200];
                printf("Enter formula to calculate: ");
                fgets(input, sizeof(input), stdin);
                input[strcspn(input,"\n")]=0;
                tokenize(input);
                postfix pf=POSTFIX(tokens,token_count);
                const char *expr[token_max];
                for(int i=0;i<pf.count;i++) expr[i]=pf.output[i].string_value;
                double result;
                erors e=calculate(expr,pf.count,&result);
                if(e==succetion) printf("Result = %lf\n", result);
                else printf("Error in calculation\n");
                break;
            }
            case 3: {
                char addr[10];
                int r,c;
                printf("Enter cell address: ");
                scanf("%s", addr);
                getchar();
                if(!addressToNum(addr,&r,&c)) { printf("Invalid address\n"); break; }
                Cell *cell=sheetFindCell(&sh,r,c);
                if(cell) printf("Cell %s value = %lf\n", addr, cell->value);
                else printf("Cell not found\n");
                break;
            }
            default: printf("Invalid choice\n"); break;
        }
    }

    freesheet(&sh);
    printf("Program ended.\n");
    return 0;
}



        




void savesheet(Sheet *s,const char name[])
{
    FILE *f;
    f = fopen(name ,"w");
    if(f==NULL)
    {
        printf("Unable to open the file \n");
        return;
    }
        fprintf(f,"%d %d\n",s->rows,s->cols );
        
        for(int i=0;i<s->rows;i++)
         {
            for(int j=0;j<s->cols;j++)
             {if(s->cells[i][j].type==NUMBER)
                fprintf(f,"%lf",s->cells[i][j].value);
                else
                {
                    fprintf(f,"%s",s->cells[i][j].text);
                }

             }
             fprintf(f,"\n");
         }
    fclose(f);
}
void loadsheet(Sheet *s,const char name[])
{
    FILE *f;
    f=fopen(name,"r");
    if(f==NULL)
    {
        printf("Unable to open the file \n");
        return;
    }
    fscanf(f,"%d %d",&s->rows,&s->cols);
    for(int i=0;i<s->rows;i++)
        {
            for(int j=0;j<s->cols;j++)
            {
               if(s->cells[i][j].type==NUMBER)
                fscanf(f,"%lf",&s->cells[i][j].value);
                else
                {
                    fscanf(f,"%s",s->cells[i][j].text);
                }
            }
        }

        fclose(f);
}
void Formula(Sheet *s,const char formula[])
{
    printf("Formula: %s",formula);
}

token tokens[token_max];
int token_count;

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


double truth_sin(double n){
    return sin(n);}
double truth_cos(double n){
    return cos(n);}
double truth_tan(double n)
    {return tan(n);}
double truth_exp(double n){
    return exp(n);}
double truth_abs(double n){ 
    return fabs(n);}
 double truth_sinh(double n){
    return sinh(n);}
double truth_cosh(double n) {
    return cosh(n);}
double truth_tanh(double n){
    return tanh(n);}
double truth_ln(double n)
{
    if(n<=0)
    return NAN;
    return log(n);
}


erors push(memorystack *sone, double value){
    if (sone->most >= max-1) 
    {return action_peroblem;
    }
    sone->data[++(sone->most)] =value;
    return succetion;
}
void inistialzationstack(memorystack *sone){
    sone->most=-1;
}
erors pop(memorystack *sone, double *value){
    if (sone->most<0)
    {return action_peroblem;
    }
    *value = sone->data[(sone->most)--];
    return succetion;}
 double truth_log(double n){
    if (n<=0)
    {return NAN; 
    }
    return log10(n);
}
double safe_ln(double n){
    if (n<=0)
    {return NAN;}
    return log(n);}

  double truth_sqrt(double n){
    if(n<0)
    {return NAN;
    }
    return sqrt(n);
}  
 double truth_cot(double x){
    double tan_val=tan(x);
    if (fabs(tan_val)<1e-15)
    {return NAN;
    }
    return 1.0/tan_val;
} 
funmath math_functions[]={
    {"sin", truth_sin},
    {"cos", truth_cos},
{"tan", truth_tan},
    {"cot", truth_cot},
    {"sqrt", truth_sqrt},
    {"log", truth_log},
        {"ln", truth_ln},
    {"exp", truth_exp},
    {"abs", truth_abs},
{"sinh", truth_sinh},
    {"cosh", truth_cosh},
    {"tanh", truth_tanh},
    {NULL,NULL}
    };
funmath* findfun(const char *name1){
    for (int i=0; math_functions[i].name1 !=NULL;++i)
        {if (strcmp(name1,math_functions[i].name1) == 0) 
            {return &math_functions[i];}}
    return NULL; }
    erors calculate(const char **input,int countinput,double *output){
    memorystack stack;
    inistialzationstack(&stack);
    erors status;
    double action1,action2,value;
    for (int i=0; i<countinput; ++i)
        {const char *token_str=input[i];
        char *endptr;
        value=strtod(token_str, &endptr);
        if (*endptr == '\0' && token_str!= endptr)
            {if (push(&stack, value) != succetion) 
                return action_peroblem;}
        else{
            if (strcmp(token_str, "+") == 0){
                if (pop(&stack, &action2)!=succetion) 
                    return action_peroblem;
                    if (pop(&stack, &action1)!= succetion)
                    return action_peroblem;
                if (push(&stack, action1 + action2)!=succetion)
                    return action_peroblem;
            } else if(strcmp(token_str, "-")==0){
                if (pop(&stack, &action2)!=succetion)
                    return action_peroblem;
                    if (pop(&stack, &action1)!=succetion)
                    return action_peroblem;
                if (push(&stack, action1 -action2)!=succetion)
                    return action_peroblem;
            } else if (strcmp(token_str, "*") == 0){
                if (pop(&stack, &action2)!=succetion)
                    return action_peroblem;
                    if (pop(&stack, &action1)  !=succetion)
                    return action_peroblem;
                if(push(&stack, action1 * action2)!=succetion)
                    return action_peroblem;}
            else if (strcmp(token_str, "/") == 0){
                if (pop(&stack, &action2) != succetion)
                    return action_peroblem;
                if (pop(&stack, &action1) != succetion)
                    return action_peroblem;
                    if (fabs(action2) < 1e-15)
                    {return devision_peroblem;
                }
                if (push(&stack,action1/action2)!=succetion) return action_peroblem;
            } else
                {funmath *mf = findfun(token_str);
                if (mf != NULL)
                    {if (pop(&stack,&action1)!=succetion) 
                        return action_peroblem;
                    double fun_result = mf->fun1(action1);
                    if (isnan(fun_result)) {
                        {return domain_peroblem;
                    }
                    if (push(&stack,fun_result) !=succetion) return action_peroblem;
                } else {
                    return input_peroblem;
                }}}}
    if (stack.most!= 0)
        {return action_peroblem;
    }
    if (pop(&stack, output)!=succetion)
        {return phraise_peroblem;}
     if ( isnan(*output))
        {return domain_peroblem ;}
   
    return succetion;}}
    //برای دسترسی به سلولی در سطری بزرگتر از اندازه کنونی جدول
static int rowExpand(sheet *s, size_t expandedR ){
    if(!s )
    return 0;
    if (expandedR <= s -> rows)
    return 1; //نیازی به افزایش نیست

    Cell **newPtr=(Cell** )realloc (s-> cells, expandedR * sizeof(Cell* ));
    if(!newPtr ){
    return 0;}
    s->cells =newPtr;//بعد از افزایش آرایه اصلی باید از ادرس جدید استفاده کند
    //ایجاد سطرهای جدید
    for ( size_t r=s -> rows; r <expandedR; r++ ){
        //برای سطرهای جدید به تعداد ستون ها سلول میسازیم
        s -> cells[r]= (Cell* )malloc( s-> cols * sizeof(Cell));
        if(!s -> cells[r]) {
            for ( size_t k =s-> rows; k<r; k++){
                free(s->cells[k]);//جلوگیری از نشت حافظه
                s->cells[k]=NULL;
            }
            return 0;
        }
        //مقداردهی پیش فرض برای سلول های جدید
        for (size_t c=0; c<s -> cols; c++){
            s ->cells[r][c]= cellMake( (int)r,(int)c);}
    }
    s ->rows =expandedR; //تعداد جدید سطر ها
    return 1;
}

//دسترسی به ستونی بزرگ تر از تعداد ستون های فعلی جدول
static int colsExpand(sheet *s , size_t expandedC) {
    if(!s ) return 0;
    if (expandedC <=s -> cols)//ستون های فعلی کافیست
    return 1;//افزایش لازم نیست
    size_t beforexpandC=s ->cols;//تعداد ستون های قبلی را نگهداری میکند
    //برای هر سطر تعداد ستون های ان را افزایش میدهیم
    for (size_t r= 0; r< s ->rows; r++){
        //افزایش  سلول های جدید به هرسطر به دلیل افزایش ستون
        Cell *expandedRow=(Cell* )realloc( s-> cells[r] , expandedC * sizeof(Cell));
        if(!expandedRow){
            //در صورت عدم موفقیت در افزایش سطر را دوباره کوچک میکنیم
            for (size_t k=0; k<r; k++){
                Cell *back =(Cell* )realloc( s->cells[k], beforexpandC * sizeof(Cell));
                if(back )
                s->cells[k]= back;
            }
            return 0;//عدم افزایش
        }
        s -> cells[r]=expandedRow;//پوینتر سطر را به ادرس جدید تعییر میدهیم
        //سلول های جدید را با مقادیر پیش فرض مقداردهی میکنیم
        for(size_t c=beforexpandC; c< expandedC; c++){
            s -> cells[r][c]=cellMake((int)r , (int)c);}
    }
    s->cols = expandedC; //تعداد ستون های جدول را به مقدار جدید اپدیت میکینیم
    return 1;
}

//ساخت جدول با اندازه های اولیه سطر و ستون
int createsheet(sheet *s, size_t rows , size_t cols) {
    if (!s || rows==0 || cols==0)
    return 0;
    s ->rows = rows;
    s ->cols = cols;
    //ارایه ای مسازیم که در ان به تعداد سطر ها اشاره گر به سطر قرار میگیرد
    s ->cells = (Cell** )malloc( rows * sizeof(Cell*)) ;
    if ( !s-> cells){
        s -> rows=0;
        s -> cols=0;
        return 0;
    }
    //برای هر سطر ارایه ای سلول ها میسازیم
    for(size_t r=0; r<rows; r++ ){
        s ->cells[r] =(Cell* )malloc (cols * sizeof(Cell));
        if(!s -> cells[r]){
            for (size_t k = 0; k < r; k++) free(s->cells[k]);
            free(s -> cells );
            s->cells =NULL;
            s->rows =0;
            s->cols= 0;
            return 0;
        }
        for (size_t c=0; c<cols; c++) {
            //مقدار دهی اولیه
            s -> cells[r][c]=cellMake ((int)r, (int)c);
        }
    }return 1; 
}


void freesheet(sheet *s){
    if (!s || !s->cells)
    return;
    for(size_t r=0; r<s -> rows; r++) {
        if ( !s -> cells[r])
        continue;
        for  (size_t c= 0; c<s ->cols; c++){
            cellFree (&s ->cells[r][c]) ;
        }
        free(s->cells[r]);
        s-> cells[r]=NULL;
    }
    free(s->cells);
    s ->cells = NULL;
    s ->rows = 0;
    s ->cols = 0;
}

//تبدیل یک ادرس به شماره سطر و ستون 
int addressToNum(const char *address, int *resultRow, int *resultCol){
    if (!address || !resultRow || !resultCol)
    return 0;

    size_t i=0;
    if(!isalpha( (unsigned char)address[i]))
    return 0;//اگر اولین کاراکتر ادرس حرف نبود نامعتبر است

    int col= 0;
    while(isalpha((unsigned char)address[i])) {//تا وقتی حرف میبینی ادامه بده
        char ch=(char)toupper((unsigned char)address[i]);
        //فرقی بین حروف بزرگ و کوچک نباشد
        if (ch<'A' || ch> 'Z') //نامعتبر
        return 0;
        col = col * 26 +(ch- 'A' +1);//تبدیل حرف به عدد
        i++;
    }
    if (col<=0)//نامعتبر
    return 0;
    col -= 1;//شمارش ستون ها از صفر شروع میشود

    if(!isdigit((unsigned char)address[i]))
    return 0;//اگر بعد از حروف عدد نیامد ممعتبر نیست

    int row =0;
    while(isdigit((unsigned char)address[i])){//تا رقم میبینی ادامه بده
        row=row * 10+ (address[i]-'0');//تبدیل رقم به عدد
        i++;
    }

    if (address[i] !='\0')
    return 0;//اگر بعد از عددهاکاراکتر باشد معتبر نیست
    if(row<=0) 
    return 0;
    row -= 1;//چون اولین سطر، سطرصفر است

    *resultRow=row;
    *resultCol=col;
    return 1;
}

//اگر سلول داخل محدوده جدول باشد ادرس ان را برمیگرداند
//در غیر این صورت NULL برمیگرداند
Cell* sheetFindCell(sheet *s, int row, int col){
    if(!s || !s -> cells)
    return NULL;
    if (row<0 || col<0)
    return NULL;
    if((size_t)row >= s->rows || (size_t)col >= s->cols)
    return NULL;
    return &s -> cells[row][col];
}

//در این تابع اگر سلول خارج از حدوده باشد ابتدا جدول را بزرگ میکند
//سپس ادرس سلول را برمیگرداند
Cell* sheetExpandOrGet(sheet *s, int row, int col){
    if (!s || !s->cells)
    return NULL;
    if(row<0 || col<0) 
    return NULL;
    //تعداد سطر و ستون لازم:
    size_t Rneeded= (size_t) row+1;
    size_t Cneeded= (size_t) col+1;

    if (Rneeded>s -> rows) {//اگر تعداد سطرهای فعلی ناکافی
        if(!rowExpand(s , Rneeded))//افزایش سطر
        return NULL;
    }
    if(Cneeded > s -> cols) {//اگر تعداد ستون فعلی ناکافی
        if(!colsExpand(s, Cneeded))//افزایش ستون
        return NULL;
    }
    return &s->cells[row][col];
}
//این تابع یک کپی جدید از رشته ورودی میگیرد و ان را برمیگرداند
static char* duplicateStr(const char *s){
    if (!s ) 
    return NULL;
    size_t n = strlen(s);
    char *p = (char *) malloc(n+1);
    if (!p) return NULL;
    memcpy(p, s, n + 1);
    return p;
}
//این تابع سلول جدید را میسازد ئ مقدار دهی اولیه را انجام میدهد
Cell cellMake (int row, int col) {
    Cell c;
    c.row=row;
    c.col=col;
    c.value=0.0;
    c.formula=NULL;
    c.err=ERR_OK;
    return c;
}
//این تابع حافظه ای را که قبلا برای رشته ی فرمول اشغال شده را آزاد میکند
//و اشارهگر ان را NULL میگذارد تا نشت حافظه رخ ندهد
void cellFree(Cell *c) {
    if(!c) 
    return;
    free (c->formula);
    c->formula= NULL;
}
//این تابع با کنترل خطا و بدون نشت حافظه فرمول سلول را تغییر میدهد
ErrorCode cellSetFormula(Cell *c, const char *formula){
    if(!c) return ERR_PARSE; //تابع اشتباه صدا زده شده

    free(c->formula); //ازاد کردن فرمول قبلی
    c->formula = NULL;

    if(!formula || formula[0]=='\0')// فرمول جدید خالی باشد و فرمول قبلی نیز حذف شده
    return ERR_OK;

    c->formula = duplicateStr(formula); //کپی کردن فرمول جدید 
    if (!c->formula)
    return ERR_NOMEM; //اگر حافظه کم باشد ارور میدهد

    return ERR_OK;
}