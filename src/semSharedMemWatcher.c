/**
 *  \file semSharedWatcher.c (implementation file)
 *
 *  \brief Problem name: Smokers
 *
 *  Synchronization based on semaphores and shared memory.
 *  Implementation with SVIPC.
 *
 *  Definition of the operations carried out by the watcher:
 *     \li waitForIngredient
 *     \li updateReservations
 *     \li informSmoker
 *
 *  \author Nuno Lau - December 2019
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "probConst.h"
#include "probDataStruct.h"
#include "logging.h"
#include "sharedDataSync.h"
#include "semaphore.h"
#include "sharedMemory.h"

/** \brief logging file name */
static char nFic[51];

/** \brief shared memory block access identifier */
static int shmid;

/** \brief semaphore set access identifier */
static int semgid;

/** \brief pointer to shared memory region */
static SHARED_DATA *sh;

/** \brief watcher waits for ingredient generated by agent */
static bool waitForIngredient (int id);

/** \brief watcher updates reservations in shared mem and checks if some smoker can complete a cigarette */
static int updateReservations (int id);

/** \brief watcher informs smoker that he can use the available ingredients to roll cigarette */
static void informSmoker(int id, int smokerReady);

/**
 *  \brief Main program.
 *
 *  Its role is to generate the life cycle of one of intervening entities in the problem: the watcher.
 */
int main (int argc, char *argv[])
{
    int key;                                            /*access key to shared memory and semaphore set */
    char *tinp;                                                       /* numerical parameters test flag */

    /* validation of command line parameters */
    if (argc != 5) { 
        freopen ("error_WT", "a", stderr);
        fprintf (stderr, "Number of parameters is incorrect!\n");
        return EXIT_FAILURE;
    }
    else { 
        freopen (argv[4], "w", stderr);
        setbuf(stderr,NULL);
    }

    int n = (unsigned int) strtol (argv[1], &tinp, 0);
    if ((*tinp != '\0') || (n >= NUMINGREDIENTS )) { 
        fprintf (stderr, "Watcher process identification is wrong!\n");
        return EXIT_FAILURE;
    }
    strcpy (nFic, argv[2]);
    key = (unsigned int) strtol (argv[3], &tinp, 0);
    if (*tinp != '\0') {
        fprintf (stderr, "Error on the access key communication!\n");
        return EXIT_FAILURE;
    }

    /* connection to the semaphore set and the shared memory region and mapping the shared region onto the
       process address space */
    if ((semgid = semConnect (key)) == -1) { 
        perror ("error on connecting to the semaphore set");
        return EXIT_FAILURE;
    }
    if ((shmid = shmemConnect (key)) == -1) { 
        perror ("error on connecting to the shared memory region");
        return EXIT_FAILURE;
    }
    if (shmemAttach (shmid, (void **) &sh) == -1) { 
        perror ("error on mapping the shared region on the process address space");
        return EXIT_FAILURE;
    }

    /* initialize random generator */
    srandom ((unsigned int) getpid ());              

    /* simulation of the life cycle of the watcher */
    int id = n, smokerReady;
    while( waitForIngredient (id) ) {
        smokerReady = updateReservations(id); 
        if(smokerReady>=0) informSmoker(id, smokerReady);
    }

    /* unmapping the shared region off the process address space */
    if (shmemDettach (sh) == -1) {
        perror ("error on unmapping the shared region off the process address space");
        return EXIT_FAILURE;;
    }

    return EXIT_SUCCESS;
}

/**
 *  \brief watcher waits for ingredient generated by agent
 *
 *  Watcher updates state and waits for ingredient from agent, then checks agent is closing.
 *  If agent is closing, watcher should update state again and inform the smoker that holds 
 *  the ingredient of the watcher so that it can terminate.
 *  The internal state should be saved.
 *
 *  \param id watcher id
 * 
 *  \return false if closing; true if not closing
 */
static bool waitForIngredient(int id)
{
    bool ret=true;
    
    if (semDown (semgid, sh->mutex) == -1)  {                                                     /* enter critical region */
        perror ("error on the up operation for semaphore access (WT)");
        exit (EXIT_FAILURE);
    }
    
    sh->fSt.st.watcherStat[id]=WAITING_ING;
    saveState(nFic,&sh->fSt);

    /* TODO: insert your code here */
 
    if (semUp (semgid, sh->mutex) == -1) {                                                         /* exit critical region */
        perror ("error on the down operation for semaphore access (WT)");
        exit (EXIT_FAILURE);
    }
    if  (semDown(semgid,sh->ingredient[id])==-1){
        perror("error on the down operation for semaphore (WT)");
        exit (EXIT_FAILURE);
    }

    /* TODO: insert your code here */
     if(sh->fSt.closing){
        sh->fSt.st.watcherStat[id]=CLOSING_W;
        saveState(nFic,&sh->fSt);
    }   
    /*
    if  (semDown(semgid,sh->ingredient[id])==-1){
        perror("error on the down operation for semaphore (WT)");
        exit (EXIT_FAILURE);
    }
*/
    if (semDown (semgid, sh->mutex) == -1 )  {                                                     /* enter critical region */
        perror ("error on the up operation for semaphore access (WT)");
        exit (EXIT_FAILURE);
    }
  
    /* TODO: insert your code here */

    if (semUp (semgid, sh->mutex) == -1) {                                                         /* exit critical region */
        perror ("error on the down operation for semaphore access (WT)");
        exit (EXIT_FAILURE);
    }
    if  (semUp(semgid,sh->wait2Ings[id])==-1){
            perror("error on the up operation for semaphore access (WT)");
        exit(EXIT_FAILURE);
     }
    return ret;
}

/**
 *  \brief watcher updates reservations in shared mem and checks if some smoker can complete a cigarette
 *
 *  Watcher updates state and reserves ingredient and then checks if some smoker may start rolling a cigarette.
 *  If a smoker may start rolling, then this smoker id is returned.
 *
 *  \param id watcher id
 * 
 *  \ret id of smoker that may start rolling cigarette; -1 if no smoker is ready
 *
 */
static int updateReservations (int id)
{
    int ret = -1; 

    if (semDown (semgid, sh->mutex) == -1)  {                                                     /* enter critical region */
        perror ("error on the up operation for semaphore access (WT)");
        exit (EXIT_FAILURE);
    }
    sh->fSt.st.watcherStat [id]=UPDATING;
    saveState(nFic,&sh->fSt);
    //reserve the quantity of the agent ingredient in cause
    sh->fSt.reserved[id]=sh->fSt.ingredients[id];

    //Returns the id
    if (sh->fSt.reserved[HAVETOBACCO]>0 && sh->fSt.reserved[HAVEPAPER]>0){
        sh->fSt.reserved[HAVETOBACCO]--;
        sh->fSt.reserved[HAVEPAPER]--;
        ret=MATCHES;
    } 
    if(sh->fSt.reserved[HAVETOBACCO]>0 && sh->fSt.reserved[HAVEMATCHES]>0) {
        sh->fSt.reserved[HAVETOBACCO]--;
        sh->fSt.reserved[HAVEMATCHES]--;
        ret=PAPER;
    }
    if(sh->fSt.reserved[HAVEMATCHES]>0 && sh->fSt.reserved[HAVEPAPER]>0){
        sh->fSt.reserved[HAVEMATCHES]--;
        sh->fSt.reserved[HAVEPAPER]--;
        ret =TOBACCO;
    }


    /* TODO: insert your code here */
    
    if (semUp (semgid, sh->mutex) == -1) {                                                         /* exit critical region */
        perror ("error on the down operation for semaphore access (WT)");
        exit (EXIT_FAILURE);
    }

    return ret;
}

/**
 *  \brief watcher informs smoker that he can use the available ingredients to roll cigarette
 *
 * The watcher updates its state and notifies smoker that he may start rolling cigarette.  
 *
 *  \param id watcher id
 *  \param smokerReady  id of smoker that may start rolling
 */

static void informSmoker (int id, int smokerReady)
{
    if (semDown (semgid, sh->mutex) == -1)  {                                                     /* enter critical region */
        perror ("error on the up operation for semaphore access (WT)");
        exit (EXIT_FAILURE);
    }
    sh->fSt.st.watcherStat [id]=INFORMING;
    saveState(nFic,&sh->fSt);
    /* TODO: insert your code here */

    if (semUp (semgid, sh->mutex) == -1) {                                                         /* exit critical region */
        perror ("error on the down operation for semaphore access (WT)");
        exit (EXIT_FAILURE);
    }
    if(semUp(semgid, sh->wait2Ings[smokerReady]) == -1){
        perror ("error on the up operation for semaphore access (WT)");
        exit (EXIT_FAILURE);
    }
    /* TODO: insert your code here */
}

