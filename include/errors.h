#ifndef ERRORS_H
#define ERRORS_H
//کد های خطا که در پروژه استفاده میشود:
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

#endif
