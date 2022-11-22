#include "print_ast.h"
#include "ast.h"
#include <stdio.h>
#include <stdlib.h>

void p(ASTNode *ast, int offset)
{
  for (int i = 0; i < offset; i++)
  {
    printf(" ");
  }
  // printf("%.*s\n", ast->strlen, ast->start);
  if (ast->strlen >= 1)
    printf("Node %d (%.*s)\n", ast->type, ast->strlen, ast->start);
  else
    printf("Node %d (_)\n", ast->type);
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