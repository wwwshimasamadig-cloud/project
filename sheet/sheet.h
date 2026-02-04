#ifndef SHEET_H
#define SHEET_H

#include <stddef.h>

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

//تبدیل یک ادرس به شماره سطر و ستون 
int addressToNum(const char *address, int *resultRow, int *resultCol);

//این تابع سلول جدید را میسازد ئ مقدار دهی اولیه را انجام میدهد
Cell cellMake (int row, int col);

//این تابع حافظه ای را که قبلا برای رشته ی فرمول اشغال شده را آزاد میکند
//و اشارهگر ان را NULL میگذارد تا نشت حافظه رخ ندهد
void cellFree(Cell *c );

//این تابع با کنترل خطا و بدون نشت حافظه فرمول سلول را تغییر میدهد
ErrorCode cellSetFormula(Cell *c, const char *formula);

//ساخت جدول با اندازه های اولیه سطرو ستون
int createsheet(sheet *s, size_t rows, size_t cols);

//حافظه ای که برای شیت گرفته شده راازاد میکند تا نشت حافظه رخ ندهد
//ازاد سازی در سه مرحله ی سلول، سطر و ارایه سطر ها انجام یدهد
void freesheet (sheet *s);

//اگر سلول داخل محدوده جدول باشد ادرس ان را برمیگرداند
//در غیر این صورت NULL برمیگرداند
Cell *sheetFindCell(sheet *s, int row, int col);

//در این تابع اگر سلول خارج از حدوده باشد ابتدا جدول را بزرگ میکند
//سپس ادرس سلول را برمیگرداند
Cell *sheetExpandOrGet(sheet *s, int row , int col);

#endif