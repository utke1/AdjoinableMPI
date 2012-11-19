#include <stdlib.h>
#include <assert.h>
#include "ampi/internal/modified.h"

int AMPI_Init(int* argc, 
	      char*** argv) { 
  return MPI_Init(argc,argv);
}

int AMPI_Finalize(int* argc, 
		  char*** argv) { 
  return MPI_Finalize();
}

int AMPI_Pack_size(int incount,
		   MPI_Datatype datatype,
		   AMPI_Activity isActive,
		   MPI_Comm comm,
		   int *size) { 
  if (isActive!=AMPI_PASSIVE) MPI_Abort(comm, MPI_ERR_TYPE); 
  return  MPI_Pack_size(incount,
			datatype,
			comm,
			size);
}

int AMPI_Buffer_attach(void *buffer, 
		       int size) { 
  return MPI_Buffer_attach(buffer,
			   size);

}

int AMPI_Buffer_detach(void *buffer, 
		       int *size){ 
  return MPI_Buffer_detach(buffer,
			   size);
}


int AMPI_Send(void* buf, 
	      int count, 
	      MPI_Datatype datatype, 
	      AMPI_Activity isActive,
	      int dest, 
	      int tag,
	      AMPI_PairedWith pairedWith,
	      MPI_Comm comm) {
  if (!(
	pairedWith==AMPI_RECV 
	|| 
	pairedWith==AMPI_IRECV_WAIT
	||
	pairedWith==AMPI_IRECV_WAITALL
	)) MPI_Abort(comm, MPI_ERR_ARG);
  if (isActive!=AMPI_PASSIVE) MPI_Abort(comm, MPI_ERR_TYPE); 
  return MPI_Send(buf,
		  count,
		  datatype,
		  dest,
		  tag,
		  comm);
}

int AMPI_Recv(void* buf, 
	      int count,
	      MPI_Datatype datatype, 
	      AMPI_Activity isActive,
	      int src, 
	      int tag,
	      AMPI_PairedWith pairedWith,
	      MPI_Comm comm,
	      MPI_Status* status) { 
  if (!(
	pairedWith==AMPI_SEND 
	|| 
	pairedWith==AMPI_BSEND 
	||
	pairedWith==AMPI_RSEND 
	||
	pairedWith==AMPI_ISEND_WAIT
	||
	pairedWith==AMPI_ISEND_WAITALL
	)) MPI_Abort(comm, MPI_ERR_ARG);
  if (isActive!=AMPI_PASSIVE) MPI_Abort(comm, MPI_ERR_TYPE); 
  return MPI_Recv(buf,
		  count,
		  datatype,
		  src,
		  tag,
		  comm,
		  status);
}  

int AMPI_Isend (void* buf, 
		int count, 
		MPI_Datatype datatype, 
		AMPI_Activity isActive,
		int dest, 
		int tag,
		AMPI_PairedWith pairedWith,
		MPI_Comm comm, 
		AMPI_Request* request) { 
  if (!(
	pairedWith==AMPI_RECV 
	|| 
	pairedWith==AMPI_IRECV_WAIT
	||
	pairedWith==AMPI_IRECV_WAITALL
	)) MPI_Abort(comm, MPI_ERR_ARG);
  if (isActive!=AMPI_PASSIVE) MPI_Abort(comm, MPI_ERR_TYPE); 
  return MPI_Isend(buf,
		   count,
		   datatype,
		   dest,
		   tag,
		   comm,
#ifdef AMPI_FORTRANCOMPATIBLE
		   request
#else 
		   &(request->plainRequest)
#endif 
		   );
}

int AMPI_Irecv (void* buf, 
		int count, 
		MPI_Datatype datatype, 
		AMPI_Activity isActive,
		int src, 
		int tag,
		AMPI_PairedWith pairedWith,
		MPI_Comm comm, 
		AMPI_Request* request) { 
  if (!(
	pairedWith==AMPI_SEND 
	|| 
	pairedWith==AMPI_BSEND 
	||
	pairedWith==AMPI_RSEND 
	||
	pairedWith==AMPI_ISEND_WAIT
	||
	pairedWith==AMPI_ISEND_WAITALL
	)) MPI_Abort(comm, MPI_ERR_ARG);
  if (isActive!=AMPI_PASSIVE) MPI_Abort(comm, MPI_ERR_TYPE);
  return MPI_Irecv(buf,
		   count,
		   datatype,
		   src,
		   tag,
		   comm,
#ifdef AMPI_FORTRANCOMPATIBLE
		   request
#else 
		   &(request->plainRequest)
#endif 
		   );
}

int AMPI_Bsend(void *buf, 
	       int count, 
	       MPI_Datatype datatype, 
	       AMPI_Activity isActive,
	       int dest, 
	       int tag,
	       AMPI_PairedWith pairedWith,
	       MPI_Comm comm) { 
  if (!(
	pairedWith==AMPI_RECV 
	|| 
	pairedWith==AMPI_IRECV_WAIT
	)) MPI_Abort(comm, MPI_ERR_ARG);
  if (isActive!=AMPI_PASSIVE) MPI_Abort(comm, MPI_ERR_TYPE); 
  return MPI_Bsend(buf,
		   count,
		   datatype,
		   dest,
		   tag,
		   comm);
}

int AMPI_Rsend(void *buf, 
	       int count, 
	       MPI_Datatype datatype, 
	       AMPI_Activity isActive,
	       int dest, 
	       int tag,
	       AMPI_PairedWith pairedWith,
	       MPI_Comm comm) { 
  if (!(
	pairedWith==AMPI_RECV 
	|| 
	pairedWith==AMPI_IRECV_WAIT
	)) MPI_Abort(comm, MPI_ERR_ARG);
  if (isActive!=AMPI_PASSIVE) MPI_Abort(comm, MPI_ERR_TYPE); 
  return MPI_Rsend(buf,
		   count,
		   datatype,
		   dest,
		   tag,
		   comm);
}

int AMPI_Wait(AMPI_Request *request,
	      MPI_Status *status) { 
  return MPI_Wait(
#ifdef AMPI_FORTRANCOMPATIBLE
		   request
#else 
		   &(request->plainRequest)
#endif 
		   ,status);
}

int AMPI_Waitall (int count, 
		  AMPI_Request requests[], 
		  MPI_Status statuses[]) { 
  int i; 
#ifndef AMPI_FORTRANCOMPATIBLE
  /* extract original requests */
  MPI_Request * origRequests=(MPI_Request*)malloc(count*sizeof(MPI_Request));
  assert(origRequests);
  for (i=0;i<count;++i) origRequests[i]=requests[i].plainRequest; 
#endif 
  return MPI_Waitall(count,
#ifdef AMPI_FORTRANCOMPATIBLE
		     requests,
#else
		     origRequests,
#endif
		     statuses);
}

int AMPI_Awaitall (int count, 
		   AMPI_Request requests[], 
		   MPI_Status statuses[]) { 
  return MPI_SUCCESS;
}

int AMPI_Reduce (void* sbuf, 
		 void* rbuf, 
		 int count, 
		 MPI_Datatype datatype, 
		 AMPI_Activity isActive,
		 MPI_Op op, 
		 int root, 
		 MPI_Comm comm) { 
  if (isActive!=AMPI_PASSIVE) MPI_Abort(comm, MPI_ERR_TYPE);
  return MPI_Reduce(sbuf,
		    rbuf,
		    count,
		    datatype,
		    op,
		    root,
		    comm);
}
