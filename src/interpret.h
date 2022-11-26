#ifndef toc_interpret
#define toc_interpret
#include "ast.h"
typedef enum
{
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
} InterpretResult;

InterpretResult interpret(ASTNode *ast);

#endif