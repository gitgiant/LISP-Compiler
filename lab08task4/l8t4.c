/**
 * Name: Mattias Huber
 * Lab/task: lab 08 Task 4
 * Date: 03/20/2016
 **/
#include "l8t4.h"

//extern double passing_value;

int main(void)
{
    yyparse();
    return 0;
}

void yyerror(char *s)
{
    fprintf(stderr, "%s\n", s);
}

// find out which function it is
int resolveFunc(char *func)
{
   char *funcs[] = { "neg", "abs", "exp", "sqrt", "add", "sub", "mult", "div", "remainder", "log", "pow", "max", "min", "exp2", "cbrt", "hypot", ""};
   
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

// create a node for a number
AST_NODE *number(double value)
{
    AST_NODE *p;
    size_t nodeSize;

    // allocate space for the fixed sie and the variable part (union)
    nodeSize = sizeof(AST_NODE) + sizeof(NUMBER_AST_NODE);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    p->type = NUM_TYPE;
    p->data.number.value = value;

    return p;
}

// create a node for a function
AST_NODE *function(char *funcName, AST_NODE *op1, AST_NODE *op2)
{
    AST_NODE *p;
    size_t nodeSize;

    // allocate space for the fixed sie and the variable part (union)
    nodeSize = sizeof(AST_NODE) + sizeof(FUNCTION_AST_NODE);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    p->type = FUNC_TYPE;
    p->data.function.name = funcName;
    p->data.function.op1 = op1;
    p->data.function.op2 = op2;

    return p;
}

// free a node
void freeNode(AST_NODE *p)
{
    if (!p)
       return;
       
    if (p->type == FUNC_TYPE)
    {
        free(p->data.function.name);
        freeNode(p->data.function.op1);
        freeNode(p->data.function.op2);
    }
        
    free (p);
}

double eval(AST_NODE *p)
{
    if (!p)
        return 0;
    if(p->type==NUM_TYPE)
    {
        return p->data.number.value;
    }
    else if(p->type==FUNC_TYPE)
    {
        switch(resolveFunc(p->data.function.name))
        {
            case 0:
            {
                return -eval(p->data.function.op1);
                break;
            }
            case 1:
            {
                return fabs(eval(p->data.function.op1));
                break;
            }
            case 2:
            {
                return exp(eval(p->data.function.op1));
                break;
            }
            case 3:
            {
                return sqrt(eval(p->data.function.op1));
                break;
            }
            case 4:
            {
                return eval(p->data.function.op1) + eval(p->data.function.op2);
                break;
            }
            case 5:
            {
                return eval(p->data.function.op1) - eval(p->data.function.op2);
                break;
            }
            case 6:
            {
            return eval(p->data.function.op1) * eval(p->data.function.op2);
                break;
            }
            case 7:
            {
                if(eval(p->data.function.op2) != 0)
                    return eval(p->data.function.op1) / eval(p->data.function.op2);
                else
                {
                    printf("Cannot divide by zero.\n");
                }
                break;
            }
            case 8:
            {
                if(eval(p->data.function.op2) != 0)
                    return fmod(eval(p->data.function.op1), eval(p->data.function.op2));
                else
                {
                    printf("Cannot divide by zero.\n");
                }
                break;
            }
            case 9:
            {
                double temp = eval(p->data.function.op1);
                if(temp <=0)
                {
                    printf("Cannot log by <=0");
                }
                else if(temp ==2)
                {
                    return log(eval(p->data.function.op2))/log(2); //logbase2 = logbaseE(value) / logbaseE(2)
                }
                else if(temp == 10)
                {
                    return log10(eval(p->data.function.op2));
                }
                break;
            }
            case 10:
            {
                return pow(eval(p->data.function.op1), eval(p->data.function.op2));
                break;
            }
            case 11:
            {
                return ceil(eval(p->data.function.op1));
                break;
            }
            case 12:
            {
                return floor(eval(p->data.function.op1));
                break;
            }
            case 13:
            {
                return (pow(2, eval(p->data.function.op1)));
                break;
            }
            case 14:
            {
                return (cbrt(eval(p->data.function.op1)));
                break;
            }
            case 15:
            {
                return (hypot(eval(p->data.function.op1), eval(p->data.function.op2)));
                break;
            }
        } //end switch
    } //end else if
}
void printvalue(double value)
{
    printf("%f", value);
}

