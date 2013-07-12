#ifndef _AMPI_MODIFIED_H_
#define _AMPI_MODIFIED_H_

/**
 * \file 
 * AMPI routines that have adjoint functionality and do no merely pass through to the MPI originals; the routines may signatures with additional parameters compared to their original MPI counterparts
 */ 

#include "ampi/userIF/pairedWith.h"
#include "ampi/userIF/request.h"

/**
 * active variant of the predefined  MPI_DOUBLE
 */
extern MPI_Datatype AMPI_ADOUBLE;

/**
 * active variant of the predefined  MPI_FLOAT
 */
extern MPI_Datatype AMPI_AFLOAT;

#ifdef AMPI_FORTRANCOMPATIBLE

/**
 * active variant of the predefined  MPI_DOUBLE_PRECISION
 */
extern MPI_Datatype AMPI_ADOUBLE_PRECISION;

/**
 * active variant of the predefined  MPI_REAL
 */
extern MPI_Datatype AMPI_AREAL;

#endif

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
	      int dest, 
	      int tag, 
	      AMPI_PairedWith pairedWith,
	      MPI_Comm comm);

int AMPI_Recv(void* buf, 
	      int count,
	      MPI_Datatype datatype, 
	      int src, 
	      int tag, 
	      AMPI_PairedWith pairedWith,
	      MPI_Comm comm,
	      MPI_Status* status);

int AMPI_Isend (void* buf, 
		int count, 
		MPI_Datatype datatype, 
		int dest, 
		int tag, 
		AMPI_PairedWith pairedWith,
		MPI_Comm comm, 
		AMPI_Request* request);

int AMPI_Irecv (void* buf, 
		int count, 
		MPI_Datatype datatype, 
		int src, 
		int tag,
		AMPI_PairedWith pairedWith, 
		MPI_Comm comm, 
		AMPI_Request* request);

int AMPI_Bsend(void *buf, 
	       int count, 
	       MPI_Datatype datatype, 
	       int dest, 
	       int tag,
	       AMPI_PairedWith pairedWith, 
	       MPI_Comm comm);

int AMPI_Rsend(void *buf, 
	       int count, 
	       MPI_Datatype datatype, 
	       int dest, 
	       int tag,
	       AMPI_PairedWith pairedWith,
	       MPI_Comm comm);

int AMPI_Bcast (void* buf,
		int count,
		MPI_Datatype datatype,
		int root,
		MPI_Comm comm);

int AMPI_Reduce (void* sbuf, 
		 void* rbuf, 
		 int count, 
		 MPI_Datatype datatype, 
		 MPI_Op op, 
		 int root, 
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



int AMPI_Gatherv(void *sendbuf,
                 int sendcnt,
                 MPI_Datatype sendtype,
                 void *recvbuf,
                 int *recvcnts,
                 int *displs,
                 MPI_Datatype recvtype,
                 int root,
                 MPI_Comm comm);

int AMPI_Scatterv(void *sendbuf,
                  int *sendcnts,
                  int *displs,
                  MPI_Datatype sendtype,
                  void *recvbuf,
                  int recvcnt,
                  MPI_Datatype recvtype,
                  int root, MPI_Comm comm);

#endif
