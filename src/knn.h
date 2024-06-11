#ifndef KNN_H
#define KNN_H

#include <stdio.h>
#include <stdlib.h>

typedef struct point {
    char label;
    double x;
    double y;
    double distance;
} point_t;

typedef struct point_list {
    point_t* list;
    long size;
} point_list_t;

char knn(point_list_t* list, point_t find, int k);
double euclidean_distance_no_sqrt(point_t toEvaluate, point_t point);

#endif