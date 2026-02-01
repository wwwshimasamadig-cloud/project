#include "cell.h"
#include <string.h>
#include <stdlib.h>

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