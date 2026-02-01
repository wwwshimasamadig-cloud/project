#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>



typedef enum { NUMBER,TEXT}CellType;
typedef struct{
    CellType type;
    union{
        double value;
        char text[1000];
    };
}Cell;


typedef struct
{
    int rows;
    int cols;
    Cell cells [100][100];
}Sheet;

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



// توابع

void inisheet(Sheet *s,int r,int c);
void savesheet(Sheet *s,const char name[]);
void loadsheet(Sheet *s,const char name[]);
void Formula(Sheet *s,const char formula[]);
void inisheet(Sheet *s,int r,int c);
void tokenize(const char *expr);
int parentheses();



int main()
{   Sheet *s;
    int choice;
    char formula[200];
    int r,c;
    scanf("%d %d ",&r,&c);
    getchar();
    
    
    
    
    
    
    s=(Sheet*)malloc(sizeof(Sheet));
    inisheet(s,r,c);
    
    while(1)
    {
        scanf("%d",&choice);
        getchar();

    
    switch (choice)
    {
    case 1:
    printf ("enter formula:");
    fgets(formula,200,stdin);
    //فاصله رو بگیره
    Formula(s,formula);
    tokenize(formula);
    if(parentheses())
        
            printf("balanced");
    else
    
            printf("error");
    
        
    
    break;

    //
    case 2:
    loadsheet(s,"data.txt");
    break;
    case 3:
    savesheet(s,"data.txt");
    break;
    case 4:
    return 0;
    default:
    printf("Wrong\n");


    }

    }
    free(s);
    return 0;
}
void inisheet(Sheet *s,int r,int c)
{
    s->rows = r;
    s->cols = c;
    for(int i=0;i<r;i++)
    {
        for(int j=0;j<c;j++)
        {
            s->cells[i][j].type=NUMBER;
            s->cells[i][j].value=0;
            s->cells[i][j].text[0]="\0";
        }
    }  
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

#define token_max 256
token tokens[token_max];
int token_count = 0;


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
