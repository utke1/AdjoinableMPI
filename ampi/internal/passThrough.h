#ifndef _AMPI_PASSTHOURGH_H_
#define _AMPI_PASSTHOURGH_H_

/**
 * \file prototypes for wrapper routines with identical signatures that pass the parameters through to the MPI routines; we do this to gave a consistent AMPI naming scheme and avoid having to mix  AMPI and MPI calls
 */ 

#include "mpi.h"

/**
 * simple wrapper; signature is identical to MPI original 
 */
int AMPI_Comm_size(MPI_Comm comm, 
		   int *size);

/**
 * simple wrapper; signature is identical to MPI original 
 */
int AMPI_Comm_rank(MPI_Comm comm, 
		   int *rank);

/** 
 * simple wrapper; signature is identical to MPI original 
 */
int AMPI_Get_processor_name(char *name, 
			    int *resultlen );


#endif
