/**
 *  \file sharedDataSync.h (interface file)
 *
 *  \brief Problem name: Smokers
 *
 *  Synchronization based on semaphores and shared memory.
 *  Implementation with SVIPC.
 *
 *  \brief Definition of the shared data and the synchronization devices.
 *
 *  Both the format of the shared data, which represents the full state of the problem, and the identification of
 *  the different semaphores, which carry out the synchronization among the intervening entities, are provided.
 *
 *  \author Nuno Lau - December 2019
 */

#ifndef SHAREDDATASYNC_H_
#define SHAREDDATASYNC_H_

#include "probConst.h"
#include "probDataStruct.h"

/**
 *  \brief Definition of <em>shared information</em> data type.
 */
typedef struct
        { /** \brief full state of the problem */
          FULL_STAT fSt;

          /* semaphores ids */
          /** \brief identification of critical region protection semaphore – val = 1 */
          unsigned int mutex;
          /** \brief identification of semaphore used by watchers to wait for agent - val = 0 */
          unsigned int ingredient[NUMINGREDIENTS];
          /** \brief identification of semaphore used by agent to wait for smoker to finish rolling - val = 0 */
          unsigned int waitCigarette;
          /** \brief identification of semaphore used by smoker to wait for watchers – val = 0  */
          unsigned int wait2Ings[NUMSMOKERS];

        } SHARED_DATA;

/** \brief number of semaphores in the set */
#define SEM_NU               ( 2 + NUMINGREDIENTS + NUMSMOKERS )

#define MUTEX                  1
#define WAITCIGARETTE          2
#define INGREDIENT             (WAITCIGARETTE + 1)
#define WAIT2INGS              (INGREDIENT + NUMINGREDIENTS)

#endif /* SHAREDDATASYNC_H_ */
