/**
 * Name: Mattias Huber
 * Lab/task: lab 08 Task 9
 * Date: 04/14/2016
 **/
#include "l8t9.h"

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
   char *funcs[] = {"neg", "abs", "exp", "sqrt", "add", "sub", "mult", "div", "remainder", "log", "pow", "max", "min", "exp2", "cbrt", "hypot", "print","smaller","larger","equal","rand","read", "newValue",""};
   
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

    p->scope = NULL;
    p->parent = NULL;

    return p;
}

// create a node for a function
AST_NODE *function(char *funcName, AST_NODE *op1, AST_NODE *op2)
{
    AST_NODE *p;
    size_t nodeSize;

    // allocate space for the fixed side and the variable part (union)
    nodeSize = sizeof(AST_NODE) + sizeof(FUNCTION_AST_NODE);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    p->type = FUNC_TYPE;
    p->data.function.name = funcName;
    if(op1 != 0)
    {
        p->data.function.op1 = op1;
        p->data.function.op1->parent = p;
    }
    else
        p->data.function.op1 = NULL;
    
    if(op2 != 0)
    {
        p->data.function.op2 = op2;
        p->data.function.op2->parent = p; 
    }
    else
        p->data.function.op2 = NULL;

    p->scope = NULL;
    p->parent = NULL;

    return p;
}

//create a node for just a symbol, not with s_expr
AST_NODE *createSymbol(char *symbolName)
{
    AST_NODE *p;
    size_t nodeSize;

    // allocate space for the fixed side and the variable part (union)
    nodeSize = sizeof(AST_NODE) + sizeof(SYMBOL_AST_NODE);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    p->type = SYMBOL_TYPE;
    p->data.symbol.symbolName = symbolName;

    p->parent = NULL;
    p->scope = NULL;

    //printf("Created symbol name: <%s>\n", p->data.symbol.symbolName);
    return p;
}

AST_NODE *createLetElem(char *letElemType, char *symbolName, AST_NODE *s_expr)
{

    AST_NODE *p;
    size_t nodeSize;

    // allocate space for the fixed side and the variable part (union)
    nodeSize = sizeof(AST_NODE) + sizeof(SYMBOL_TABLE_AST_NODE);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    p->type = SYMBOL_TABLE_TYPE;
    p->data.symbolTable.type = letElemType;
    p->data.symbolTable.symbolName = symbolName;
    p->data.symbolTable.symbolValue = s_expr;

    p->scope = NULL;
    p->parent = NULL;
    //printf("Created letElem with name <%s>, type <%s>, and s_expr<%f>\n",p->data.symbolTable.symbolName, p->data.symbolTable.type, eval(p->data.symbolTable.symbolValue));

    return p;
}

AST_NODE *createScope(AST_NODE *rootSymbol, AST_NODE *s_expr)
{
    //printf("rootSymbol type: %d, s_expr type %d\n", rootSymbol->type, s_expr->type);
    
    //should already be evaulated to head node of scope
    if(s_expr->scope == NULL)
    {
        s_expr->scope = rootSymbol;
    }
    else
    {
        s_expr->scope->scope = rootSymbol;
    }
    //printf("Created scope with node: <%s>, s_expr head: <%s>\n", rootSymbol->data.symbol.symbolName,s_expr->data.symbol.symbolName);
    return s_expr;

    //traverseSymbolTable(rootSymbol);
}

//add symbol to HEAD of symbol table
AST_NODE *symbolTableInsert(AST_NODE *rootNode, AST_NODE *insertSymbol)
{
    //used to traverse linked list symbol table
    AST_NODE *temp = rootNode;

    //need to check if symbol is already in symbol table, redeclaration in same scope is not allowed
    while(temp != NULL)
    {
        if(strcmp(insertSymbol->data.symbolTable.symbolName, temp->data.symbolTable.symbolName)==0)
        {
            printf("ERROR: redeclaration of variable <%s> attempted", insertSymbol->data.symbolTable.symbolName);
            exit(2);
        }
        //move to the next node
        temp = temp->data.symbolTable.nextSymbol;
    }
    //$1 pointer to list so far $2 new node
    //set insert symbol's next to head of symbol table
    insertSymbol->data.symbolTable.nextSymbol = rootNode;

    freeNode(temp);
    //return new node of head of list
    return insertSymbol;
}

void traverseSymbolTable(AST_NODE *rootNode)
{
    //printf("***Traversing symbol table***\n");
    AST_NODE *traverse = rootNode;
    while(traverse != NULL)
    {
        //printf("Node: <%s>\n", traverse->data.symbol.symbolName);
        traverse = traverse->data.symbolTable.nextSymbol;
    }
    free(traverse);

}

//symbol lookup searches linked list, returns node in correct scope
AST_NODE *symbolLookup(AST_NODE *parentNode, char *searchName)
{

    AST_NODE *temp = parentNode;
    //printf("Performing Symbol Lookup on <%s>, parent type: <%d>\n", searchName, parentNode->type);
    AST_NODE *scope = NULL;
    AST_NODE *found = NULL;
    AST_NODE *head = NULL;

    //traverse to top parent
    while(temp !=NULL)
    {
        //shift over to scope
        scope = temp->scope;

        //save the head of the symbol table, its scope is the next symbol table to check for inherited types
        head = scope;

        //printf("Searching in parent : <%s> scope: <%s>\n", temp->data.symbolTable.symbolName, scope->data.symbolTable.symbolName);
        
        //while there is a symbol table associated with this parent
        while(scope != NULL)
        {
            //printf("Searching in scope <%s>\n", scope->data.symbolTable.symbolName);

            if(strcmp(scope->data.symbolTable.symbolName, searchName)==0)
            {
                found = scope;
            }
            //go to next symbol in symbolTable
            scope = scope->data.symbolTable.nextSymbol;
        }
        //if there was something found, print and return with found node
        if(found != NULL)
        {
            //if the datatype is null
            if(found->data.symbolTable.type == NULL)
            {
                //printf("Found symbol with name: <%s> value: <%f> type = NULL searching for inherited type\n", found->data.symbolTable.symbolName, eval(found->data.symbolTable.symbolValue));
                //search in the head of this lists scope, it is the head of the next list
                AST_NODE *lookup = symbolLookup(head, searchName);
                found->data.symbolTable.type = lookup->data.symbolTable.type;
                return found;
            }
            else
            {
                //printf("Found symbol with name: <%s> value: <%f> type <%s>\n", found->data.symbolTable.symbolName, eval(found->data.symbolTable.symbolValue), found->data.symbolTable.type);
                return found;
            }
           
        }
        //move to the next parent in tree
        temp = temp->parent;
    }
    
    printf("Compilation Error: Symbol <%s> not found\n", searchName);
    exit(1);
    return NULL;
}

// free a node
void freeNode(AST_NODE *p)
{
    if (!p)
       return;
       
    if (p->type == FUNC_TYPE)
    {
        free(p->data.function.name);
        if(p->data.function.op1 !=NULL)
            freeNode(p->data.function.op1);
        if(p->data.function.op2 != NULL)
            freeNode(p->data.function.op2);
    }
    if(p->type == SYMBOL_TYPE)   
    {
        free(p->data.symbol.symbolName);
    } 
    if(p->type == SYMBOL_TABLE_TYPE)   
    {
        free(p->data.symbolTable.symbolName);
        free(p->data.symbolTable.symbolValue);
        free(p->data.symbolTable.nextSymbol);
    } 
    if(p->type == CONDITION_TYPE)
    {
        free(p->data.condition.conditionNode);
        free(p->data.condition.s_expr1);
        free(p->data.condition.s_expr2);
    }
    free (p);
}

//creates a conditional AST_NODE, actual evaluation is handled in eval
AST_NODE *conditional(AST_NODE *condition, AST_NODE *s_expr1, AST_NODE *s_expr2)
{
    AST_NODE *p;
    size_t nodeSize;

    // allocate space for the fixed side and the variable part (union)
    nodeSize = sizeof(AST_NODE) + sizeof(CONDITION_AST_NODE);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    p->type = CONDITION_TYPE;
    p->data.condition.conditionNode = condition;
    p->data.condition.s_expr1 = s_expr1;
    p->data.condition.s_expr2 = s_expr2;
    
    //conditions and s_expr need p's scope in order to evaluate
    // condition->scope = p;
    // s_expr1->scope = p;
    // s_expr2->scope = p;

    p->scope = NULL;
    p->parent = NULL;
    return p;
}
//make return an abstract type with its associated type? or make two evals, one real and one long?
double eval(AST_NODE *p)
{
    if (!p)
        return -69;
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
            case 16:
            {
                printvalue(eval(p->data.function.op1));
                break;
            }
            case 17:
            {
                double temp1 = eval(p->data.function.op1);
                double temp2 = eval(p->data.function.op2);
                if(temp1 < temp2)
                    return 1;
                else
                    return 0;    
            }
            case 18:
            {
                double temp1 = eval(p->data.function.op1);
                double temp2 = eval(p->data.function.op2);
                if(temp1 > temp2)
                    return 1;
                else
                    return 0;    
            }
            case 19:
            {
                double temp1 = eval(p->data.function.op1);
                double temp2 = eval(p->data.function.op2);
                if(temp1 == temp2)
                    return 1;
                else
                    return 0;    
            }
            case 20:
            {
                return rand();
            }
            case 21:
            {
                fflush(stdin);
                double temp;
                printf("Please enter a number:\n");
                scanf("%lf", &temp);
                printf("\n");
                fflush(stdin);
                p->data.function.op1 = number(temp);
                p->data.function.op1->parent = p;
                p->data.function.name = "newValue";
                return eval(p->data.function.op1);
            }
            case 22:
            {
                return eval(p->data.function.op1);
            }
            
        } //end switch
        // if(strcmp(p->data.function.name, "newValue")==0)
        // {
        //     return eval(p->data.function.op1);
        // }
    } //end else if
    else if(p->type == SYMBOL_TYPE)
    {
        AST_NODE *temp = symbolLookup(p->parent, p->data.symbol.symbolName);
        if (strcmp(temp->data.symbolTable.type, "integer")==0)
        {
            long longValue = (long) (eval(temp) +.5);
            return longValue;
        }
        else if (strcmp(temp->data.symbolTable.type, "real")==0)
        {
            return eval(temp->data.symbolTable.symbolValue);
        }
    }
    //probably not needed?
    else if(p->type == SYMBOL_TABLE_TYPE)
    {
        return eval(p->data.symbolTable.symbolValue);
    }
    else if(p->type == CONDITION_TYPE)
    {
        p->data.condition.conditionNode->scope = p->scope;
        p->data.condition.s_expr1->scope = p->scope;
        p->data.condition.s_expr2->scope = p->scope;
        if(eval(p->data.condition.conditionNode) != 0)
            return eval(p->data.condition.s_expr1);
        else
            return eval(p->data.condition.s_expr2);
    }
}

void printvalue(double value)
{
    double temp = value;
    if (fabsf(roundf(temp) - temp) <= 0.00001f) 
    {
        printf("%g\n", temp);
    }   
    else 
    {
        printf("%.2f\n", value);
    }
}
