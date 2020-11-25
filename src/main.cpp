#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "binary_tree.h"
#include "../libs/log_generator.h"
#include "../libs/file_manager.h"

const char* EMPTY_SPACE_DELIMS = " \t";

BTNode*   newNode          (NodeType type, double value, BTNode* left, BTNode* right);
BTNode*   copyTree         (BTNode* node);

bool      loadExpression   (BinaryTree* tree, const char* filename);
bool      treeConstruct    (BTNode* root, char* expr, size_t exprLength);
char*     skipDelims       (char* str);
size_t    processNumber    (const char* token, double* number);
Operation processOperation (char* token);
bool      isVariable       (char token);

void      dump             (BinaryTree* tree);
void      dumpSubtree      (FILE* file, BTNode* node);

int main(int argc, char* argv[])
{
    LG_Init();

    BinaryTree exprTree = {};
    construct(&exprTree);

    if (!loadExpression(&exprTree, argv[1])) 
    { 
        LG_LogMessage("loadExpression() returned false.", LG_STYLE_CLASS_ERROR);

        destroy(&exprTree);
        LG_Close();

        return -1; 
    }

    BinaryTree derivTree = {};
    construct(&derivTree);

    dump(&exprTree);

    destroy(&exprTree);
    destroy(&derivTree);

    LG_Close();
}

bool loadExpression(BinaryTree* tree, const char* filename)
{
    assert(tree     != nullptr);
    assert(filename != nullptr);

    FILE* file = fopen(filename, "r");
    if (file == nullptr) 
    {
        printf("Unable to open file '%s'.\n", filename);
        return false;  
    }

    size_t fileSize = getFileSize(filename);

    char* exprBuffer = (char*) calloc(getFileSize(filename), sizeof(char));
    if (exprBuffer == nullptr) 
    {
        printf("Unable to load whole file '%s'\n.", filename);
        fclose(file);
        return false;  
    }

    if (fread(exprBuffer, sizeof(char), fileSize, file) == 0)
    {
        printf("File '%s' is empty.\n", filename);

        fclose(file);
        free(exprBuffer);

        return false;  
    }

    setRoot(tree, newNode());
    
    if (!treeConstruct(getRoot(tree), exprBuffer, fileSize - 1))
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

bool treeConstruct(BTNode* root, char* expr, size_t exprLength)
{
    assert(expr       != nullptr);
    assert(root       != nullptr);
    assert(exprLength >  0);

    size_t    numberLength = 0;
    double    number       = 0;
    Operation operation    = OP_INVALID;

    char*   curr = expr;
    BTNode* node = root;

    while (*curr != '\0' && *curr != '\n' && curr - expr < exprLength - 1)
    {
        curr = skipDelims(curr);

        if (*curr == '(')
        {
            if (getLeft(node) == nullptr && getValue(node).value < UNARY_OPERATIONS_START)
            {
                setLeft(node, newNode());
                setParent(getLeft(node), node);

                node = getLeft(node);
            }
            else if (getRight(node) == nullptr)
            {
                setRight(node, newNode());
                setParent(getRight(node), node);

                node = getRight(node);
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
            node = getParent(node);
            curr++;
        }
        else if ((numberLength = processNumber(curr, &number)) > 0)
        {
            setValue(node, { TYPE_CONST, number });
            curr += numberLength;
        }
        else if ((operation = processOperation(curr)) != OP_INVALID)
        {
            setValue(node, { TYPE_OP, operation });
            curr += strlen(OPERATIONS[operation]);
        }
        else if (isVariable(*curr) && strchr(" \t()", *(curr + 1)) != NULL)
        {
            setValue(node, { TYPE_VAR, *curr });
            curr++;
        }
        else
        {
            printf("Syntax error: invalid symbol '%c'.\n", *curr);
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

size_t processNumber(const char* token, double* number)
{
    assert(token  != nullptr);
    assert(number != nullptr);

    size_t numberLength = 0;

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

    for (int i = 0; i < OPERATIONS_COUNT; i++)
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

bool isVariable(char token)
{
    return isalpha(token) && token != 'e';
}

void dump(BinaryTree* tree)
{
    assert(tree != nullptr);

    int count = 0;
    
    FILE* counterFile = fopen("log/dump_count.cnt", "r+");
    if (counterFile == nullptr)
    {
        counterFile = fopen("log/dump_count.cnt", "w");
        count = -1;
    }
    else
    {
        fscanf(counterFile, "%u", &count);
        fseek(counterFile, 0, SEEK_SET);
    }

    fprintf(counterFile, "%u", count + 1);
    fclose(counterFile);

    char textFilename[128] = {};
    snprintf(textFilename, 128, "%s%u.txt", "log/tree_dumps/text/tree", count);

    char imageFilename[128] = {};
    snprintf(imageFilename, 128, "%s%u.svg", "log/tree_dumps/tree", count);

    FILE* file = fopen(textFilename, "w");
    assert(file != nullptr);

    fprintf(file,
            "digraph structs {\n"
            "\tnode [shape=\"circle\", style=\"filled\", fontcolor=\"#DCDCDC\", fillcolor=\"#2F4F4F\"];\n\n");

    if (getRoot(tree) == nullptr)
    {
        fprintf(file,
                "\t\"ROOT\" [label = \"Empty database\"];\n");
    }
    else
    {
        dumpSubtree(file, getRoot(tree));
    }

    fprintf(file, "}");

    fclose(file);

    char dotCmd[256] = {};

    snprintf(dotCmd, 256, "dot -Tsvg %s -o %s", textFilename, imageFilename);
    system(dotCmd);

    snprintf(dotCmd, 256, "start %s", imageFilename);
    system(dotCmd);
}

void dumpSubtree(FILE* file, BTNode* node)
{
    assert(file != nullptr);
    
    if (node == nullptr) { return; }

    fprintf(file, "\t\"%p\" [label=", node);

    NodeType type = getValue(node).type; 
    switch (type)
    {
        case TYPE_CONST:
            fprintf(file, "\"%lg\", color=\"#D2691E\", fillcolor=\"#FFFAEF\", fontcolor=\"#FF7F50\"];\n", getValue(node).value);
            break;

        case TYPE_VAR:
            fprintf(file, "<<B><I>%c</I></B>>, color=\"#367ACC\", fillcolor=\"#E0F5FF\", fontcolor=\"#4881CC\"];\n", (char) getValue(node).value);
            break;

        case TYPE_OP:
            fprintf(file, "\"%s\", color=\"#000000\", fillcolor=\"#FFFFFF\", fontcolor=\"#000000\"];\n", OPERATIONS[(int) getValue(node).value]);
            break;

        default:
            assert(! "VALID TYPE");
            break;
    }

    if (getParent(node) != nullptr)
    {
        if (isLeft(node))
        {
            fprintf(file, "\t\"%p\":sw->\"%p\";\n", getParent(node), node);
        }
        else
        {
            fprintf(file, "\t\"%p\":se->\"%p\";\n", getParent(node), node);
        }
    }   

    dumpSubtree(file, getLeft(node));
    dumpSubtree(file, getRight(node));
}