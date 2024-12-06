#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAX_RESOURCES 100

typedef struct {
    int thread_id;
    int resource_count;
    int resources[MAX_RESOURCES];
    int resource_request; // Added Characteristic based on Professor request
} ThreadInfo;

int thread_count = 0, resource_c = 0, deadlock = 0;

void *use_resources(void *arg) {
    ThreadInfo *info = (ThreadInfo *)arg;

    printf("Thread %d is using resources: ", info->thread_id);
    for (int i = 0; i < info->resource_count; i++) {
        printf("%d ", info->resources[i]);
    }
    printf("\n");

    printf("Thread %d requested additional resource: %d\n", info->thread_id, info->resource_request);
    pthread_exit(NULL);
}

void detector(ThreadInfo thread_info[], int allocated[]) {

    for (int i = 0; i < thread_count; i++) {
        for (int j = 0; j < thread_info[i].resource_count; j++){
            allocated[thread_info[i].resources[j]] += 1;
        }
    }
    for (int i = 0; i < (int)sizeof(*allocated); i++){
        if (allocated[i] > 1){
            deadlock = 1;
            break;
        }

    }
}

void requestdetector(ThreadInfo thread_info[], int allocated[], int requested[]) {
    int reqchain = 0;
    for (int i = 0; i < thread_count; i++) {
        for (int j = 0; j < thread_info[i].resource_count; j++){
            requested[thread_info[i].resource_request]++;
        }
    }
    for (int i = 0; i < (int)sizeof(*requested); i++){
        if (requested[i] > 0 && allocated[i] > 1){
            for (int k = 0; k < thread_count; k++){
                for (int j = 0; j < thread_info[k].resource_count; j++){
                    if (thread_info[k].resources[j] == i && allocated[thread_info[k].resource_request] > 0){
                        deadlock = 1;
                        
                    }
                }
            }
        }

    }
}

void displayDeadlock(int allocated[], int requested[]){
    char rD[(int)sizeof(*allocated)];
    for(int i = 0; i < (int)sizeof(*allocated); i++){
        if(allocated[i] + requested[i] > 1){
            rD[i] = i;
            printf("% d \n", allocated[i]);
        }
    }
    printf("The Resources that are causing problems are : %s \n", rD);
}

void deadlockDetected(pthread_t threads[], ThreadInfo thread_info[], int allocated[], int requested[]){
    int choice, toDelete, toRelease;
    printf("A deadlock has been detected what would you like to do? \n");
    printf("Deadlock is created from. \n");
    displayDeadlock(allocated, requested);
    printf("1. Terminate a Process\n");
    printf("2. Release a Resource\n");
    //scanf("%d", &choice);

    /*
     printf("Which Process would you like to delete? \n");
    scanf("%d", &toDelete);
    */
    if(choice == 1){
        printf("");
        printf("Which Process would you like to delete? \n");
        scanf("%d", &toDelete);
        pthread_t tempT [thread_count - 1];
        ThreadInfo tempTI [thread_count - 1];
        for(int i = 0; i < (int)sizeof(*threads); i++){
            if(i != toDelete){
                tempT[i] = threads[i];
                tempTI[i] = thread_info[i];
            }
        }
        threads = tempT;
        thread_info = tempTI;
    

    }else if (choice == 2){
        printf("Which resource would you like to release? \n");
        scanf("%d", &toRelease);
        
        for(int i = 0; i < (int)(sizeof(*threads)); i++){
            int final[100];
            printf("%d \n", thread_info[i].resource_count);
            for(int k = 0, j = 0; k < 100; k++){
                if(thread_info[i].resources[k] != (int)toRelease){
                    final[j] = thread_info[i].resources[k];
                    j++;
                }
            }
            for(int w = 0; w < (int)thread_info[i].resource_count; w++){
                    thread_info[i].resources[w] = final[w];
            }
        }
        
    }
}


int requestchain(k,j,i) 


int main() {
    

    // Input number of threads
    printf("Enter the number of threads: ");
    scanf("%d", &thread_count);

    // Input number of resources
    printf("Enter the number of resources: ");
    scanf("%d", &resource_c);

    if (thread_count <= 0 || resource_c <= 0) {
        printf("Thread count and resource count must be positive integers.\n");
        return 1;
    }

    pthread_t threads[thread_count];
    ThreadInfo thread_info[thread_count];
    int allocated[resource_c];
    int requested[resource_c];

    for(int i = 0; i < resource_c; i++){
        allocated[i] = 0;
        requested[i] = 0;
    }
    /*
        Input resource allocation for each thread, handles numeric input separated by commas
        so that a single thread can be started preusing multiple resources for added realism

        Alternatively, you can always just only use 1 resource per thread if you so wish.
    */

    for (int i = 0; i < thread_count; i++) {
        printf("Enter resources for thread %d (comma separated): ", i);
        char input[256];
        scanf("%s", input);

        thread_info[i].thread_id = i;
        thread_info[i].resource_count = 0;

        char *token = strtok(input, ",");
        while (token != NULL) {
            int resource = atoi(token);
            if (resource >= 0 && resource < resource_c) {
                thread_info[i].resources[thread_info[i].resource_count++] = resource;
            } else {
                printf("Invalid resource %d for thread %d. Ignoring.\n", resource, i);
            }
            token = strtok(NULL, ",");
        }

        /*
            NEW** Initiates the resource request value to be -1 (no request) 
            ((NO request has been given yet))
            (((Simply to populate the variable so its not null)))
        */
        thread_info[i].resource_request = -1;
    }

    /*
    ======================================================================================
        Input a call to the deadlock detector tool that detects if the resource allocation
        at this point here would create a deadlock. If the return is false, then continue
        on after to resource requests per thread.
    ======================================================================================
    */
   detector(thread_info, allocated);
   if(deadlock == 0){

   } else if (deadlock == 1){
    deadlockDetected(threads, thread_info, allocated,requested );
   }
    /*
        User input for singular resource request for each thread

        For testing purposes and simplicity, each thread gets prompted to request 1 additional resource

    */ 
    for (int i = 0; i < thread_count; i++) {
        printf("Enter resource request for thread %d: ", i);
        int request;
        scanf("%d", &request);

        if (request >= 0 && request < resource_c) {
            thread_info[i].resource_request = request;
        } else {
            printf("Invalid resource request %d for thread %d. Setting to -1.\n", request, i);
            thread_info[i].resource_request = -1;
        }
    }

    requestdetector(thread_info, allocated, requested);

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
