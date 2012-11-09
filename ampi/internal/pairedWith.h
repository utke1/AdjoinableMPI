#ifndef _AMPI_PAIREDWITH_H_
#define _AMPI_PAIREDWITH_H_

/**
 * \file enumeration to distinguish between active and passive variants of MPI_Datatype parameters passed to AMPI routines 
 */ 

enum AMPI_pairedWith { 
  AMPI_RECV,
  AMPI_SEND,
  AMPI_IRECV_WAIT,
  AMPI_IRECV_WAITALL,
  AMPI_ISEND_WAIT,
  AMPI_ISEND_WAITALL,
  AMPI_BSEND,
  AMPI_RSEND
};

#endif
