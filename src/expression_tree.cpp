#include <assert.h>
#include <direct.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "expression_tree.h"

#define CHECK_NULL(value, action) if (value == nullptr) { action; }

bool deleteNode            (ETNode* node, va_list args);

bool preOrderTraverse      (ETNode* root, bool (*function)(ETNode* node, va_list args), va_list args);
bool inOrderTraverse       (ETNode* root, bool (*function)(ETNode* node, va_list args), va_list args);
bool postOrderTraverse     (ETNode* root, bool (*function)(ETNode* node, va_list args), va_list args);
    
int  counterFileUpdate     (const char* filename);
void graphDumpSubtree      (FILE* file, ETNode* node);

bool skipFirstParantheses  (ETNode* operationNode);
bool skipSecondParantheses (ETNode* operationNode);
void latexDumpSubtree      (FILE* file, ETNode* node);

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

ExprTree* construct(ExprTree* tree)
{
    CHECK_NULL(tree, return nullptr);

    tree->root = nullptr;

    return tree;
}

ExprTree* newTree()
{
    ExprTree* tree = (ExprTree*) calloc(1, sizeof(ExprTree));
    CHECK_NULL(tree, return nullptr);

    return construct(tree);
}

void destroy(ExprTree* tree)
{
    assert(tree != nullptr);

    if (tree->root != nullptr) { destroySubtree(tree->root); }

    tree->root = nullptr;
}

void destroySubtree(ETNode* root)
{
    assert(root != nullptr);

    postOrderTraverse(root, &deleteNode);
}

void deleteTree(ExprTree* tree)
{
    assert(tree != nullptr);

    destroy(tree);
    free(tree);
}

ETNode* newNode()
{
    return (ETNode*) calloc(1, sizeof(ETNode));
}

ETNode* newNode(NodeType type, ETNodeData data, ETNode* left, ETNode* right)
{
    ETNode* node = newNode();
    CHECK_NULL(node, return nullptr);

    node->type    = type;
    node->data    = data;

    node->parent  = nullptr;
    node->left    = left;
    node->right   = right;

    if (left  != nullptr) { left->parent  = node; }
    if (right != nullptr) { right->parent = node; }

    return node;
}

void deleteNode(ETNode* node)
{
    assert(node != nullptr);

    node->parent = nullptr;
    node->left   = nullptr;
    node->right  = nullptr;

    free(node);
}

bool deleteNode(ETNode* node, va_list args)
{
    deleteNode(node);

    return ET_TRAVERSE_RUN;
}

void copyNode(ETNode* dest, const ETNode* src)
{
    assert(dest != nullptr);
    assert(src  != nullptr);

    dest->type   = src->type;
    dest->data   = src->data;
    dest->parent = src->parent;
    dest->left   = src->left;
    dest->right  = src->right;

    if (src->left  != nullptr) { src->left->parent  = dest; }
    if (src->right != nullptr) { src->right->parent = dest; }
}

ETNode* copyTree(const ETNode* node)
{
    if (node == nullptr) { return nullptr; }

    return newNode(node->type, node->data, copyTree(node->left), copyTree(node->right));
}

#define TRAVERSE_SUBTREE(traverse, side) if (traverse(root->side, function, args)  == !ET_TRAVERSE_RUN) \
                                         {                                                              \
                                             return !ET_TRAVERSE_RUN;                                   \
                                         }

void preOrderTraverse(ETNode* root, bool (*function)(ETNode* node, va_list args), ...)
{
    CHECK_NULL(root, return);

    va_list args = {};
    va_start(args, &function);

    preOrderTraverse(root, function, args);

    va_end(args);
}

bool preOrderTraverse(ETNode* root, bool (*function)(ETNode* node, va_list args), va_list args)
{
    CHECK_NULL(root, return ET_TRAVERSE_RUN);

    if (function(root, args) == !ET_TRAVERSE_RUN) { return !ET_TRAVERSE_RUN; };

    TRAVERSE_SUBTREE(preOrderTraverse, left);
    TRAVERSE_SUBTREE(preOrderTraverse, right);

    return ET_TRAVERSE_RUN;
}

void inOrderTraverse(ETNode* root, bool (*function)(ETNode* node, va_list args), ...)
{
    CHECK_NULL(root, return);

    va_list args = {};
    va_start(args, &function);

    inOrderTraverse(root, function, args);

    va_end(args);
}

bool inOrderTraverse(ETNode* root, bool (*function)(ETNode* node, va_list args), va_list args)
{
    CHECK_NULL(root, return ET_TRAVERSE_RUN);

    TRAVERSE_SUBTREE(inOrderTraverse, left);

    if (function(root, args) == !ET_TRAVERSE_RUN) { return !ET_TRAVERSE_RUN; };

    TRAVERSE_SUBTREE(inOrderTraverse, right);

    return ET_TRAVERSE_RUN;
}

void postOrderTraverse(ETNode* root, bool (*function)(ETNode* node, va_list args), ...)
{
    CHECK_NULL(root, return);

    va_list args = {};
    va_start(args, &function);

    postOrderTraverse(root, function, args);

    va_end(args);
}

bool postOrderTraverse(ETNode* root, bool (*function)(ETNode* node, va_list args), va_list args)
{
    CHECK_NULL(root, return ET_TRAVERSE_RUN);

    TRAVERSE_SUBTREE(postOrderTraverse, left);
    TRAVERSE_SUBTREE(postOrderTraverse, right);

    if (function(root, args) == !ET_TRAVERSE_RUN) { return !ET_TRAVERSE_RUN; };

    return ET_TRAVERSE_RUN;
}

bool isLeft(const ETNode* node)
{
    assert(node         != nullptr);
    assert(node->parent != nullptr);

    return node == node->parent->left;
}

bool isTypeNumber(const ETNode* node)
{
    assert(node != nullptr);

    return node->type == TYPE_NUMBER;
}

bool isTypeVar(const ETNode* node)
{
    assert(node != nullptr);

    return node->type == TYPE_VAR;
}

bool isTypeOp(const ETNode* node)
{
    assert(node != nullptr);

    return node->type == TYPE_OP;
}

double evaluateSubtree(ETNode* root)
{
    if (isTypeOp(root))
    {
        Operation operation = root->data.op;

        if (isOperationUnary(operation))
        {
            return evaluateUnary(operation, evaluateSubtree(root->right));
        }

        return evaluateBinary(operation, evaluateSubtree(root->left), evaluateSubtree(root->right));
    }
    else if (isTypeNumber(root))
    {
        return root->data.number;
    }

    return 0;
}

void substitute(ETNode* root, char variable, double value)
{
    if (root == nullptr) { return; }

    assert(isVariable(variable));

    if (isTypeVar(root) && root->data.var == variable)
    {
        root->type = TYPE_NUMBER;
        root->data.number = value;
    }

    substitute(root->left,  variable, value);
    substitute(root->right, variable, value);
}

bool hasVariable(ETNode* root, char variable)
{
    if (root == nullptr) { return false; }

    if (root->type == TYPE_VAR && root->data.var == variable) { return true; }

    return hasVariable(root->left, variable) || hasVariable(root->right, variable);
}

void setData(ETNode* node, NodeType type, ETNodeData data)
{
    assert(node != nullptr);

    node->type = type;
    node->data = data;
}

void setData(ETNode* node, double number)
{
    assert(node != nullptr);

    node->type        = TYPE_NUMBER;
    node->data.number = number;
}

void setData(ETNode* node, char var)
{
    assert(node != nullptr);

    node->type     = TYPE_VAR;
    node->data.var = var;
}

void setData(ETNode* node, Operation op)
{
    assert(node != nullptr);

    node->type    = TYPE_OP;
    node->data.op = op;
}

int counterFileUpdate(const char* filename)
{
    assert(filename != nullptr);

    int count = 0;

    FILE* counterFile = fopen(filename, "r+");
    if (counterFile == nullptr)
    {
        counterFile = fopen(filename, "w");
        count = -1;
    }
    else
    {
        fscanf(counterFile, "%u", &count);
        fseek(counterFile, 0, SEEK_SET);
    }

    fprintf(counterFile, "%u", count + 1);
    fclose(counterFile);

    return count;
}

void graphDump(ExprTree* tree)
{
    assert(tree != nullptr);

    int count = counterFileUpdate("log/tree_dumps/graph/count.cnt");

    char textFilename[128] = {};
    snprintf(textFilename, sizeof(textFilename), "%s%u.txt", "log/tree_dumps/graph/text/tree", count);

    char imageFilename[128] = {};
    snprintf(imageFilename, sizeof(imageFilename), "%s%u.svg", "log/tree_dumps/graph/img/tree", count);

    FILE* file = fopen(textFilename, "w");
    assert(file != nullptr);

    fprintf(file,
            "digraph structs {\n"
            "\tnode [shape=\"circle\", style=\"filled\", fontcolor=\"#DCDCDC\", fillcolor=\"#2F4F4F\"];\n\n");

    if (tree->root == nullptr) { fprintf(file, "\t\"ROOT\" [label = \"Empty database\"];\n"); }
    else                       { graphDumpSubtree(file, tree->root); }

    fprintf(file, "}");

    fclose(file);

    char dotCmd[256] = {};

    snprintf(dotCmd, sizeof(dotCmd), "dot -Tsvg %s -o %s", textFilename, imageFilename);
    system(dotCmd);

    snprintf(dotCmd, sizeof(dotCmd), "start %s", imageFilename);
    system(dotCmd);
}

void graphDumpSubtree(FILE* file, ETNode* node)
{
    assert(file != nullptr);
    
    if (node == nullptr) { return; }

    fprintf(file, "\t\"%p\" [label=", node);

    NodeType    type      = node->type; 
    const char* constName = nullptr;
    switch (type)
    {
        case TYPE_NUMBER:
            constName = getConstantName(node->data.number);
            if (constName != nullptr)
            {
                fprintf(file, "<<B><I>%s</I></B>>, color=\"#75A673\", fillcolor=\"#EDFFED\", fontcolor=\"#75A673\"];\n", constName);
            }
            else
            {
                fprintf(file, "\"%lg\", color=\"#D2691E\", fillcolor=\"#FFFAEF\", fontcolor=\"#FF7F50\"];\n", node->data.number);
            }

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

    graphDumpSubtree(file, node->left);
    graphDumpSubtree(file, node->right);
}

void latexDump(ExprTree* tree)
{
    assert(tree       != nullptr);
    assert(tree->root != nullptr);

    int count = counterFileUpdate("log/tree_dumps/latex/count.cnt");

    const char* texDir = "log/tree_dumps/latex/text/";
    const char* pdfDir = "log/tree_dumps/latex/pdf";

    char filename[128] = {};
    snprintf(filename, sizeof(filename), "%stree%u.tex", texDir, count);

    FILE* file = fopen(filename, "w");
    assert(file != nullptr);

    fprintf(file, "\\documentclass{article}\n"
                  "\\begin{document}\n"
                  "$");

    latexDumpSubtree(file, tree->root);


    fprintf(file, "$\n"
                  "\\end{document}");

    fclose(file);

    char dir[128] = {};
    _getcwd(dir, sizeof(dir));

    char cmd[256] = {};
    snprintf(cmd, sizeof(cmd), "pdflatex --output-directory=%s/%s --job-name=tree%u %s > nul", dir, pdfDir, count, filename);
    system(cmd);

    snprintf(cmd, sizeof(cmd), "start %s/tree%u.pdf", pdfDir, count);
    system(cmd);
}

#define IS_ADD         op == OP_ADD
#define IS_SUB         op == OP_SUB
#define IS_MUL         op == OP_MUL
#define IS_DIV         op == OP_DIV
#define IS_POW         op == OP_POW
#define IS_EXP         op == OP_EXP
#define IS_ADD_SUB_MUL (op == OP_ADD || op == OP_SUB || op == OP_MUL)

bool skipFirstParantheses(ETNode* operationNode)
{
    assert(operationNode       != nullptr);
    assert(operationNode->type == TYPE_OP);
    assert(operationNode->left != nullptr);

    Operation op = operationNode->data.op;

    return operationNode->left->type != TYPE_OP  || 
           IS_ADD                                || 
           IS_SUB                                ||
           (IS_MUL && operationNode->left->type == TYPE_OP && isOperationUnary(operationNode->left->data.op));
}

bool skipSecondParantheses(ETNode* operationNode)
{
    assert(operationNode        != nullptr);
    assert(operationNode->type  == TYPE_OP);
    assert(operationNode->right != nullptr);

    Operation op = operationNode->data.op;

    return operationNode->right->type != TYPE_OP  || 
           IS_POW                                 || 
           IS_EXP                                 || 
           (IS_ADD_SUB_MUL && operationNode->right->type == TYPE_OP && isOperationUnary(operationNode->right->data.op));
}

#undef IS_ADD        
#undef IS_SUB        
#undef IS_MUL        
#undef IS_DIV        
#undef IS_POW        
#undef IS_EXP        
#undef IS_ADD_SUB_MUL

void latexDumpSubtree(FILE* file, ETNode* node)
{
    assert(file != nullptr);
    if (node == nullptr) { return; }

    if (node->type == TYPE_OP && !isOperationUnary(node->data.op))
    {
        if (node->data.op == OP_DIV)
        {
            fprintf(file, "\\frac{");
            latexDumpSubtree(file, node->left);

            fprintf(file, "}{");
            latexDumpSubtree(file, node->right);
            fprintf(file, "}");

            return;
        }   

        bool skipFirst = skipFirstParantheses(node);

        fprintf(file, "{");
        if (!skipFirst) { fprintf(file, "("); }
        
        latexDumpSubtree(file, node->left);

        if (!skipFirst) { fprintf(file, ")"); }
        fprintf(file, "} ");
    }

    if (node->type == TYPE_NUMBER)
    {
        const char* constant = getConstantName(node->data.number);

        if (constant == nullptr) 
        { 
            fprintf(file, "%lg", node->data.number);
        }
        else
        {
            if (strlen(constant) > 1) { fprintf(file, "\\%s", constant); }
            else                      { fprintf(file, "%s",   constant); }
        }
    }  
    else if (node->type == TYPE_VAR)
    {
        fprintf(file, "%c", node->data.var);
    }
    else if (node->type == TYPE_OP)
    {
        bool skipSecond = skipSecondParantheses(node);

        switch (node->data.op)
        {
            case OP_ADD: { fprintf(file, " + ");      break; }
            case OP_SUB: { fprintf(file, " - ");      break; }
            case OP_MUL: { fprintf(file, " \\cdot "); break; }
            case OP_POW: { fprintf(file, " ^ ");      break; }
            case OP_LOG: { fprintf(file, " \\ln");    break; }
            case OP_EXP: { fprintf(file, "{e}^");     break; }
            case OP_SIN: { fprintf(file, "\\sin");    break; } 
            case OP_COS: { fprintf(file, "\\cos");    break; }
            case OP_TAN: { fprintf(file, "\\tan");    break; }

            default:     { break; }
        }

        fprintf(file, "{");
        if (!skipSecond) { fprintf(file, "("); }

        latexDumpSubtree(file, node->right);
        
        if (!skipSecond) { fprintf(file, ")"); }
        fprintf(file, "}");
    }
    else
    {
        fprintf(file, "ERROR: invalid node type");
    }
}