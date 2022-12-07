#include <stdio.h>
#include <stdbool.h>
#include "interpret.h"
#include "ast.h"
#include "map.h"
#include "value.h"

hashmap *map;
hashmap *declaredMap;
bool hadError = false;

int str_to_int(const char *str, int len)
{
  int i;
  int ret = 0;
  for (i = 0; i < len; ++i)
  {
    ret = ret * 10 + (str[i] - '0');
  }
  return ret;
}

typedef enum
{
  ADD,
  SUBTRACT,
  MULTIPLY,
  DIVIDE,
  EQUALS,
  GREATER_THAN
} OperatorType;

typedef void *(*Function)(ASTNode *);

static void *main_program(ASTNode *);
static void *declaration(ASTNode *);
static void *program(ASTNode *);
static void *for_statement(ASTNode *);
static void *assignment(ASTNode *);
static void *expr_or_assignment(ASTNode *);
static void *expression(ASTNode *);
static void *equality(ASTNode *);
static void *comparison(ASTNode *);
static void *term(ASTNode *);
static void *factor(ASTNode *);
static void *unary(ASTNode *);
static void *integer(ASTNode *);
static void *identifier(ASTNode *);
static void *primary(ASTNode *);
static void *statement(ASTNode *);
static void *read(ASTNode *);
static void *write(ASTNode *);
static void *operator(ASTNode *);
static void *ghost(ASTNode *);

Function rules[] = {
    [NODE_MAIN_PROGRAM] = main_program,
    [NODE_PROGRAM] = main_program,
    [NODE_BLOCK] = main_program,
    [NODE_DECLARATION] = declaration,
    [NODE_FOR_STMT] = for_statement,
    [NODE_ASSIGN_STMT] = assignment,
    [NODE_EXPR_OR_ASSIGN] = expr_or_assignment,
    [NODE_EXPR] = expression,
    [NODE_EQUALITY] = equality,
    [NODE_COMPARISON] = comparison,
    [NODE_TERM] = term,
    [NODE_FACTOR] = factor,
    [NODE_UNARY] = unary,
    [NODE_PRIMARY] = primary,
    [NODE_INTEGER] = integer,
    [NODE_IDENTIFIER] = identifier,
    [NODE_READ_STMT] = read,
    [NODE_WRITE_STMT] = write,
    [NODE_KEYWORD] = operator,
    [NODE_LITERAL] =
    operator,
    [NODE_OPERATOR] =
    operator,
    [NODE_GHOST] = ghost };

#define VISIT(node) (rules[node.type])(&(node))

void error(const char *message)
{
  fprintf(stderr, "Runtime Error: %s\n", message);
  exit(70);
  hadError = true;
}

static void *operator(ASTNode *node)
{
  char lexeme;
  if (node->length == 0)
    lexeme = node->start[0];
  else
  {
    lexeme = node->children[0].start[0];
  }
  OperatorType *type;
  switch (lexeme)
  {
  case '+':
    *type = ADD;
    break;
  case '-':
    *type = SUBTRACT;
    break;
  case '*':
    *type = MULTIPLY;
    break;
  case '/':
    *type = DIVIDE;
    break;
  case '=':
    *type = EQUALS;
    break;
  case '>':
    *type = GREATER_THAN;
    break;
  default:
    // error("Unknown operator");
    return NULL;
  }
  return type;
}

static void *ghost(ASTNode *node)
{
  return makeBool(true);
}

static void *write(ASTNode *node)
{
  // const char *start = node->children[1].start;
  // int len = node->children[1].strlen;
  // uintptr_t value;
  // if (!hashmap_get(map, start, len, &value))
  // {
  //   error("Variable not initialized");
  //   return NULL;
  // }
  // Value *val = (Value *)value;
  Value *val = VISIT(node->children[0]);
  if (IS_BOOL(*val))
  {
    printf(AS_BOOL(*val) ? "true\n" : "false\n");
  }
  else
  {
    printf("%d\n", AS_INT(*val));
  }
}

static void *read(ASTNode *node)
{
  const char *start = node->children[0].start;
  int len = node->children[0].strlen;
  uintptr_t _;
  if (!hashmap_get(declaredMap, start, len, &_))
  {
    error("Variable not declared");
    return NULL;
  }
  int value;
  scanf("%d", &value);

  hashmap_set(map, start, len, (uintptr_t)makeInt(value));
  return NULL;
}

static void *for_statement(ASTNode *node)
{
  // initializer
  VISIT(node->children[0]);
  // condition
  Value *val = VISIT(node->children[1]);
  if (!IS_BOOL(*val))
  {
    error("Condition must be a boolean");
    return NULL;
  }
  while (AS_BOOL(*val))
  {
    // body
    VISIT(node->children[3]);
    // increment
    VISIT(node->children[2]);
    // condition
    val = VISIT(node->children[1]);
  }
  return NULL;
}

static void *main_program(ASTNode *node)
{
  for (int i = 0; i < node->length; i++)
  {
    VISIT(node->children[i]);
  }
  return NULL;
}

static void *identifier(ASTNode *node)
{
  // printf("Identifier %d\n", node->length);
  const char *start = node->start;
  int length = node->strlen;
  uintptr_t value;
  if (!hashmap_get(declaredMap, start, length, &value))
  {
    // error("Undeclared variable ''.");
    fprintf(stderr, "Runtime Error: Variable '%.*s' is not declared.\n", length, start);
    exit(70);
    hadError = true;
    return NULL;
  }
  if (!hashmap_get(map, start, length, &value))
  {
    fprintf(stderr, "Runtime Error: Variable '%.*s' is not initialized.\n", length, start);
    exit(70);
    hadError = true;
    return NULL;
  }
  return value;
}

static void *primary(ASTNode *node)
{
  // printf("Primary %d\n", node->length);

  if (node->length > 1)
  {
    // grouping
    return VISIT(node->children[1]);
  }
  // printf("Primary type %d\n", node->children[0].type);
  return VISIT(node->children[0]);
}

static void *integer(ASTNode *node)
{
  // printf("Integer %d\n", node->length);
  int value = str_to_int(node->start, node->strlen);
  return makeInt(value);
}

static void *unary(ASTNode *node)
{
  // printf("unary %d\n", node->length);

  if (node->children[0].type == NODE_OPERATOR)
  {
    Value *right = VISIT(node->children[1]);
    if (IS_BOOL(*right))
    {
      error("Invalid type: got boolean");
      return NULL;
    }
    return makeInt(-AS_INT(*right));
  }
  return VISIT(node->children[0]);
}

static void *factor(ASTNode *node)
{
  // printf("factor %d\n", node->length);

  Value *left = VISIT(node->children[0]);
  if (node->length == 1)
  {
    return left;
  }
  int result = AS_INT(*left);
  for (int i = 1; i < node->length; i += 2)
  {
    Value *right = VISIT(node->children[i + 1]);
    if (!IS_INT(*right))
    {
      error("Expected int, got boolean instead.");
      return makeInt(0);
    }
    char operator=(node->children[i + 1].start[0]);
    switch (operator)
    {
    case MULTIPLY:
    case '*':
      result = result * AS_INT(*right);
      break;
    case DIVIDE:
    case '/':
      result = result / AS_INT(*right);
      break;
    default:
      error("Invalid operator");
      hadError = true;
      break;
    }
  }
  return (void *)makeInt(result);
}

static void *term(ASTNode *node)
{
  // printf("term %d\n", node->length);

  Value *left = VISIT(node->children[0]);
  if (node->length == 1)
    return left;
  if (!IS_INT(*left))
  {
    error("Expected int, got boolean instead.");
    return makeInt(0);
  }
  int result = AS_INT(*left);
  for (int i = 1; i < node->length; i += 2)
  {
    // OperatorType *operator= VISIT(node->children[i]);
    char operator= node->children[i].start[0];

    Value *right = VISIT(node->children[i + 1]);
    if (!IS_INT(*right))
    {
      error("Expected int, got boolean instead.");
      return makeInt(0);
    }
    switch (operator)
    {
    case ADD:
    case '+':
      result = result + AS_INT(*right);
      break;
    case SUBTRACT:
    case '-':
      result = result - AS_INT(*right);
      break;
    default:
      fprintf(stderr, "Invalid operator");
      hadError = true;
      exit(1);
      break;
    }
  }
  return (void *)makeInt(result);
}

static void *comparison(ASTNode *node)
{
  // printf("Comp %d\n", node->length);
  Value *left = VISIT(node->children[0]);
  int result = AS_INT(*left);
  if (node->length == 1)
  {
    return left;
  }
  for (int i = 1; i < node->length; i += 1)
  {
    Value *right = VISIT(node->children[i]);
    if (!IS_INT(*right))
    {
      error("Expected int, got boolean instead.");
      return makeInt(0);
    }

    // char operator=(node->children[i].start[0]);
    result = result > AS_INT(*right);

    // switch (operator)
    // {
    // case GREATER_THAN:
    // case '>':
    //   result = result > AS_INT(*right);
    //   break;
    // default:
    //   fprintf(stderr, "Invalid operator");
    //   hadError = true;
    //   break;
    // }
  }
  return (void *)makeBool(result);
}

static void *equality(ASTNode *node)
{
  // printf("Eq: %d\n", node->length);
  Value *left = VISIT(node->children[0]);
  int len = node->length - 1;
  if (len == 0)
  {
    return left;
  }
  bool result = AS_BOOL(*left);
  int i = 1;
  for (int i = 1; i < node->length; i++)
  {
    // len -= 2;
    // OperatorType *operator= VISIT(node->children[i]);
    Value *right = VISIT(node->children[i]);
    if (IS_BOOL(*left) && IS_BOOL(*right))
    {
      result = AS_BOOL(*left) == AS_BOOL(*right);
    }
    else if (IS_INT(*left) && IS_INT(*right))
    {
      result = AS_INT(*left) == AS_INT(*right);
    }
    else
    {
      fprintf(stderr, "Runtime Error: Types do not match\n");
      hadError = true;
      return NULL;
    }
  }
  return (void *)makeBool(result);
}

static void *expression(ASTNode *node)
{
  return equality(&node->children[0]);
}
static void *expr_or_assignment(ASTNode *node)
{
  if (node->length < 3)
  {
    return expression(node);
  }
  else
  {
    // first child is equality, ignore
    // second is the ghost node, extract the var name from here
    // third is the RHS expression.
    ASTNode ghost = node->children[1];
    const char *start = ghost.start;
    int strlen = ghost.strlen;
    uintptr_t v;
    if (!hashmap_get(declaredMap, start, strlen, &v))
    {
      fprintf(stderr, "Runtime Error: Variable %.*s is not declared\n", strlen, start);
      hadError = true;
      // exit(70);
      return NULL;
    }
    Value *value = VISIT(node->children[3]);
    hashmap_set(map, start, strlen, (uintptr_t)value);
    return value;
  }
}
static void *assignment(ASTNode *node)
{
  // first child is the var name
  // second child is '='
  // third child is the value
  const char *start = node->start;
  int strlen = node->strlen;
  uintptr_t v;
  if (!hashmap_get(declaredMap, start, strlen, &v))
  {
    fprintf(stderr, "Runtime Error: Variable %.*s is not declared\n", strlen, start);
    hadError = true;
    // exit(70);
    return NULL;
  }
  Value *value = VISIT(node->children[1]); // rules[node->children[1].type](&node->children[1]);
  hashmap_set(map, start, strlen, (uintptr_t)value);
  // printf("------\n");
  // printf("Type is bool: %d\n", IS_BOOL(*value));
  return value;
}
static void *declaration(ASTNode *node)
{
  // printf("De");
  // skip first child as it is the keyword
  for (int i = 1; i < node->length; i++)
  {
    ASTNode *child = &node->children[i];
    if (child->type == NODE_IDENTIFIER)
    {
      hashmap_set(declaredMap, child->start, child->strlen, 0);
    }
    else
    {
      fprintf(stderr, "Unexpected error wth");
    }
  }
  return NULL;
}

InterpretResult interpret(ASTNode *ast)
{
  map = hashmap_create();
  declaredMap = hashmap_create();
  (rules[NODE_MAIN_PROGRAM])(ast);
  return INTERPRET_OK;
}