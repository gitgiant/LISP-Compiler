/**
 * Name: Mattias Huber
 * Lab/task: lab 08 Task 5
 * Date: 04/07/2016
 **/
#include "l8t5.h"

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
    p->data.function.op1 = op1;
    p->data.function.op1->parent = p;
    if(op2 != 0)
    {
        p->data.function.op2 = op2;
        p->data.function.op2->parent = p; 
    }
    

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

    //needed?
    p->parent = NULL;
    p->scope = NULL;

    //printf("Created symbol name: <%s>\n", p->data.symbol.symbolName);
    return p;
}

//LPAREN LPAREN LET let_list RPAREN s_expr RPAREN
//createScope($4, $6);
//create scope with each let expression
AST_NODE *createScope(AST_NODE *rootSymbol, AST_NODE *s_expr)
{
    //should already be evaulated to head node of scope
    s_expr->scope=rootSymbol;
    
    //printf("Created scope with node: <%s>\n", rootSymbol->data.symbol.symbolName);
    
    
}

AST_NODE *createLetElem(char *symbolName, AST_NODE *s_expr)
{
    AST_NODE *p;
    size_t nodeSize;

    // allocate space for the fixed side and the variable part (union)
    nodeSize = sizeof(AST_NODE) + sizeof(SYMBOL_TABLE_AST_NODE);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    p->type = SYMBOL_TABLE_TYPE;
    p->data.symbolTable.symbolName = symbolName;
    p->data.symbolTable.symbolValue = s_expr;

    p->scope = NULL;
    p->parent = NULL;

    return p;
}
//add symbol to HEAD of symbol table
AST_NODE *symbolTableInsert(AST_NODE *rootNode, AST_NODE *insertSymbol)
{
    traverseSymbolTable(rootNode, insertSymbol);

    //$1 pointer to list so far $2 new node
    //set insert symbol's next to head of symbol table
    insertSymbol->data.symbolTable.nextSymbol = rootNode;

    //return new node of head of list
    return insertSymbol;
}

//used for diagnostics
void traverseSymbolTable(AST_NODE *rootNode, AST_NODE *s_expr)
{
    //printf("***Traversing symbol table***\n");
    AST_NODE *traverse = rootNode;
    while(traverse != NULL)
    {
        //printf("Traverse <%s> s_expr <%s>",traverse->data.symbolTable.symbolName, s_expr->data.symbolTable.symbolName);
        if(strcmp(traverse->data.symbolTable.symbolName, s_expr->data.symbolTable.symbolName)==0)
        {
            printf("Warning: Redeclaration of symbol <%S> detected, using later definition.\n", s_expr->data.symbol.symbolName);
        }
        traverse = traverse->data.symbolTable.nextSymbol;
    }
    free(traverse);

}

//symbol lookup searches linked list, returns node in correct scope
AST_NODE *symbolLookup(AST_NODE *parentNode, char *searchName)
{

    AST_NODE *temp = parentNode;
    //printf("Performing Symbol Lookup on <%s>, parent: <%s>\n", searchName, parentNode->data.function.name);
    AST_NODE *scope = NULL;
    AST_NODE *found = NULL;

    //traverse to top parent
    while(temp !=NULL)
    {
        //shift over to scope
        scope = temp->scope;

        //printf("Searching in parent : <%s> scope: <%s>\n", temp->data.symbolTable.symbolName, scope->data.symbolTable.symbolName);

        //while there is a symbol table associated with this parent
        while(scope != NULL)
        {
            //printf("Searching in scope <%s>\n", scope->data.symbolTable.symbolName);

            if(strcmp(scope->data.symbolTable.symbolName, searchName)==0)
            {
                found = scope;
                return found;
            }
            //go to next symbol in symbolTable
            scope = scope->data.symbolTable.nextSymbol;
        }
        //if there was something found, print and return with found node
        if(found != NULL)
        {
            //printf("Found symbol with name: <%s> value: <%f>\n", found->data.symbolTable.symbolName, eval(found->data.symbolTable.symbolValue));
            return found;
        }
        temp = temp->parent;
    }
    
    printf("Compilation Error: Symbol <%s> not found\n", searchName);
    exit (1);
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
        freeNode(p->data.function.op1);
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
    free (p);
}

double eval(AST_NODE *p)
{
    if (!p)
        return -69;
    if(p->type==NUM_TYPE)
    {
        //printf("number detected: %f\n",p->data.number.value);
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
    else if(p->type == SYMBOL_TYPE)
    {
        return eval(symbolLookup(p->parent, p->data.symbol.symbolName));
    }
    //probably not needed?
    else if(p->type == SYMBOL_TABLE_TYPE)
    {
        return eval(p->data.symbolTable.symbolValue);
    }
}
void printvalue(double value)
{
    printf("%f\n", value);
}
