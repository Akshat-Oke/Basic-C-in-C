#include "value.h"
#include <stdbool.h>

Value *makeBool(bool value)
{
  // Value v = {VAL_BOOL, {.boolean = value}};
  Value *v = malloc(sizeof(Value));
  v->type = VAL_BOOL;
  v->as.boolean = value;
  return v;
}
Value *makeInt(int value)
{
  // Value v = {VAL_BOOL, {.boolean = value}};
  Value *v = malloc(sizeof(Value));
  v->type = VAL_INT;
  v->as.integer = value;
  return v;
}