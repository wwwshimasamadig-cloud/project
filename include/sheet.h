#ifndef SHEET_H
#define SHEET_H

#include <stddef.h>
#include "cell.h"

typedef struct{
    size_t rows;//تعداد
    size_t cols;
    Cell  **cells;//یک ارایه از اشاره گر به سطرها
    // هر سطر خود یک ارایه از cell
}sheet;

//اعلان توابع:
int createsheet (sheet *s, size_t rows, size_t cols );
//جدول را با اندازه های اولیه میسازد
void freesheet (sheet *s);
//آزاد کردن حافظه
int addressToNum(const char *address, int *resultRow, int *resultCol);
//این تابع ادرس سلول را تبدیل به اعداد سطر و ستون میکند
Cell* sheetFindCell(sheet *s, int row, int col );
//ادرس سلول را اگر داخل محدوده باشد میگیرد
Cell* sheetExpandOrGet (sheet *s, int row , int col);
//ادرس سلول اگر خارج از محدوده باشد جدول را بزرگ میکند

#endif