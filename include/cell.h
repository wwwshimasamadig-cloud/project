#ifndef CELL_H
#define CELL_H

#include "errors.h" // که بتوان از انواع کدهای خطا استفاده کرد
//تعریف ساختار هر سلول:
typedef struct{
    int row; //شماره سطر سلول
    int col; //شماره ستون
    double value; //مقدار عددی نهایی سلول
    char *formula; //اگر سلول فرمول داشته باشد محل ذخیره آن اینجاست
    ErrorCode err;//مشخص میکند سلول خطا دارد یا خیر و نوع خطا
}Cell;
//اعلام توابع:

Cell cellMake(int row, int col);
//این تابع یک سلول جدید با سطر و ستون مشخص میسازد
// در این تابع value = 0.0
//formula = NULL
//err = ERR_OK

void cellFree(Cell *c);
//حافظه سلول را آزاد میکند

ErrorCode cellSetFormula (Cell *c, const char *formula);
//فرمول را برای سلول ثبت میکند

#endif