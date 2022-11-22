#ifndef toc_ast
#define toc_ast
#include "value.h"
#include <stdbool.h>

typedef enum
{
  COMPILE_OK,
  COMPILE_ERROR
} CompileStatus;
typedef enum
{
  NODE_MAIN_PROGRAM,
  NODE_PROGRAM,
  NODE_DECLARATION,
  NODE_FOR_STMT,
  NODE_ASSIGN_STMT,
  NODE_EXPR,
  NODE_EQUALITY,
  NODE_COMPARISON,
  NODE_TERM,
  NODE_FACTOR,
  NODE_UNARY,
  NODE_PRIMARY,
  NODE_INT,
  NODE_IDENTIFIER,
  NODE_READ_STMT,
  NODE_WRITE_STMT,
  NODE_KEYWORD,
  NODE_LITERAL,
  NODE_OPERATOR
} NodeType;

typedef struct ASTNode
{
  NodeType type;
  const char *start;
  int strlen;
  struct ASTNode *children;
  int length;
} ASTNode;

ASTNode *newASTNode(NodeType type, const char *start, int strlen);
// ASTNode *newIdentifierNode(const char* start, int strlen)
// {
//   ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
//   node->strlen = strlen;
//   node->start = start;
//   node->type = NODE_IDENTIFIER;
//   node->children = NULL;
//   node->length = 0;
//   return node;
// }

void addChild(ASTNode *ast, ASTNode *child);

bool buildAST(const char *source, ASTNode *ast);

#endif