#ifndef _AMPI_MODIFIED_H_
#define _AMPI_MODIFIED_H_

/**
 * \file 
 * AMPI routines that have adjoint functionality and do no merely pass through to the MPI originals; the routines may signatures with additional parameters compared to their original MPI counterparts
 */ 


#include <mpi.h>
#include "ampi/userIF/activity.h"
#include "ampi/userIF/pairedWith.h"
#include "ampi/userIF/request.h"

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

/**
 * pass through for isActive==AMPI_PASSIVE;  
 * for isActive==AMPI_ACTIVE this consider the size of the active type; 
 * this is AD tool dependent and part of the AD tool interface
 */
int AMPI_Pack_size(int incount,
		   MPI_Datatype datatype,
		   AMPI_Activity isActive,
		   MPI_Comm comm,
		   int *size);

/**
 * adjoint needs to detach; signature identical to original MPI call
 */ 
int AMPI_Buffer_attach(void *buffer, 
		       int size); 

/**
 * adjoint needs to attach; signature identical to original MPI call
 */ 
int AMPI_Buffer_detach(void *buffer, 
		       int *size);

int AMPI_Send(void* buf, 
	      int count, 
	      MPI_Datatype datatype, 
	      AMPI_Activity isActive,
	      int dest, 
	      int tag, 
	      AMPI_PairedWith pairedWith,
	      MPI_Comm comm);

int AMPI_Recv(void* buf, 
	      int count,
	      MPI_Datatype datatype, 
	      AMPI_Activity isActive,
	      int src, 
	      int tag, 
	      AMPI_PairedWith pairedWith,
	      MPI_Comm comm,
	      MPI_Status* status);

int AMPI_Isend (void* buf, 
		int count, 
		MPI_Datatype datatype, 
		AMPI_Activity isActive,
		int dest, 
		int tag, 
		AMPI_PairedWith pairedWith,
		MPI_Comm comm, 
		AMPI_Request* request);

int AMPI_Irecv (void* buf, 
		int count, 
		MPI_Datatype datatype, 
		AMPI_Activity isActive,
		int src, 
		int tag,
		AMPI_PairedWith pairedWith, 
		MPI_Comm comm, 
		AMPI_Request* request);

int AMPI_Bsend(void *buf, 
	       int count, 
	       MPI_Datatype datatype, 
	       AMPI_Activity isActive,
	       int dest, 
	       int tag,
	       AMPI_PairedWith pairedWith, 
	       MPI_Comm comm);

int AMPI_Rsend(void *buf, 
	       int count, 
	       MPI_Datatype datatype, 
	       AMPI_Activity isActive,
	       int dest, 
	       int tag,
	       AMPI_PairedWith pairedWith,
	       MPI_Comm comm);

/**
 * before we start reverse we need to make sure there are no pending requests in our userIF bookkeeping 
 */
int AMPI_Wait(AMPI_Request *request, 
	      MPI_Status *status);

int AMPI_Waitall (int count, 
		  AMPI_Request requests[], 
		  MPI_Status statuses[]);

int AMPI_Awaitall (int count, 
		   AMPI_Request requests[], 
		   MPI_Status statuses[]);

int AMPI_Reduce (void* sbuf, 
		 void* rbuf, 
		 int count, 
		 MPI_Datatype datatype, 
		 AMPI_Activity isActive,
		 MPI_Op op, 
		 int root, 
		 MPI_Comm comm); 

#endif
