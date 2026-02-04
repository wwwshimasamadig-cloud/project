#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "sheet.h"
#include "tokenizer.h"
#include "mathfuncs.h"
#include "eval.h"
#include "app.h"
#include "io.h"

//تابع main
int main()
{
     sheet sh;           //ساختار اصلی شیت
    int rows, cols;     //تعداد سطر و ستون ها
    int choice = 0;     // انتخاب کاربر در منو

    printf("Enter number of rows:");
    if (scanf("%d", &rows)!=1) //کاربر سطر را انتخاب میکند اگر انتخاب کند برنامه 1 میده و اگز کاربر انتخاب نکند از برنامه خارج می شود
        return 0;
    printf("Enter number of cols:");//کاربر ستون را انتخاب میکند و اگر انتخاب نکند از برنامه خارج می شود
    if (scanf("%d", &cols)!=1)    
        return 0;
    getchar(); //اگر کاربر عدد را با اینتر وارد کند اینتر در بافر ذخیره می شود و این خط از این کار جلوگیری می کند

    if (!createsheet(&sh, (size_t)rows, (size_t)cols)) //تابع شیت برای ساخت جدول استفاده می شود اگر جدول به درستی ساخته نشده باشد متن زیر و کامنت می کند و از برنامه خارج می شود
    {
        printf("Allocation failed.\n");
        return 0;
    }
    g_sheet_ctx = &sh;              //این متغیر باعث می شود که توابع دیگر به شیت فعلی دست یابند (شیت اصلی مه تازه ساخته شده است شیت فعلی است)


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
           
            char addr[32];  //آدرس سلول در این ذخیره می شود
            char input[512];//فرمول را ذخیره می کند


            printf("Enter cell address (e.g., A1): ");
            if (scanf("%31s", addr) != 1)
                break;
            getchar();

            printf("Enter value or formula (you may type 'B2 = sin(A1)+2'):\n> ");
            if (!fgets(input, sizeof(input), stdin)) //فاصله ها رو هم در نظر میگیره
                break;
            input[strcspn(input, "\r\n")] = 0; // /rبرگشت به ابتدای خط و /n خط بعد است اما وقتی با هم استفاده کنیم enter می شوداین تابع باعث می شود وقتی enter بزنیم دیگه از ما فرمول نخواد

            ErrorCode ec = set_cell_from_input(&sh, addr, input); //مشخص می کند که این تابع درست کار می کند یا نه اگر نه نوع ارور را بر می گرداند

            if (token_error) //0 خطا نداره 1 باشه خطا پیدا شده
            {
                printf("%s\n", token_error_msg); // چاپ پیام خطا برای کاربر
                token_error = 0;                
                token_error_msg[0] = '\0';      //خالی کردن رشته
                break;
            }

            
            if (ec == ERR_OK)                  //بدون خطا
            {
                int r, c;                       //سطر و ستون
                if (addressToNum(addr, &r, &c)) //این تابع آدرس متنی را به شماره سطر و ستون نبدیل می کند اگر معتبر بود خط زیر اجرا می شود 
                {
                    Cell *cellp = sheetFindCell(&sh, r, c); //اگر این سلول وجود داشت اشاره گر به آن را می گیرد
                    if (cellp)
                        printf("%s = %.10g\n", addr, cellp->value);//نمایش مقدار نهایی سلول ذخیره شده
                    else
                        printf("OK.\n"); //اگر آدرس پیدا نشدفقط موفقیت کلی میده
                }
                else
                {
                    printf("OK.\n"); 
                }
            }
            else
            {
                printf("Error:%s(cell NOT changed)\n",error_to_string(ec)); //پیام خطا رو میده و سلول به حالت قبل بر می گردد
            }
            break;
        }

         case 2: //فرمول رو ذخیره می کند ولی هیچ جا ذخیره نمی کند
        {
            char input[512]; //گرقتن فرمول
            printf("Enter formula to calculate: ");
            if (!fgets(input, sizeof(input), stdin)) 
                break;
            input[strcspn(input, "\r\n")] = 0; //با زدن enter دیگه فرمول وارد نمی شود 

            tokenize(input); //تبدیل رشته فرمول به توکن
            if (token_error) //خطا 
            {
                printf("%s\n", token_error_msg);
                token_error = 0;
                token_error_msg[0] = '\0';
                break;
            }

            if (!valid_parentheses()) //بررسی باز و بسته بودن پرانتز ها
            {
                printf("Error: Bad parentheses\n");
                break;
            }

            postfix pf = POSTFIX(tokens, token_count); //تبدیل فرمول به حالت پست فیکس(عملگر بعد از عملوند)
            const char *expr[token_max];
            for (int i = 0; i < pf.count; i++) //آماده سازی برای ورود به محاسبات
                expr[i] = pf.output[i].string_value;

            double result = NAN; //عدد نداره
            g_eval_err = ERR_OK;//اگر سلول ارجاع نامعتبر باشه یا خطای دیگر این متغیر تغییر می کنه
            erors e = calculate(expr, pf.count, &result);// اجرای محاسبه
            if (e == succetion) // اگر درست بود
                printf("Result = %.10g\n", result);
            else
            {
                ErrorCode ec = map_calc_error(e); //تبدیل خطای داخلی به خطای قابل فهم
                if (ec == ERR_PARSE && g_eval_err == ERR_BADREF)// اگر نتونه فرمول و اجرا کنه خطا میده
                    ec = ERR_BADREF;         //پیام دقیق خطا رو میده
                printf("Error: %s\n", error_to_string(ec));
            }
            break;
        }

        case 3:
        {
            char addr[32]; //ذخیره سلول
            int r, c;

            printf("Enter cell address: ");
            if (scanf("%31s", addr) != 1) //خواندن آدرس یک سلول
                break;
            getchar();

            if (!addressToNum(addr, &r, &c))//تبدیل آدرس به شماره سطر و ستون
            {
                printf("Invalid address\n");
                break;
            }

            Cell *cellp = sheetFindCell(&sh, r, c); //بررسی وجود داشتن سلول مشخص شده
            if (!cellp)
            {
                printf("Cell not found\n");
                break;
            }

            if (cellp->err != ERR_OK) //اگر سلول خطا بدهد پیام خطا میره و مقدار ذخیره شده قبلی و چاپ می کند اگر هم خطا نداشت مقدار همین سلول و چاپ می کند
            {
                printf("Cell %s ERROR: %s (code=%d)\n", addr, error_to_string(cellp->err), cellp->err);
                printf("Stored value (unchanged on error) = %.10g\n", cellp->value);
            }
            else
            {
                printf("Cell %s value = %.10g\n", addr, cellp->value);
            }

            if (cellp->formula) //چاپ کردن فرمول توی سلول
                printf("Formula: %s\n", cellp->formula);
            break;
        }

        case 4:
        {
            char filename[256]; //ذخیره فایل 
            printf("Enter filename to save: ");
            if (!fgets(filename, sizeof(filename), stdin))// وارد کردن اسم بافاصله هم مینواند وارد کند
                break;
            filename[strcspn(filename, "\r\n")] = 0;
            if (filename[0] == '\0')
                break;
            savesheet(&sh, filename); //ذخیره کردن
            break;
        }

        case 5:
        {//loadکرن
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
        {//تعداد سطر و ستون هایی که کاربر می خواهد اضافه شود
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

            if (addR < 0) //اگر عدد منفی وارد کند 0 در نظر میگیریم تا کوچک نشود
                addR = 0;
            if (addC < 0)
                addC = 0;

            size_t newR = sh.rows + (size_t)addR; //اندازه نهایی
            size_t newC = sh.cols + (size_t)addC;

            if (!rowExpand(&sh, newR)) // شیت و تا تعداد سطر جدید افرایش میدیم
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
            recalc_all(&sh);//بررسی تمام سلول های شیت
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