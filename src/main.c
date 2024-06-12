#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <float.h>
#include "knn.h"
#include "parser.h"

int compute_bcast_pack_size() {
    int points, k, total_points;
    MPI_Pack_size(2, MPI_DOUBLE, MPI_COMM_WORLD, &points);
    MPI_Pack_size(1, MPI_INT, MPI_COMM_WORLD, &k);
    MPI_Pack_size(1, MPI_LONG, MPI_COMM_WORLD, &total_points);
    return points + k + total_points;
}

int compute_scatter_pack_size() {
    int points, label;
    MPI_Pack_size(2, MPI_DOUBLE, MPI_COMM_WORLD, &points); // x, y
    MPI_Pack_size(1, MPI_CHAR, MPI_COMM_WORLD, &label); // label
    return points + label;
}

int compute_gatter_pack_size() {
    int distance, label;
    MPI_Pack_size(1, MPI_DOUBLE, MPI_COMM_WORLD, &distance);
    MPI_Pack_size(1, MPI_CHAR, MPI_COMM_WORLD, &label);
    return distance + label;
}

int main(int argc, char* argv[]) {
    int num_tasks, rank, points_per_job, k;
    long points_count;
    input_t * input;
    point_list_t job_points;
    point_t point;
    point_list_t neightbours;

    int scatter_pack_size, gatter_pack_size, bcast_pack_size;
    void * scatter_send_buffer = NULL;
    void * scatter_recv_buffer = NULL;
    void * gatter_send_buffer = NULL;
    void * gatter_recv_buffer = NULL;
    void * bcast_buffer = NULL;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_tasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (argc < 2) {
        printf("You should provide a input file.\n");
        MPI_Finalize();
        exit(1);
    }

    scatter_pack_size = compute_scatter_pack_size(); // Scatter
    gatter_pack_size = compute_gatter_pack_size(); // Gatter
    bcast_pack_size = compute_bcast_pack_size(); // Broadcast
    bcast_buffer = malloc(bcast_pack_size);
    if (bcast_buffer == NULL) {
        printf("Rank %d: Error allocating memory for bcast_buffer.\n", rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    if (rank == 0) {
        input = parse_input(argv[1]);
        printf("Rank %d: %ld points loaded.\nClassifying (%lf, %lf) w/ k = %d.\n", rank, input->list.size, input->point.x, input->point.y, input->k);
        points_count = input->list.size;
        k = input->k;
        point.x = input->point.x;
        point.y = input->point.y;

        int bcast_send_position = 0;
        MPI_Pack(&points_count, 1, MPI_LONG, bcast_buffer, bcast_pack_size, &bcast_send_position, MPI_COMM_WORLD);
        MPI_Pack(&k, 1, MPI_INT, bcast_buffer, bcast_pack_size, &bcast_send_position, MPI_COMM_WORLD);
        MPI_Pack(&point.x, 1, MPI_DOUBLE, bcast_buffer, bcast_pack_size, &bcast_send_position, MPI_COMM_WORLD);
        MPI_Pack(&point.y, 1, MPI_DOUBLE, bcast_buffer, bcast_pack_size, &bcast_send_position, MPI_COMM_WORLD);
        MPI_Bcast(bcast_buffer, bcast_pack_size, MPI_PACKED, 0, MPI_COMM_WORLD);

        points_per_job = (points_count + (num_tasks - 1)) / num_tasks; // Round up division. the last proccess will proccess less.
        scatter_send_buffer = malloc(points_count * scatter_pack_size);
        if (scatter_send_buffer == NULL) {
           printf("Rank %d: Error allocating memory for scatter_send_buffer.\n", rank);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        for (long i = 0; i < points_count; i++) {
            int scatter_send_position = 0;
            MPI_Pack(&input->list.list[i].x, 1, MPI_DOUBLE, (scatter_send_buffer + (i * scatter_pack_size)), scatter_pack_size, &scatter_send_position, MPI_COMM_WORLD);
            MPI_Pack(&input->list.list[i].y, 1, MPI_DOUBLE, (scatter_send_buffer + (i * scatter_pack_size)), scatter_pack_size, &scatter_send_position, MPI_COMM_WORLD);
            MPI_Pack(&input->list.list[i].label, 1, MPI_CHAR, (scatter_send_buffer + (i * scatter_pack_size)), scatter_pack_size, &scatter_send_position, MPI_COMM_WORLD);
        }

        free_input(input);
    } else {
        MPI_Bcast(bcast_buffer, bcast_pack_size, MPI_PACKED, 0, MPI_COMM_WORLD);
        int bcast_rcsv_position = 0;
        MPI_Unpack(bcast_buffer, bcast_pack_size, &bcast_rcsv_position, &points_count, 1, MPI_LONG, MPI_COMM_WORLD);
        MPI_Unpack(bcast_buffer, bcast_pack_size, &bcast_rcsv_position, &k, 1, MPI_INT, MPI_COMM_WORLD);
        MPI_Unpack(bcast_buffer, bcast_pack_size, &bcast_rcsv_position, &point.x, 1, MPI_DOUBLE, MPI_COMM_WORLD);
        MPI_Unpack(bcast_buffer, bcast_pack_size, &bcast_rcsv_position, &point.y, 1, MPI_DOUBLE, MPI_COMM_WORLD);
        
        points_per_job = (points_count + (num_tasks - 1)) / num_tasks; // Round up division. the last proccess will proccess less.
    }

    scatter_recv_buffer = malloc(points_per_job * scatter_pack_size); // o master tb trabalha fdp.
    if (scatter_recv_buffer == NULL) {
        printf("Rank %d: Error allocating memory for scatter_rect_buffer.\n", rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    free(bcast_buffer);

    MPI_Scatter(scatter_send_buffer, points_per_job * scatter_pack_size, MPI_PACKED, scatter_recv_buffer, points_per_job * scatter_pack_size, MPI_PACKED, 0, MPI_COMM_WORLD);
    job_points.list = (point_t *) malloc(sizeof(point_t) * k);
    job_points.size = points_per_job;
    if (job_points.list == NULL) {
        printf("Rank %d: Error allocating memory for job_points.list.\n", rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    if (rank == (num_tasks - 1)) {
        points_per_job = points_count - (points_per_job * (num_tasks - 1)); // las thread displacement.
    }

    for (int i = 0; i < points_per_job; i++) {
        int scatter_unpack_position = 0;
        MPI_Unpack(scatter_recv_buffer + i * scatter_pack_size, scatter_pack_size, &scatter_unpack_position, &job_points.list[i].x, 1, MPI_DOUBLE, MPI_COMM_WORLD);
        MPI_Unpack(scatter_recv_buffer + i * scatter_pack_size, scatter_pack_size, &scatter_unpack_position, &job_points.list[i].y, 1, MPI_DOUBLE, MPI_COMM_WORLD);
        MPI_Unpack(scatter_recv_buffer + i * scatter_pack_size, scatter_pack_size, &scatter_unpack_position, &job_points.list[i].label, 1, MPI_CHAR, MPI_COMM_WORLD);
    }
    job_points.size = points_per_job;
    
    free(scatter_send_buffer);
    free(scatter_recv_buffer);

    knn_compute_distance(&job_points, point);
    knn_sort_points(&job_points);
    gatter_send_buffer = malloc(k * gatter_pack_size);
    if (gatter_send_buffer == NULL) {
        printf("Rank %d: Error allocating memory for gatter_send_buffer.\n", rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    if (rank == 0) {
        gatter_recv_buffer = malloc(k * gatter_pack_size * num_tasks);
        if (gatter_recv_buffer == NULL) {
            printf("Rank %d: Error allocating memory for gatter_recv_buffer.\n", rank);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    for (int i = 0; i < k; i++) {
        if (i >= job_points.size) {
            job_points.list[i].distance = DBL_MAX - 1;
            job_points.list[i].label = -1;
        }

        int gatter_send_position = 0;
        MPI_Pack(&job_points.list[i].label, 1, MPI_CHAR, gatter_send_buffer + (i * gatter_pack_size), gatter_pack_size, &gatter_send_position, MPI_COMM_WORLD);
        MPI_Pack(&job_points.list[i].distance, 1, MPI_DOUBLE, gatter_send_buffer + (i * gatter_pack_size), gatter_pack_size, &gatter_send_position, MPI_COMM_WORLD);
    }

    free(job_points.list);

    MPI_Gather(gatter_send_buffer, k * gatter_pack_size, MPI_PACKED, gatter_recv_buffer, k * gatter_pack_size, MPI_PACKED, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Rank %d: Recv Gatter.\n", rank);

        int buff_size;


        if (points_per_job < k) {
            buff_size = points_per_job * num_tasks;
        } else {
            buff_size = k * num_tasks;
        }

        neightbours.list = (point_t *) malloc(sizeof(point_t) * buff_size);
        neightbours.size = buff_size;

        if (neightbours.list == NULL) {
            printf("Rank %d: Error allocating memory for neightbours.list.\n", rank);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        for (int i = 0; i < neightbours.size; i++) {
            int gatter_recv_position = 0;
            MPI_Unpack(gatter_recv_buffer + i * gatter_pack_size, num_tasks * k * gatter_pack_size, &gatter_recv_position, &neightbours.list[i].label, 1, MPI_CHAR, MPI_COMM_WORLD);
            MPI_Unpack(gatter_recv_buffer + i * gatter_pack_size, num_tasks * k * gatter_pack_size, &gatter_recv_position, &neightbours.list[i].distance, 1, MPI_DOUBLE, MPI_COMM_WORLD);
        }

        knn_sort_points(&neightbours);
        char result = knn_most_frequent(&neightbours, k);
        printf("Result: %c", result);
        free(neightbours.list);
    }

    free(gatter_send_buffer);
    free(gatter_recv_buffer);

    MPI_Finalize();
    return 0;
}