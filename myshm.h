#include <semaphore.h>

struct Memory {
     int  counter;
     int  food;
     sem_t sp1;
     sem_t sp2;
     sem_t sp3;
     pid_t pid1;
     pid_t pid2;
     pid_t pid3;
     int count1,count2,count3;
};