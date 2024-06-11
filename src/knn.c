#include "knn.h"

double euclidean_distance_no_sqrt(point_t a, point_t b) {
    return ((b.x - a.x) * ((b.x - a.x))) + ((b.y - a.y) * (b.y - a.y));
}

int compare_label_for_sort(const void *a, const void *b) {
  return *(char*)a - *(char*)b;
}

char knn(point_list_t* points, point_t find, int k) {
    long   x, y, z, size;
    char   labels[k];
    double distances[k];

    for (x = 0; x < k; x++) {
        labels[x] = -1;
        distances[x] = -1;
    }

    for (x = 0; x < points->size; x++) {
        point_t point = points->list[x];
        double distance = euclidean_distance_no_sqrt(find, point);

        for (y = 0; y < k; y++) {
            if (distance < distances[y] || distances[y] == -1) {
                for (z = k - 1; z > y; z--) {
                    distances[z] = distances[z - 1];
                    labels[z] = labels[z - 1];
                }

                distances[y] = distance;
                labels[y] = point.label;

                break;
            }
        }
    }

    qsort(labels, k, sizeof(char), compare_label_for_sort);

    char most_frequent = labels[0];
    int most_frequent_count = 1;
    int current_frequency = 1;

    for (x = 1; x < k; x++) {
        if (labels[x] != labels[x - 1]) {
            if (current_frequency > most_frequent_count) {
                most_frequent = labels[x - 1];
                most_frequent_count = current_frequency;
            }

            current_frequency = 1;
        } else {
            current_frequency++;
        }

        if (x == k - 1 && current_frequency > most_frequent_count) {
            most_frequent = labels[x - 1];
            most_frequent_count = current_frequency;
        }
    }

    return most_frequent;
}
