#ifndef _AMPI_TAPE_SUPPORT_H_
#define _AMPI_TAPE_SUPPORT_H_

#include <mpi.h>
#if defined(__cplusplus)
extern "C" {
#endif

#include "ampi/userIF/request.h"

/**
 * \file 
 * \brief interface to a reusable tape to read and write in particular the opaque MPI types
 */ 

void TAPE_AMPI_init();
void TAPE_AMPI_resetRead();

void TAPE_AMPI_push_int(int an_int);
void TAPE_AMPI_pop_int(int *an_int);
void TAPE_AMPI_read_int(int* an_int);

void TAPE_AMPI_push_MPI_Datatype(MPI_Datatype an_MPI_Datatype);
void TAPE_AMPI_pop_MPI_Datatype(MPI_Datatype *an_MPI_Datatype);
void TAPE_AMPI_read_MPI_Datatype(MPI_Datatype* an_MPI_Datatype);

void TAPE_AMPI_push_MPI_Comm(MPI_Comm an_MPI_Comm);
void TAPE_AMPI_pop_MPI_Comm(MPI_Comm *an_MPI_Comm);
void TAPE_AMPI_read_MPI_Comm(MPI_Comm* an_MPI_Comm);

void TAPE_AMPI_push_MPI_Request(MPI_Request an_MPI_Request);
void TAPE_AMPI_pop_MPI_Request(MPI_Request *an_MPI_Request);
void TAPE_AMPI_read_MPI_Request(MPI_Request* an_MPI_Request);

#if defined(__cplusplus)
}
#endif

#endif

