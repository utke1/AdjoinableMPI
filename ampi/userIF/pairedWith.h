#ifndef _AMPI_PAIREDWITH_H_
#define _AMPI_PAIREDWITH_H_

/**
 * \file 
 * \ingroup UserInterfaceHeaders
 * enumeration to distinguish between active and passive variants of MPI_Datatype parameters passed to AMPI routines 
 */ 

#include "ampi/userIF/libConfig.h"

/** \ingroup UserInterfaceDeclarations
 * @{
 */
/**
 * the enumeration for communication pairings as explained in \ref pairings
 */
enum AMPI_PairedWith_E { 
  AMPI_RECV,
  AMPI_SEND,
  AMPI_IRECV_WAIT,
  AMPI_IRECV_WAITALL,
  AMPI_ISEND_WAIT,
  AMPI_ISEND_WAITALL,
  AMPI_BSEND,
  AMPI_RSEND,
  AMPI_ISEND,
  AMPI_IRECV,
  AMPI_WAIT,
  AMPI_GATHER,
  AMPI_SCATTER,
  AMPI_ALLGATHER,
  AMPI_GATHERV,
  AMPI_SCATTERV, 
  AMPI_ALLGATHERV,
  AMPI_BCAST,
  AMPI_REDUCE,
  AMPI_ALLREDUCE,
  AMPI_BARRIER,
  AMPI_GET,
  AMPI_PUT,
  AMPI_WIN_CREATE,
  AMPI_WIN_FENCE,
  AMPI_WIN_FREE
};

#ifdef AMPI_FORTRANCOMPATIBLE
typedef int AMPI_PairedWith;
#else 
typedef enum AMPI_PairedWith_E AMPI_PairedWith;
#endif 
/** @} */

#endif
