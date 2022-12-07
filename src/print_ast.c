#include "print_ast.h"
#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
extern const char *node_labels[22];
//├── ─
//└── └──
void print_offset(int offset)
{
  if (offset <= 0)
  {
    printf(" ");
    return;
  }
  // min offset is 2
  printf(" ");
  for (int i = 0; i < offset; i += 2)
  {
    printf("│ ");
  }
}
void p(ASTNode *ast, int offset)
{
  if (ast->type == NODE_GHOST)
    return;
  if (offset > 0)
  {
    print_offset(offset - 2);
    printf("└── ");
  }
  // print_offset(offset);
  // printf("%.*s\n", ast->strlen, ast->start);
  if (ast->strlen >= 1)
    printf("%s (%.*s)", node_labels[ast->type], ast->strlen, ast->start);
  else
    printf("%s (_)", node_labels[ast->type]);
  if (ast->length > 0)
    printf(" [\n");
  else
    printf("\n");
  for (int i = 0; i < ast->length; i++)
  {
    p(&ast->children[i], offset + 2);
  }
  if (ast->length > 0)
  {
    print_offset(offset);
    printf("]\n");
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