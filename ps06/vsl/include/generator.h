#ifndef TAC_H
#define TAC_H

#include "tree.h"

typedef enum {
    PLUS, MINUS, MULT, DIV, JMP, IFZ, IFNZ, ARG, CALL, RETURN
} tacop_t;

typedef struct {
    tacop_t operation;
} tacinst_t;


void generate ( node_t *root );

#endif
