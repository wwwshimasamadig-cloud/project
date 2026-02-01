#include <stdio.h>
#include<stdlib.h>
#include<string.h>

#include "sheet.h"
#include "formula.h"
#include "file.h"

typedef enum { NUMBER,TEXT}CellType;
typedef struct{
    CellType type;
    union{
        double value;
        char text[1000];
    };
}Cell;


typedef struct
{
    int rows;
    int cols;
    Cell cells [100][100];
}Sheet;
void inisheet(Sheet *s,int r,int c);
void savesheet(Sheet *s,char name[]);
void loadsheet(Sheet *s,char name[]);
void Formula(Sheet *s,char formula[]);

int main()
{   Sheet *s;
    int choice;
    char formula[200];
    int r,c;
    scanf("%d %d ",&r,&c);
    getchar();
    
    
    
    
    
    
    s=(Sheet*)malloc(sizeof(Sheet));
    
    while(1)
    {
        scanf("%d",&choice);
        getchar();

    
    switch (choice)
    {
    case 1:
    printf ("enter formula:");
    fgets(formula,200,stdin);
    //فاصله رو بگیره
    Formula(s,formula);
    break;

    //
    case 2:
    loadsheet(s,"data.txt");
    break;
    case 3:
    savesheet(s,"data.txt");
    break;
    case 4:
    return 0;
    default:
    printf("Wrong\n");


    }

    }
    free(s);
    return 0;
}
void inisheet(Sheet *s,int r,int c)
{
    s->rows = r;
    s->cols = c;
    for(int i=0;i<r;i++)
    {
        for(int j=0;j<c;j++)
        {
            s->cells[i][j].value=0;
        }
    }
    





    
}



void savesheet(Sheet *s,char name[])
{
    FILE *f;
    f = fopen(name ,"w");
    if(f==NULL)
    {
        printf("Unable to open the file \n");
        return;
    }
        fprintf(f,"%d %d\n",s->rows,s->cols );
        
        for(int i=0;i<s->rows;i++)
         {
            for(int j=0;j<s->cols;j++)
             {if(s->cells[i][j].type==NUMBER)
                fprintf(f,"%lf",s->cells[i][j].value);
                else
                {
                    fprintf(f,"%s",s->cells[i][j].text);
                }

             }
             fprintf(f,"\n");
         }
    fclose(f);
}
void loadsheet(Sheet *s,char name[])
{
    FILE *f;
    f=fopen(name,"r");
    if(f==NULL)
    {
        printf("Unable to open the file \n");
        return;
    }
    fscanf(f,"%d %d",&s->rows,&s->cols);
    for(int i=0;i<s->rows;i++)
        {
            for(int j=0;j<s->cols;j++)
            {
               if(s->cells[i][j].type==NUMBER)
                fscanf(f,"%lf",s->cells[i][j].value);
                else
                {
                    fprintf(f,"%s",s->cells[i][j].text);
                }
            }
        }

        fclose(f);
}