#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "ast.h"
#include "scanner.h"
#include "value.h"
#include "map.h"

typedef struct
{
  Token current;
  Token previous;
  bool hadError;
  bool runtimeError;
} Parser;

Parser parser;
hashmap *map;
hashmap *declaredMap;

ASTNode *newASTNode(NodeType type, const char *start, int strlen)
{
  // printf("%.*s\n", strlen, start);
  ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
  const char *s = malloc((sizeof(char) * strlen + 1));
  memcpy(s, start, strlen);
  node->strlen = strlen;
  node->start = s;
  node->type = type;
  node->children = NULL;
  node->length = 0;
  return node;
}

void addChild(ASTNode *ast, ASTNode *child)
{
  ast->length++;
  ast->children = (ASTNode *)realloc(ast->children, ast->length * sizeof(ASTNode));
  ast->children[ast->length - 1] = *child;
}
static ASTNode *makeNode(NodeType type)
{
  return newASTNode(type, parser.current.start, parser.current.length);
}
static ASTNode *makePrevAsNode(NodeType type)
{
  return newASTNode(type, parser.previous.start, parser.previous.length);
}

static void errorAt(Token *token, const char *message)
{
  fprintf(stderr, "[line %d, col %d] Error", token->line, token->column);
  if (token->type == TOKEN_EOF)
  {
    fprintf(stderr, " at end");
  }
  else if (token->type == TOKEN_ERROR)
  {
  }
  else
  {
    fprintf(stderr, " at '%.*s'", token->length, token->start);
  }
  fprintf(stderr, ": %s\n", message);
  parser.hadError = true;
  exit(65);
}

static void error(const char *message)
{
  errorAt(&parser.previous, message);
}

static void errorAtCurrent(const char *message)
{
  errorAt(&parser.current, message);
}
static void advance();
static void consume(TokenType type, const char *message)
{
  if (parser.current.type == type)
  {
    advance();
    return;
  }
  errorAtCurrent(message);
}
static ASTNode *integer()
{
  return makePrevAsNode(NODE_INTEGER);
}
static void advance()
{
  parser.previous = parser.current;
  // skip all error tokens
  for (;;)
  {
    parser.current = scanToken();
    if (parser.current.type != TOKEN_ERROR)
      break;
    errorAtCurrent(parser.current.start);
  }
}
static bool isAtEnd()
{
  return parser.current.type == TOKEN_EOF;
}
static bool check(TokenType type)
{
  if (isAtEnd())
    return false;
  return parser.current.type == type;
}
static bool match(TokenType type)
{
  if (!check(type))
    return false;
  advance();
  return true;
}
static ASTNode *identifier()
{
  // printf("identifier: %.*s\n", parser.previous.length, parser.previous.start);
  // return INT_VAL(24);
  return makePrevAsNode(NODE_IDENTIFIER);
}
static ASTNode *expression();
static ASTNode *primary()
{
  ASTNode *node = makePrevAsNode(NODE_PRIMARY);
  ASTNode *child;
  if (match(TOKEN_INT))
  {
    child = integer();
  }
  else if (match(TOKEN_IDENTIFIER))
  {
    child = identifier();
  }
  else if (match(TOKEN_LEFT_PAREN))
  {
    addChild(node, makePrevAsNode(NODE_LITERAL));
    addChild(node, expression());
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
    addChild(node, makePrevAsNode(NODE_LITERAL));
    return node;
  }
  else
  {
    errorAtCurrent("Expect expression.");
    return NULL;
  }
  addChild(node, child);
  return node;
}
static ASTNode *unary()
{
  ASTNode *node = makePrevAsNode(NODE_UNARY);

  if (match(TOKEN_MINUS))
  {
    addChild(node, makePrevAsNode(NODE_OPERATOR));
    addChild(node, unary());
    return node;
  }
  addChild(node, primary());
  return node;
}
static ASTNode *factor()
{
  ASTNode *node = makePrevAsNode(NODE_FACTOR);

  addChild(node, unary());
  while (match(TOKEN_STAR) || match(TOKEN_SLASH))
  {
    // TokenType operatorType = parser.previous.type;
    addChild(node, makePrevAsNode(NODE_OPERATOR));
    addChild(node, unary());
  }
  return node;
}
static ASTNode *term()
{
  ASTNode *node = makePrevAsNode(NODE_TERM);

  addChild(node, factor());
  while (match(TOKEN_PLUS) || match(TOKEN_MINUS))
  {
    // Token operator = parser.previous;
    addChild(node, makePrevAsNode(NODE_OPERATOR));
    addChild(node, factor());
    // Value right = factor();
  }

  return node;
}
static ASTNode *comparison()
{
  ASTNode *node = makePrevAsNode(NODE_COMPARISON);

  addChild(node, term());
  if (match(TOKEN_GREATER))
  {
    addChild(node, term());
    // val = makeBool(AS_INT(val) > AS_INT(right));
  }
  return node;
}
static ASTNode *equality()
{
  ASTNode *node = makePrevAsNode(NODE_EQUALITY);

  addChild(node, comparison());
  if (match(TOKEN_EQUAL_EQUAL))
  {
    //  comparison();
    addChild(node, comparison());
    // val = makeBool(AS_BOOL(val) == AS_BOOL(right) || AS_INT(val) == AS_INT(right));
  }
  return node;
}
static ASTNode *expression()
{
  return equality();
}
static ASTNode *writeStatement()
{
  ASTNode *node = makePrevAsNode(NODE_WRITE_STMT);
  addChild(node, expression());
  // printf("%d\n", AS_INT(val));
  return node;
}
static ASTNode *assignment()
{
  ASTNode *node = makePrevAsNode(NODE_ASSIGN_STMT);
  // char *name = parser.previous.start;
  // int length = parser.previous.length;
  consume(TOKEN_EQUAL, "Expect '=' after variable name.");
  addChild(node, makePrevAsNode(NODE_LITERAL));
  addChild(node, expression());
  return node;
  // if (hashmap_get(declaredMap, (void *)name, length - 1, NULL))
  // {
  //   hashmap_set(map, name, length - 1, (uintptr_t)AS_INT(value));
  // }
  // else
  // {
  //   error("Undefined variable.");
  //   parser.runtimeError = true;
  // }
}
static ASTNode *readStatement()
{
  ASTNode *node = makePrevAsNode(NODE_READ_STMT);
  consume(TOKEN_IDENTIFIER, "Expected identifier for 'read' statement.");
  // const char *start = parser.previous.start;
  // int length = parser.previous.length;
  addChild(node, makePrevAsNode(NODE_IDENTIFIER));
  return node;
  // int value;
  // scanf("%d", &value);
  // hashmap_set(map, (void *)start, length - 1, (uintptr_t)value);
}
static ASTNode *program();
static ASTNode *statement();
static ASTNode *block()
{
  ASTNode *node = makePrevAsNode(NODE_BLOCK);
  while (!check(TOKEN_RIGHT_BRACE) && !isAtEnd())
  {
    addChild(node, statement());
  }
  consume(TOKEN_RIGHT_BRACE, "Expect '}' after block.");
  return node;
}
static ASTNode *forStatement()
{
  ASTNode *node = makePrevAsNode(NODE_FOR_STMT);
  consume(TOKEN_LEFT_PAREN, "Expect '(' after 'for'.");
  addChild(node, expression());
  consume(TOKEN_SEMICOLON, "Expected ';' after first clause.");
  addChild(node, expression());
  consume(TOKEN_SEMICOLON, "Expected ';' after second clause.");
  addChild(node, expression());
  consume(TOKEN_RIGHT_PAREN, "Expect ')' after 'for'.");
  consume(TOKEN_LEFT_BRACE, "Expect '{'");
  addChild(node, block());
  // consume(TOKEN_RIGHT_BRACE, "Expect '}' after 'for' body.");
  return node;
}
static ASTNode *forStatement1()
{
  ASTNode *node = makePrevAsNode(NODE_FOR_STMT);

  consume(TOKEN_LEFT_PAREN, "Expected '(' after 'for'");
  // first clause
  if (!check(TOKEN_SEMICOLON))
  {
    addChild(node, expression());
  }
  consume(TOKEN_SEMICOLON, "Expected ';' after first clause of 'for' statement.");
  addChild(node, makePrevAsNode(NODE_LITERAL));
  // second clause
  if (!check(TOKEN_SEMICOLON))
  {
    addChild(node, expression());
  }
  consume(TOKEN_SEMICOLON, "Expected ';' after first clause of 'for' statement.");
  addChild(node, makePrevAsNode(NODE_LITERAL));
  // third clause
  if (!check(TOKEN_RIGHT_PAREN))
  {
    addChild(node, expression());
  }
  // consume right paren
  consume(TOKEN_RIGHT_PAREN, "Expected ')' after 'for' statement.");
  addChild(node, makePrevAsNode(NODE_LITERAL));
  consume(TOKEN_LEFT_BRACE, "Expected '{' after 'for' statement.");
  addChild(node, makePrevAsNode(NODE_LITERAL));
  addChild(node, program());
  return node;
}
static ASTNode *statement()
{
  ASTNode *node;
  if (match(TOKEN_IDENTIFIER))
  {
    node = assignment();
  }
  else if (match(TOKEN_READ))
  {
    node = readStatement();
  }
  else if (match(TOKEN_WRITE))
  {
    node = writeStatement();
  }
  else if (match(TOKEN_FOR))
  {
    node = forStatement();
  }
  else
  {
    errorAtCurrent("Expected statement.");
    return NULL;
  }
  consume(TOKEN_SEMICOLON, "Expected ';' after statement.");
  return node;
}
static ASTNode *program()
{
  ASTNode *node = makeNode(NODE_PROGRAM);
  while (!isAtEnd() && !parser.hadError && !parser.runtimeError)
  {
    addChild(node, statement());
  }
  consume(TOKEN_EOF, "Expected end of file");
  return node;
}
static ASTNode *declarations()
{
  ASTNode *declNode = makeNode(NODE_DECLARATION);
  if (match(TOKEN_INT))
  {
    addChild(declNode, makePrevAsNode(NODE_KEYWORD));
    consume(TOKEN_IDENTIFIER, "Expected identifier in declaration list.");
    addChild(declNode, makePrevAsNode(NODE_IDENTIFIER));

    while (!isAtEnd() && !parser.hadError && !check(TOKEN_SEMICOLON))
    {
      // printf("Type: %d\n", parser.current.type);
      consume(TOKEN_COMMA, "Expected ',' after identifier.");
      consume(TOKEN_IDENTIFIER, "Expected identifier in declaration list.");
      if (parser.hadError)
      {
        break;
      }
      // const char *start = parser.previous.start;
      // int length = parser.previous.length;
      addChild(declNode, makePrevAsNode(NODE_IDENTIFIER));
      // hashmap_set(map, (void *)start, length - 1, (uintptr_t)0);
    }
    consume(TOKEN_SEMICOLON, "Expected ';' after declaration list.");
  }
  return declNode;
}
bool buildAST(const char *source, ASTNode *ast)
{
  initScanner(source);
  advance();
  // Value expr = expression();
  addChild(ast, declarations());
  addChild(ast, program());
  // printf("Answer: %d\n", AS_INT(expr));
  consume(TOKEN_EOF, "Expected end of file.");
  addChild(ast, makePrevAsNode(NODE_LITERAL));
  // if (parser.hadError)
  //   return INTERPRET_COMPILE_ERROR;
  // if (parser.runtimeError)
  //   return INTERPRET_RUNTIME_ERROR;
  // return INTERPRET_OK;
  return !parser.hadError;
}

void printTokens(const char *source)
{
  initScanner(source);
  int line = -1;
  for (;;)
  {
    Token token = scanToken();
    if (token.line != line)
    {
      printf("%4d ", token.line);
      line = token.line;
    }
    else
    {
      printf("   | ");
    }
    printf("%2d '%.*s'\n", token.type, token.length, token.start);
    if (token.type == TOKEN_EOF)
      break;
  }
}
const char *node_labels[20] = {
    "NODE_MAIN_PROGRAM",
    "NODE_PROGRAM",
    "NODE_BLOCK",
    "NODE_DECLARATION",
    "NODE_FOR_STMT",
    "NODE_ASSIGN_STMT",
    "NODE_EXPR",
    "NODE_EQUALITY",
    "NODE_COMPARISON",
    "NODE_TERM",
    "NODE_FACTOR",
    "NODE_UNARY",
    "NODE_PRIMARY",
    "NODE_INTEGER",
    "NODE_IDENTIFIER",
    "NODE_READ_STMT",
    "NODE_WRITE_STMT",
    "NODE_KEYWORD",
    "NODE_LITERAL",
    "NODE_OPERATOR",
};