#ifndef _AMPI_MODIFIED_H_
#define _AMPI_MODIFIED_H_

/**
 * \file AMPI routines that have adjoint functionality and do no merely pass through to the MPI originals; the routines may signatures with additional parameters compared to their original MPI counterparts
 */ 


#include <mpi.h>
#include "ampi/internal/activity.h"

/**
 * adjoint needs to MPI_Finalize; signature identical to original MPI call
 */
int AMPI_Init(int* argc, 
	      char*** argv);

/**
 * adjoint needs to MPI_Init; signature identical to AMPI_Init (adjoint symmetry)
 */
int AMPI_Finalize(int* argc, 
		  char*** argv);

int AMPI_Send(void* buf, 
	      int count, 
	      MPI_Datatype datatype, 
	      char isActive,
	      int dest, 
	      int tag, 
	      MPI_Comm comm);

int AMPI_Recv(void* buf, 
	      int count,
	      MPI_Datatype datatype, 
	      char isActive,
	      int src, 
	      int tag, 
	      MPI_Comm comm,
	      MPI_Status* status);

int AMPI_Isend (void* buf, 
		int count, 
		MPI_Datatype datatype, 
		char isActive,
		int dest, 
		int tag, 
		MPI_Comm comm, 
		MPI_Request* request);

int AMPI_Irecv (void* buf, 
		int count, 
		MPI_Datatype datatype, 
		char isActive,
		int src, 
		int tag, 
		MPI_Comm comm, 
		MPI_Request* request);

int AMPI_Wait(MPI_Request *request, 
	      MPI_Status *status);

int AMPI_Waitall (int count, 
		  MPI_Request requests[], 
		  MPI_Status statuses[]);

int AMPI_Awaitall (int count, 
		   MPI_Request requests[], 
		   MPI_Status statuses[]);

int AMPI_Reduce (void* sbuf, 
		 void* rbuf, 
		 int count, 
		 MPI_Datatype datatype, 
		char isActive,
		 MPI_Op op, 
		 int root, 
		 MPI_Comm comm); 

#endif
