#ifndef _AMPI_LIBCOMMON_MODIFIED_H_
#define _AMPI_LIBCOMMON_MODIFIED_H_

/**
 * \file 
 * common AD implementation portion of AMPI routines from ampi/userIF/modifief.h
 */ 

#include <mpi.h>

#if defined(__cplusplus)
extern "C" {
#endif

#include "ampi/userIF/pairedWith.h"
#include "ampi/userIF/request.h"

/** 
 * forward sweep variant of \ref AMPI_Recv 
 */
int FW_AMPI_Recv(void* buf, 
		 int count,
		 MPI_Datatype datatype, 
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
		 int src, 
		 int tag, 
		 AMPI_PairedWith pairedWith,
		 MPI_Comm comm,
		 MPI_Status* status);

/** 
 * forward sweep variant of \ref AMPI_Irecv 
 */
int FW_AMPI_Irecv (void* buf, 
		   int count, 
		   MPI_Datatype datatype, 
		   int source, 
		   int tag,
		   AMPI_PairedWith pairedWith,
		   MPI_Comm comm, 
		   AMPI_Request* request);

/** 
 * backward sweep variant of \ref AMPI_Irecv 
 */
int BW_AMPI_Irecv (void* buf, 
		   int count, 
		   MPI_Datatype datatype, 
		   int source, 
		   int tag,
		   AMPI_PairedWith pairedWith,
		   MPI_Comm comm, 
		   AMPI_Request* request);

/** 
 * forward sweep variant of \ref AMPI_Send
 */
int FW_AMPI_Send (void* buf, 
                  int count, 
                  MPI_Datatype datatype, 
                  int dest, 
                  int tag,
                  AMPI_PairedWith pairedWith,
                  MPI_Comm comm);

/** 
 * backward sweep variant of \ref AMPI_Send
 */
int BW_AMPI_Send (void* buf,
                  int count, 
                  MPI_Datatype datatype, 
                  int dest, 
                  int tag,
                  AMPI_PairedWith pairedWith,
                  MPI_Comm comm);

/** 
 * forward sweep variant of \ref AMPI_Isend 
 */
int FW_AMPI_Isend (void* buf, 
		   int count, 
		   MPI_Datatype datatype, 
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
		   int dest, 
		   int tag, 
		   AMPI_PairedWith pairedWith,
		   MPI_Comm comm, 
		   AMPI_Request* request);

/** 
 * forward sweep variant of \ref AMPI_Wait 
 */
int FW_AMPI_Wait(AMPI_Request *request, 
		 MPI_Status *status);

/** 
 * backward sweep variant of \ref AMPI_Wait 
 */
int BW_AMPI_Wait(AMPI_Request *request, 
		 MPI_Status *status);

/**
 * forward sweep variant of \ref AMPI_Gatherv
 */
int FW_AMPI_Gatherv(void *sendbuf,
                    int sendcnt,
                    MPI_Datatype sendtype,
                    void *recvbuf,
                    int *recvcnts,
                    int *displs,
                    MPI_Datatype recvtype,
                    int root,
                    MPI_Comm comm);

/**
 * backward sweep variant of \ref AMPI_Gatherv
 * NOTE: recvcnts and displs are passed with a non-NULL pointer then they must be allocated to the correct size
 */
int BW_AMPI_Gatherv(void *sendbuf,
                    int sendcnt,
                    MPI_Datatype sendtype,
                    void *recvbuf,
                    int *recvcnts,
                    int *displs,
                    MPI_Datatype recvtype,
                    int root,
                    MPI_Comm comm);

/**
 * forward sweep variant of \ref AMPI_Scatterv
 */
int FW_AMPI_Scatterv(void *sendbuf,
                     int *sendcnts,
                     int *displs,
                     MPI_Datatype sendtype,
                     void *recvbuf,
                     int recvcnt,
                     MPI_Datatype recvtype,
                     int root, MPI_Comm comm);

/**
 * backward sweep variant of \ref AMPI_Scatterv
 * NOTE: sendcnts and displs are passed with a non-NULL pointer then they must be allocated to the correct size
 */
int BW_AMPI_Scatterv(void *sendbuf,
                     int *sendcnts,
                     int *displs,
                     MPI_Datatype sendtype,
                     void *recvbuf,
                     int recvcnt,
                     MPI_Datatype recvtype,
                     int root, MPI_Comm comm);

#if defined(__cplusplus)
}
#endif

#endif
