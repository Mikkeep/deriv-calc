#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utilib.h"
#include "expression_tree.h"
#include "../libs/log_generator.h"
#include "../libs/file_manager.h"

const char*  EMPTY_SPACE_DELIMS = " \t";

bool      loadExpression   (ExprTree* tree, const char* filename);
bool      parseExpression  (ETNode* root, char* expr, size_t exprLength);
char*     skipDelims       (char* str);
int       processNumber    (const char* token, double* number);
Operation processOperation (char* token);
int       processConstant  (char* token, double* constant);
bool      isVariable       (char token);

ETNode*   differentiate    (ETNode* root);
bool      hasVariable      (ETNode* root, char variable);

void      simplifyTree     (ETNode* root);
void      simplifyNode     (ETNode* node, NodeType newType, ETNodeData data);
bool      simplifyOps      (ETNode* root);
bool      simplifyTrig     (ETNode* node);
bool      precalcConsts    (ETNode* root);

// TODO: delete if won't be needed
double    evaluateUnary    (Operation operation, double arg);
double    evaluateBinary   (Operation operation, double arg1, double arg2);

void      dump             (ExprTree* tree);
void      dumpSubtree      (FILE* file, ETNode* node);

int main(int argc, char* argv[])
{
    LG_Init();

    ExprTree exprTree = {};
    construct(&exprTree);

    if (!loadExpression(&exprTree, argv[1])) 
    { 
        LG_LogMessage("loadExpression() returned false.", LG_STYLE_CLASS_ERROR);

        destroy(&exprTree);
        LG_Close();

        return -1; 
    }

    printf("[e = %lg, pi = %lg]\n", E_CONST, PI_CONST);

    ExprTree derivTree = {};
    construct(&derivTree);

    derivTree.root = differentiate(exprTree.root);

    // precalcConsts(derivTree.root);
    // simplifyTree(derivTree.root);
    dump(&exprTree);
    dump(&derivTree);

    // ExprTree test = {};
    // construct(&test);
    // test.root = copyTree(exprTree.root);
    // dump(&test);
    // destroy(&test);

    destroy(&exprTree);
    destroy(&derivTree);

    LG_Close();
}

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

    char* exprBuffer = (char*) calloc(getFileSize(filename), sizeof(char));
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
    else
    {
        exprLength--;
    }

    tree->root = newNode();
    
    if (!parseExpression(tree->root, exprBuffer, fileSize - 1))
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

    while (*curr != '\n' && (size_t) (curr - expr) < exprLength - 1)
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
        else if ((constantLength = processNumber(curr, &constant)) > 0)
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

#define LEFT  root->left
#define RIGHT root->right

#define UNARY_OP(operation, arg)  *newNode(TYPE_OP, { .op = OP_##operation }, NULL,             (ETNode*) &(arg))
#define BINARY_OP(operation)      *newNode(TYPE_OP, { .op = OP_##operation }, (ETNode*) &tree1, (ETNode*) &tree2)

// #define ADD(left, right) BINARY_OP(ADD)
// #define SUB(left, right) BINARY_OP(SUB)
// #define MUL(left, right) BINARY_OP(MUL)
// #define DIV(left, right) BINARY_OP(DIV)
// #define POW(left, right) BINARY_OP(POW)

#define LOG(arg) UNARY_OP(LOG, arg)
#define EXP(arg) UNARY_OP(EXP, arg)

#define SIN(arg) UNARY_OP(SIN, arg)
#define COS(arg) UNARY_OP(COS, arg)
#define TAN(arg) UNARY_OP(TAN, arg)

#define NUM(num) (*newNode(TYPE_NUMBER, { num }, nullptr, nullptr))

#define dL (*differentiate(LEFT))
#define dR (*differentiate(RIGHT))

#define L (*copyTree(LEFT))
#define R (*copyTree(RIGHT))

#define RETURN(arg) return &(arg)

ETNode& operator + (const ETNode& tree1, const ETNode& tree2)
{
    return BINARY_OP(ADD);
}

ETNode& operator - (const ETNode& tree1, const ETNode& tree2)
{
    return BINARY_OP(SUB);
}

ETNode& operator * (const ETNode& tree1, const ETNode& tree2)
{
    return BINARY_OP(MUL);
}

ETNode& operator / (const ETNode& tree1, const ETNode& tree2)
{
    return BINARY_OP(DIV);
}

ETNode& operator ^ (const ETNode& tree1, const ETNode& tree2)
{
    return BINARY_OP(POW);
}

ETNode* differentiate(ETNode* root)
{
    assert(root != nullptr);

    Operation operation = isTypeOp(root) ? root->data.op : OP_INVALID;

    // TODO: make at least a bit more eye-pleasing
    if (root->type == TYPE_NUMBER || root->type == TYPE_CONST || 
        (isTypeOp(root) && hasVariable(root, 'x')))
    {
        return newNode(TYPE_CONST, { 0.0 }, nullptr, nullptr);
    }

    if (root->type == TYPE_VAR)
    {
        return newNode(TYPE_NUMBER, { (double) (root->data.var == 'x') }, nullptr, nullptr);
    }

    assert(isTypeOp(root));

    switch (operation)
    {
        // case OP_ADD: return ADD(dL, dR);
        // case OP_SUB: return SUB(dL, dR);

        // // case OP_MUL: return newNode(TYPE_OP, { OP_ADD }, newNode(TYPE_OP, { OP_MUL }, differentiate(root->left), copyTree(root->right)), 
        // //                                                  newNode(TYPE_OP, { OP_MUL }, copyTree(root->left), differentiate(root->right)));

        // case OP_MUL: return ADD(MUL(dL, R), MUL(L, dR));
        // case OP_DIV: return DIV(SUB(MULL(dL, R), MULL(L, dR)), POW(R, NUM(2)));

        case OP_ADD: RETURN(dL + dR);
        case OP_SUB: RETURN(dL - dR);

        // case OP_MUL: return newNode(TYPE_OP, { OP_ADD }, newNode(TYPE_OP, { OP_MUL }, differentiate(root->left), copyTree(root->right)), 
        //                                                  newNode(TYPE_OP, { OP_MUL }, copyTree(root->left), differentiate(root->right)));

        case OP_MUL: RETURN((dL * R) + (L * dR));
        case OP_DIV: RETURN((dL * R - L * dR) / (R ^ NUM(2)));

        case OP_POW: return hasVariable(root, 'x') ? &(R * (L ^ (R - NUM(1))) * dL) : differentiate(&EXP(R * LOG(L)));

        case OP_LOG: RETURN((NUM(1) / R) * dR);
        case OP_EXP: RETURN(EXP(R) * dR);    

        case OP_SIN: RETURN(COS(R) * dR);
        case OP_COS: RETURN(NUM(-1) * SIN(R) * dR);
        case OP_TAN: RETURN((NUM(1) / (COS(R) ^ NUM(2))) * dR);

        default:     return nullptr;
    }

    return nullptr;
}

bool hasVariable(ETNode* root, char variable)
{
    if (root == nullptr) { return false; }

    if (root->type == TYPE_VAR && root->data.var == variable) { return true; }

    return false;
}

void simplifyTree(ETNode* root)
{
    assert(root != nullptr);

    while (precalcConsts(root) || simplifyOps(root)) ;
}

void simplifyNode(ETNode* node, NodeType newType, ETNodeData data)
{
    assert(node != nullptr);

    setData(node, newType, data);

    if (node->left  != nullptr) { deleteNode(node->left); }
    if (node->right != nullptr) { deleteNode(node->right); }

    node->left  = nullptr;
    node->right = nullptr;
}

#define SIMPLIFY(side) if (isIdentityType(SIMPLIFY_EXPRS[i]))                            \
                           simplifyNode(root, root->side->type, root->side->data);       \
                       else                                                              \
                           simplifyNode(root, TYPE_NUMBER, { SIMPLIFY_EXPRS[i].result });

bool simplifyOps(ETNode* root)
{
    if (root == nullptr) { return false; }

    if (isTypeOp(root))
    {
        Operation    operation = root->data.op;
        SimplifyExpr exprType  = {}; 

        for (size_t i = 0; i < SIMPLIFY_EXPRS_COUNT; i++)
        {
            exprType = SIMPLIFY_EXPRS[i];

            if (operation != SIMPLIFY_EXPRS[i].operation) { continue; }

            if (compare(root->right->data.number, SIMPLIFY_EXPRS[i].arg) == 0)
            {
                SIMPLIFY(left)
                return true;
            }

            if (SIMPLIFY_EXPRS[i].commutative && compare(root->left->data.number, SIMPLIFY_EXPRS[i].arg) == 0)
            {
                SIMPLIFY(right)
                return true;
            }
        }
    }

    return simplifyOps(root->left) || simplifyOps(root->right);
}

#undef SIMPLIFY

bool simplifyTrig(ETNode* node)
{
    // if (node == nullptr || node->type != TYPE_OP || !isTrigOp(node->data.op) || hasVariable(node->right, 'x'))
    // { 
    //     return false; 
    // }

    // // TODO: temporary, add features beside precalculating constants
    // if (node->right->type != TYPE_NUMBER && node->right->type != TYPE_CONST) { return false; }

    // double number = node->right->data.number;
    // // double number = isTypeNumber(node) ? node->right->data.number : node->right->data.constant;

    // // TODO: other cases like for example multiples of pi like pi/2, pi/6 etc
    // if (compare(number, 0) == 0 || compare(number, PI_CONST) == 0)
    // {
    //     simplifyNode(node, TYPE_NUMBER, evaluateUnary(node->type, number));
    //     return true;
    // } 

    return false;
}

// just garbage value that has to not be equal to either +-1 or 0
const double GARBAGE_VALUE_FOR_PRECALC = 22022002;

//-----------------------------------------------------------------------------
//! Precalculates all expressions with constants (e.g. '2+19' -> '21').
//!
//! @param [in] root
//!
//! @return whether or not there have been any changes in the subtree.
//-----------------------------------------------------------------------------
bool precalcConsts(ETNode* root)
{
    if (root == nullptr) { return false; }

    ETNode* left   = root->left;
    ETNode* right  = root->right;

    ETNode* parent = root->parent;

    if (isTypeOp(root) && right->type == TYPE_CONST)
    {
        Operation operation = root->data.op;

        // TODO: process cases like 6/2 and 2/3 differently!
        if ((operation == OP_ADD || operation == OP_SUB || operation == OP_MUL || operation == OP_DIV) && 
            isTypeNumber(left) && isTypeNumber(right))
        {
            simplifyNode(root, TYPE_NUMBER, { evaluateBinary(operation, left->data.number, right->data.number) });
            return true;
        }

        double value = GARBAGE_VALUE_FOR_PRECALC; 

        if (isOperationUnary(operation) && isNumeric(root->right))
        {
            value = evaluateUnary(operation, root->right->data.number);
        }
        else if (!isOperationUnary(operation) && isNumeric(root->left) && isNumeric(root->right))
        {
            value = evaluateBinary(operation, root->left->data.number, root->right->data.number);
        }

        if (value != GARBAGE_VALUE_FOR_PRECALC)
        {
            simplifyNode(root, TYPE_NUMBER, { value });
            return true;
        }
    }

    return precalcConsts(root->left) || precalcConsts(root->right);
}

double evaluateUnary(Operation operation, double arg)
{
    assert(isOperationUnary(operation));

    switch (operation)
    {
        case OP_LOG: return log(arg); 
        case OP_EXP: return exp(arg);
        case OP_SIN: return sin(arg);
        case OP_COS: return cos(arg);
        case OP_TAN: return tan(arg);

        default: return NAN;
    }

    return NAN;
}

double evaluateBinary(Operation operation, double arg1, double arg2)
{
    assert(!isOperationUnary(operation));
    assert(operation != OP_INVALID);

    switch (operation)
    {
        case OP_ADD: return arg1 + arg2; 
        case OP_SUB: return arg1 - arg2;
        case OP_MUL: return arg1 * arg2;
        case OP_DIV: return arg1 / arg2;
        case OP_POW: return pow(arg1, arg2);

        default: return NAN;
    }

    return NAN;
}

void dump(ExprTree* tree)
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

    if (tree->root == nullptr)
    {
        fprintf(file,
                "\t\"ROOT\" [label = \"Empty database\"];\n");
    }
    else
    {
        dumpSubtree(file, tree->root);
    }

    fprintf(file, "}");

    fclose(file);

    char dotCmd[256] = {};

    snprintf(dotCmd, 256, "dot -Tsvg %s -o %s", textFilename, imageFilename);
    system(dotCmd);

    snprintf(dotCmd, 256, "start %s", imageFilename);
    system(dotCmd);
}

void dumpSubtree(FILE* file, ETNode* node)
{
    assert(file != nullptr);
    
    if (node == nullptr) { return; }

    fprintf(file, "\t\"%p\" [label=", node);

    NodeType type = node->type; 
    switch (type)
    {
        case TYPE_NUMBER:
            fprintf(file, "\"%lg\", color=\"#D2691E\", fillcolor=\"#FFFAEF\", fontcolor=\"#FF7F50\"];\n", node->data.number);
            break;

        case TYPE_CONST:
            fprintf(file, "\"%s\", color=\"#D2691E\", fillcolor=\"#FFFAEF\", fontcolor=\"#FF7F50\"];\n", getConstantName(node->data.constant));
            break;

        case TYPE_VAR:
            fprintf(file, "<<B><I>%c</I></B>>, color=\"#367ACC\", fillcolor=\"#E0F5FF\", fontcolor=\"#4881CC\"];\n", node->data.var);
            break;

        case TYPE_OP:
            fprintf(file, "\"%s\", color=\"#000000\", fillcolor=\"#FFFFFF\", fontcolor=\"#000000\"];\n", OPERATIONS[node->data.op]);
            break;

        default:
            assert(! "VALID TYPE");
            break;
    }

    if (node->parent != nullptr)
    {
        if (isLeft(node))
        {
            fprintf(file, "\t\"%p\":sw->\"%p\";\n", node->parent, node);
        }
        else
        {
            fprintf(file, "\t\"%p\":se->\"%p\";\n", node->parent, node);
        }
    }   

    dumpSubtree(file, node->left);
    dumpSubtree(file, node->right);
}