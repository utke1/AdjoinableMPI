#include "ampi.h"
#include "ampi_bookKeeping.h"

int  AMPI_awaitall (int count, 
		    MPI_Request * requests, 
		    MPI_Status * statuses) { 
  int i,rc=0;
  if (ampi_reverse) { 
    rc=MPI_waitall(count,requests,statuses);
    if (!rc) { 
      for (i=0;
	   i<count;
	   ++i)
	ampi_HandleRequest(requests[i]);
    }
  }
  return rc;
}

int  AMPI_isend_from_i (void* buf, 
			int count, 
			MPI_Datatype datatype, 
			int dest, 
			int tag, 
			MPI_Comm comm, 
			MPI_Request* request) {   
  int rc=0;
  if (!ampi_reverse) { 
    rc= mpi_isend(buf, count, datatype, dest, tag, comm, request);
  }
  else { 
    rc= ampi_irecv_bk(buf, count, datatype, dest, tag, comm, request);
  }
  return rc;
}

int  AMPI_irecv_from_i (void* buf, 
			int count, 
			MPI_Datatype datatype, 
			int src, 
			int tag, 
			MPI_Comm comm, 
			MPI_Request* request) {   
  int rc=0;
  if (!ampi_reverse) { 
    rc= mpi_irecv(buf, count, datatype, src, tag, comm, request);
  }
  else { 
    rc= ampi_isend_bk(buf, count, datatype, src, tag, comm, request);
  }
  return rc;
}

int  AMPI_awaitall (int count, 
		    MPI_Request * requests, 
		    MPI_Status * statuses) { 
  int rc=0;
  if (!ampi_reverse) { 
    rc=mpi_waitall(count, requests, statuses);
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
    rc=mpi_reduce(sbuf, rbuf, count, datatype, op, root, comm);
  }
  else { 
    if (op==MPI_SUM) { 
      ampi_reduce_bk(sbuf, rbuf, count, datatype, op, root, comm);
    } 
  } 
  return rc;
}
