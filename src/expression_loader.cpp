#include <assert.h>
#include <ctype.h>
#include <string.h>

#include "../libs/log_generator.h"
#include "../libs/file_manager.h"
#include "expression_loader.h"

enum ParseError
{
    PARSE_NO_ERROR,
    PARSE_UNFINISHED_EXPRESSION,
    PARSE_UNKNOWN_OPERATION,
    PARSE_NO_OPENING_BRACKET,
    PARSE_NO_CLOSING_BRACKET,
    PARSE_NO_NUMBER_FOUND
};

struct Parser
{
    const char* expression;
    size_t      ofs;
    ParseError  status;
};

bool        loadExpression  (ExprTree* tree, const char* filename);

ParseError  parseExpression (ExprTree* tree, const char* expression);
void        requireSymbol   (Parser* parser, char symbol, ParseError error);
ETNode*     getExpression   (Parser* parser);
ETNode*     getTerm         (Parser* parser);
ETNode*     getPower        (Parser* parser);
ETNode*     getFactor       (Parser* parser);
ETNode*     getUnaryOp      (Parser* parser);

ETNode*     getNumber       (Parser* parser);
ETNode*     getConstant     (Parser* parser);
ETNode*     getVariable     (Parser* parser);

void        skipSpaces      (Parser* parser);

void        incrOffset      (Parser* parser, size_t delta);
char        curSymbol       (Parser* parser);
const char* curPosition     (Parser* parser);

const char* errorString     (ParseError error);
void        syntaxError     (Parser* parser, ParseError error);

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

    if (parseExpression(tree, exprBuffer) != PARSE_NO_ERROR)
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

ParseError parseExpression(ExprTree* tree, const char* expression)
{
    assert(tree       != nullptr);
    assert(expression != nullptr);

    Parser parser = { expression, 0, PARSE_NO_ERROR };

    ETNode* root = getExpression(&parser);
    requireSymbol(&parser, '\0', PARSE_UNFINISHED_EXPRESSION);

    tree->root = root;

    return parser.status;
}

void requireSymbol(Parser* parser, char symbol, ParseError error)
{
    assert(parser != nullptr);

    if (curSymbol(parser) != symbol) { syntaxError(parser, error); }
}

ETNode* getExpression(Parser* parser)
{
    assert(parser != nullptr);

    ETNode* value1 = getTerm(parser);

    while (curSymbol(parser) == '+' || curSymbol(parser) == '-')
    {
        char operation = curSymbol(parser);
        incrOffset(parser, 1);

        ETNode* value2 = getTerm(parser);

        if (operation == '+') { value1 = newNode(TYPE_OP, { .op = OP_ADD }, value1, value2); }
        else                  { value1 = newNode(TYPE_OP, { .op = OP_SUB }, value1, value2); }
    }

    return value1;
}

ETNode* getTerm(Parser* parser)
{
    assert(parser != nullptr);

    ETNode* value1 = getPower(parser);

    while (curSymbol(parser) == '*' || curSymbol(parser) == '/')
    {
        char operation = curSymbol(parser);
        incrOffset(parser, 1);

        ETNode* value2 = getPower(parser);

        if (operation == '*') { value1 = newNode(TYPE_OP, { .op = OP_MUL }, value1, value2); }
        else                  { value1 = newNode(TYPE_OP, { .op = OP_DIV }, value1, value2); }
    }

    return value1;
}

ETNode* getPower(Parser* parser)
{
    assert(parser != nullptr);

    ETNode* value1 = getFactor(parser);

    while (curSymbol(parser) == '^')
    {
        incrOffset(parser, 1);

        value1 = newNode(TYPE_OP, { .op = OP_POW }, value1, getFactor(parser));
    }

    return value1;
}

ETNode* getFactor(Parser* parser)
{
    assert(parser != nullptr);

    ETNode* value = 0;

    if (curSymbol(parser) == '(')
    {
        incrOffset(parser, 1);

        value = getExpression(parser);
        requireSymbol(parser, ')', PARSE_NO_CLOSING_BRACKET);

        incrOffset(parser, 1);
    }
    else
    {
        value = getNumber(parser);

        if (value == nullptr) { value = getConstant(parser); }
        if (value == nullptr) { value = getVariable(parser); }
        if (value == nullptr) { value = getUnaryOp(parser);  }
    }

    return value;
}

ETNode* getUnaryOp(Parser* parser)
{
    assert(parser != nullptr);

    bool        valid     = false;
    const char* curPos    = curPosition(parser);
    Operation   operation = OP_INVALID;

    for (size_t i = UNARY_OPERATIONS_START; i < OPERATIONS_COUNT; i++)
    {   
        size_t length = strlen(OPERATIONS[i]);

        if (strncmp(curPos, OPERATIONS[i], length) == 0)
        {
            operation = (Operation) i;
            valid     = true;

            incrOffset(parser, length);
            continue;
        }
    }

    if (!valid) { syntaxError(parser, PARSE_UNKNOWN_OPERATION); }

    ETNode* value = getFactor(parser);

    switch (operation)
    {
        case OP_LOG: 
            value = newNode(TYPE_OP, { .op = OP_LOG }, nullptr, value);
            break;

        case OP_EXP: 
            value = newNode(TYPE_OP, { .op = OP_EXP }, nullptr, value);
            break;
 
        case OP_SIN: 
            value = newNode(TYPE_OP, { .op = OP_SIN }, nullptr, value);
            break;

        case OP_COS: 
            value = newNode(TYPE_OP, { .op = OP_COS }, nullptr, value);
            break;

        case OP_TAN:
            value = newNode(TYPE_OP, { .op = OP_TAN }, nullptr, value);
            break;

        default:
            syntaxError(parser, PARSE_UNKNOWN_OPERATION);
            break;
    }

    return value;
}

ETNode* getNumber(Parser* parser)
{
    assert(parser != nullptr);

    char*  numberEnd = nullptr;
    double value     = strtod(curPosition(parser), &numberEnd);

    if (numberEnd == curPosition(parser)) { return nullptr; }

    incrOffset(parser, numberEnd - curPosition(parser));

    return newNode(TYPE_NUMBER, { .number = value }, nullptr, nullptr);
}

ETNode* getConstant(Parser* parser)
{
    assert(parser != nullptr);

    size_t length = 0;

    for (size_t i = 0; i < CONSTANTS_COUNT; i++)
    {
        length = CONSTANTS[i].nameLength;

        if (strncmp(curPosition(parser), CONSTANTS[i].name, length) == 0)
        {
            incrOffset(parser, length);

            return newNode(TYPE_NUMBER, { .number = CONSTANTS[i].value }, nullptr, nullptr);
        }
    }

    return nullptr;
}

ETNode* getVariable(Parser* parser)
{
    assert(parser != nullptr);

    char symbol = curSymbol(parser);
    if (isVariable(symbol))
    {
        char nextSymbol = curPosition(parser)[1];
        if (nextSymbol != '\0' && isalpha(nextSymbol)) { return nullptr; }

        incrOffset(parser, 1);

        return newNode(TYPE_VAR, { .var = symbol }, nullptr, nullptr);
    }

    return nullptr;
}

void skipSpaces(Parser* parser)
{
    assert(parser             != nullptr);
    assert(parser->expression != nullptr);

    parser->ofs += strspn(curPosition(parser), " \t");
}

void incrOffset(Parser* parser, size_t delta)
{
    assert(parser             != nullptr);
    assert(parser->expression != nullptr);

    parser->ofs += delta;

    skipSpaces(parser);
}

char curSymbol(Parser* parser)
{
    assert(parser             != nullptr);
    assert(parser->expression != nullptr);

    return *curPosition(parser);
}

const char* curPosition(Parser* parser)
{
    assert(parser             != nullptr);
    assert(parser->expression != nullptr);

    return &(parser->expression[parser->ofs]);
}

const char* errorString(ParseError error)
{
    switch (error)
    {
        case PARSE_NO_ERROR:              return "no error";
        case PARSE_UNFINISHED_EXPRESSION: return "unfinished expression";
        case PARSE_UNKNOWN_OPERATION:     return "unknown operation";
        case PARSE_NO_OPENING_BRACKET:    return "no opening bracket found";
        case PARSE_NO_CLOSING_BRACKET:    return "no closing bracket found";
        case PARSE_NO_NUMBER_FOUND:       return "expected a number";
    }

    return nullptr;
}

void syntaxError(Parser* parser, ParseError error)
{
    assert(parser != nullptr);

    if (error == PARSE_NO_ERROR) { return; }

    parser->status = error;

    printf("SYNTAX ERROR: %s\n", errorString(error));
    printf("%s\n", parser->expression);

    for (size_t i = 0; i < parser->ofs; i++)
    {
        putchar(' ');
    }

    printf("^\n");
}