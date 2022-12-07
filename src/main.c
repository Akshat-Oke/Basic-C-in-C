#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "interpret.h"
#include "ast.h"
#include "print_ast.h"

static char *readFile(const char *path);
static void runFile(const char *path, bool unaryNegation);

int main(int argc, const char *argv[])
{
  bool unaryNegation = false;
  if (argc == 2)
  {
    // check if it is "-u" and enable unary negation
    if (strlen(argv[1]) == 2 && argv[1][0] == '-' && argv[1][1] == 'u')
    {
      unaryNegation = true;
    }
    else
    {
      runFile(argv[1], unaryNegation);
      return 0;
    }
  }
  runFile("input.txt", unaryNegation);
  return 0;
}
static void runFile(const char *path, bool unaryNegation)
{
  char *source = readFile(path);
  ASTNode *ast;
  ast = newASTNode(NODE_MAIN_PROGRAM, NULL, 0);

  bool result = buildAST(source, ast, unaryNegation);
  free(source);
  if (!result)
  {
    exit(65);
  }
  printAST(ast);
  interpret(ast);
  // if (result == INTERPRET_COMPILE_ERROR)
  // exit(65);
  // if (result == INTERPRET_RUNTIME_ERROR)
  // exit(70);
}
static char *readFile(const char *path)
{
  FILE *file = fopen(path, "rb");
  if (file == NULL)
  {
    fprintf(stderr, "Could not open file \"%s\".\n", path);
    exit(74);
  }

  fseek(file, 0L, SEEK_END);
  size_t fileSize = ftell(file);
  rewind(file);

  char *buffer = (char *)malloc(fileSize + 1);
  if (buffer == NULL)
  {
    fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
    exit(74);
  }
  size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
  if (bytesRead < fileSize)
  {
    fprintf(stderr, "Could not read file \"%s\".\n", path);
    exit(74);
  }
  buffer[bytesRead] = '\0';

  fclose(file);
  return buffer;
}