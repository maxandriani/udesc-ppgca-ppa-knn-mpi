#ifndef PARSER_H
#define PARSER_H

#include "knn.h"

typedef struct input {
    point_list_t list;
    point_t point;
    int k;
} input_t;

input_t * parse_input(char filename[]);
void free_input(input_t * input);

#endif