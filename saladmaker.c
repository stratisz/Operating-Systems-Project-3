#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include "myshm.h"

#define SEGMENTSIZE sizeof(struct Memory)
#define SEGMENTPERM 0666

int main(int argc, char *argv[]){
    int lb,ub;
    int shmid;
    int nump;
    char* iliko;
    char* proccessName;
    for(int i = 1 ; i < argc ; i++){
        if(strcmp(argv[i],"-t1")==0){
            lb = atoi(argv[i+1]);
        }
        if(strcmp(argv[i],"-t2")==0){
            ub = atoi(argv[i+1]);
        }
        if(strcmp(argv[i],"-s")==0){
            shmid = atoi(argv[i+1]);
        }
        if(strcmp(argv[i],"-i")==0){
            nump = atoi(argv[i+1]);
            if(nump == 1){
                iliko = "kremudi";
                proccessName = "Saladmaker1";
            }
            else if(nump == 2){
                iliko = "ntomata";
                proccessName = "Saladmaker2";
            }
            else{
                iliko = "piperia";
                proccessName = "Saladmaker3";
            }
        }
    }
    struct Memory *shmptr;
	int retval;
	int err;

	if (argc <= 1) { printf("Need shmem id. \n"); exit(1); }

	/* Get id from command line. */
	printf("Allocated %d\n", shmid);

	/* Attach the segment. */
	shmptr = (struct Memory *) shmat(shmid,(void*) 0, 0);
	if (shmptr == (void *) -1) { perror("Attachment."); exit(2);}

    FILE *f;

    if(nump == 1){
        f = fopen("saladmaker1.log", "a+");
        shmptr->pid1 = getpid();
        shmptr->count1 = 0;
    }
    else if(nump == 2){
        f = fopen("saladmaker2.log", "a+");
        shmptr->pid2 = getpid();
        shmptr->count2 = 0;
    }
    else{
        f = fopen("saladmaker3.log", "a+");
        shmptr->pid3 = getpid();
        shmptr->count3 = 0;
    }
    FILE * fp;
    fp = fopen("chef.log", "a+");
	/* Initialize the semaphore. */
    time_t t;
    srand(time(NULL));
 
    while (shmptr->counter > 0){
        struct timeval tv;
        struct tm *info;
        if(nump == 1){
            gettimeofday(&tv, NULL);
            info = localtime(&tv.tv_sec);
            fprintf(f,"[%02d:%02d:%02d:%02d:%03ld][%d][%s][Waiting for ingredients]\n", info->tm_mday,info->tm_hour, info->tm_min, info->tm_sec, tv.tv_usec%1000,getpid(),proccessName);
            fprintf(fp,"[%02d:%02d:%02d:%02d:%03ld][%d][%s][Waiting for ingredients]\n", info->tm_mday,info->tm_hour, info->tm_min, info->tm_sec, tv.tv_usec%1000 ,getpid(),proccessName);
            fflush(fp);
            retval = sem_init(&(shmptr->sp1),1,0);
	        if (retval != 0) {
		       perror("Couldn't initialize.");
		        exit(3);
	        }
            if(shmptr->food != 0)
                sem_wait(&(shmptr->sp1));
            if(shmptr->counter <= 0) break;
            shmptr->food = -1;

            gettimeofday(&tv, NULL);
            info = localtime(&tv.tv_sec);
            fprintf(f,"[%02d:%02d:%02d:%02d:%03ld][%d][%s][Get ingredients]\n", info->tm_mday,info->tm_hour, info->tm_min, info->tm_sec, tv.tv_usec%1000 ,getpid(),proccessName);
            fprintf(fp,"[%02d:%02d:%02d:%02d:%03ld][%d][%s][Get ingredients]\n", info->tm_mday,info->tm_hour, info->tm_min, info->tm_sec, tv.tv_usec%1000 ,getpid(),proccessName);
            fprintf(f,"[%02d:%02d:%02d:%02d:%03ld][%d][%s][Start making salad]\n", info->tm_mday,info->tm_hour, info->tm_min, info->tm_sec, tv.tv_usec%1000 ,getpid(),proccessName);
            fprintf(fp,"[%02d:%02d:%02d:%02d:%03ld][%d][%s][Start making salad]\n", info->tm_mday,info->tm_hour, info->tm_min, info->tm_sec, tv.tv_usec%1000 ,getpid(),proccessName);
            fflush(fp);
            sleep(lb + (double)rand() * (ub - lb) / (double)RAND_MAX);
            /////printf("%f\n",lb + (double)rand() * (ub - lb) / (double)RAND_MAX);
            gettimeofday(&tv, NULL);
            info = localtime(&tv.tv_sec);
            fprintf(f,"[%02d:%02d:%02d:%02d:%03ld][%d][%s][End making salad]\n", info->tm_mday,info->tm_hour, info->tm_min, info->tm_sec, tv.tv_usec%1000 ,getpid(),proccessName);
            fprintf(fp,"[%02d:%02d:%02d:%02d:%03ld][%d][%s][End making salad]\n", info->tm_mday,info->tm_hour, info->tm_min, info->tm_sec, tv.tv_usec%1000 ,getpid(),proccessName);
            fflush(fp);
            shmptr->count1++;
            shmptr->counter--;
        }
        else if(nump == 2){
            gettimeofday(&tv, NULL);
            info = localtime(&tv.tv_sec);
            fprintf(f,"[%02d:%02d:%02d:%02d:%03ld][%d][%s][Waiting for ingredients]\n", info->tm_mday,info->tm_hour, info->tm_min, info->tm_sec, tv.tv_usec%1000 ,getpid(),proccessName);
            fprintf(fp,"[%02d:%02d:%02d:%02d:%03ld][%d][%s][Waiting for ingredients]\n", info->tm_mday,info->tm_hour, info->tm_min, info->tm_sec, tv.tv_usec%1000 ,getpid(),proccessName);
            fflush(fp);
            retval = sem_init(&(shmptr->sp2),1,0);
	        if (retval != 0) {
		        perror("Couldn't initialize.");
		        exit(3);
	        }
            if(shmptr->food != 1)
                sem_wait(&(shmptr->sp2));
            if(shmptr->counter <= 0) break;
            //////printf("Get ingredients\n");
            shmptr->food = -1;

            gettimeofday(&tv, NULL);
            info = localtime(&tv.tv_sec);
            fprintf(f,"[%02d:%02d:%02d:%02d:%03ld][%d][%s][Get ingredients]\n", info->tm_mday,info->tm_hour, info->tm_min, info->tm_sec, tv.tv_usec%1000 ,getpid(),proccessName);
            fprintf(fp,"[%02d:%02d:%02d:%02d:%03ld][%d][%s][Get ingredients]\n", info->tm_mday,info->tm_hour, info->tm_min, info->tm_sec, tv.tv_usec%1000 ,getpid(),proccessName);
            fprintf(f,"[%02d:%02d:%02d:%02d:%03ld][%d][%s][Start making salad]\n", info->tm_mday,info->tm_hour, info->tm_min, info->tm_sec, tv.tv_usec%1000 ,getpid(),proccessName);
            fprintf(fp,"[%02d:%02d:%02d:%02d:%03ld][%d][%s][Start making salad]\n", info->tm_mday,info->tm_hour, info->tm_min, info->tm_sec, tv.tv_usec%1000 ,getpid(),proccessName);
            fflush(fp);
            sleep(lb + (double)rand() * (ub - lb) / (double)RAND_MAX);

            gettimeofday(&tv, NULL);
            info = localtime(&tv.tv_sec);
            fprintf(f,"[%02d:%02d:%02d:%02d:%03ld][%d][%s][End making salad]\n", info->tm_mday,info->tm_hour, info->tm_min, info->tm_sec, tv.tv_usec%1000 ,getpid(),proccessName);
            fprintf(fp,"[%02d:%02d:%02d:%02d:%03ld][%d][%s][End making salad]\n", info->tm_mday,info->tm_hour, info->tm_min, info->tm_sec, tv.tv_usec%1000 ,getpid(),proccessName);
            fflush(fp);
            shmptr->count2++;
            shmptr->counter--;
        }
        else if(nump == 3){
            gettimeofday(&tv, NULL);
            info = localtime(&tv.tv_sec);
            fprintf(f,"[%02d:%02d:%02d:%02d:%03ld][%d][%s][Waiting for ingredients]\n", info->tm_mday,info->tm_hour, info->tm_min, info->tm_sec, tv.tv_usec%1000 ,getpid(),proccessName);
            fprintf(fp,"[%02d:%02d:%02d:%02d:%03ld][%d][%s][Waiting for ingredients]\n", info->tm_mday,info->tm_hour, info->tm_min, info->tm_sec, tv.tv_usec%1000 ,getpid(),proccessName);
            fflush(fp);
            retval = sem_init(&(shmptr->sp3),1,0);
	        if (retval != 0) {
		        perror("Couldn't initialize.");
		        exit(3);
	        }
            if(shmptr->food != 2)
                sem_wait(&(shmptr->sp3));
            if(shmptr->counter <= 0) break;
            /////printf("Get ingredients\n");
            shmptr->food = -1;

            gettimeofday(&tv, NULL);
            info = localtime(&tv.tv_sec);
            fprintf(f,"[%02d:%02d:%02d:%02d:%03ld][%d][%s][Get ingredients]\n", info->tm_mday,info->tm_hour, info->tm_min, info->tm_sec, tv.tv_usec%1000 ,getpid(),proccessName);
            fprintf(fp,"[%02d:%02d:%02d:%02d:%03ld][%d][%s][Get ingredients]\n", info->tm_mday,info->tm_hour, info->tm_min, info->tm_sec, tv.tv_usec%1000 ,getpid(),proccessName);
            fprintf(f,"[%02d:%02d:%02d:%02d:%03ld][%d][%s][Start making salad]\n", info->tm_mday,info->tm_hour, info->tm_min, info->tm_sec, tv.tv_usec%1000 ,getpid(),proccessName);
            fprintf(fp,"[%02d:%02d:%02d:%02d:%03ld][%d][%s][Start making salad]\n", info->tm_mday,info->tm_hour, info->tm_min, info->tm_sec, tv.tv_usec%1000 ,getpid(),proccessName);
            fflush(fp);
            sleep(lb + (double)rand() * (ub - lb) / (double)RAND_MAX);

            gettimeofday(&tv, NULL);
            info = localtime(&tv.tv_sec);
            fprintf(f,"[%02d:%02d:%02d:%02d:%03ld][%d][%s][End making salad]\n", info->tm_mday,info->tm_hour, info->tm_min, info->tm_sec, tv.tv_usec % 1000,getpid(),proccessName);
            fprintf(fp,"[%02d:%02d:%02d:%02d:%03ld][%d][%s][End making salad]\n", info->tm_mday,info->tm_hour, info->tm_min, info->tm_sec, tv.tv_usec % 1000,getpid(),proccessName);
            fflush(fp);
            shmptr->count3++;
            shmptr->counter--;
        }
    }

    fclose(f);
    fclose(fp);
	/* Remove segment. */
	err = shmdt((void *) shmptr);
	if (err == -1) perror ("Detachment.");	
	return 0;
}