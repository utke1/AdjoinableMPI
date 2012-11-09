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
		   char isActive,
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
	      char isActive,
	      int dest, 
	      int tag, 
	      MPI_Comm comm) { 
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
	      char isActive,
	      int src, 
	      int tag, 
	      MPI_Comm comm,
	      MPI_Status* status) { 
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
		char isActive,
		int dest, 
		int tag, 
		MPI_Comm comm, 
		MPI_Request* request) { 
  if (isActive!=AMPI_PASSIVE) MPI_Abort(comm, MPI_ERR_TYPE); 
  return MPI_Isend(buf,
		   count,
		   datatype,
		   dest,
		   tag,
		   comm,
		   request);
}

int AMPI_Irecv (void* buf, 
		int count, 
		MPI_Datatype datatype, 
		char isActive,
		int src, 
		int tag, 
		MPI_Comm comm, 
		MPI_Request* request) { 
  if (isActive!=AMPI_PASSIVE) MPI_Abort(comm, MPI_ERR_TYPE);
  return MPI_Irecv(buf,
		   count,
		   datatype,
		   src,
		   tag,
		   comm,
		   request);
}

int AMPI_Bsend(void *buf, 
	       int count, 
	       MPI_Datatype datatype, 
	       char isActive,
	       int dest, 
	       int tag, 
	       MPI_Comm comm) { 
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
	       char isActive,
	       int dest, 
	       int tag, 
	       MPI_Comm comm) { 
  if (isActive!=AMPI_PASSIVE) MPI_Abort(comm, MPI_ERR_TYPE); 
  return MPI_Rsend(buf,
		   count,
		   datatype,
		   dest,
		   tag,
		   comm);
}

int AMPI_Wait(MPI_Request *request, 
	      MPI_Status *status) { 
  return MPI_Wait(request,
		  status);
}

int AMPI_Waitall (int count, 
		  MPI_Request requests[], 
		  MPI_Status statuses[]) { 
  return MPI_Waitall(count,
		     requests,
		     statuses);
}

int AMPI_Awaitall (int count, 
		   MPI_Request requests[], 
		   MPI_Status statuses[]) { 
  return MPI_SUCCESS;
}

int AMPI_Reduce (void* sbuf, 
		 void* rbuf, 
		 int count, 
		 MPI_Datatype datatype, 
		 char isActive,
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
