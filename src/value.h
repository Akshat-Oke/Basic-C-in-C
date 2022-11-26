#ifndef toc_value
#define toc_value
#include <stdbool.h>
#include <stdlib.h>

typedef enum
{
  VAL_BOOL,
  VAL_INT
} ValueType;

typedef struct
{
  ValueType type;
  union
  {
    bool boolean;
    int integer;
  } as;
} Value;

#define IS_BOOL(value) ((value).type == VAL_BOOL)
#define IS_INT(value) ((value).type == VAL_INT)

#define BOOL_VAL(value) ((Value){VAL_BOOL, {.boolean = value}})
#define INT_VAL(value) ((Value){VAL_INT, {.integer = value}})

Value *makeBool(bool value);
Value *makeInt(int value);

#define AS_BOOL(value) ((value).as.boolean)
#define AS_INT(value) ((value).as.integer)

#endif