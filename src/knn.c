#include "knn.h"

double euclidean_distance_no_sqrt(point_t a, point_t b) {
    return ((b.x - a.x) * (b.x - a.x)) + ((b.y - a.y) * (b.y - a.y));
}

int compare_label_for_sort(const void *a, const void *b) {
    return ((point_t *) a)->label - ((point_t *) b)->label;
}

int compare_distance_for_sort(const void *a, const void *b) {
    return ((point_t *) a)->distance - ((point_t *) b)->distance;
}

void output_points(point_list_t * points) {
    long x;
    printf("Points: ");
    for (x = 0; x < points->size; x++) {
        printf(" (%lf, %lf, %c, %lf) ", points->list[x].x, points->list[x].y, points->list[x].label, points->list[x].distance);
    }
    printf("\n");
}

char knn_most_frequent(point_list_t* points, int k) {
    qsort(points->list, k, sizeof(point_t), compare_label_for_sort); // sort by label asc to count frequency
    char most_frequent = points->list[0].label;
    int most_frequent_count = 1;
    int current_frequency = 1;

    for (int x = 1; x < k; x++) {
        if (points->list[x].label != points->list[x - 1].label) {
            if (current_frequency > most_frequent_count) {
                most_frequent = points->list[x - 1].label;
                most_frequent_count = current_frequency;
            }

            current_frequency = 1;
        } else {
            current_frequency++;
        }

        if (x == k - 1 && current_frequency > most_frequent_count) {
            most_frequent = points->list[x - 1].label;
            most_frequent_count = current_frequency;
        }
    }

    return most_frequent;
}

void knn_compute_distance(point_list_t* points, point_t find) {
    for (int x = 0; x < points->size; x++) {
        points->list[x].distance  = euclidean_distance_no_sqrt(find, points->list[x]);
    }
}

void knn_sort_points(point_list_t * points) {
    qsort(points->list, points->size, sizeof(point_t), compare_distance_for_sort);
}
