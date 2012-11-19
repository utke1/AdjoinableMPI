#ifndef _AMPI_LIBCOMMON_MODIFIED_H_
#define _AMPI_LIBCOMMON_MODIFIED_H_

/**
 * \file 
 * common AD implementation portion of AMPI routines from ampi/internal/modifief.h
 */ 

#include <mpi.h>
#include "ampi/internal/activity.h"
#include "ampi/internal/pairedWith.h"
#include "ampi/internal/request.h"

/** 
 * forward sweep variant of \ref AMPI_Recv 
 */
int FW_AMPI_Recv(void* buf, 
		 int count,
		 MPI_Datatype datatype, 
		 AMPI_Activity isActive,
		 int src, 
		 int tag, 
		 AMPI_PairedWith pairedWith,
		 MPI_Comm comm,
		 MPI_Status* status);

/** 
 * backward sweep variant of \ref AMPI_Recv 
 */
int BW_AMPI_Recv(void* buf, 
		 int count,
		 MPI_Datatype datatype, 
		 AMPI_Activity isActive,
		 int src, 
		 int tag, 
		 AMPI_PairedWith pairedWith,
		 MPI_Comm comm,
		 MPI_Status* status);

/** 
 * forward sweep variant of \ref AMPI_Isend 
 */
int FW_AMPI_Isend (void* buf, 
		   int count, 
		   MPI_Datatype datatype, 
		   AMPI_Activity isActive,
		   int dest, 
		   int tag, 
		   AMPI_PairedWith pairedWith,
		   MPI_Comm comm, 
		   AMPI_Request* request);

/** 
 * backward sweep variant of \ref AMPI_Isend 
 */
int BW_AMPI_Isend (void* buf, 
		   int count, 
		   MPI_Datatype datatype, 
		   AMPI_Activity isActive,
		   int dest, 
		   int tag, 
		   AMPI_PairedWith pairedWith,
		   MPI_Comm comm, 
		   AMPI_Request* request);


#endif
