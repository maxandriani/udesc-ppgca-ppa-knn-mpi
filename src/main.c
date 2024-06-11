#include <stdlib.h>
#include <stdio.h>
#include "knn.h"
#include "parser.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("You should provide a input file.\n");
        return 1;
    }

    input_t* input = parse_input(argv[1]);
    printf("%ld points loaded.\nClassifying (%lf, %lf) w/ k = %d.\n", input->list.size, input->point.x, input->point.y, input->k);
    char result = knn(&input->list, input->point, input->k);
    printf("Result: %c.\n", result);
    free_input(input);
    return 0;
}