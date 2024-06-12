#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "parser.h"

void on_memory_error() {
    printf("There was no available memory.\n");
    exit(1);
}

void on_file_open_error(char* filename) {
    printf("Unnable to open file %s\n", filename);
    exit(2);
}

void on_parse_n_groups_error(FILE * file) {
    printf("Line 1: Unnable to parse number of groups.\n");
    fclose(file);
    exit(3);
}

void on_parse_group_error(long line, FILE * file) {
    printf("Line %ld: Unnable to parse group.\n", line);
    fclose(file);
    exit(4);
}

void on_parse_point_error(long line, FILE * file) {
    printf("Line %ld: Unabble to parse point.\n", line);
    fclose(file);
    exit(5);
}

void on_parse_k_error(long line, FILE * file) {
    printf("Line %ld: Unnable to parse K.\n", line);
    fclose(file);
    exit(6);
}

input_t * parse_input(char filename[]) {
    input_t * input;
    FILE * file;
    long n_groups, i, j, line = 1;
    char label;
    point_t * start;
    
    input = (input_t *) malloc(sizeof(input_t));
    if (input == NULL) on_memory_error();
    input->list.size = 0;

    file = fopen(filename, "r");
    if (file == NULL) on_file_open_error(filename);

    if (fscanf(file, " n_groups=%ld ", &n_groups) != 1) on_parse_n_groups_error(file);
    line++;
    
    point_list_t points[n_groups];
    for (i = 0; i < n_groups; i++) {
        if (fscanf(file, " label=%c ", &label) != 1) on_parse_group_error(line, file);
        line++;

        if (fscanf(file, " length=%ld ", &points[i].size) != 1) on_parse_group_error(line, file);
        line++;

        points[i].list = malloc(sizeof(point_t) * points[i].size);
        if (points[i].list == NULL) on_memory_error();

        for (j = 0; j < points[i].size; j++) {
            if (fscanf(file, " (%lf ,%lf) ", &points[i].list[j].x, &points[i].list[j].y) != 2) on_parse_point_error(line, file);
            line++;
            points[i].list[j].label = label;
            points[i].list[j].distance = 0;
        }

        input->list.size += points[i].size;
    }

    input->list.list = (point_t *) malloc(sizeof(point_t) * input->list.size);
    start = input->list.list;
    for (i = 0; i < n_groups; i++) {
        memcpy(start, points[i].list, sizeof(point_t) * points[i].size);
        start = start + points[i].size;
        free(points[i].list);
    }

    if (fscanf(file, " k=%d ", &input->k) != 1) on_parse_k_error(line, file);
    line++;

    if (fscanf(file, " (%lf ,%lf) ", &input->point.x, &input->point.y) != 2) on_parse_point_error(line, file);

    fclose(file);

    return input;
}

void free_input(input_t * input) {
    free(input->list.list);
    free(input);
}