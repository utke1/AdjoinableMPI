#include <stdlib.h>
#include <assert.h>
#include <mpi.h>
#include "ampi/libCommon/modified.h"
#include "ampi/bookkeeping/support.h"
#include "ampi/adTool/support.h"

MPI_Datatype AMPI_ADOUBLE;
MPI_Datatype AMPI_AFLOAT;

#ifdef AMPI_FORTRANCOMPATIBLE
MPI_Datatype AMPI_ADOUBLE_PRECISION;
MPI_Datatype AMPI_AREAL;
#endif

int FW_AMPI_Recv(void* buf, 
		 int count,
		 MPI_Datatype datatype, 
		 int src, 
		 int tag,
		 AMPI_PairedWith pairedWith,
		 MPI_Comm comm,
		 MPI_Status* status) { 
  int rc;
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
	)) rc=MPI_Abort(comm, MPI_ERR_ARG);
  else { 
    MPI_Status myStatus;
    double* mappedbuf=NULL;
    if(ADTOOL_AMPI_isActiveType(datatype)==AMPI_ACTIVE) {
      mappedbuf=ADTOOL_AMPI_rawData(buf,&count);
    }
    else {
      mappedbuf=buf;
    }
    rc=MPI_Recv(mappedbuf,
		count,
		datatype,
		src,
		tag,
		comm,
		&myStatus); /* because status as passed in may be MPI_STATUS_IGNORE */
    if (rc==MPI_SUCCESS && ADTOOL_AMPI_isActiveType(datatype)==AMPI_ACTIVE) {
      ADTOOL_AMPI_writeData(buf,&count);
      if(tag==MPI_ANY_TAG) tag=myStatus.MPI_TAG;
      if(src==MPI_ANY_SOURCE) src=myStatus.MPI_SOURCE;
      ADTOOL_AMPI_pushSRinfo(buf,
			     count,
			     datatype,
			     src,
			     tag,
			     pairedWith,
			     comm);
      ADTOOL_AMPI_push_CallCode(AMPI_RECV);
    }
    if (status!=MPI_STATUS_IGNORE) *status=myStatus;
  }
  return rc;
}  

int BW_AMPI_Recv(void* buf, 
		 int count,
		 MPI_Datatype datatype, 
		 int src, 
		 int tag,
		 AMPI_PairedWith pairedWith,
		 MPI_Comm comm,
		 MPI_Status* status) { 
  int rc;
  void *idx=NULL;
  ADTOOL_AMPI_popSRinfo(&buf, 
			&count,
			&datatype,
			&src,
			&tag,
			&pairedWith,
			&comm,
			&idx);
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
	)) rc=MPI_Abort(comm, MPI_ERR_ARG);
  else { 
    switch(pairedWith) { 
    case AMPI_ISEND_WAIT:
    case AMPI_SEND: { 
      ADTOOL_AMPI_getAdjointCount(&count,datatype);   
      rc=MPI_Send(buf,
		  count,
		  datatype,
		  src,
		  tag,
		  comm);
      ADTOOL_AMPI_adjointNullify(count,datatype,comm,
				 buf, buf, buf);
      break;
    }
    default:  
      rc=MPI_Abort(comm, MPI_ERR_TYPE);
      break;
    }
  }
  return rc;
}  

int FW_AMPI_Irecv (void* buf,
		   int count,
		   MPI_Datatype datatype,
		   int source,
		   int tag,
		   AMPI_PairedWith pairedWith,
		   MPI_Comm comm,
		   AMPI_Request* request) {
  int rc;
  if (!(
	pairedWith==AMPI_SEND
        ||
        pairedWith==AMPI_ISEND_WAIT
	||
	pairedWith==AMPI_ISEND_WAITALL
	)) rc=MPI_Abort(comm, MPI_ERR_ARG);
  else {
    double* mappedbuf=NULL;
    if(ADTOOL_AMPI_isActiveType(datatype)==AMPI_ACTIVE) {
      mappedbuf=ADTOOL_AMPI_rawData(buf,&count);
    }
    else {
      mappedbuf=buf;
    }
    rc= MPI_Irecv(mappedbuf,
		  count,
		  datatype,
		  source,
		  tag,
		  comm,
#ifdef AMPI_FORTRANCOMPATIBLE
                  request
#else
                  &(request->plainRequest)
#endif
		  );
    struct AMPI_Request_S *ampiRequest;
#ifdef AMPI_FORTRANCOMPATIBLE
    struct AMPI_Request_S ampiRequestInst;
    ampiRequest=&ampiRequestInst;
    ampiRequest->plainRequest=*request;
#else 
    ampiRequest=request;
#endif
    /* fill in the other info */
    ampiRequest->endPoint=source;
    ampiRequest->tag=tag;
    ampiRequest->count=count;
    ampiRequest->datatype=datatype;
    ampiRequest->comm=comm;
    ampiRequest->origin=AMPI_RECV_ORIGIN;
    ampiRequest->pairedWith=pairedWith;
    ADTOOL_AMPI_mapBufForAdjoint(ampiRequest,buf);
    ampiRequest->tracedRequest=ampiRequest->plainRequest;
#ifdef AMPI_FORTRANCOMPATIBLE
    BK_AMPI_put_AMPI_Request(ampiRequest);
#endif
    if (ADTOOL_AMPI_isActiveType(datatype)==AMPI_ACTIVE) {
      ADTOOL_AMPI_push_CallCode(AMPI_IRECV);
#ifdef AMPI_REQUESTONTRACE
      ADTOOL_AMPI_push_request(ampiRequest->tracedRequest);
#endif
    }
  }
  return rc;
}

int BW_AMPI_Irecv (void* buf, 
		   int count, 
		   MPI_Datatype datatype, 
		   int source, 
		   int tag,
		   AMPI_PairedWith pairedWith,
		   MPI_Comm comm, 
		   AMPI_Request* request) {
  int rc;
  MPI_Request *plainRequest;
  struct AMPI_Request_S *ampiRequest;
#ifdef AMPI_REQUESTONTRACE
  MPI_Request tracedRequest;
#endif
#ifdef AMPI_FORTRANCOMPATIBLE
  struct AMPI_Request_S ampiRequestInst;
  ampiRequest=&ampiRequestInst;
  plainRequest=request;
#else
  plainRequest=&(request->plainRequest) ;
  ampiRequest=request;
#endif
#if defined AMPI_FORTRANCOMPATIBLE || defined AMPI_REQUESTONTRACE
#ifdef AMPI_REQUESTONTRACE
  tracedRequest=ADTOOL_AMPI_pop_request();
  BK_AMPI_get_AMPI_Request(&tracedRequest,ampiRequest,1);
#else 
  BK_AMPI_get_AMPI_Request(plainRequest,ampiRequest,0);
#endif
#endif
  assert(ampiRequest->origin==AMPI_RECV_ORIGIN) ;
  if (!(
	ampiRequest->pairedWith==AMPI_SEND 
	|| 
	ampiRequest->pairedWith==AMPI_ISEND_WAIT
	||
	ampiRequest->pairedWith==AMPI_ISEND_WAITALL
	)) rc=MPI_Abort(comm, MPI_ERR_ARG);
  else { 
    switch(ampiRequest->pairedWith) { 
    case AMPI_SEND:
    case AMPI_ISEND_WAIT: {
      rc=MPI_Wait(plainRequest,
		  MPI_STATUS_IGNORE);
      ADTOOL_AMPI_adjointNullify(ampiRequest->adjointCount,
                                 ampiRequest->datatype,
                                 ampiRequest->comm,
                                 ampiRequest->buf,
                                 ampiRequest->adjointBuf,
                                 buf);
      break ;
    }
    default:  
      rc=MPI_Abort(ampiRequest->comm, MPI_ERR_TYPE);
      break;
    }
  }
  return rc;
}

int FW_AMPI_Send (void* buf, 
                  int count, 
                  MPI_Datatype datatype, 
                  int dest, 
                  int tag,
                  AMPI_PairedWith pairedWith,
                  MPI_Comm comm) {
  int rc;
  if (!(
	pairedWith==AMPI_RECV 
	|| 
	pairedWith==AMPI_IRECV_WAIT
	||
	pairedWith==AMPI_IRECV_WAITALL
	)) rc=MPI_Abort(comm, MPI_ERR_ARG);
  else { 
    double* mappedbuf=NULL;
    if(ADTOOL_AMPI_isActiveType(datatype)==AMPI_ACTIVE) {
      mappedbuf=ADTOOL_AMPI_rawData(buf,&count);
    }
    else {
      mappedbuf=buf;
    }
    rc=MPI_Send(mappedbuf,
		count,
		datatype,
		dest,
		tag,
		comm);
    if (rc==MPI_SUCCESS && ADTOOL_AMPI_isActiveType(datatype)==AMPI_ACTIVE) {
      ADTOOL_AMPI_pushSRinfo(buf,
			     count,
			     datatype,
			     dest,
			     tag,
			     pairedWith,
			     comm);
      ADTOOL_AMPI_push_CallCode(AMPI_SEND);
    }
  }
  return rc;
}

int BW_AMPI_Send (void* buf,
                  int count, 
                  MPI_Datatype datatype, 
                  int dest, 
                  int tag,
                  AMPI_PairedWith pairedWith,
                  MPI_Comm comm) {
  int rc;
  void *idx=NULL;
  ADTOOL_AMPI_popSRinfo(&buf, 
			&count,
			&datatype,
			&dest,
			&tag,
			&pairedWith,
			&comm,
			&idx);
  if (!(
	pairedWith==AMPI_RECV 
	|| 
	pairedWith==AMPI_IRECV_WAIT
	||
	pairedWith==AMPI_IRECV_WAITALL
	)) rc=MPI_Abort(comm, MPI_ERR_ARG);
  else {
    switch(pairedWith) {
    case AMPI_IRECV_WAIT:
    case AMPI_RECV: {
      void *tempBuf = ADTOOL_AMPI_allocateTempBuf(count,datatype,comm) ;
      rc=MPI_Recv(tempBuf,
                  count,
                  datatype,
                  dest,
                  tag,
                  comm,
                  MPI_STATUS_IGNORE) ;
      ADTOOL_AMPI_adjointIncrement(count,
                                   datatype,
                                   comm,
				   buf,
				   buf,
                                   buf,
                                   tempBuf,
				   idx);
      ADTOOL_AMPI_releaseAdjointTempBuf(tempBuf);
      break;
    }
    default:  
      rc=MPI_Abort(comm, MPI_ERR_TYPE);
      break;
    }
  }
  return rc;
}

int FW_AMPI_Isend (void* buf,
		   int count, 
		   MPI_Datatype datatype, 
		   int dest, 
		   int tag,
		   AMPI_PairedWith pairedWith,
		   MPI_Comm comm, 
		   AMPI_Request* request) { 
  int rc;
  if (!(
	pairedWith==AMPI_RECV 
	|| 
	pairedWith==AMPI_IRECV_WAIT
	||
	pairedWith==AMPI_IRECV_WAITALL
	)) rc=MPI_Abort(comm, MPI_ERR_ARG);
  else { 
    double* mappedbuf=NULL;
    if(ADTOOL_AMPI_isActiveType(datatype)==AMPI_ACTIVE) {
      mappedbuf=ADTOOL_AMPI_rawData(buf,&count);
    }
    else {
      mappedbuf=buf;
    }
    rc= MPI_Isend(mappedbuf,
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
    struct AMPI_Request_S *ampiRequest;
#ifdef AMPI_FORTRANCOMPATIBLE
    struct AMPI_Request_S ampiRequestInst;
    ampiRequest=&ampiRequestInst;
    ampiRequest->plainRequest=*request;
#else 
    ampiRequest=request;
#endif
    /* fill in the other info */
    ampiRequest->endPoint=dest;
    ampiRequest->tag=tag;
    ampiRequest->count=count;
    ampiRequest->datatype=datatype;
    ampiRequest->comm=comm;
    ampiRequest->origin=AMPI_SEND_ORIGIN;
    ampiRequest->pairedWith=pairedWith;
    ADTOOL_AMPI_mapBufForAdjoint(ampiRequest,buf);
    ampiRequest->tracedRequest=ampiRequest->plainRequest;
#ifdef AMPI_FORTRANCOMPATIBLE
    BK_AMPI_put_AMPI_Request(ampiRequest);
#endif
    if (ADTOOL_AMPI_isActiveType(datatype)==AMPI_ACTIVE) {
      ADTOOL_AMPI_push_CallCode(AMPI_ISEND);
#ifdef AMPI_REQUESTONTRACE
      ADTOOL_AMPI_push_request(ampiRequest->tracedRequest);
#endif
    }
  }
  return rc;
}

int BW_AMPI_Isend (void* buf, 
		   int count, 
		   MPI_Datatype datatype, 
		   int dest, 
		   int tag,
		   AMPI_PairedWith pairedWith,
		   MPI_Comm comm, 
		   AMPI_Request* request) { 
  int rc;
  MPI_Request *plainRequest;
  struct AMPI_Request_S *ampiRequest;
#ifdef AMPI_REQUESTONTRACE
  MPI_Request tracedRequest;
#endif
#ifdef AMPI_FORTRANCOMPATIBLE
  struct AMPI_Request_S ampiRequestInst;
  ampiRequest=&ampiRequestInst;
  plainRequest=request;
#else 
  ampiRequest=request;
  plainRequest=&(ampiRequest->plainRequest);
#endif
#if defined AMPI_FORTRANCOMPATIBLE || defined AMPI_REQUESTONTRACE
#ifdef AMPI_REQUESTONTRACE
  tracedRequest=ADTOOL_AMPI_pop_request();
  BK_AMPI_get_AMPI_Request(&tracedRequest,ampiRequest,1);
#else 
  BK_AMPI_get_AMPI_Request(plainRequest,ampiRequest,0);
#endif
#endif
  assert(ampiRequest->origin==AMPI_SEND_ORIGIN) ;
  if (!(
	ampiRequest->pairedWith==AMPI_RECV 
	|| 
	ampiRequest->pairedWith==AMPI_IRECV_WAIT
	||
	ampiRequest->pairedWith==AMPI_IRECV_WAITALL
	)) rc=MPI_Abort(comm, MPI_ERR_ARG);
  else { 
    switch(ampiRequest->pairedWith) { 
    case AMPI_RECV:
    case AMPI_IRECV_WAIT: { 
      rc=MPI_Wait(plainRequest,
		  MPI_STATUS_IGNORE);
      ADTOOL_AMPI_adjointIncrement(ampiRequest->adjointCount,
                                   ampiRequest->datatype,
                                   ampiRequest->comm,
				   ampiRequest->buf,
				   ampiRequest->adjointBuf,
                                   buf,
                                   ampiRequest->adjointTempBuf,
	                           ampiRequest->idx);
      ADTOOL_AMPI_releaseAdjointTempBuf(ampiRequest->adjointTempBuf);
      break;
    }
    default:  
      rc=MPI_Abort(ampiRequest->comm, MPI_ERR_TYPE);
      break;
    }
  }
  return rc;
}

int FW_AMPI_Wait(AMPI_Request *request,
		 MPI_Status *status) { 
  int rc;
  MPI_Request *plainRequest;
  struct AMPI_Request_S *ampiRequest;
#ifdef AMPI_FORTRANCOMPATIBLE
  struct AMPI_Request_S ampiRequestInst;
  ampiRequest=&ampiRequestInst;
  plainRequest=request;
  /*[llh] doubt about the 3rd argument (0?) for the OO traced case: */
  BK_AMPI_get_AMPI_Request(plainRequest,ampiRequest,0);
#else 
  plainRequest=&(request->plainRequest);
  ampiRequest=request;
#endif 
  rc=MPI_Wait(plainRequest,
	      status);
  if (rc==MPI_SUCCESS && ADTOOL_AMPI_isActiveType(ampiRequest->datatype)==AMPI_ACTIVE) {
    ADTOOL_AMPI_writeData(ampiRequest->buf,&ampiRequest->count);
    ADTOOL_AMPI_push_AMPI_Request(ampiRequest);
    ADTOOL_AMPI_push_CallCode(AMPI_WAIT);
  }
  return rc;
}

int BW_AMPI_Wait(AMPI_Request *request,
		 MPI_Status *status) {
  int rc; 
  struct AMPI_Request_S *ampiRequest;
#ifdef AMPI_FORTRANCOMPATIBLE
  struct AMPI_Request_S ampiRequestInst;
  ampiRequest=&ampiRequestInst;
#else 
  ampiRequest=request;
#endif 
  /* pop request  */
  ADTOOL_AMPI_pop_AMPI_Request(ampiRequest);
  switch(ampiRequest->origin) { 
  case AMPI_SEND_ORIGIN: { 
    ADTOOL_AMPI_setAdjointCountAndTempBuf(ampiRequest);   
    rc=MPI_Irecv(ampiRequest->adjointTempBuf,
		 ampiRequest->adjointCount,
		 ampiRequest->datatype,
		 ampiRequest->endPoint,
		 ampiRequest->tag,
		 ampiRequest->comm,
		 &(ampiRequest->plainRequest));
    break;
  }
  case AMPI_RECV_ORIGIN: { 
    ADTOOL_AMPI_setAdjointCount(ampiRequest);
    rc=MPI_Isend(ADTOOL_AMPI_rawAdjointData(ampiRequest->adjointBuf),
		 ampiRequest->adjointCount,
		 ampiRequest->datatype,
		 ampiRequest->endPoint,
		 ampiRequest->tag,
		 ampiRequest->comm,
		 &(ampiRequest->plainRequest));
    break;
  }
  default:  
    rc=MPI_Abort(ampiRequest->comm, MPI_ERR_TYPE);
    break;
  }
#ifdef AMPI_FORTRANCOMPATIBLE 
  *request=ampiRequest->plainRequest;
#endif
#if defined AMPI_FORTRANCOMPATIBLE || defined AMPI_REQUESTONTRACE
  BK_AMPI_put_AMPI_Request(ampiRequest);
#endif
  return rc;
}

int FW_AMPI_Gatherv(void *sendbuf,
                    int sendcnt,
                    MPI_Datatype sendtype,
                    void *recvbuf,
                    int *recvcnts,
                    int *displs,
                    MPI_Datatype recvtype,
                    int root,
                    MPI_Comm comm) {
  void *rawSendBuf=NULL, *rawRecvBuf=NULL;
  int rc=MPI_SUCCESS;
  int myRank, myCommSize;
  MPI_Comm_rank(comm, &myRank);
  MPI_Comm_size(comm, &myCommSize);
  if (ADTOOL_AMPI_isActiveType(sendtype)!=ADTOOL_AMPI_isActiveType(recvtype)) {
    rc=MPI_Abort(comm, MPI_ERR_ARG);
  }
  else {
    if (ADTOOL_AMPI_isActiveType(sendtype)==AMPI_ACTIVE)  rawSendBuf=ADTOOL_AMPI_rawData(sendbuf,&sendcnt);
    else rawSendBuf=sendbuf;
    if (myRank==root) {
      if (ADTOOL_AMPI_isActiveType(recvtype)==AMPI_ACTIVE)  rawRecvBuf=ADTOOL_AMPI_rawDataV(recvbuf,recvcnts, displs);
      else rawRecvBuf=recvbuf;
    }
    rc=MPI_Gatherv(rawSendBuf,
                   sendcnt,
                   sendtype,
                   rawRecvBuf,
                   recvcnts,
                   displs,
                   recvtype,
                   root,
                   comm);
    if (rc==MPI_SUCCESS && ADTOOL_AMPI_isActiveType(recvtype)==AMPI_ACTIVE) {
      if (myRank==root) ADTOOL_AMPI_writeDataV(recvbuf,recvcnts, displs);
      ADTOOL_AMPI_pushGSVinfo(((myRank==root)?myCommSize:0),
                              recvbuf,
                              recvcnts,
                              displs,
                              recvtype,
                              sendbuf,
                              sendcnt,
                              sendtype,
                              root,
                              comm);
      ADTOOL_AMPI_push_CallCode(AMPI_GATHERV);
    }
  }
  return rc;
}

int BW_AMPI_Gatherv(void *sendbuf,
                    int sendcnt,
                    MPI_Datatype sendtype,
                    void *recvbuf,
                    int *recvcnts,
                    int *displs,
                    MPI_Datatype recvtype,
                    int root,
                    MPI_Comm comm) {
  void *idx=NULL;
  int i;
  int rc=MPI_SUCCESS;
  int myRank, commSizeForRootOrNull, rTypeSize;
  int *tRecvCnts=recvcnts, *tDispls=displs;
  char tRecvCntsFlag=0, tDisplsFlag=0;
  ADTOOL_AMPI_popGSVcommSizeForRootOrNull(&commSizeForRootOrNull);
  if (tRecvCnts==NULL) {
    tRecvCnts=(int*)malloc(sizeof(int)*commSizeForRootOrNull);
    tRecvCntsFlag=1;
  }
  if (tDispls==NULL) {
    tDispls=(int*)malloc(sizeof(int)*commSizeForRootOrNull);
    tDisplsFlag=1;
  }
  ADTOOL_AMPI_popGSVinfo(commSizeForRootOrNull,
                         &recvbuf,
                         tRecvCnts,
                         tDispls,
                         &recvtype,
                         &sendbuf,
                         &sendcnt,
                         &sendtype,
                         &root,
                         &comm);
  MPI_Comm_rank(comm, &myRank);
  ADTOOL_AMPI_getAdjointCount(&sendcnt,sendtype);
  void *tempBuf = ADTOOL_AMPI_allocateTempBuf(sendcnt,sendtype,comm) ;
  rc=MPI_Scatterv(recvbuf,
                  tRecvCnts,
                  tDispls,
                  recvtype,
                  tempBuf,
                  sendcnt,
                  sendtype,
                  root,
                  comm);
  ADTOOL_AMPI_adjointIncrement(sendcnt,
                               sendtype,
                               comm,
                               sendbuf,
                               sendbuf,
                               sendbuf,
                               tempBuf,
                               idx);
  if (myRank==root) {
    MPI_Type_size(recvtype,&rTypeSize);
    for (i=0;i<commSizeForRootOrNull;++i) {
      void* buf=recvbuf+(rTypeSize*tDispls[i]); /* <----------  very iffy! */
      ADTOOL_AMPI_adjointNullify(tRecvCnts[i],recvtype,comm,
                                 buf , buf, buf);
    }
  }
  ADTOOL_AMPI_releaseAdjointTempBuf(tempBuf);
  if (tRecvCntsFlag) free((void*)(tRecvCnts));
  if (tDisplsFlag) free((void*)(tDispls));
  return rc;
}

int FW_AMPI_Scatterv(void *sendbuf,
                     int *sendcnts,
                     int *displs,
                     MPI_Datatype sendtype,
                     void *recvbuf,
                     int recvcnt,
                     MPI_Datatype recvtype,
                     int root,
                     MPI_Comm comm) {
  int rc=MPI_SUCCESS;
  int myRank, myCommSize;
  void *rawSendBuf=sendbuf, *rawRecvBuf=recvbuf;
  MPI_Comm_rank(comm, &myRank);
  MPI_Comm_size(comm, &myCommSize);
  if (ADTOOL_AMPI_isActiveType(sendtype)!=ADTOOL_AMPI_isActiveType(recvtype)) {
    rc=MPI_Abort(comm, MPI_ERR_ARG);
  }
  else {
    if (myRank==root) {
      if (ADTOOL_AMPI_isActiveType(sendtype)==AMPI_ACTIVE)  rawSendBuf=ADTOOL_AMPI_rawDataV(sendbuf,sendcnts,displs);
    }
    if (ADTOOL_AMPI_isActiveType(recvtype)==AMPI_ACTIVE)  rawRecvBuf=ADTOOL_AMPI_rawData(recvbuf,&recvcnt);
    rc=MPI_Scatterv(rawSendBuf,
                    sendcnts,
                    displs,
                    sendtype,
                    rawRecvBuf,
                    recvcnt,
                    recvtype,
                    root,
                    comm);
    if (rc==MPI_SUCCESS && ADTOOL_AMPI_isActiveType(recvtype)==AMPI_ACTIVE) {
      ADTOOL_AMPI_writeData(recvbuf,&recvcnt);
      ADTOOL_AMPI_pushGSVinfo(((myRank==root)?myCommSize:0),
                              sendbuf,
                              sendcnts,
                              displs,
                              sendtype,
                              recvbuf,
                              recvcnt,
                              recvtype,
                              root,
                              comm);
      ADTOOL_AMPI_push_CallCode(AMPI_SCATTERV);
    }
  }
  return rc;
}

int BW_AMPI_Scatterv(void *sendbuf,
                     int *sendcnts,
                     int *displs,
                     MPI_Datatype sendtype,
                     void *recvbuf,
                     int recvcnt,
                     MPI_Datatype recvtype,
                     int root,
                     MPI_Comm comm) {
  int rc=MPI_SUCCESS;
  void *idx=NULL;
  int sendSize=0,i, typeSize;
  int myRank, commSizeForRootOrNull, *tempDispls;
  int *tSendCnts=sendcnts, *tDispls=displs;
  char tSendCntsFlag=0, tDisplsFlag=0;
  ADTOOL_AMPI_popGSVcommSizeForRootOrNull(&commSizeForRootOrNull);
  if (tSendCnts==NULL && commSizeForRootOrNull>0) {
    tSendCnts=(int*)malloc(sizeof(int)*commSizeForRootOrNull);
    tSendCntsFlag=1;
  }
  if (tDispls==NULL && commSizeForRootOrNull>0) {
    tDispls=(int*)malloc(sizeof(int)*commSizeForRootOrNull);
    tDisplsFlag=1;
  }
  ADTOOL_AMPI_popGSVinfo(commSizeForRootOrNull,
                         &sendbuf,
                         tSendCnts,
                         tDispls,
                         &sendtype,
                         &recvbuf,
                         &recvcnt,
                         &recvtype,
                         &root,
                         &comm);
  MPI_Comm_rank(comm, &myRank);
  tempDispls=(int*)malloc(sizeof(int)*commSizeForRootOrNull);
  for (i=0;i<commSizeForRootOrNull;++i) {
    tempDispls[i]=sendSize;
    sendSize+=tSendCnts[i];
  }
  void *tempBuf = NULL;
  if (commSizeForRootOrNull>0) tempBuf=ADTOOL_AMPI_allocateTempBuf(sendSize,sendtype,comm);
  rc=MPI_Gatherv(recvbuf,
                 recvcnt,
                 recvtype,
                 tempBuf,
                 tSendCnts,
                 tempDispls,
                 sendtype,
                 root,
                 comm);
  ADTOOL_AMPI_adjointNullify(recvcnt,recvtype,comm,
                             recvbuf, recvbuf, recvbuf);
  if (commSizeForRootOrNull>0) {
    MPI_Type_size(sendtype,&typeSize);
    for (i=0;i<commSizeForRootOrNull;++i) {
      void* buf=sendbuf+(typeSize*tDispls[i]); /* <----------  very iffy! */
      void* sourceBuf=tempBuf+(typeSize*tempDispls[i]);
      ADTOOL_AMPI_adjointIncrement(tSendCnts[i],
                                   sendtype,
                                   comm,
                                   buf,
                                   buf,
                                   buf,
                                   sourceBuf,
                                   idx);
    }
    ADTOOL_AMPI_releaseAdjointTempBuf(tempBuf);
  }
  if (tSendCntsFlag) free((void*)(tSendCnts));
  if (tDisplsFlag) free((void*)(tDispls));
  return rc;
}
