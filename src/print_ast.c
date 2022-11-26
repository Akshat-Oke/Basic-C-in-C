#include "print_ast.h"
#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
extern const char *node_labels[20];
//├── ─
//└── └──

void p(ASTNode *ast, int offset)
{
  for (int i = 0; i < offset; i++)
  {
    printf(" ");
  }
  // printf("%.*s\n", ast->strlen, ast->start);
  if (ast->strlen >= 1)
    printf("%s (%.*s)\n", node_labels[ast->type], ast->strlen, ast->start);
  else
    printf("%s (_)\n", node_labels[ast->type]);
  for (int i = 0; i < ast->length; i++)
  {
    p(&ast->children[i], offset + 2);
  }
}
void printAST(ASTNode *ast)
{
  p(ast, 0);
  // printf("Node %d:[", ast->type);
  // for (int i = 0; i < ast->length; i++)
  // {
  //   printAST(&ast->children[i]);
  // }
  // printf("]\n");
}