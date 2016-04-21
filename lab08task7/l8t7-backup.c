/**
 * Name: Mattias Huber
 * Lab/task: lab 08 Task 6
 * Date: 04/13/2016
 **/
#include "l8t6.h"

 //global head of symbolTable
//AST_NODE *symbolTable = NULL;

//global head of scope
//AST_NODE *scopeNode = NULL;

int main(void)
{
    yyparse();
    return 0;
}

void yyerror(char *s)
{
    fprintf(stderr, "%s\n", s);
}
a
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

    p->data.number.type = NULL;
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

    p->parent = NULL;
    p->scope = NULL;

    printf("Created symbol name: <%s>\n", p->data.symbol.symbolName);
    return p;
}

AST_NODE *createLetElem(char *letElemType, char *symbolName, AST_NODE *s_expr)
{

    AST_NODE *p;
    size_t nodeSize;

    //check for inherited datatypes from symbols if undeclared type? replace the symbol if found
    // if(letElemType == NULL)
    // {
    //     printf("NULL LetElem type detected, performing symbol lookup on <%s> s_expr\n", symbolName);
    //     p = symbolLookup(s_expr, symbolName);
    //     if(p == NULL)
    //     {
    //         printf("Compilation Error: Symbol <%s> not declared\n", symbolName);
    //     }
    // }

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
    printf("Created letElem with name <%s>, type <%s>, and s_expr<%f>\n",p->data.symbolTable.symbolName, p->data.symbolTable.type, eval(p->data.symbolTable.symbolValue));

    return p;
}
//LPAREN LPAREN LET let_list RPAREN s_expr RPAREN
//createScope($4, $6);
//create scope with each let expression
//s_expr is the root of the s_expr $6, while $4 is the root symbol of the symbol tree
AST_NODE *createScope(AST_NODE *rootSymbol, AST_NODE *s_expr)
{
    printf("rootSymbol type: %d, s_expr type %d\n", rootSymbol->type, s_expr->type);
    //should already be evaulated to head node of scope
    if(s_expr->scope == NULL)
    {
        s_expr->scope = rootSymbol;
    }
    else
    {
        s_expr->scope->scope = rootSymbol;
    }
    printf("Created scope with node: <%s>, s_expr head: <%s>\n", rootSymbol->data.symbol.symbolName,s_expr->data.symbol.symbolName);
    return s_expr;
    // if(rootSymbol->type == 3)
    // {
    //     printf("Created scope with node: <%s>, symbolTable head: <%s>\n", rootSymbol->data.symbol.symbolName,s_expr->data.symbol.symbolName);
    //     return rootSymbol;
    // }
    // else if(s_expr->type == 3)
    // {
    //     printf("Created scope with node: <%s>, s_expr head: <%s>\n", rootSymbol->data.symbol.symbolName,s_expr->data.symbol.symbolName);
    //     return s_expr;
    // }
    // else
    // {
    //     printf("Created scope with node: <%s>, s_expr head: <%s>\n", rootSymbol->data.symbol.symbolName,s_expr->data.symbol.symbolName);
    //     return s_expr;
    // }
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
    printf("***Traversing symbol table***\n");
    AST_NODE *traverse = rootNode;
    while(traverse != NULL)
    {
        printf("Node: <%s>\n", traverse->data.symbol.symbolName);
        traverse = traverse->data.symbolTable.nextSymbol;
    }
    free(traverse);

}

//symbol lookup searches linked list, returns node in correct scope
AST_NODE *symbolLookup(AST_NODE *parentNode, char *searchName)
{

    AST_NODE *temp = parentNode;
    printf("Performing Symbol Lookup on <%s>, parent type: <%d>\n", searchName, parentNode->type);
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
                printf("Found symbol with name: <%s> value: <%f> type = NULL searching for inherited type\n", found->data.symbolTable.symbolName, eval(found->data.symbolTable.symbolValue));
                //search in the head of this lists scope, it is the head of the next list
                AST_NODE *lookup = symbolLookup(head, searchName);
                found->data.symbolTable.type = lookup->data.symbolTable.type;
                return found;
            }
            else
            {
                printf("Found symbol with name: <%s> value: <%f> type <%s>\n", found->data.symbolTable.symbolName, eval(found->data.symbolTable.symbolValue), found->data.symbolTable.type);
                return found;
            }
           
        }
        //move to the next parent in tree
        temp = temp->parent;
    }
    
    printf("Compilation Error: Symbol <%s> not found\n", searchName);
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

//make return an abstract type with its associated type? or make two evals, one real and one long?
double eval(AST_NODE *p)
{
    if (!p)
        return -69;
    if(p->type==NUM_TYPE)
    {
        if(p->data.number.type == "integer")
        {
            return (long) p->data.number.longValue;
        }
        else
        {
            return p->data.number.value;
        }
    }
    else if(p->type==FUNC_TYPE)
    {
        if(strcmp(p->data.function.name,"neg")==0)
        {
            return -eval(p->data.function.op1);
        }
        if(strcmp(p->data.function.name,"abs")==0)
        {
            return fabs(eval(p->data.function.op1));
        }
        if(strcmp(p->data.function.name,"exp")==0)
        {
            return exp(eval(p->data.function.op1));
        }
        if(strcmp(p->data.function.name,"sqrt")==0)
        {
            return sqrt(eval(p->data.function.op1));
        }
        if(strcmp(p->data.function.name,"add")==0)
        {
            return (eval(p->data.function.op1) + eval(p->data.function.op2));
        }
        if(strcmp(p->data.function.name,"sub")==0)
        {
            return (eval(p->data.function.op1) - eval(p->data.function.op2));
        }
        if(strcmp(p->data.function.name,"mult")==0)
        {
            return (eval(p->data.function.op1) * eval(p->data.function.op2));
        }
        if(strcmp(p->data.function.name,"div")==0)
        {   
            if(eval(p->data.function.op2) != 0)
                return (eval(p->data.function.op1) / eval(p->data.function.op2));
            else
            {
                printf("Cannot divide by zero.\n");
            }
        }
        if(strcmp(p->data.function.name,"remainder")==0)
        {
            if(eval(p->data.function.op2) != 0)
                return fmod(eval(p->data.function.op1), eval(p->data.function.op2));
            else
            {
                printf("Cannot divide by zero.\n");
            }
        }
        if(strcmp(p->data.function.name,"log")==0)  //make log (base,value)?
        {
            return log(eval(p->data.function.op1));
        }
        if(strcmp(p->data.function.name,"pow")==0)
        {
            return pow(eval(p->data.function.op1), eval(p->data.function.op2));
        }
        if(strcmp(p->data.function.name,"max")==0)
        {
            return ceil(eval(p->data.function.op1));
        }
        if(strcmp(p->data.function.name,"min")==0)
        {
            return floor(eval(p->data.function.op1));
        }
    }
    else if(p->type == SYMBOL_TYPE)
    {
        //symbol needs integer/real type?
        //eval first then return proper type?
        AST_NODE *temp = symbolLookup(p->parent, p->data.symbol.symbolName);
        if (strcmp(temp->data.symbolTable.type, "integer")==0)
        {
            //round up or down and truncate after decimal place
            AST_NODE *longNumber;
            if ((longNumber = malloc(sizeof(AST_NODE) + sizeof(NUMBER_AST_NODE))) == NULL)
                yyerror("out of memory");
            longNumber->type = NUM_TYPE;
            longNumber->data.number.type = temp->data.symbolTable.type;
            longNumber->data.number.longValue = (long) (eval(temp) + .5);
            //printf("Integer (long) Value: %li\n", longValue);
            freeNode(temp);
            eval(longNumber);
        }
        else if (strcmp(temp->data.symbolTable.type, "real")==0)
        {
            AST_NODE *realNumber;
            if ((realNumber = malloc(sizeof(AST_NODE) + sizeof(NUMBER_AST_NODE))) == NULL)
                yyerror("out of memory");
            realNumber->type = NUM_TYPE;
            realNumber->data.number.type = temp->data.symbolTable.type;
            realNumber->data.number.value = eval(temp);
            freeNode(temp);
            eval(realNumber);
        }
    }
    //probably not needed?
    else if(p->type == SYMBOL_TABLE_TYPE)
    {
        return eval(p->data.symbolTable.symbolValue);
    }
}
void printvalue(double value)
{
    printf("%f", value);
    // double temp = value;
    // if (fabsf(roundf(temp) - temp) <= 0.00001f) 
    // {
    //     printf("Integer value: %g\n", temp);
    // }   
    // else 
    // {
    //     printf("Real value: %f\n", value);
    // }
    // if(value->data.number.type == "integer")
    // {
    //     printf("%d", value->data.number.longValue);
    // }
    // else
    // {
    //     printf("%f", value->data.number.realValue);
    // }
}
