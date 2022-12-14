#ifndef toc_scanner
#define toc_scanner

typedef enum TokenType {
  TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
  TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
  TOKEN_COMMA, TOKEN_SEMICOLON,
  TOKEN_EQUAL,
  TOKEN_GREATER, TOKEN_EQUAL_EQUAL,
  TOKEN_PLUS, TOKEN_MINUS,
  TOKEN_STAR, TOKEN_SLASH,
  // Keywords
  TOKEN_FOR, TOKEN_INT, TOKEN_READ,
  TOKEN_NUMBER, //number literal (123, 234)
  TOKEN_WRITE,
  TOKEN_IDENTIFIER,
  TOKEN_ERROR, TOKEN_EOF
} TokenType;

typedef struct {
  TokenType type;
  const char* start;
  int length;
  int line;
  int column;
} Token;

void initScanner(const char *source);
Token scanToken();

#endif