/**
 *  \file probSemSharedMemSmokers.c (implementation file)
 *
 *  \brief Problem name: Smokers
 *
 *  Synchronization based on semaphores and shared memory.
 *  Implementation with SVIPC.
 *
 *  Generator process of the intervening entities.
 *
 *  Upon execution, one parameter is requested:
 *    \li name of the logging file.
 *
 *  \author Nuno Lau - December 2019
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <string.h>
#include <math.h>

#include "probConst.h"
#include "probDataStruct.h"
#include "logging.h"
#include "sharedDataSync.h"
#include "semaphore.h"
#include "sharedMemory.h"

/** \brief name of agent program */
#define   AGENT               "./agent"

/** \brief name of watcher program */
#define   WATCHER             "./watcher"

/** \brief name of smoker program */
#define   SMOKER              "./smoker"


/**
 *  \brief Main program.
 *
 *  Its role is starting the simulation by generating the intervening entities processes (agent, watcher and smokers)
 *  and waiting for their termination.
 */
int main (int argc, char *argv[])
{
    char nFic[51];                                                                              /*name of logging file */
    char nFicErr[] = "error_        ";                                                     /* base name of error files */
    int shmid,                                                                      /* shared memory access identifier */
        semgid;                                                                     /* semaphore set access identifier */
    unsigned int  m;                                                                             /* counting variables */
    SHARED_DATA *sh;                                                                /* pointer to shared memory region */
    int pidAG,                                                                             /* agent process identifier */
        pidWT[NUMINGREDIENTS],                                                    /* watchers process identifier array */
        pidSM[NUMSMOKERS];                                                         /* smokers process identifier array */
    int key;                                                           /*access key to shared memory and semaphore set */
    char num[2][12];                                                     /* numeric value conversion (up to 10 digits) */
    int status,                                                                                    /* execution status */
        info;                                                                                               /* info id */

    /* getting log file name */
    if(argc==2) {
        strcpy(nFic, argv[1]);
    }
    else strcpy(nFic, "");

    /* composing command line */
    if ((key = ftok (".", 'a')) == -1) {
        perror ("error on generating the key");
        exit (EXIT_FAILURE);
    }
    sprintf (num[1], "%d", key);

    /* creating and initializing the shared memory region and the log file */
    if ((shmid = shmemCreate (key, sizeof (SHARED_DATA))) == -1) { 
        perror ("error on creating the shared memory region");
        exit (EXIT_FAILURE);
    }
    if (shmemAttach (shmid, (void **) &sh) == -1) { 
        perror ("error on mapping the shared region on the process address space");
        exit (EXIT_FAILURE);
    }

    /* initialize random generator */
    srandom ((unsigned int) getpid ());                                

    /* initialize problem internal status */
    sh->fSt.st.agentStat        = PREPARING;                            /* the agent prepares ingredients */
    int w;
    for (w = 0; w < NUMINGREDIENTS; w++) {
        sh->fSt.st.watcherStat[w] = WAITING_ING;                              /* watchers are initialized */
        sh->fSt.ingredients[w]=0;
    }
    int s;
    for (s = 0; s < NUMSMOKERS; s++) {
        sh->fSt.st.smokerStat[s] = WAITING_2ING;                               /* smokers are initialized */
        sh->fSt.nCigarettes[s]=0;
    }

    sh->fSt.nIngredients = NUMINGREDIENTS;
    sh->fSt.nSmokers     = NUMSMOKERS;

    sh->fSt.nOrders      = NUMORDERS;


    /* create log file */
    createLog (nFic, &sh->fSt);                                  
    saveState(nFic,&sh->fSt);

    /* initialize semaphore ids */
    sh->mutex                       = MUTEX;                                /* mutual exclusion semaphore id */
    sh->waitCigarette               = WAITCIGARETTE;                         
 
    int i;
    for(i=0;i<NUMINGREDIENTS;i++) {
       sh->ingredient[i]            = INGREDIENT+i;                                                      
    }
    for(s=0;s<NUMSMOKERS;s++) {
       sh->wait2Ings[s]             = WAIT2INGS+s;                                                      
    }

    /* creating and initializing the semaphore set */
    if ((semgid = semCreate (key, SEM_NU)) == -1) { 
        perror ("error on creating the semaphore set");
        exit (EXIT_FAILURE);
    }
    if (semUp (semgid, sh->mutex) == -1) {                   /* enabling access to critical region */
        perror ("error on executing the up operation for semaphore access");
        exit (EXIT_FAILURE);
    }

    /* generation of intervening entities processes */                            
    /* agent process */
    strcpy (nFicErr + 6, "AG");
    if ((pidAG = fork ()) < 0)  {                            
        perror ("error on the fork operation for the agent");
        exit (EXIT_FAILURE);
    }
    if (pidAG == 0) {
        if (execl (AGENT, AGENT, nFic, num[1], nFicErr, NULL) < 0) {
            perror ("error on the generation of the agent process");
            exit (EXIT_FAILURE);
        }
    }
    /* watcher processes */
    strcpy (nFicErr + 6, "WT");
    for (w = 0; w < NUMINGREDIENTS; w++) {           
        if ((pidWT[w] = fork ()) < 0) {
            perror ("error on the fork operation for the watcher");
            exit (EXIT_FAILURE);
        }
        sprintf(num[0],"%d",w);
        sprintf(nFicErr+8,"%02d",w); 
        if (pidWT[w] == 0)
            if (execl (WATCHER, WATCHER, num[0], nFic, num[1], nFicErr, NULL) < 0) { 
                perror ("error on the generation of the watcher process");
                exit (EXIT_FAILURE);
            }
    }

    /* smoker processes */
    strcpy (nFicErr + 6, "SM");
    for (s = 0; s < NUMSMOKERS; s++) {           
        if ((pidSM[s] = fork ()) < 0) {
            perror ("error on the fork operation for the smoker");
            exit (EXIT_FAILURE);
        }
        sprintf(num[0],"%d",s);
        sprintf(nFicErr+8,"%02d",s); 
        if (pidSM[s] == 0)
            if (execl (SMOKER, SMOKER, num[0], nFic, num[1], nFicErr, NULL) < 0) { 
                perror ("error on the generation of the watcher process");
                exit (EXIT_FAILURE);
            }
    }


    /* signaling start of operations */
    if (semSignal (semgid) == -1) {
        perror ("error on signaling start of operations");
        exit (EXIT_FAILURE);
    }

    /* waiting for the termination of the intervening entities processes */
    m = 0;
    do {
        info = wait (&status);
        if (info == -1) { 
            perror ("error on aiting for an intervening process");
            exit (EXIT_FAILURE);
        }
        m += 1;
    } while (m < 1 + NUMINGREDIENTS + NUMSMOKERS);

    /* destruction of semaphore set and shared region */
    if (semDestroy (semgid) == -1) {
        perror ("error on destructing the semaphore set");
        exit (EXIT_FAILURE);
    }
    if (shmemDettach (sh) == -1) { 
        perror ("error on unmapping the shared region off the process address space");
        exit (EXIT_FAILURE);
    }
    if (shmemDestroy (shmid) == -1) { 
        perror ("error on destructing the shared region");
        exit (EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
