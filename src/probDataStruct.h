/**
 *  \file probDataStruct.h (interface file)
 *
 *  \brief Problem name: Smokers
 *
 *  Definition of internal data structures.
 *
 *  They specify internal metadata about the status of the intervening entities.
 *
 *  \author Nuno Lau - December 2019
 */

#ifndef PROBDATASTRUCT_H_
#define PROBDATASTRUCT_H_

#include <stdbool.h>

#include "probConst.h"

/**
 *  \brief Definition of <em>state of the intervening entities</em> data type.
 */
typedef struct {
    /** \brief agent state */
    unsigned int agentStat;
    /** \brief watchers state */
    unsigned int watcherStat[NUMINGREDIENTS];
    /** \brief smokers state */
    unsigned int smokerStat[NUMSMOKERS];

} STAT;


/**
 *  \brief Definition of <em>full state of the problem</em> data type.
 */
typedef struct
{   /** \brief state of all intervening entities */
    STAT st;

    /** \brief number of ingredients */
    int nIngredients;

    /** \brief number of orders to be performed by agent (each order includes a pack of 2 ingredients) */
    int nOrders;

    /** \brief number of smokers */
    int nSmokers;

    /** \brief flag used by agent to close factory */
    bool closing;

    /** \brief inventory of ingredients */
    int ingredients[NUMINGREDIENTS];

    /** \brief number of ingredients already reserved by watcher */
    int reserved[NUMINGREDIENTS];

    /** \brief number of cigarettes each smoker smoked */
    int nCigarettes[NUMSMOKERS];

} FULL_STAT;


#endif /* PROBDATASTRUCT_H_ */
