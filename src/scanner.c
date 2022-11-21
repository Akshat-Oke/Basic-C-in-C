#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "common.h"
#include "scanner.h"

typedef struct
{
  // start of lexeme
  const char *start;
  // current character in the lexeme
  // that starts at `start`
  const char *current;
  int line;
  int column;
} Scanner;

Scanner scanner;

void initScanner(const char *source)
{
  scanner.start = source;
  scanner.current = source;
  scanner.line = 1;
  scanner.column = 1;
}
static bool isAtEnd()
{
  return *scanner.current == '\0';
}

static Token makeToken(TokenType type)
{
  Token token;
  token.type = type;
  token.start = scanner.start;
  token.length = (int)(scanner.current - scanner.start);
  token.line = scanner.line;
  token.column = scanner.column;
  return token;
}

static Token errorToken(const char *message)
{
  Token token;
  token.type = TOKEN_ERROR;
  token.start = message;
  token.length = (int)strlen(message);
  token.line = scanner.line;
  token.column = scanner.column;
  return token;
}

static char advance()
{
  scanner.current++;
  scanner.column++;
  return scanner.current[-1];
}
static bool match(char expected)
{
  if (isAtEnd())
    return false;
  if (*scanner.current != expected)
    return false;
  scanner.current++;
  scanner.column++;
  return true;
}
static char peek()
{
  return *scanner.current;
}
static void skipWhitespace()
{
  for (;;)
  {
    char c = peek();
    switch (c)
    {
    case ' ':
    case '\r':
    case '\t':
      advance();
      break;
    case '\n':
      scanner.line++;
      scanner.column = 1;
      advance();
      break;
    default:
      return;
    }
  }
}

static bool isDigit(char c)
{
  return c >= '0' && c <= '9';
}

static Token number()
{
  while (isDigit(peek()))
    advance();
  return makeToken(TOKEN_INT);
}
static bool isAlpha(char c)
{
  return (c >= 'a' && c <= 'z') ||
         (c >= 'A' && c <= 'Z') ||
         c == '_';
}
static TokenType identifierType()
{
  switch (scanner.start[0])
  {
  case 'f':
    return TOKEN_FOR;
  case 'i':
    return TOKEN_INT;
  case 'r':
    return TOKEN_READ;
  case 'w':
    return TOKEN_WRITE;
  default:
    return TOKEN_IDENTIFIER;
  }
  return TOKEN_IDENTIFIER;
}
static Token identifier()
{
  while (isAlpha(peek()) || isDigit(peek()))
    advance();
  return makeToken(identifierType());
}
Token scanToken()
{
  skipWhitespace();
  scanner.start = scanner.current;

  if (isAtEnd())
    return makeToken(TOKEN_EOF);

  char c = advance();
  if (isAlpha(c))
    return identifier();
  if (isDigit(c))
    return number();

  switch (c)
  {
  case '(':
    return makeToken(TOKEN_LEFT_PAREN);
  case ')':
    return makeToken(TOKEN_RIGHT_PAREN);
  case ',':
    return makeToken(TOKEN_COMMA);
  case ';':
    return makeToken(TOKEN_SEMICOLON);
  case '=':
    return makeToken(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
  case '>':
    return makeToken(TOKEN_GREATER);
  case '+':
    return makeToken(TOKEN_PLUS);
  case '-':
    return makeToken(TOKEN_MINUS);
  case '*':
    return makeToken(TOKEN_STAR);
  case '/':
    return makeToken(TOKEN_SLASH);
  }

  return errorToken("Unexpected character.");
}