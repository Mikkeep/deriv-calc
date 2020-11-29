#include <assert.h>
#include <ctype.h>
#include <string.h>

#include "../libs/log_generator.h"
#include "../libs/file_manager.h"
#include "expression_loader.h"

const char*  EMPTY_SPACE_DELIMS = " \t";

bool      parseExpression  (ETNode* root, char* expr, size_t exprLength);
char*     skipDelims       (char* str);
int       processNumber    (const char* token, double* number);
Operation processOperation (char* token);
int       processConstant  (char* token, double* constant);
bool      isVariable       (char token);

bool loadExpression(ExprTree* tree, const char* filename)
{
    assert(tree     != nullptr);
    assert(filename != nullptr);

    FILE* file = fopen(filename, "r");
    if (file == nullptr) 
    {
        printf("Unable to open file '%s'.\n", filename);
        return false;  
    }

    size_t fileSize   = getFileSize(filename);
    size_t exprLength = 0;

    // allocating fileSize + 1 to be able to put '\0' at the end
    char* exprBuffer = (char*) calloc(fileSize + 1, sizeof(char));
    if (exprBuffer == nullptr) 
    {
        printf("Unable to load whole file '%s'\n.", filename);
        fclose(file);
        return false;  
    }

    exprLength = fread(exprBuffer, sizeof(char), fileSize, file);

    if (exprLength == 0)
    {
        printf("File '%s' is empty.\n", filename);
        fclose(file);
        free(exprBuffer);
        return false;  
    }

    char* lineEnd = strchr(exprBuffer, '\n');
    if (lineEnd != nullptr) { exprLength = lineEnd - exprBuffer; }

    exprBuffer[exprLength] = '\0';

    tree->root = newNode();
    
    if (!parseExpression(tree->root, exprBuffer, exprLength))
    {
        LG_LogMessage("Expression tree hasn't been constructed correctly.", LG_STYLE_CLASS_ERROR, filename);

        fclose(file);
        free(exprBuffer);

        return false;  
    }

    fclose(file);
    free(exprBuffer);

    return true;
}

#define GO_TO_CHILD(side) node->side         = newNode();  \
                          node->side->parent = node;       \
                          node               = node->side; 

bool parseExpression(ETNode* root, char* expr, size_t exprLength)
{
    assert(expr       != nullptr);
    assert(root       != nullptr);
    assert(exprLength >  0);

    size_t    numberLength   = 0;
    double    number         = 0;
    int       constantLength = 0;
    double    constant       = 0;
    Operation operation      = OP_INVALID;

    char*   curr = expr;
    ETNode* node = root;

    while ((size_t) (curr - expr) < exprLength)
    {
        curr = skipDelims(curr);

        if (*curr == '(')
        {
            if (node->left  == nullptr && node->data.op < UNARY_OPERATIONS_START) 
            { 
                GO_TO_CHILD(left); 
            }
            else if (node->right == nullptr) 
            { 
                GO_TO_CHILD(right); 
            }
            else
            {
                printf("Syntax error: opening bracket after both operands been found.\n");
                return false;
            }

            curr++;
        }
        else if (*curr == ')')
        {
            node = node->parent;
            curr++;
        }
        else if ((numberLength = processNumber(curr, &number)) > 0)
        {
            setData(node, number);
            curr += numberLength;
        }
        else if ((operation = processOperation(curr)) != OP_INVALID)
        {
            setData(node, operation);
            curr += strlen(OPERATIONS[operation]);
        }
        else if ((constantLength = processConstant(curr, &constant)) > 0)
        {
            setData(node, constant);
            curr += constantLength;
        }
        else if (isVariable(*curr) && strchr(" \t()", *(curr + 1)) != NULL)
        {
            setData(node, *curr);
            curr++;
        }
        else
        {
            printf("Syntax error: invalid symbol ->'[%c]%s'.\n", *curr, curr + 1);
            return false;
        }

        curr = skipDelims(curr);
    }

    if (node != root)
    {
        printf("Syntax error: ')' not found.\n");
        return false;
    }

    return true;
}

char* skipDelims(char* str)
{
    assert(str != nullptr);

    return str + strspn(str, EMPTY_SPACE_DELIMS);
}

int processNumber(const char* token, double* number)
{
    assert(token  != nullptr);
    assert(number != nullptr);

    int numberLength = 0;

    if (sscanf(token, "%lg%n", number, &numberLength) != 1) { return 0; }

    return numberLength;
}

Operation processOperation(char* token)
{
    assert(token != nullptr);

    char* operationEnd = token + strcspn(token, " \t()"); 
    char  prevSymb     = *operationEnd;

    *operationEnd = '\0';

    Operation operation = OP_INVALID; 

    for (size_t i = 0; i < OPERATIONS_COUNT; i++)
    {
        if (strcmp(token, OPERATIONS[i]) == 0)
        {
            operation = (Operation) i;

            break;
        }
    }

    *operationEnd = prevSymb;

    return operation;
}

int processConstant(char* token, double* constant)
{
    assert(token != nullptr);

    char* constantEnd = token + strcspn(token, " \t()"); 
    char  prevSymb    = *constantEnd;

    *constantEnd = '\0';

    int length = 0; 

    if      (strcmp(token, "pi") == 0) { length = 2; *constant = PI_CONST; }
    else if (strcmp(token, "e")  == 0) { length = 1; *constant = E_CONST;  }

    *constantEnd = prevSymb;

    return length;
}

bool isVariable(char token)
{
    return isalpha(token) && token != 'e';
}