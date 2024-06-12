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

char knn_most_frequent(point_list_t* points, int k);
void knn_compute_distance(point_list_t* points, point_t find);
double euclidean_distance_no_sqrt(point_t toEvaluate, point_t point);
void knn_sort_points(point_list_t * points);

#endif