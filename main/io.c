#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "io.h"
#include "sheet.h"
#include "app.h"

static int read_line(FILE *f,char *buf,size_t cap)//  این تابع فقط داخل این فایل استفاده می شود
//bufآدرسی که خط در آن خوانده شده رو نگه میدارد
//size_t capحداکثر مقداری که می تواند بخواند
{
    if (!fgets(buf,(int)cap,f))//خواندن خط
        return 0; //اگز نتونه این و بخونه از فایل خارج میشه
    size_t n = strlen(buf); //طول رشته بوف و حساب می کند از size_t استفاده می کنیم تا فقط اعداد + و بگیره
    while (n && (buf[n-1] == '\n'||buf[n-1]=='\r'))//تا وقتی که رشته خالی نیست اجرا می شود اگر در خواندن خط به سر خط یا خط بعد برسیم حلقه تمام می شود
        buf[--n] = 0;
    return 1;
}
void savesheet(sheet *s, const char name[])
{FILE *f = fopen(name, "wb");// به دلیل اینکه در ورود فرمول ها از \n استفاده می کنیم و ممکن تغییر کنند از فایل باینری استفاده می کنیم تا تفییر و در نظر نگیرن 
    if (!f)//بررسی می کند فایل با موفقیت باز شده یانه
    {printf("Unable to open the file\n");
        return;
    }

    fprintf(f, "%zu %zu\n", s->rows, s->cols);//از zu استفاده می کنیم چون که size_tتعریف کردیم

    for (size_t r = 0; r < s->rows; r++)
    {for (size_t c = 0; c < s->cols; c++)
        {Cell *cellp = &s->cells[r][c];
            if (cellp->formula && cellp->formula[0])//بررسی خالی نیودن رشته
            {size_t len = strlen(cellp->formula);//محاسبه طول رشته
            fprintf(f, "F %zu ", len);//مناسب string
            fwrite(cellp->formula, 1, len, f);//مناسب فایل باینری و بدون تغییر ذخیره می کند
            fputc('\n', f);//یک پارامتر واحد را در فایل می نویسد1بایت
            }
            else
            {fprintf(f, "V %.17g\n", cellp->value);//برای نوشتن اعداد خیلی کوچک یا بزرگ ازgکه به صورت نماد علمی استفاده می کنیم
            }
        }
    }

    fclose(f);
    printf("Saved to %s\n", name);
}

void loadsheet(sheet *s, const char name[])
{FILE *f = fopen(name, "rb");
    if (!f)
    {printf("Unable to open the file\n");
        return;
    }

    size_t rows = 0, cols = 0;
    if (fscanf(f, "%zu %zu", &rows, &cols) != 2 || rows == 0 || cols == 0)
    { fclose(f);
    printf("Bad file format\n");
    return;
    }
    int ch;
    while ((ch = fgetc(f)) != '\n' && ch != EOF)//کارکتر به کارکتر میخواند و وقتی به \nیا پایان برسد دیگر حلقه نمی زند
    {
    }freesheet(s);//آزاد کردن حافظه
    if(!createsheet(s, rows, cols))//ساخت شیت جدید
    {
        fclose(f);
        printf("failed\n");
        return;
    }

    char line[4096];//برای نکهداری خط
    for (size_t r = 0; r < rows; r++)
    {for (size_t c = 0; c < cols; c++)
        {if (!read_line(f, line, sizeof(line)))
            {
            fclose(f);
            printf("EOF\n");
            return;
            }

            Cell *cellp = &s->cells[r][c];
            cellp->err = ERR_OK;

            if (line[0] == 'V' && line[1] == ' ')//در اینجا هر value باید باV ,  وارد کنیم 
            {double v = 0.0;
            sscanf(line + 2, "%lf", &v);
            free(cellp->formula);//خالی کردن حافظه
            cellp->formula = NULL;//از بین بردن آدرس
            cellp->value = v;//نگهداری از سلول
            }
            else if (line[0] == 'F' && line[1] == ' ')
            {const char *p = line + 2;
            (void)strtoull(p, (char **)&p, 10);//تبدیل رشته به عدد صحیح
            while (*p == ' ')//از بین بردن فاصله ها
                 p++;
            cellSetFormula(cellp, p);
            }
            else
            {
            cellp->err = ERR_PARSE;
            }
        }
    }
    fclose(f);

    recalc_all(s);
    printf("Loaded from %s\n", name);
}