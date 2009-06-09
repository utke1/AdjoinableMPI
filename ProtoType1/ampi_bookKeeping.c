#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include "ampi_bookKeeping.h"

/* receive buffer info (linked list element) */
struct rBufAssoc { 
  void * addr; 
  void * taddr;
  int length;
  MPI_Request req; /* MPI request id */
  struct rBufAssoc* next;
} rBufFirst = {0,0,0,0}; 

/* send buffer info (linked list element) */
struct sBufAssoc { 
  void * addr;
  int length;
  MPI_Request req; /* MPI request id */
  struct sBufAssoc* next;
} sBufFirst = {0,0,0}; 

static struct rBufAssoc* rBufAssoc_head=&rBufFirst; /* receive info list head */
static struct sBufAssoc* sBufAssoc_head=&sBufFirst; /* send info list head */

void complain(int rc) { 
  printf("adjoint run received MPI return code %d\n",rc);
  exit(-1);
}

/* associate a receive buffer 
   with a request id by making 
   a list element and linking it */
void associateR(void* buf, 
		void* tBuf,
		int count, 
		MPI_Request req) {
  struct rBufAssoc* thisAssoc=rBufAssoc_head;
  while (1) { 
    if (thisAssoc->req==0)
      break; 
    if (thisAssoc->next==0)
      break;
    thisAssoc=thisAssoc->next;
  }
  if (thisAssoc->req!=0) { 
    thisAssoc->next=(struct rBufAssoc*)malloc(sizeof(struct rBufAssoc));
    thisAssoc=thisAssoc->next;
    thisAssoc->next=0;
  }
  thisAssoc->addr  =buf;
  thisAssoc->taddr =tBuf;
  thisAssoc->length=count;
  thisAssoc->req   =req;
}

/* associate a send buffer 
   with a request id by making 
   a list element and linking it */
void associateS(void* buf, 
		int count, 
		MPI_Request req) {
  struct sBufAssoc* thisAssoc=sBufAssoc_head;
  while (1) { 
    if (thisAssoc->req==0)
      break; 
    if (thisAssoc->next==0)
      break;
    thisAssoc=thisAssoc->next;
  }
  if (thisAssoc->req!=0) { 
    thisAssoc->next=(struct sBufAssoc*)malloc(sizeof(struct sBufAssoc));
    thisAssoc=thisAssoc->next;
    thisAssoc->next=0;
  }
  thisAssoc->addr  =buf;
  thisAssoc->length=count;
  thisAssoc->req   =req;
}

/* 
 combine temporary buffer allocation 
 and bookkeeping
*/
void ampi_irecv_bk(void *buf,
		  int count, 
		  MPI_Datatype datatype, 
		  int src, 
		  int tag, 
		  MPI_Comm comm, 
		  MPI_Request *req) {
  double * tBuf;
  int myId, rc=0;
  rc = MPI_Comm_rank(MPI_COMM_WORLD, &myId);
  if (rc)
    complain(rc);
  tBuf=malloc(count*sizeof(double));
  rc = MPI_Irecv( tBuf, 
		  count, 
		  datatype, 
		  src, 
		  tag, 
		  comm, 
		  req);
  if (rc)
    complain(rc);
  associateR(buf, tBuf,count, *req);
} 

/* 
 combine temporary buffer allocation 
 and bookkeeping
*/
void ampi_isend_bk(void *buf,
		   int count, 
		   MPI_Datatype datatype, 
		   int dest, 
		   int tag, 
		   MPI_Comm comm, 
		   MPI_Request *req) { 
  int myId, rc=0;
  rc = MPI_Comm_rank(MPI_COMM_WORLD, &myId);
  if (rc)
    complain(rc);
  rc = MPI_Isend( buf, 
		  count, 
		  datatype, 
		  dest, 
		  tag, 
		  comm, 
		  req);
  if (rc)
    complain(rc);
  associateS(buf,count, *req);
} 

/* in the adjoint of the 
   awaitall we need to increment 
   or nullify the buffers associated 
   with each request we were waiting for */
void ampi_HandleRequest(int r) { 
  int done=0;
  double *tBuf;
  double *rBuf;
  int i;
  int myId,ierror;
  struct rBufAssoc* rAssoc=rBufAssoc_head;
  struct sBufAssoc* sAssoc=sBufAssoc_head;
  ierror = MPI_Comm_rank(MPI_COMM_WORLD, &myId);
  while(sAssoc) { 
    if (r==sAssoc->req) { 
	memset(sAssoc->addr,0,
	       sAssoc->length*sizeof(double));
	sAssoc->req=0; 
	done=1;
	printf("%i: handle S request r:%i\n",myId, r);
	break; 
    }
    sAssoc=sAssoc->next;
  }
  if (done)
    return;
  while(rAssoc) { 
    if (r==rAssoc->req) { 
      tBuf=(double *)rAssoc->taddr;
      rBuf=(double *)rAssoc->addr;
      for(i=0;i<rAssoc->length;i++) { 
	rBuf[i]+=tBuf[i];
      }
      free(tBuf);
      rAssoc->req=0;
      printf("%i: handle R request r:%i\n",myId, r);
      done=1;
      break; 
    }
    rAssoc=rAssoc->next;
  }
  if (!done)
    printf("%i: cannot handle request r:%i\n",myId, r);
}

void ampi_reduce_bk(void* sbuf, 
		    void* rbuf, 
		    int count, 
		    MPI_Datatype datatype, 
		    MPI_Op op, 
		    int root, 
		    MPI_Comm comm) {
  int i;
  if (op==MPI_SUM && datatype==MPI_DOUBLE_PRECISION) {
    for (i=0;i<count;++i) { 
      ((double*)sbuf)[i]+=*((double*)rbuf);
    }
  } 
  else 
    complain(-1);
}
