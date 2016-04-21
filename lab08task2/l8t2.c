/**
 * Name: Mattias Huber
 * Lab/task: lab 08 Task 2
 * Date: 03/17/2016
 **/

#include "l8t2.h"

int main(void)
{
    yyparse();
    return 0;
}

void yyerror(char *s)
{
    fprintf(stderr, "%s\n", s);
}

int resolveFunc(char *func)
{
   char *funcs[] = { "neg", "abs", "exp", "sqrt", "add", "sub", "mult", "div", "remainder", "log", "pow", "max", "min", "exp2","cbrt","hypot", ""};
   
   int i = 0;
   while (funcs[i][0] !='\0')
   {
      if (!strcmp(funcs[i], func))
         return i;
         
      i++;
   }
   yyerror("invalid function"); // paranoic -- should never happen
   return -1;
}

double calc(char *func, double op1, double op2)
{
    double temp;
    switch(resolveFunc(func))
    {
        case 0:
        {
            temp = -op1;
            return temp;
            break;
        }
        case 1:
        {
            temp = abs(op1);
            return temp;
            break;
        }
        case 2:
        {
            temp = exp(op1);
            return temp;
            break;
        }
        case 3: 
        {
            if(op1 < 0)
            {
                printf("Ya cant square root a negative number.!\n");
                return 0;
                break;   
            }
            else
            {
                temp = sqrt(op1);
                return temp;
                break;
            }
        }
        case 4:
        {
            temp = op1 + op2;
            return temp;
            break;
        }
        case 5:
        {
            temp = op1 - op2;
            return temp;
            break;
        }
        case 6:
        {
            temp = op1 * op2;
            return temp;
            break;
        }
        case 7:
        {
            if(op2 != 0)
                {          
                    temp = op1 / op2;
                    return temp;
                    break;
                }
            else
                {
                    printf("Ya cant divide by zero.\n");
                    return 0;
                    break;
                }
        }
        case 8:
        {   
            if(op2 != 0)
                {          
                    temp = fmod(op1, op2);
                    return temp;
                    break;
                }
            else
                {
                    printf("Ya cant divide by zero.\n");
                    return 0;
                    break;
                }
        }
        case 9:
        {
            if(op1 <= 0)
            {
                printf("Cant log <= 0\n");
                return 0;
                break;
            }
            else if(op1 == 2)
            {
                temp = (log(op2)/log(2));  //logbase2 = logbaseE(value) / logbaseE(2)
                return temp;
                break;
            }
            else if(op1 == 10)
            {
                temp = log10(op2);
                return temp;
                break;
            }
            else
            {
                printf("Please enter a log base of either 2 or 10\n");
            }
        }
        case 10:
        {
            temp = pow(op1, op2);
            return temp;
            break;
        }
        case 11:
        {
            temp = ceil(op1);
            return temp;
            break;
        }
        case 12:
        {
            temp = floor(op1);
            return temp;
            break;
        }
        
        case 13:
        {
            temp = pow(2, op1);
            return temp;
            break;
        }
        case 14:
        {
            if(op1 < 0)
            {
                printf("Ya cant find the cube root of a negative number.");
                return 0;
                break;  
            }
            else
            {
                temp = cbrt(op1);
                return temp;
                break;
            }
        }
        case 15:
        {
            temp = hypot(op1, op2);
            return temp;
            break;
        }
        return 0.0; 
    }
    
}  