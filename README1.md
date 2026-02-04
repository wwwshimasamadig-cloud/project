#Yaldaeai-Spreadsheet Formula Engine (C)
##هدف پروژه
هدف از این پروژه پیاده سازی یک سیستم ساده مشابه نرم افزار  Excelبا استفاده از زبان C است برنامه یلدایی امکان تعریف سلول ها،پردازش فرمول های ریاضی،ارجاع بین سلول ها و مدیریت خطا را فراهم می کند.
این پروژه در چارچوب درس مبانی کامپیوتر و برنامه نویسی دانشگاه صنعتی خواجه نصیرالدین طوسی انجام شده است
https://github.com/wwwshimasamadig-cloud/project                                                                                                                              : لینک پروژه در گیت هاب
---
#قابلیت ها 
-پردازش فرمول های در سلول ها
-پشتیبانی از عملگر های پایه:
-'+'،'-'،'*'،'/'،'()'
-'sin'،'cos'،'tan'،'cot'
-'sinh'،'cosh'،'tanh'
-'sqrt'،'pow'،'exp'
-'ln'،'log'
-'abs'
-امکان ارجاع سلول ها به هم
-مثال A2=sin(A1)
-مقداردهی اولیه سلول های خالی برابر با صفر 
-مدیریت خطا بدون متوقف شدن برنامه
-قابلیت ذخیره و بارگذاری اطلاعات در فایل 
-قابلیت توسعه جدول(افزایش سطر و ستون )
---
##معماری کلی پروژه 
برای پیاده سازی ساخت یافته و قابل توسعه ،پروژه «یلدایی» به بخش های زیر تقسیم شده است:
1###.فایل 'main.c'
-نقطه شروع اجرا برنامه
-دریافت ورودی از کاربر
-مدیریت کلی اجرای برنامه
2###.بخش Formula Engine
-تجزیه عبارات ریاضی (Parsing)
-شناسایی عملگرها و توابع 
-جایگزینی مقادیر سلول ها
-انجام محاسبات نهایی با استفاده از کتابخانه 'math.h'
3###.بخش Save and Load
-ذخیره اطلاعات جدول در فایل 
-بارگذاری اطلاعات از فایل 
-استفاده از توابع ورودی/خروجی زبان C
---
##ساختار داده ها
###ساختار Cell
هر سلول شامل اطلاعات زیر می باشد:
-آدرس سلول (مانند A1،B3)
-مقدار عددی نهایی از نوع double
-فرمول در صورت وجود
-وضعیت خطا
###ساختار Sheet
-جدول به صورت آرایه دوبعدی از سلول ها پیاده سازی شده است
-مقدار اولیه تمام سلول ها برابر با صفر در نظر گرفته شده است
---
##نحوه پردازش فرمول
برای مثال در عبارت زیر :
A3=tan(A5+A1)*ln(sinh(A2))
مراحل پردازش به صورت زیر است:
1.تشخیص سلول مقصد ('A3')
2.تجزیه عبارت ریاضی
3.جایگزینی مقادیر سلول های ارجاع داده شده
4.شناسایی توابع ریاضی
5.محاسبه مقدار نهایی
6.ذخیره نتیجه در سلول مقصد
---
##مدیریت خطاها
خطاهای پشتیبانی شده:
-تقسیم بر صفر
-استفاده از تابع ناشناخته
-ارجاع به سلول نامعتبر
-پرانتز گذاری نادرست
-خطای دامنه توابع ریاضی
در صورت بروز خطا:
-برنامه متوقف نمی شود
-پیام خطای مناسب نمایش داده میشود
-مقدار قبلی سلول بدون تغییر باقی میماند
---
#نحوه اجرا
##کامپایل برنامه :
ورودی:
A1=30
A2=10
A3=sin(A1)+sqrt(A2)
خروجی:
A3=4.15
#نمونه ورودی و خروجی 
Enter number of rows: 2
Enter number of cols: 2

Menu:
1-Set cell value/formula
2-Calculate formula (no store)
3-Show cell value
4-Save sheet
5-Load sheet
6-Extend sheet (add rows/cols)
7-Recalculate all formulas
8-Exit
Choice: 1
Enter cell address (e.g., A1): A1
Enter value or formula (you may type 'B2 = sin(A1)+2'):
> 5
OK. A1 = 5

Menu:
1-Set cell value/formula
2-Calculate formula (no store)
3-Show cell value
4-Save sheet
5-Load sheet
6-Extend sheet (add rows/cols)
7-Recalculate all formulas
8-Exit
Choice: 1
Enter cell address (e.g., A1): B1
Enter value or formula (you may type 'B2 = sin(A1)+2'):
> =A1 * 2 + 3
OK. B1 = 13

Menu:
1-Set cell value/formula
2-Calculate formula (no store)
3-Show cell value
4-Save sheet
5-Load sheet
6-Extend sheet (add rows/cols)
7-Recalculate all formulas
8-Exit
Choice: 2
Enter formula to calculate: sqrt(B1) - A1
Result = -1.394448725

Menu:
1-Set cell value/formula
2-Calculate formula (no store)
3-Show cell value
4-Save sheet
5-Load sheet
6-Extend sheet (add rows/cols)
7-Recalculate all formulas
8-Exit
Choice: 3
Enter cell address: B1
Cell B1 value = 13
Formula: =A1 * 2 + 3

Menu:
1-Set cell value/formula
2-Calculate formula (no store)
3-Show cell value
4-Save sheet
5-Load sheet
6-Extend sheet (add rows/cols)
7-Recalculate all formulas
8-Exit
Choice: 8
Program ended.
#ساختار فایل
این پروژه به صورت تک فایلی و در قالب یک فایل 'main.c'پباده سازی شده است
به منظور حفظ سادگی و تمرکز بر مفاهیم پایه C،تمام بخش های برنامه به صورت تابع بندی شده در یک فایل قرار گرفته اند.
ساختار منطقی فایل 'main.c'به شرح زیر است:
-معرفی پروژه و توضیحات اولیه
-کتابخانه های مورد استفاده
-تعریف ثابت ها و محدودیت ها
-تعریف ساختار های داده (Sheet وCell)
-اعلان توابع
-تابع'main'
-توابع مدیریت جدول و سلول ها
-توابع پردازش فرمول های ریاضی
-توابع ذخیره و بارگذاری
-توابع مدیریت و کنترل خطا 

