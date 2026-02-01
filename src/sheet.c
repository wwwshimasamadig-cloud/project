#include <stdlib.h>
#include <ctype.h>
#include "sheet.h"

//برای دسترسی به سلولی در سطری بزرگتر از اندازه کنونی جدول
static int rowExpand(sheet *s, size_t expandedR ){
    if(!s )
    return 0;
    if (expandedR <= s -> rows)
    return 1; //نیازی به افزایش نیست

    Cell **newPtr=(Cell** )realloc (s-> cells, expandedR * sizeof(Cell* ));
    if(!newPtr ){
    return 0;}
    s->cells =newPtr;//بعد از افزایش آرایه اصلی باید از ادرس جدید استفاده کند
    //ایجاد سطرهای جدید
    for ( size_t r=s -> rows; r <expandedR; r++ ){
        //برای سطرهای جدید به تعداد ستون ها سلول میسازیم
        s -> cells[r]= (Cell* )malloc( s-> cols * sizeof(Cell));
        if(!s -> cells[r]) {
            for ( size_t k =s-> rows; k<r; k++){
                free(s->cells[k]);//جلوگیری از نشت حافظه
                s->cells[k]=NULL;
            }
            return 0;
        }
        //مقداردهی پیش فرض برای سلول های جدید
        for (size_t c=0; c<s -> cols; c++){
            s ->cells[r][c]= cellMake( (int)r,(int)c);}
    }
    s ->rows =expandedR; //تعداد جدید سطر ها
    return 1;
}

//دسترسی به ستونی بزرگ تر از تعداد ستون های فعلی جدول
static int colsExpand(sheet *s , size_t expandedC) {
    if(!s ) return 0;
    if (expandedC <=s -> cols)//ستون های فعلی کافیست
    return 1;//افزایش لازم نیست
    size_t beforexpandC=s ->cols;//تعداد ستون های قبلی را نگهداری میکند
    //برای هر سطر تعداد ستون های ان را افزایش میدهیم
    for (size_t r= 0; r< s ->rows; r++){
        //افزایش  سلول های جدید به هرسطر به دلیل افزایش ستون
        Cell *expandedRow=(Cell* )realloc( s-> cells[r] , expandedC * sizeof(Cell));
        if(!expandedRow){
            //در صورت عدم موفقیت در افزایش سطر را دوباره کوچک میکنیم
            for (size_t k=0; k<r; k++){
                Cell *back =(Cell* )realloc( s->cells[k], beforexpandC * sizeof(Cell));
                if(back )
                s->cells[k]= back;
            }
            return 0;//عدم افزایش
        }
        s -> cells[r]=expandedRow;//پوینتر سطر را به ادرس جدید تعییر میدهیم
        //سلول های جدید را با مقادیر پیش فرض مقداردهی میکنیم
        for(size_t c=beforexpandC; c< expandedC; c++){
            s -> cells[r][c]=cellMake((int)r , (int)c);}
    }
    s->cols = expandedC; //تعداد ستون های جدول را به مقدار جدید اپدیت میکینیم
    return 1;
}

//ساخت جدول با اندازه های اولیه سطر و ستون
int createsheet(sheet *s, size_t rows , size_t cols) {
    if (!s || rows==0 || cols==0)
    return 0;
    s ->rows = rows;
    s ->cols = cols;
    //ارایه ای مسازیم که در ان به تعداد سطر ها اشاره گر به سطر قرار میگیرد
    s ->cells = (Cell** )malloc( rows * sizeof(Cell*)) ;
    if ( !s-> cells){
        s -> rows=0;
        s -> cols=0;
        return 0;
    }
    //برای هر سطر ارایه ای سلول ها میسازیم
    for(size_t r=0; r<rows; r++ ){
        s ->cells[r] =(Cell* )malloc (cols * sizeof(Cell));
        if(!s -> cells[r]){
            for (size_t k = 0; k < r; k++) free(s->cells[k]);
            free(s -> cells );
            s->cells =NULL;
            s->rows =0;
            s->cols= 0;
            return 0;
        }
        for (size_t c=0; c<cols; c++) {
            //مقدار دهی اولیه
            s -> cells[r][c]=cellMake ((int)r, (int)c);
        }
    }return 1; 
}


void freesheet(sheet *s){
    if (!s || !s->cells)
    return;
    for(size_t r=0; r<s -> rows; r++) {
        if ( !s -> cells[r])
        continue;
        for  (size_t c= 0; c<s ->cols; c++){
            cellFree (&s ->cells[r][c]) ;
        }
        free(s->cells[r]);
        s-> cells[r]=NULL;
    }
    free(s->cells);
    s ->cells = NULL;
    s ->rows = 0;
    s ->cols = 0;
}

//تبدیل یک ادرس به شماره سطر و ستون 
int addressToNum(const char *address, int *resultRow, int *resultCol){
    if (!address || !resultRow || !resultCol)
    return 0;

    size_t i=0;
    if(!isalpha( (unsigned char)address[i]))
    return 0;//اگر اولین کاراکتر ادرس حرف نبود نامعتبر است

    int col= 0;
    while(isalpha((unsigned char)address[i])) {//تا وقتی حرف میبینی ادامه بده
        char ch=(char)toupper((unsigned char)address[i]);
        //فرقی بین حروف بزرگ و کوچک نباشد
        if (ch<'A' || ch> 'Z') //نامعتبر
        return 0;
        col = col * 26 +(ch- 'A' +1);//تبدیل حرف به عدد
        i++;
    }
    if (col<=0)//نامعتبر
    return 0;
    col -= 1;//شمارش ستون ها از صفر شروع میشود

    if(!isdigit((unsigned char)address[i]))
    return 0;//اگر بعد از حروف عدد نیامد ممعتبر نیست

    int row =0;
    while(isdigit((unsigned char)address[i])){//تا رقم میبینی ادامه بده
        row=row * 10+ (address[i]-'0');//تبدیل رقم به عدد
        i++;
    }

    if (address[i] !='\0')
    return 0;//اگر بعد از عددهاکاراکتر باشد معتبر نیست
    if(row<=0) 
    return 0;
    row -= 1;//چون اولین سطر، سطرصفر است

    *resultRow=row;
    *resultCol=col;
    return 1;
}

//اگر سلول داخل محدوده جدول باشد ادرس ان را برمیگرداند
//در غیر این صورت NULL برمیگرداند
Cell* sheetFindCell(sheet *s, int row, int col){
    if(!s || !s -> cells)
    return NULL;
    if (row<0 || col<0)
    return NULL;
    if((size_t)row >= s->rows || (size_t)col >= s->cols)
    return NULL;
    return &s -> cells[row][col];
}

//در این تابع اگر سلول خارج از حدوده باشد ابتدا جدول را بزرگ میکند
//سپس ادرس سلول را برمیگرداند
Cell* sheetExpandOrGet(sheet *s, int row, int col){
    if (!s || !s->cells)
    return NULL;
    if(row<0 || col<0) 
    return NULL;
    //تعداد سطر و ستون لازم:
    size_t Rneeded= (size_t) row+1;
    size_t Cneeded= (size_t) col+1;

    if (Rneeded>s -> rows) {//اگر تعداد سطرهای فعلی ناکافی
        if(!rowExpand(s , Rneeded))//افزایش سطر
        return NULL;
    }
    if(Cneeded > s -> cols) {//اگر تعداد ستون فعلی ناکافی
        if(!colsExpand(s, Cneeded))//افزایش ستون
        return NULL;
    }
    return &s->cells[row][col];
}