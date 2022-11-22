// #include <stdio.h>
// #include <stdlib.h>
// #include <stdbool.h>

// #include "ast.h"
// #include "interpret.h"
// #include "scanner.h"
// #include "value.h"
// #include "map.h"

// typedef struct
// {
//   Token current;
//   Token previous;
//   bool hadError;
//   bool runtimeError;
// } Parser;

// Parser parser;
// hashmap *map;
// hashmap *declaredMap;

// static void map_set(bool exec, hashmap *m, void *key, size_t ksize, uintptr_t value)
// {
//   if (exec)
//   {
//     hashmap_set(m, key, ksize, value);
//   }
// }

// static void errorAt(Token *token, const char *message)
// {
//   fprintf(stderr, "[line %d, col %d] Error", token->line, token->column);
//   if (token->type == TOKEN_EOF)
//   {
//     fprintf(stderr, " at end");
//   }
//   else if (token->type == TOKEN_ERROR)
//   {
//   }
//   else
//   {
//     fprintf(stderr, " at '%.*s'", token->length, token->start);
//   }
//   fprintf(stderr, ": %s\n", message);
//   parser.hadError = true;
// }

// static void error(const char *message)
// {
//   errorAt(&parser.previous, message);
// }

// static void errorAtCurrent(const char *message)
// {
//   errorAt(&parser.current, message);
// }
// static void advance();
// static void consume(TokenType type, const char *message)
// {
//   if (parser.current.type == type)
//   {
//     advance();
//     return;
//   }
//   errorAtCurrent(message);
// }
// static Value integer()
// {
//   int value = atoi(parser.previous.start);
//   return INT_VAL(value);
// }
// static void advance()
// {
//   parser.previous = parser.current;
//   // skip all error tokens
//   for (;;)
//   {
//     parser.current = scanToken();
//     if (parser.current.type != TOKEN_ERROR)
//       break;
//     errorAtCurrent(parser.current.start);
//   }
// }
// static bool isAtEnd()
// {
//   return parser.current.type == TOKEN_EOF;
// }
// static bool check(TokenType type)
// {
//   if (isAtEnd())
//     return false;
//   return parser.current.type == type;
// }
// static bool match(TokenType type)
// {
//   if (!check(type))
//     return false;
//   advance();
//   return true;
// }
// static Value identifier()
// {
//   // printf("identifier: %.*s\n", parser.previous.length, parser.previous.start);
//   // return INT_VAL(24);
//   uintptr_t val;
//   if (hashmap_get(map, (void *)parser.previous.start, parser.previous.length - 1, &val))
//   {
//     return INT_VAL((int)val);
//   }
//   else
//   {
//     error("Uninitialized variable.");
//     parser.runtimeError = true;
//     return INT_VAL(0);
//   }
// }
// static Value expression();
// static Value primary()
// {
//   if (match(TOKEN_INT))
//   {
//     return integer();
//   }
//   else if (match(TOKEN_IDENTIFIER))
//   {
//     return identifier();
//   }
//   else if (match(TOKEN_LEFT_PAREN))
//   {
//     Value value = expression();
//     consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
//     return value;
//   }
//   else
//   {
//     errorAtCurrent("Expect expression.");
//     return INT_VAL(0);
//   }
// }
// static Value unary()
// {
//   if (match(TOKEN_MINUS))
//   {
//     Value val = unary();
//     int res = -(AS_INT(val));
//     return INT_VAL(res);
//   }
//   return primary();
// }
// static Value factor()
// {
//   Value value = unary();
//   int result = AS_INT(value);
//   while (match(TOKEN_STAR) || match(TOKEN_SLASH))
//   {
//     TokenType operatorType = parser.previous.type;
//     Value right = unary();
//     switch (operatorType)
//     {
//     case TOKEN_STAR:
//       result *= AS_INT(right);
//       break;
//     case TOKEN_SLASH:
//       result /= AS_INT(right);
//       break;
//     default:
//       break;
//     }
//   }
//   return INT_VAL(result);
// }
// static Value term()
// {
//   Value value = factor();
//   while (match(TOKEN_PLUS) || match(TOKEN_MINUS))
//   {
//     Token operator= parser.previous;
//     Value right = factor();
//     int result;
//     if (operator.type == TOKEN_PLUS)
//     {
//       result = AS_INT(value) + AS_INT(right);
//     }
//     else
//     {
//       result = AS_INT(value) - AS_INT(right);
//     }
//     value = makeInt(result);
//   }

//   return value;
// }
// static Value comparison()
// {
//   Value val = term();
//   if (match(TOKEN_GREATER))
//   {
//     Value right = term();
//     val = makeBool(AS_INT(val) > AS_INT(right));
//   }
//   return val;
// }
// static Value equality()
// {
//   Value val = comparison();
//   if (match(TOKEN_EQUAL_EQUAL))
//   {
//     Value right = comparison();
//     val = makeBool(AS_BOOL(val) == AS_BOOL(right) || AS_INT(val) == AS_INT(right));
//   }
//   return val;
// }
// static Value expression()
// {
//   return equality();
// }
// static void writeStatement(bool exec)
// {
//   Value val = expression();
//   if (exec)
//     printf("%d\n", AS_INT(val));
// }
// static void assignment(bool exec)
// {
//   char *name = parser.previous.start;
//   int length = parser.previous.length;
//   consume(TOKEN_EQUAL, "Expect '=' after variable name.");
//   Value value = expression();
//   if (hashmap_get(declaredMap, (void *)name, length - 1, NULL))
//   {
//     map_set(exec, map, name, length - 1, (uintptr_t)AS_INT(value));
//   }
//   else
//   {
//     error("Undefined variable.");
//     parser.runtimeError = true;
//   }
// }
// static void readStatement(bool exec)
// {
//   consume(TOKEN_IDENTIFIER, "Expected identifier for 'read' statement.");
//   const char *start = parser.previous.start;
//   int length = parser.previous.length;
//   int value;
//   scanf("%d", &value);
//   hashmap_set(map, (void *)start, length - 1, (uintptr_t)value);
// }
// static void forStatement(bool exec)
// {
//   consume(TOKEN_LEFT_PAREN, "Expected '(' after 'for'");
//   // first clause
//   if (!check(TOKEN_SEMICOLON))
//   {
//     expression();
//   }

// }
// static void statement(bool exec)
// {
//   if (match(TOKEN_IDENTIFIER))
//   {
//     assignment(exec);
//   }
//   else if (match(TOKEN_READ))
//   {
//     readStatement(exec);
//   }
//   else if (match(TOKEN_WRITE))
//   {
//     writeStatement(exec);
//   }
//   else if (match(TOKEN_FOR))
//   {
//     forStatement(exec);
//   }
//   else
//   {
//     errorAtCurrent("Expected statement.");
//   }
//   consume(TOKEN_SEMICOLON, "Expected ';' after statement.");
// }
// static void program(bool execute)
// {
//   while (!isAtEnd() && !parser.hadError && !parser.runtimeError)
//   {
//     statement(execute);
//   }
//   consume(TOKEN_EOF, "Expected end of file");
// }
// static void declarations()
// {
//   if (match(TOKEN_INT))
//   {
//     while (!isAtEnd() && !parser.hadError)
//     {
//       consume(TOKEN_IDENTIFIER, "Expected identifier in declaration list.");
//       if (parser.hadError)
//       {
//         break;
//       }
//       const char *start = parser.previous.start;
//       int length = parser.previous.length;
//       hashmap_set(map, (void *)start, length - 1, (uintptr_t)0);
//     }
//     consume(TOKEN_SEMICOLON, "Expected ';' after declaration list.");
//   }
// }
// InterpretResult interpret(const char *source)
// {
//   initScanner(source);
//   map = hashmap_create();
//   declaredMap = hashmap_create();
//   advance();
//   // Value expr = expression();
//   declarations();
//   program(true);
//   // printf("Answer: %d\n", AS_INT(expr));
//   consume(TOKEN_EOF, "Expected end of file.");
//   if (parser.hadError)
//     return INTERPRET_COMPILE_ERROR;
//   if (parser.runtimeError)
//     return INTERPRET_RUNTIME_ERROR;
//   return INTERPRET_OK;
// }

// void printTokens(const char *source)
// {
//   initScanner(source);
//   int line = -1;
//   for (;;)
//   {
//     Token token = scanToken();
//     if (token.line != line)
//     {
//       printf("%4d ", token.line);
//       line = token.line;
//     }
//     else
//     {
//       printf("   | ");
//     }
//     printf("%2d '%.*s'\n", token.type, token.length, token.start);
//     if (token.type == TOKEN_EOF)
//       break;
//   }
// }