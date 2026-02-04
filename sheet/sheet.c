#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "sheet.h"

//پروتوتایپ ها
static void numToAddress (int row, int col, char out[16] );
static char *duplicateStr( const char *s);

static int rowExpand(sheet *s, size_t expandedR);
static int colsExpand(sheet *s,  size_t expandedC);

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