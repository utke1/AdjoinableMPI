#ifndef _AMPI_PAIREDWITH_H_
#define _AMPI_PAIREDWITH_H_

/**
 * \file 
 * enumeration to distinguish between active and passive variants of MPI_Datatype parameters passed to AMPI routines 
 */ 

#include "ampi/userIF/libConfig.h"

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
  AMPI_IRECV
};

#ifdef AMPI_FORTRANCOMPATIBLE
typedef int AMPI_PairedWith;
#else 
typedef enum AMPI_PairedWith_E AMPI_PairedWith;
#endif 

#endif
