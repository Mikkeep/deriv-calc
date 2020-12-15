#include <assert.h>
#include <direct.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "funnyentific_paper.h"
#include "expression_simplifier.h"
#include "utilib.h"

const size_t MAX_FILENAME_LENGTH   = 256;
const size_t MAX_COMMAND_LENGTH    = 512;

const size_t MIN_SUBSTITUTION_SIZE = 15;
const size_t MAX_SUBSTITUTION_SIZE = 20;
const size_t MAX_SUBSTITUTIONS     = 16;

const size_t MESSAGES_DERIVATIVE_NUMBER_COUNT = 4;
const char*  MESSAGES_DERIVATIVE_NUMBER[] = { "Derivative of a constant is always zero",
                                              "Even a nine-year-old knows it's 0",
                                              "It's obviously zero",
                                              "The derivative of this is equal to my chances of not being expelled..." }; 

const size_t MESSAGES_DERIVATIVE_VAR_COUNT = 4;
const char*  MESSAGES_DERIVATIVE_VAR[] = { "Derivative of x is always 1",
                                           "Everyone knows that this is 1",
                                           "It's obviously 1",
                                           "The derivative of this is the number of the best dormitory ever." }; 

const size_t MESSAGES_DERIVATIVE_ADD_COUNT = 3;
const char*  MESSAGES_DERIVATIVE_ADD[] = { "Derivative of a sum is the sum of the terms",
                                           "Derivatives are linear because limits are, so",
                                           "It's obviously the following" }; 

const size_t MESSAGES_DERIVATIVE_MUL_COUNT = 3;
const char*  MESSAGES_DERIVATIVE_MUL[] = { "Using the product rule",
                                           "Hope it won't look too ugly",
                                           "From our experience it's the following" }; 

const size_t MESSAGES_DERIVATIVE_DIV_COUNT = 3;
const char*  MESSAGES_DERIVATIVE_DIV[] = { "Using the quotient rule",
                                           "So, it's going to be",
                                           "Assuming we all understand calculus the derivative is" }; 

const size_t MESSAGES_DERIVATIVE_POW_COUNT = 3;
const char*  MESSAGES_DERIVATIVE_POW[] = { "Using the rule for the derivative of an exponent",
                                           "If the only thing you'll remember from me is to use exponents, my goal has been achieved (c) Vadim Vitalyevich",
                                           "Supposing we are all on the same page" }; 

const size_t MESSAGES_DERIVATIVE_UNR_COUNT = 3;
const char*  MESSAGES_DERIVATIVE_UNR[] = { "If you don't believe in the following, you can use wolfram alpha",
                                           "By looking at the table of the derivatives",
                                           "I guess the following is pretty self-explanatory" }; 

const size_t MESSAGES_SIMPLIFICATION_COUNT = 9;
const char*  MESSAGES_SIMPLIFICATION[] = { "The stars tell me it's true",
                                           "No, I didn't cheat, I know the material",
                                           "We can see that",
                                           "How come making something easier makes life seem harder?..",
                                           "Let's make things simpler",
                                           "Lol, it turns out, that",
                                           "How could I not see that",
                                           "May the Divines spare my soul",
                                           "And this is" };

void    writeHeader           (FILE* file);
void    writeFooter           (FILE* file);
void    writeBody             (FILE* file, ETNode* root);
void    writeSection          (FILE* file, const char* name);
void    writeRandomMessage    (FILE* file, const char** messages, size_t count);

ETNode* writeDerivative       (FILE* file, ETNode* root);
ETNode* writeDerivativeNumber (FILE* file, ETNode* root);
ETNode* writeDerivativeVar    (FILE* file, ETNode* root);
ETNode* writeDerivativeAddSub (FILE* file, ETNode* root);
ETNode* writeDerivativeMul    (FILE* file, ETNode* root);
ETNode* writeDerivativeDiv    (FILE* file, ETNode* root);
ETNode* writeDerivativePow    (FILE* file, ETNode* root);
ETNode* writeDerivativeLog    (FILE* file, ETNode* root);
ETNode* writeDerivativeExp    (FILE* file, ETNode* root);
ETNode* writeDerivativeSin    (FILE* file, ETNode* root);
ETNode* writeDerivativeCos    (FILE* file, ETNode* root);
ETNode* writeDerivativeTan    (FILE* file, ETNode* root);

void    writeResult           (FILE* file, ETNode* root);
void    makeSubstitutions     (ETNode* node, Substitution* substitutions, size_t* substitutionsCount);

void    simplifyNode          (FILE* file, ETNode* node, NodeType newType, ETNodeData data);
void    simplifyNode          (FILE* file, ETNode* node, ETNode* child);
bool    simplifyOps           (FILE* file, ETNode* root);
bool    precalcConstExprs     (FILE* file, ETNode* root);

void makeScientificPaper(ETNode* root)
{
    assert(root != nullptr);

    int count = counterFileUpdate("log/tree_dumps/latex/count.cnt");

    const char* texDir = "log/tree_dumps/latex/text/";
    const char* pdfDir = "log/tree_dumps/latex/pdf";

    char filename[MAX_FILENAME_LENGTH] = {};
    snprintf(filename, sizeof(filename), "%stree%u.tex", texDir, count);

    FILE* file = fopen(filename, "w");
    assert(file != nullptr);

    writeHeader(file);
    writeBody(file, root);
    writeFooter(file);

    fclose(file);

    char dir[MAX_COMMAND_LENGTH] = {};
    _getcwd(dir, sizeof(dir));

    char cmd[MAX_COMMAND_LENGTH] = {};
    snprintf(cmd, sizeof(cmd), "pdflatex --output-directory=%s/%s --job-name=tree%u %s", dir, pdfDir, count, filename);
    system(cmd);

    snprintf(cmd, sizeof(cmd), "start %s/tree%u.pdf", pdfDir, count);
    system(cmd);
}

void writeHeader(FILE* file)
{
    assert(file != nullptr);

    fprintf(file, "\\documentclass[10pt]{article}\n"
                  "\\usepackage{graphicx}\n"
                  "\\usepackage[a4paper, total={6in, 8in}]{geometry}"
                  "\\usepackage{hyperref}\n"
                  "\\usepackage[latin1]{inputenc}\n"
                  "\\usepackage{indentfirst}\n"
                  "\\title{Paper on the most elegant way of differentiating}\n"
                  "\\author{Mochalov Nikita}\n"
                  "\\begin{document}\n"
                  "\\maketitle\n"
                  "This particular paper is about, arguably, the most pivotal concept in calculus as a whole - derivatives."
                  "Calculus, known in its early history as infinitesimal calculus, is a mathematical discipline focused on limits, "
                  "continuity, derivatives, integrals, and infinite series. Isaac Newton and Gottfried Wilhelm Leibniz independently "
                  "developed the theory of infinitesimal calculus in the later 17th century. By the end of the 17th century, each "
                  "scholar claimed that the other had stolen his work, and the Leibniz-Newton calculus controversy continued until "
                  "the death of Leibniz in 1716.\n\n"
                  "The derivative of a function of a real variable measures the sensitivity to change "
                  "of the function value (output value) with respect to a change in its argument (input value). Derivatives are a "
                  "fundamental tool of calculus. \n\n"
                  "Therefore, the limit of the difference quotient as h approaches zero, if it exists, "
                  "should represent the slope of the tangent line to $(a, f(a))$. This limit is defined to be the derivative of the "
                  "function $f$ at $a$: \n\n"
                  "$$f^{'}(a)=\\lim_{h\\to0}{\\frac{f(a+h)-f(a)}{h}}$$\n\n");
}

void writeFooter(FILE* file)
{
    assert(file != nullptr);

    fprintf(file, "\\end{document}");
}

void writeBody(FILE* file, ETNode* root)
{
    assert(file != nullptr);
    assert(root != nullptr);

    writeSection(file, "Taking the derivative");

    srand(time(NULL)); 

    // while (precalcConstExprs(file, root) || simplifyOps(file, root))
    //     ;

    ETNode* derivative = writeDerivative(file, root);
    assert(derivative != nullptr);

    fprintf(file, "So the result is:\n\n$$(");
    latexDumpSubtree(file, root);
    fprintf(file, ")'=");
    latexDumpSubtree(file, derivative);
    fprintf(file, "$$\n\n");

    while (precalcConstExprs(file, derivative) || simplifyOps(file, derivative))
        ;

    fprintf(file, "So the result is:\n\n$$(");
    latexDumpSubtree(file, root);
    fprintf(file, ")'=");

    writeResult(file, derivative);

    writeSection(file, "References");
    fprintf(file, "\\begin{enumerate}\n"
                  "\\item Calculus textbook\n"
                  "\\item Wikipedia (oh yeah, this devil's den)\n"
                  "\\item Sbornik zadach on calculus by Kudryavzev\n"
                  "\\item Moskalev Nikita (makes not obvious things ochevom)\n"
                  "\\item Mikhail Shishatsky (the best mentor of all time)\n"
                  "\\item My github https://github.com/tralf-strues\n"
                  "\\end{enumerate}\n");
}

void writeSection(FILE* file, const char* name)
{
    assert(file != nullptr);
    assert(name != nullptr);

    fprintf(file, "\\section{%s}\n\n", name);
}

void writeRandomMessage(FILE* file, const char** messages, size_t count)
{
    assert(file     != nullptr);
    assert(messages != nullptr);

    fprintf(file, "%s: \n\n", messages[rand() % count]);
}

#define LEFT  root->left
#define RIGHT root->right

#define dL (*writeDerivative(file, LEFT))
#define dR (*writeDerivative(file, RIGHT))
#define L  (*copyTree(LEFT))
#define R  (*copyTree(RIGHT))

#define RETURN(arg) return &(arg)

ETNode* writeDerivative(FILE* file, ETNode* root)
{
    assert(file != nullptr);
    assert(root != nullptr);

    Operation operation = isTypeOp(root) ? root->data.op : OP_INVALID;

    if (root->type == TYPE_NUMBER || (isTypeOp(root) && !hasVariable(root, 'x')))
    {
        return writeDerivativeNumber(file, root);
    }

    if (root->type == TYPE_VAR)
    {
        return writeDerivativeVar(file, root);
    }

    assert(isTypeOp(root));

    switch (operation)
    {
        case OP_ADD: return writeDerivativeAddSub(file, root);
        case OP_SUB: return writeDerivativeAddSub(file, root);

        case OP_MUL: return writeDerivativeMul(file, root);
        case OP_DIV: return writeDerivativeDiv(file, root);

        case OP_POW: return writeDerivativePow(file, root);

        case OP_LOG: return writeDerivativeLog(file, root);
        case OP_EXP: return writeDerivativeExp(file, root);    

        case OP_SIN: return writeDerivativeSin(file, root);
        case OP_COS: return writeDerivativeCos(file, root);
        case OP_TAN: return writeDerivativeTan(file, root);

        default:     return nullptr;
    }

    return nullptr;
}

ETNode* writeDerivativeNumber(FILE* file, ETNode* root)
{
    assert(file != nullptr);
    assert(root != nullptr);

    writeRandomMessage(file, MESSAGES_DERIVATIVE_NUMBER, MESSAGES_DERIVATIVE_NUMBER_COUNT);

    fprintf(file, "$$(");
    latexDumpSubtree(file, root);
    fprintf(file, ")'=");

    ETNode* derivative = newNode(TYPE_NUMBER, { 0.0 }, nullptr, nullptr);
    latexDumpSubtree(file, derivative);
    fprintf(file, "$$\n\n");

    return derivative;
}

ETNode* writeDerivativeVar(FILE* file, ETNode* root)
{
    assert(file != nullptr);
    assert(root != nullptr);

    writeRandomMessage(file, MESSAGES_DERIVATIVE_VAR, MESSAGES_DERIVATIVE_VAR_COUNT);

    fprintf(file, "$$");
    latexDumpSubtree(file, root);
    fprintf(file, "'=");

    ETNode* derivative = newNode(TYPE_NUMBER, { 1.0 }, nullptr, nullptr);
    latexDumpSubtree(file, derivative);
    fprintf(file, "$$\n\n");

    return derivative;
}

ETNode* writeDerivativeAddSub(FILE* file, ETNode* root)
{
    assert(file != nullptr);
    assert(root != nullptr);

    writeRandomMessage(file, MESSAGES_DERIVATIVE_ADD, MESSAGES_DERIVATIVE_ADD_COUNT);

    fprintf(file, "$$(");
    latexDumpSubtree(file, root);
    fprintf(file, ")'=(");

    latexDumpSubtree(file, LEFT);
    fprintf(file, ")'%c(", root->data.op == OP_ADD ? '+' : '-');

    latexDumpSubtree(file, RIGHT);
    fprintf(file, ")'$$\n\n");

    RETURN(root->data.op == OP_ADD ? dL + dR : dL - dR);
}

ETNode* writeDerivativeMul(FILE* file, ETNode* root)
{
    assert(file != nullptr);
    assert(root != nullptr);

    writeRandomMessage(file, MESSAGES_DERIVATIVE_MUL, MESSAGES_DERIVATIVE_MUL_COUNT);

    fprintf(file, "$$(");
    latexDumpSubtree(file, root);
    fprintf(file, ")'=(");

    latexDumpSubtree(file, LEFT);
    fprintf(file, ")'\\cdot ");
    latexDumpSubtree(file, RIGHT);

    fprintf(file, "+");
    latexDumpSubtree(file, LEFT);
    fprintf(file, "\\cdot (");
    latexDumpSubtree(file, RIGHT);

    fprintf(file, ")'$$\n\n");

    RETURN((dL * R) + (L * dR));
}

ETNode* writeDerivativeDiv(FILE* file, ETNode* root)
{
    assert(file != nullptr);
    assert(root != nullptr);

    writeRandomMessage(file, MESSAGES_DERIVATIVE_DIV, MESSAGES_DERIVATIVE_DIV_COUNT);

    fprintf(file, "$$(");
    latexDumpSubtree(file, root);
    fprintf(file, ")'=\\frac{(");

    latexDumpSubtree(file, LEFT);
    fprintf(file, ")'\\cdot (");
    latexDumpSubtree(file, RIGHT);

    fprintf(file, ")-(");
    latexDumpSubtree(file, RIGHT);
    fprintf(file, ")\\cdot(");
    latexDumpSubtree(file, LEFT);

    fprintf(file, ")}{(");
    latexDumpSubtree(file, RIGHT);
    fprintf(file, ")^2}$$\n\n");

    RETURN((dL * R - L * dR) / (R ^ NUM(2)));
}

ETNode* writeDerivativePow(FILE* file, ETNode* root)
{
    assert(file != nullptr);
    assert(root != nullptr);

    writeRandomMessage(file, MESSAGES_DERIVATIVE_POW, MESSAGES_DERIVATIVE_POW_COUNT);

    fprintf(file, "$$(");
    latexDumpSubtree(file, root);
    fprintf(file, ")'=(");

    if (!hasVariable(RIGHT, 'x'))
    {
        latexDumpSubtree(file, RIGHT);
        fprintf(file, ")\\cdot (");
        latexDumpSubtree(file, LEFT);
        fprintf(file, ")^{");
        latexDumpSubtree(file, RIGHT);
        fprintf(file, "-1}\\cdot (");
        latexDumpSubtree(file, LEFT);
        fprintf(file, ")'$$\n\n");

        RETURN(R * (L ^ (R - NUM(1))) * dL);
    }

    latexDumpSubtree(file, LEFT);
    fprintf(file, ")^{");
    latexDumpSubtree(file, RIGHT);
    fprintf(file, "}\\cdot ((");
    latexDumpSubtree(file, RIGHT);
    fprintf(file, ")'\\cdot \\log(");
    latexDumpSubtree(file, LEFT);
    fprintf(file, ")+\\frac{(");
    latexDumpSubtree(file, RIGHT);
    fprintf(file, ")\\cdot (");
    latexDumpSubtree(file, LEFT);
    fprintf(file, ")'}{");
    latexDumpSubtree(file, LEFT);
    fprintf(file, "})$$\n\n");

    RETURN((L ^ R) * (dR * LOG(L) + R * dL / L));
}

ETNode* writeDerivativeLog(FILE* file, ETNode* root)
{
    assert(file != nullptr);
    assert(root != nullptr);

    writeRandomMessage(file, MESSAGES_DERIVATIVE_UNR, MESSAGES_DERIVATIVE_UNR_COUNT);

    fprintf(file, "$$(");
    latexDumpSubtree(file, root);
    fprintf(file, ")'=\\frac{1}{");

    latexDumpSubtree(file, RIGHT);
    fprintf(file, "}\\cdot (");
    latexDumpSubtree(file, RIGHT);
    fprintf(file, ")'$$\n\n");


    RETURN((NUM(1) / R) * dR);
}

ETNode* writeDerivativeExp(FILE* file, ETNode* root)
{
    assert(file != nullptr);
    assert(root != nullptr);

    writeRandomMessage(file, MESSAGES_DERIVATIVE_UNR, MESSAGES_DERIVATIVE_UNR_COUNT);

    fprintf(file, "$$(");
    latexDumpSubtree(file, root);
    fprintf(file, ")'=e^{");

    latexDumpSubtree(file, RIGHT);
    fprintf(file, "}\\cdot (");
    latexDumpSubtree(file, RIGHT);
    fprintf(file, ")'$$\n\n");

    RETURN(EXP(R) * dR);
}

ETNode* writeDerivativeSin(FILE* file, ETNode* root)
{
    assert(file != nullptr);
    assert(root != nullptr);

    writeRandomMessage(file, MESSAGES_DERIVATIVE_UNR, MESSAGES_DERIVATIVE_UNR_COUNT);

    fprintf(file, "$$(");
    latexDumpSubtree(file, root);
    fprintf(file, ")'=\\cos(");

    latexDumpSubtree(file, RIGHT);
    fprintf(file, ")\\cdot (");
    latexDumpSubtree(file, RIGHT);
    fprintf(file, ")'$$\n\n");

    RETURN(COS(R) * dR);
}

ETNode* writeDerivativeCos(FILE* file, ETNode* root)
{
    assert(file != nullptr);
    assert(root != nullptr);

    writeRandomMessage(file, MESSAGES_DERIVATIVE_UNR, MESSAGES_DERIVATIVE_UNR_COUNT);

    fprintf(file, "$$(");
    latexDumpSubtree(file, root);
    fprintf(file, ")'=-1\\cdot \\sin(");

    latexDumpSubtree(file, RIGHT);
    fprintf(file, ")\\cdot (");
    latexDumpSubtree(file, RIGHT);
    fprintf(file, ")'$$\n\n");


    RETURN(NUM(-1) * SIN(R) * dR);
}

ETNode* writeDerivativeTan(FILE* file, ETNode* root)
{
    assert(file != nullptr);
    assert(root != nullptr);

    writeRandomMessage(file, MESSAGES_DERIVATIVE_UNR, MESSAGES_DERIVATIVE_UNR_COUNT);

    fprintf(file, "$$(");
    latexDumpSubtree(file, root);
    fprintf(file, ")'=\\frac{1}{(\\cos(");

    latexDumpSubtree(file, RIGHT);
    fprintf(file, "))^2}\\cdot (");
    latexDumpSubtree(file, RIGHT);
    fprintf(file, ")'$$\n\n");


    RETURN((NUM(1) / (COS(R) ^ NUM(2))) * dR);
}

void writeResult(FILE* file, ETNode* root)
{
    assert(file != nullptr);
    assert(root != nullptr);

    Substitution substitutions[MAX_SUBSTITUTIONS] = {};

    size_t substitutionsCount = 0;
    makeSubstitutions(root, substitutions, &substitutionsCount);
    
    latexDumpSubtree(file, root, substitutions, substitutionsCount);
    fprintf(file, "$$\n\n");
    
    if (substitutionsCount > 0) { fprintf(file, "Where:\n\n"); }
    for (size_t i = 0; i < substitutionsCount; i++)
    {
        fprintf(file, "$$%c = ", substitutions[i].letter);
        latexDumpSubtree(file, substitutions[i].root);
        fprintf(file, "$$\n\n");
    }
}

void makeSubstitutions(ETNode* node, Substitution* substitutions, size_t* substitutionsCount)
{
    assert(substitutions != nullptr);
    assert(substitutionsCount != nullptr);

    if (node == nullptr || (*substitutionsCount) >= MAX_SUBSTITUTIONS) { return; }
    size_t size = 0;
    treeSize(node, &size);

    if (size >= MIN_SUBSTITUTION_SIZE && size <= MAX_SUBSTITUTION_SIZE && node->parent != nullptr)
    {
        substitutions[*substitutionsCount].root = node;
        substitutions[*substitutionsCount].letter = 'A' + (*substitutionsCount);
        (*substitutionsCount)++;
        return;
    }
    makeSubstitutions(node->left, substitutions, substitutionsCount);
    makeSubstitutions(node->right, substitutions, substitutionsCount);
}

void simplifyNode(FILE* file, ETNode* node, NodeType newType, ETNodeData data)
{
    assert(node != nullptr);

    setData(node, newType, data);

    if (node->left  != nullptr) { destroySubtree(node->left);  }
    if (node->right != nullptr) { destroySubtree(node->right); }

    node->left  = nullptr;
    node->right = nullptr;
}

void simplifyNode(FILE* file, ETNode* node, ETNode* child)
{
    assert(node  != nullptr);
    assert(child != nullptr);

    ETNode* other = isLeft(child) ? node->right : node->left;
    if (other != nullptr) { destroySubtree(other); }

    child->parent = node->parent;
    copyNode(node, child);

    deleteNode(child);
}

#define SIMPLIFY(otherSide) if (isIdentityType(simplifyType))                                                     \
                            {                                                                                     \
                                writeRandomMessage(file, MESSAGES_SIMPLIFICATION, MESSAGES_SIMPLIFICATION_COUNT); \
                                fprintf(file, "$$");                                                              \
                                latexDumpSubtree(file, root);                                                     \
                                fprintf(file, "=");                                                               \
                                simplifyNode(file, root, root->otherSide);                                        \
                                latexDumpSubtree(file, root);                                                     \
                                fprintf(file, "$$\n\n");                                                          \
                            }                                                                                     \
                            else                                                                                  \
                            {                                                                                     \
                                writeRandomMessage(file, MESSAGES_SIMPLIFICATION, MESSAGES_SIMPLIFICATION_COUNT); \
                                fprintf(file, "$$");                                                              \
                                latexDumpSubtree(file, root);                                                     \
                                fprintf(file, "=");                                                               \
                                simplifyNode(file, root, TYPE_NUMBER, { simplifyType.result });                   \
                                latexDumpSubtree(file, root);                                                     \
                                fprintf(file, "$$\n\n");                                                          \
                            }

bool simplifyOps(FILE* file, ETNode* root)
{
    if (root == nullptr) { return false; }

    bool isChanged = simplifyOps(file, root->left) || simplifyOps(file, root->right);

    if (isTypeOp(root))
    {
        Operation         operation      = root->data.op;
        SimplifyExpr      simplifyType   = {}; 
        SimplifyArgTarget simplifyTarget = {};

        for (size_t i = 0; i < SIMPLIFY_EXPRS_COUNT; i++)
        {
            simplifyType   = SIMPLIFY_EXPRS[i];
            simplifyTarget = simplifyType.target;

            if (operation != simplifyType.operation) { continue; }
            assert(root->right != nullptr);

            if (simplifyTarget == SAT_EQL && areTreesEqual(root->left, root->right))
            {
                SIMPLIFY(left)
                isChanged = true;
                break;
            }

            if ((simplifyTarget == SAT_SND || simplifyTarget == SAT_ANY) && isTypeNumber(root->right) && 
                dcompare(root->right->data.number, simplifyType.arg) == 0)
            {
                SIMPLIFY(left)
                isChanged = true;
                break;
            }

            if ((simplifyTarget == SAT_FST || simplifyTarget == SAT_ANY) && isTypeNumber(root->left) && 
                dcompare(root->left->data.number, simplifyType.arg) == 0)
            {
                SIMPLIFY(right)
                isChanged = true;
                break;
            }
        }
    }

    return isChanged;
}

#undef SIMPLIFY

// just garbage value that has to not be equal to either +-1 or 0
const double GARBAGE_VALUE_FOR_PRECALC = 22022002;

#define SIMPLIFY(result) writeRandomMessage(file, MESSAGES_SIMPLIFICATION, MESSAGES_SIMPLIFICATION_COUNT); \
                         fprintf(file, "$$");                                                              \
                         latexDumpSubtree(file, root);                                                     \
                         fprintf(file, "=");                                                               \
                         simplifyNode(file, root, TYPE_NUMBER, {result});                                  \
                         latexDumpSubtree(file, root);                                                     \
                         fprintf(file, "$$\n\n");                                                          

//-----------------------------------------------------------------------------
//! Precalculates all expressions with constants (e.g. '2+19' -> '21').
//!
//! @param [in] root
//!
//! @return whether or not there have been any changes in the subtree.
//-----------------------------------------------------------------------------
bool precalcConstExprs(FILE* file, ETNode* root)
{
    if (root == nullptr) { return false; }

    ETNode* left   = root->left;
    ETNode* right  = root->right;

    bool isChanged = precalcConstExprs(file, root->left) || precalcConstExprs(file, root->right);

    if (!isTypeOp(root) || hasVariable(root, 'x')) { return isChanged; } 


    Operation operation = root->data.op;
    double    value     = evaluateSubtree(root); 

    if ((operation == OP_ADD || operation == OP_SUB || operation == OP_MUL) && 
        isTypeNumber(left) && isTypeNumber(right) &&
        !isConstant(left->data.number) && !isConstant(right->data.number))
    {
        SIMPLIFY(evaluateBinary(operation, left->data.number, right->data.number));
        isChanged = true;
    }

    if (dcompare(value,  0.0) == 0) { SIMPLIFY( 0.0); return true; }
    if (dcompare(value,  1.0) == 0) { SIMPLIFY( 1.0); return true; }
    if (dcompare(value, -1.0) == 0) { SIMPLIFY(-1.0); return true; }

    return isChanged;
}

#undef SIMPLIFY