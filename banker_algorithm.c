#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>

int R,P;
int *resource,*safeSeq;
int **allocated,**maxRequired,**need;
int nProcessRan = 0;
pthread_mutex_t lockResource;
pthread_cond_t condition;
bool SafeSeq();
void* processCode(void* );

int main(int argc, char** argv) {
	srand(time(NULL));
        printf("\n\n\t\t\t\t\t\t\t\t\t\t\tCSE 316\n");
        printf("\t\t\t\t\t\t\t\t\tOPERATING SYSTEM PROJECT\n");
        printf("\t\t\t\t\t\t\t\t\tBANKERS ALGORITHM\n\n\n\n\n");
        printf("Enter the require information about the processes!!! \n\n\n");

        printf("\nNumber of process? ");
        scanf("%d", &P);

        printf("\nNumber of resource? ");	
        scanf("%d", &R);

        resource = (int *)malloc(R * sizeof(*resource));
        printf("\nCurrently Available resources ? ");
        for(int i=0; i<R; i++)
                scanf("%d", &resource[i]);

        allocated = (int **)malloc(P * sizeof(*allocated));
        for(int i=0; i<P; i++)
                allocated[i] = (int *)malloc(R * sizeof(**allocated));

        maxRequired = (int **)malloc(P * sizeof(*maxRequired));
        for(int i=0; i<P; i++)
                maxRequired[i] = (int *)malloc(R * sizeof(**maxRequired));

        printf("\n");
        for(int i=0; i<P ;i++) {
                printf("\nResource allocated to process %d ? ", i+1);
                for(int j=0; j<R; j++)
                        scanf("%d", &allocated[i][j]);
        }
        printf("\n");
        for(int i=0; i<P; i++) {
                printf("\nMaximum resource required by process %d ? ", i+1);
                for(int j=0; j<R; j++)
                        scanf("%d", &maxRequired[i][j]);
        }
        printf("\n");

        need = (int **)malloc(P * sizeof(*need));
        for(int i=0; i<P ;i++)
                need[i] = (int *)malloc(R * sizeof(**need));

        for(int i=0; i<P ;i++)
                for(int j=0; j<R; j++)
                        need[i][j] = maxRequired[i][j] - allocated[i][j];

	safeSeq = (int *)malloc(P * sizeof(*safeSeq));
        for(int i=0; i<P; i++) safeSeq[i] = -1;

        if(!SafeSeq()) {
                printf("\nUnsafe State!\n\n");
                exit(-1);
        }

        printf("\n\nSafe Sequence Found : ");
        for(int i=0; i<P; i++) {
                printf("%-3d", safeSeq[i]+1);
        }

        printf("\nExecuting Processes wait\n\n");
        sleep(1);
	pthread_t process[P];
        pthread_attr_t attr;
        pthread_attr_init(&attr);

	int processNumber[P];
	for(int i=0; i<P; i++) processNumber[i] = i;

        for(int i=0; i<P; i++)
                pthread_create(&process[i], &attr, processCode, (void *)(&processNumber[i]));

        for(int i=0; i<P; i++)
                pthread_join(process[i], NULL);

        printf("\nAll Processes Finished\n");	
        free(resource);
        for(int i=0; i<P; i++) {
                free(allocated[i]);
                free(maxRequired[i]);
		free(need[i]);
        }
        free(allocated);
        free(maxRequired);
	free(need);
        free(safeSeq);
}


bool SafeSeq() {
        int tempRes[R];
        for(int i=0; i<R; i++) tempRes[i] = resource[i];

        bool finished[P];
        for(int i=0; i<P; i++) finished[i] = false;
        int nfinished=0;
        while(nfinished < P) {
                bool safe = false;

                for(int i=0; i<P; i++) {
                      if(!finished[i]) {
                             bool possible = true;

                             for(int j=0; j<R; j++)
                                     if(need[i][j] > tempRes[j]) {
                                             possible = false;
                                             break;
                                        }

                                     if(possible) {
                                         for(int j=0; j<R; j++)
                                        tempRes[j] += allocated[i][j];
                                        safeSeq[nfinished] = i;
                                        finished[i] = true;
                                        ++nfinished;
                                        safe = true;
                                }
                        }
                }

                if(!safe) {
                        for(int k=0; k<P; k++) safeSeq[k] = -1;
                        return false; 
                }
        }
        return true; 
}
void* processCode(void *arg) {
        int p = *((int *) arg);

        pthread_mutex_lock(&lockResource);
        while(p != safeSeq[nProcessRan])
                pthread_cond_wait(&condition, &lockResource);

        printf("\n Process %d", p+1);
        printf("\n\tAllocated : ");
        for(int i=0; i<R; i++)
                printf("%3d", allocated[p][i]);

        printf("\n\tNeeded    : ");
        for(int i=0; i<R; i++)
                printf("%3d", need[p][i]);

        printf("\n\tAvailable : ");
        for(int i=0; i<R; i++)
                printf("%3d", resource[i]);

        printf("\n"); sleep(1);

        printf("\tResource Allocated!");
        printf("\n"); sleep(1);
        printf("\tProcess Code Running wait");
        printf("\n"); sleep(rand()%3 + 2); 
        printf("\tProcess Code Completed");
        printf("\n"); sleep(1);
        printf("\tProcess Releasing Resource wait");
        printf("\n"); sleep(1);
        printf("\tResource Released!");

	for(int i=0; i<R; i++)
                resource[i] += allocated[p][i];

        printf("\n\tNow Available : ");
        for(int i=0; i<R; i++)
                printf("%3d", resource[i]);
        printf("\n\n");

        sleep(1);

        nProcessRan++;
        pthread_cond_broadcast(&condition);
        pthread_mutex_unlock(&lockResource);
	pthread_exit(NULL);
}
