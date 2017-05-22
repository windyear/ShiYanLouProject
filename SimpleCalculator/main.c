#include <stdio.h>
#include <stdlib.h>

int main()
{
    double num1;
    double num2;
    char op;
    printf("Hello world!\n");
    printf("Please input the expression that you want to calculate:\n");
    while(scanf_s("%lf%c%lf",&num1,&op,&num2)!=EOF){
        switch(op){
        case '+':
            printf("=%lf\n",num1+num2);
            break;
        case '-':
            printf("=%lf\n",num1-num2);
            break;
        case '*':
            printf("=%lf\n",num1*num2);
            break;
        case '/':
            if(num2==0){
                printf("Incorrect input\n");
                break;
            }
            printf("=%1f\n",num1/num2);
            break;
        case '%':
            if((long)num2==0)
                printf("Error!");
            else
                printf("=%ld\n",(long)num1%(long)num2);
        default:
            printf("Input error!");
        }
    }
    return 0;
}
