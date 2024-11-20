#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAX_RESOURCES 100

typedef struct {
    int thread_id;
    int resource_count;
    int resources[MAX_RESOURCES];
} ThreadInfo;

void *use_resources(void *arg) {
    ThreadInfo *info = (ThreadInfo *)arg;

    printf("Thread %d is using resources: ", info->thread_id);
    for (int i = 0; i < info->resource_count; i++) {
        printf("%d ", info->resources[i]);
    }
    printf("\n");

    pthread_exit(NULL);
}

int main() {
    int thread_count, resource_count;

    // Input number of threads
    printf("Enter the number of threads: ");
    scanf("%d", &thread_count);

    // Input number of resources
    printf("Enter the number of resources: ");
    scanf("%d", &resource_count);

    if (thread_count <= 0 || resource_count <= 0) {
        printf("Thread count and resource count must be positive integers.\n");
        return 1;
    }

    pthread_t threads[thread_count];
    ThreadInfo thread_info[thread_count];

    // Input resource allocation for each thread
    for (int i = 0; i < thread_count; i++) {
        printf("Enter resources for thread %d (comma separated): ", i);
        char input[256];
        scanf("%s", input);

        thread_info[i].thread_id = i;
        thread_info[i].resource_count = 0;

        char *token = strtok(input, ",");
        while (token != NULL) {
            int resource = atoi(token);
            if (resource >= 0 && resource < resource_count) {
                thread_info[i].resources[thread_info[i].resource_count++] = resource;
            } else {
                printf("Invalid resource %d for thread %d. Ignoring.\n", resource, i);
            }
            token = strtok(NULL, ",");
        }
    }

    // Create threads
    for (int i = 0; i < thread_count; i++) {
        if (pthread_create(&threads[i], NULL, use_resources, (void *)&thread_info[i]) != 0) {
            perror("Failed to create thread");
            return 1;
        }
    }

    // Wait for threads to complete
    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
