#ifndef _AMPI_ACTIVITY_H_
#define _AMPI_ACTIVITY_H_

/**
 * \file enumeration to distinguish between active and passive variants of MPI_Datatype parameters passed to AMPI routines 
 */ 

enum AMPI_Activity { 
  AMPI_PASSIVE=0,
  AMPI_ACTIVE=1
};

#endif
