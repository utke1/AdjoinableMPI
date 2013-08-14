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

/**
 * user-defined type data
 * only one instance of derivedTypeData exists at once
 * get pointer from getDTypeData, add new stuff with addDTypeData
 */
typedef struct {
  int size;
  int pos;
  int* num_actives;
  int* first_active_indices;
  int* last_active_indices;
  MPI_Datatype* derived_types;
  int* counts;
  int** arrays_of_blocklengths;
  MPI_Aint** arrays_of_displacements;
  MPI_Datatype** arrays_of_types;
  int* mapsizes;
  /* corresponding typemaps packed for sending */
  MPI_Datatype* packed_types;
  int** arrays_of_p_blocklengths;
  MPI_Aint** arrays_of_p_displacements;
  MPI_Datatype** arrays_of_p_types;
  int* p_mapsizes;
} derivedTypeData;

derivedTypeData* getDTypeData();
  /* addDTypeData takes derived type data and adds a new entry; returns
     position of new type in data struct; returns -1 if struct contains
     no active types; doubles data struct size every time there's overflow */
int addDTypeData(derivedTypeData* dat,
		 int count,
		 int array_of_blocklengths[],
		 MPI_Aint array_of_displacements[],
		 MPI_Datatype array_of_types[],
		 int mapsize,
		 int array_of_p_blocklengths[],
		 MPI_Aint array_of_p_displacements[],
		 MPI_Datatype array_of_p_types[],
		 int p_mapsize,
		 MPI_Datatype* newtype,
		 MPI_Datatype* packed_type);
int derivedTypeIdx(MPI_Datatype datatype);
int isDerivedType(int dt_idx);

/**
 * user-defined reduction op data
 * only one instance of userDefinedOpData exists at once
 * get pointer from getUOpData, add new stuff with addUOpData
 */
typedef struct {
  int size;
  int pos;
  MPI_Op* ops;
  MPI_User_function** functions;
  int* commutes;
} userDefinedOpData;

userDefinedOpData* getUOpData();
/* addUOpData takes user-defined op data and adds a new entry; returns
   position of new type in data struct; doubles data struct size every
   time there's overflow */
int addUOpData(userDefinedOpData* dat,
		MPI_Op* op,
		MPI_User_function* function,
		int commute);
int userDefinedOpIdx(MPI_Op op);
int isUserDefinedOp(int uop_idx);

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

int AMPI_Allreduce (void* sbuf,
                    void* rbuf,
                    int count,
                    MPI_Datatype datatype,
                    MPI_Op op,
                    MPI_Comm comm);

int AMPI_Type_create_struct (int count,
			     int array_of_blocklengths[],
			     MPI_Aint array_of_displacements[],
			     MPI_Datatype array_of_types[],
			     MPI_Datatype *newtype);

int AMPI_Type_commit (MPI_Datatype *datatype);

int AMPI_Op_create(MPI_User_function *function,
		   int commute,
		   MPI_Op *op);

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

int AMPI_Barrier(MPI_Comm comm);

int AMPI_Gather(void *sendbuf,
		int sendcnt,
		MPI_Datatype sendtype,
		void *recvbuf,
		int recvcnt,
		MPI_Datatype recvtype,
		int root,
		MPI_Comm comm);

int AMPI_Scatter(void *sendbuf,
		 int sendcnt,
		 MPI_Datatype sendtype,
		 void *recvbuf,
		 int recvcnt,
		 MPI_Datatype recvtype,
		 int root, 
		 MPI_Comm comm);

int AMPI_Allgather(void *sendbuf,
                   int sendcount,
                   MPI_Datatype sendtype,
                   void *recvbuf,
                   int recvcount,
                   MPI_Datatype recvtype,
                   MPI_Comm comm);

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

int AMPI_Allgatherv(void *sendbuf,
                    int sendcnt,
                    MPI_Datatype sendtype,
                    void *recvbuf,
                    int *recvcnts,
                    int *displs,
                    MPI_Datatype recvtype,
                    MPI_Comm comm);

#endif
