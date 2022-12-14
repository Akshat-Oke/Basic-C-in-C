#include "print_ast.h"
#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
extern const char *node_labels[22];
// ├── ─
// └── └──
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
    // printf("│ ");
    printf("  ");
  }
}
void p(ASTNode *ast, int offset)
{
  if (ast->type == NODE_GHOST)
    return;
  if (offset > 0)
  {
    print_offset(offset - 2);
    printf("  ");
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
    // if (i < ast->length - 1)
    //   printf(",");
  }
  if (ast->length > 0)
  {
    print_offset(offset);
    printf("]\n");
  }
}

void print_generalised_list(ASTNode *node, int offset)
{
  print_offset(offset);
  // do not print ghost nodes
  if (node->type == NODE_GHOST)
    return;
  // print the data if node type is a literal
  if (node->type == NODE_LITERAL)
  {
    printf("[ '%.*s' ]", node->strlen, node->start);
    return;
  }
  else
    printf("[ %s \n", node_labels[node->type]);
  for (int i = 0; i < node->length; i++)
  {
    // printf("%.*s", node->children[i].strlen, node->children[i].start);
    print_generalised_list(&node->children[i], offset + 2);
    // if (i < node->length - 1)
    //   printf(", ");
  }
  print_offset(offset);
  printf(" ]\n");
}

void printAST(ASTNode *ast)
{
  // print_generalised_list(ast, 0);
  // printf("\n");
  p(ast, 0);
}