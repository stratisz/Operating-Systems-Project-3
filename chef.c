#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include "myshm.h"

#define SEGMENTSIZE sizeof(struct Memory)
#define SEGMENTPERM 0666

void time_intervals(){
    FILE* file = fopen("chef.log", "r");
    char line[256];
    char* delim = "[]\n";
    int working = 0;
    char* start2;
    while (fgets(line, sizeof(line), file)) {
        ///printf("%s\n", line); 
        char *ptr = strtok(line, delim);
        char* time = malloc(strlen(ptr)+1);
        strcpy(time,ptr);
        ///printf("%s\n",time);

        ptr = strtok(NULL, delim);

        ptr = strtok(NULL, delim);

        ptr = strtok(NULL, delim);
	   	char* message = malloc(strlen(ptr)+1);
	   	strcpy(message,ptr);
        ///printf("%s\n",message);

        if(strcmp(message,"Start making salad") == 0){
            working++;
            if(working == 3){
                printf("[%s",time);
            }
            if(working == 2){
                start2 = malloc(strlen(time)+1);
                strcpy(start2,time);
            }
        }
        if(strcmp(message,"End making salad") == 0){
            if(working == 3){
                printf(", %s] : %d concurrent salad makers\n",time,working);
            }
            if(working == 2){
                printf("[%s, %s] : %d concurrent salad makers\n",start2,time,working);
                free(start2);
            }
            working--;
        }
        free(time);
        free(message);

        ///////printf("%s", line); 
    }

    fclose(file);
}

int main(int argc, char *argv[]){
    int numOfSlds;
    int mantime;
    for(int i = 1 ; i < argc ; i++){
        if(strcmp(argv[i],"-n")==0){
            numOfSlds = atoi(argv[i+1]);
        }
        if(strcmp(argv[i],"-m")==0){
            mantime = atoi(argv[i+1]);
        }
    }
    struct timeval tv;
    struct tm *info;
    FILE * fp;
    fp = fopen("chef.log", "a+");

	sem_t *sp;
	int retval;
	int id, err;
    struct Memory* shmptr;

	/* Make shared memory segment. */
	id = shmget(IPC_PRIVATE, SEGMENTSIZE, SEGMENTPERM);
	if (id == -1) perror("Creation");
	else printf("Allocated %d\n", id);

	/* Attach the segment. */
	shmptr = (struct Memory*) shmat(id,(void*) 0, 0);
	if ( shmptr == (void *) -1) { perror("Attachment."); exit(2);}

    shmptr->counter = numOfSlds;
    shmptr->food = -1;

	/* Initialize the semaphore. */
	retval = sem_init(&(shmptr->sp1),1,2);
	if (retval != 0) {
		perror("Couldn't initialize.\n");
		exit(3);
	}
    retval = sem_init(&(shmptr->sp2),1,2);
	if (retval != 0) {
		perror("Couldn't initialize.\n");
		exit(3);
	}
    retval = sem_init(&(shmptr->sp3),1,2);
	if (retval != 0) {
		perror("Couldn't initialize.\n");
		exit(3);
	}

    time_t t;
    srand((unsigned) time(&t));
    int i = 0;
    int previous = -1;
    int temp;
    for( i = 0 ; i < numOfSlds ; i++ ) {
        while((shmptr->food)!=-1);
        ////printf("%d\n", rand() % 3);
        temp = rand() % 3;
        while( temp == previous) temp = rand() % 3;
        previous = temp;
        shmptr->food = temp;
        if(temp == 0){
            sem_post(&(shmptr->sp1));
            gettimeofday(&tv, NULL);
            info = localtime(&tv.tv_sec);
            fprintf(fp,"[%02d:%02d:%02d:%02d:%03ld][%d][Chef][Selecting ingredients ntomata piperia]\n", info->tm_mday,info->tm_hour, info->tm_min, info->tm_sec, tv.tv_usec % 1000,getpid());
            fprintf(fp,"[%02d:%02d:%02d:%02d:%03ld][%d][Chef][Notify Saladmaker1]\n", info->tm_mday,info->tm_hour, info->tm_min, info->tm_sec, tv.tv_usec % 1000,getpid());
            fflush(fp);
            sem_post(&(shmptr->sp1));
        }
        else if(temp == 1){
            gettimeofday(&tv, NULL);
            info = localtime(&tv.tv_sec);
            fprintf(fp,"[%02d:%02d:%02d:%02d:%03ld][%d][Chef][Selecting ingredients kremudi piperia]\n", info->tm_mday,info->tm_hour, info->tm_min, info->tm_sec, tv.tv_usec % 1000,getpid());
            fprintf(fp,"[%02d:%02d:%02d:%02d:%03ld][%d][Chef][Notify Saladmaker2]\n", info->tm_mday,info->tm_hour, info->tm_min, info->tm_sec, tv.tv_usec % 1000,getpid());
            fflush(fp);
            sem_post(&(shmptr->sp2));
        }
        else if(temp == 2){
            gettimeofday(&tv, NULL);
            info = localtime(&tv.tv_sec);
            fprintf(fp,"[%02d:%02d:%02d:%02d:%03ld][%d][Chef][Selecting ingredients kremudi ntomata]\n", info->tm_mday,info->tm_hour, info->tm_min, info->tm_sec, tv.tv_usec / 1000,getpid());
            fprintf(fp,"[%02d:%02d:%02d:%02d:%03ld][%d][Chef][Notify Saladmaker3]\n", info->tm_mday,info->tm_hour, info->tm_min, info->tm_sec, tv.tv_usec % 1000,getpid());
            fflush(fp);
            sem_post(&(shmptr->sp3));
        }
        fprintf(fp,"[%02d:%02d:%02d:%02d:%03ld][%d][Chef][Man time for resting]\n", info->tm_mday,info->tm_hour, info->tm_min, info->tm_sec, tv.tv_usec % 1000,getpid());
        fflush(fp);
        sleep(mantime);
    }
    while(shmptr->counter > 0);
    sem_post(&(shmptr->sp1));
    sem_post(&(shmptr->sp2));
    sem_post(&(shmptr->sp3));
    retval = sem_init(&(shmptr->sp1),1,2);
    retval = sem_init(&(shmptr->sp2),1,2);
    retval = sem_init(&(shmptr->sp3),1,2);
    printf("Total #salads: %d\n",numOfSlds);
    printf("#salads of salad_maker1 %d : %d\n",shmptr->pid1,shmptr->count1);
    printf("#salads of salad_maker2 %d : %d\n",shmptr->pid2,shmptr->count2);
    printf("#salads of salad_maker3 %d : %d\n",shmptr->pid3,shmptr->count3);

    fclose(fp);
    time_intervals();


	sem_destroy(&(shmptr->sp1));
    sem_destroy(&(shmptr->sp2));
    sem_destroy(&(shmptr->sp3));

	/* Remove segment. */
	err = shmctl(id, IPC_RMID, 0);
	if (err == -1) perror("Removal.");
	
	return 0;

}
