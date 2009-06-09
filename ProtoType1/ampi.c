#include "ampi.h"
#include "ampi_bookKeeping.h"

int ampi_reverse=0;

int  AMPI_awaitall (int count, 
		    MPI_Request * requests, 
		    MPI_Status * statuses) { 
  int i,rc=0;
  if (ampi_reverse) { 
    rc=MPI_Waitall(count,requests,statuses);
    if (!rc) { 
      for (i=0;
	   i<count;
	   ++i)
	ampi_HandleRequest(requests[i]);
    }
  }
  return rc;
}

int  AMPI_isend (void* buf, 
		 int count, 
		 MPI_Datatype datatype, 
		 int dest, 
		 int tag, 
		 MPI_Comm comm, 
		 MPI_Request* request) {   
  int rc=0;
  if (!ampi_reverse) { 
    rc= MPI_Isend(buf, count, datatype, dest, tag, comm, request);
  }
  else { 
    ampi_irecv_bk(buf, count, datatype, dest, tag, comm, request);
  }
  return rc;
}

int  AMPI_irecv (void* buf, 
		 int count, 
		 MPI_Datatype datatype, 
		 int src, 
		 int tag, 
		 MPI_Comm comm, 
		 MPI_Request* request) {   
  int rc=0;
  if (!ampi_reverse) { 
    rc= MPI_Irecv(buf, count, datatype, src, tag, comm, request);
  }
  else { 
    ampi_isend_bk(buf, count, datatype, src, tag, comm, request);
  }
  return rc;
}

int  AMPI_waitall (int count, 
		   MPI_Request * requests, 
		   MPI_Status * statuses) { 
  int rc=0;
  if (!ampi_reverse) { 
    rc=MPI_Waitall(count, requests, statuses);
  }
  return rc;
}

int AMPI_reduce(void* sbuf, 
		void* rbuf, 
		int count, 
		MPI_Datatype datatype, 
		MPI_Op op, 
		int root, 
		MPI_Comm comm) { 
  int rc=0;
  if (!ampi_reverse) { 
    rc=MPI_Reduce(sbuf, rbuf, count, datatype, op, root, comm);
  }
  else { 
    ampi_reduce_bk(sbuf, rbuf, count, datatype, op, root, comm);
  } 
  return rc;
}
