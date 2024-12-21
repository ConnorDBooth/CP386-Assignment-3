#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_THREADS 100

typedef struct{
    int p_id;
    int arr_time;
    int burst_time;
    int waiting_time;
    int turn_around_time;
    int completion_time;
} threadInfo; 

threadInfo threads[MAX_THREADS];
int num_threads = 0; 

int turn_around_time(int thread_completion_time, int thread_arrival_time){
    return thread_completion_time - thread_arrival_time;
}

int waiting_time(int turn_around_time, int thread_burst_time){
    return turn_around_time - thread_burst_time;
}

void read_file(const char *file_name){
    FILE *file = fopen(file_name, "r");
    if (file == NULL){
        perror("error opening file");
        exit(EXIT_FAILURE);
    }

    while (fscanf(file, "%d, %d, %d", &threads[num_threads].p_id, &threads[num_threads].arr_time, &threads[num_threads].burst_time) == 3){
        num_threads++;
    }

    fclose(file); 
}

void calc(){
    int current_time = 0;

    for (int i = 0; i < num_threads; i++){
        if (current_time < threads[i].arr_time){
            current_time = threads[i].arr_time;
        }
        threads[i].completion_time = current_time + threads[i].burst_time;

        threads[i].turn_around_time = turn_around_time(threads[i].completion_time, threads[i].arr_time);
        threads[i].waiting_time = waiting_time(threads[i].turn_around_time, threads[i].burst_time);

        current_time = threads[i].completion_time;
    }
}

void display(){
    int total_waiting_time = 0;
    int total_turn_around_time = 0;

    printf("Thread ID\tArrival Time\tBurstTime\tCompletion Time\tTurn-Around Time\tWaiting Time\n");
    for(int i = 0; i < num_threads; i++){
        total_waiting_time += threads[i].waiting_time;
        total_turn_around_time += threads[i].turn_around_time;
        printf("%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t\t%d\n",
            threads[i].p_id,
            threads[i].arr_time,
            threads[i].burst_time,
            threads[i].completion_time,
            threads[i].turn_around_time,
            threads[i].waiting_time);
    }

    float avg_waiting_time = (float)total_waiting_time / num_threads;
    float avg_turnaround_time = (float)total_turn_around_time / num_threads;

    printf("\nThe average waiting time: %.2f\n", avg_waiting_time);
    printf("\nThe average turnaround time: %.2f\n", avg_turnaround_time);

}

int main(){

    read_file("sample_in_schedule.txt");

    calc();

    display();

    return 0;

}