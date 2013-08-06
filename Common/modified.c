#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
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
    int dt_idx = derivedTypeIdx(datatype);
    int is_derived = isDerivedType(dt_idx);
    derivedTypeData* dtd = getDTypeData();
    if(ADTOOL_AMPI_isActiveType(datatype)==AMPI_ACTIVE) {
      mappedbuf=ADTOOL_AMPI_rawData(buf,&count);
    }
    else if(is_derived) {
      mappedbuf=ADTOOL_AMPI_allocateTempBuf(count,datatype,comm);
    }
    else {
      mappedbuf=buf;
    }
    rc=MPI_Recv(mappedbuf,
		count,
		is_derived ? dtd->packed_types[dt_idx] : datatype,
		/* if derived then need to replace typemap */
		src,
		tag,
		comm,
		&myStatus); /* because status as passed in may be MPI_STATUS_IGNORE */
    if (rc==MPI_SUCCESS && (ADTOOL_AMPI_isActiveType(datatype)==AMPI_ACTIVE || is_derived)) {
      if (is_derived) {
	ADTOOL_AMPI_unpackDType(mappedbuf,buf,&count,dt_idx);
	ADTOOL_AMPI_releaseAdjointTempBuf(mappedbuf);
      }
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
      int propercount, propertype, dt_idx = derivedTypeIdx(datatype);
      if (isDerivedType(dt_idx)) {
	derivedTypeData* dtd = getDTypeData();
	propercount = dtd->num_actives[dt_idx]*count;
	propertype = MPI_DOUBLE;
      }
      else {
	propercount = count;
	propertype = datatype;
      }
      ADTOOL_AMPI_getAdjointCount(&count,datatype);   
      rc=MPI_Send(buf,
		  propercount,
		  propertype,
		  src,
		  tag,
		  comm);
      ADTOOL_AMPI_adjointNullify(propercount,propertype,comm,
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
    int dt_idx = derivedTypeIdx(datatype);
    int is_derived = isDerivedType(dt_idx);
    derivedTypeData* dtd = getDTypeData();
    if(ADTOOL_AMPI_isActiveType(datatype)==AMPI_ACTIVE) {
      mappedbuf=ADTOOL_AMPI_rawData(buf,&count);
    }
    else if(is_derived) {
      mappedbuf=ADTOOL_AMPI_rawData_DType(buf,&count,dt_idx);
    }
    else {
      mappedbuf=buf;
    }
    rc=MPI_Send(mappedbuf,
		count,
		is_derived ? dtd->packed_types[dt_idx] : datatype,
		/* if derived then need to replace typemap */
		dest,
		tag,
		comm);
    if (is_derived) ADTOOL_AMPI_releaseAdjointTempBuf(mappedbuf);
    if (rc==MPI_SUCCESS && (ADTOOL_AMPI_isActiveType(datatype)==AMPI_ACTIVE || is_derived)) {
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
      int propercount, propertype, dt_idx = derivedTypeIdx(datatype);
      if (isDerivedType(dt_idx)) {
	derivedTypeData* dtd = getDTypeData();
	propercount = dtd->num_actives[dt_idx]*count;
	propertype = MPI_DOUBLE;
      }
      else {
	propercount = count;
	propertype = datatype;
      }
      void *tempBuf = ADTOOL_AMPI_allocateTempBuf(propercount,propertype,comm) ;
      rc=MPI_Recv(tempBuf,
                  propercount,
                  propertype,
                  dest,
                  tag,
                  comm,
                  MPI_STATUS_IGNORE) ;
      ADTOOL_AMPI_adjointIncrement(propercount,
                                   propertype,
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

int FW_AMPI_Bcast (void* buf,
                   int count,
                   MPI_Datatype datatype,
                   int root,
                   MPI_Comm comm) {
  int rc;
  double* mappedbuf=NULL;
  if(ADTOOL_AMPI_isActiveType(datatype)==AMPI_ACTIVE) {
    mappedbuf=ADTOOL_AMPI_rawData(buf,&count);
  }
  else {
    mappedbuf=buf;
  }
  MPI_Datatype newtype = AMPI_ADOUBLE==MPI_DOUBLE ? datatype : MPI_DOUBLE;
  /* ^ very makeshift */
  rc=MPI_Bcast(mappedbuf,
               count,
               newtype,
               root,
               comm);
  if (rc==MPI_SUCCESS && ADTOOL_AMPI_isActiveType(datatype)==AMPI_ACTIVE) {
    ADTOOL_AMPI_pushBcastInfo(buf,
			      count,
			      datatype,
			      root,
			      comm);
    ADTOOL_AMPI_push_CallCode(AMPI_BCAST);
  }
  return rc;
}

int BW_AMPI_Bcast (void* buf,
                   int count,
                   MPI_Datatype datatype,
                   int root,
                   MPI_Comm comm) {
  int rc,rank;
  void *idx=NULL;
  ADTOOL_AMPI_popBcastInfo(&buf,
			   &count,
			   &datatype,
			   &root,
			   &comm,
			   &idx);
  MPI_Comm_rank(comm,&rank);
  void *tempBuf = ADTOOL_AMPI_allocateTempBuf(count,datatype,comm);
  MPI_Datatype newtype = AMPI_ADOUBLE==MPI_DOUBLE ? datatype : MPI_DOUBLE;
  rc=MPI_Reduce(buf,
                tempBuf,
                count,
                newtype,
                MPI_SUM,
                root,
                comm);
  ADTOOL_AMPI_adjointNullify(count, datatype, comm,
                             buf, buf, buf);
  if (rank==root) {
    ADTOOL_AMPI_adjointIncrement(count, datatype, comm,
                                 buf, buf, buf, tempBuf, idx);
  }
  ADTOOL_AMPI_releaseAdjointTempBuf(tempBuf);
  return rc;
}

int FW_AMPI_Reduce (void* sbuf,
		    void* rbuf,
		    int count,
		    MPI_Datatype datatype,
		    MPI_Op op,
		    int root,
		    MPI_Comm comm) {
  int rc,rank;
  MPI_Comm_rank(comm,&rank);
  double* mappedsbuf=NULL;
  double* mappedrbuf=NULL;
  if(ADTOOL_AMPI_isActiveType(datatype)==AMPI_ACTIVE) {
    mappedsbuf=ADTOOL_AMPI_rawData(sbuf,&count);
    mappedrbuf=ADTOOL_AMPI_rawData(rbuf,&count);
  }
  else {
    mappedsbuf=sbuf;
    mappedrbuf=rbuf;
  }
  MPI_Datatype newtype = AMPI_ADOUBLE==MPI_DOUBLE ? datatype : MPI_DOUBLE;
  rc=MPI_Reduce(mappedsbuf,
		mappedrbuf,
		count,
		newtype,
		op,
		root,
		comm);
  if (rc==MPI_SUCCESS && ADTOOL_AMPI_isActiveType(datatype)==AMPI_ACTIVE) {
    ADTOOL_AMPI_pushReduceInfo(sbuf,
			       rbuf,
			       rbuf,
			       rank==root, /* also push contents of rbuf for root */
			       count,
			       datatype,
			       op,
			       root,
			       comm);
    ADTOOL_AMPI_push_CallCode(AMPI_REDUCE);
  }
  return rc;
}

int BW_AMPI_Reduce (void* sbuf,
		    void* rbuf,
		    int count,
		    MPI_Datatype datatype,
		    MPI_Op op,
		    int root,
		    MPI_Comm comm) {
  int rc,rank;
  void *idx=NULL;
  void *prevValBuf = ADTOOL_AMPI_allocateTempBuf(count,MPI_DOUBLE,comm);
  void *reduceResultBuf = ADTOOL_AMPI_allocateTempBuf(count,MPI_DOUBLE,comm);
  ADTOOL_AMPI_popReduceInfo(&sbuf,
			    &rbuf,
			    &prevValBuf,
			    &reduceResultBuf,
			    &count,
			    &datatype,
			    &op,
			    &root,
			    &comm,
			    &idx);
  MPI_Comm_rank(comm,&rank);
  rc=MPI_Bcast(reduceResultBuf,
	       count,
	       MPI_DOUBLE,
	       root,
	       comm);
  if (rc) MPI_Abort(comm, MPI_ERR_ARG);
  /* ^ root pushed result of reduction, so pop it into reduceResultBuf and
     broadcast; now everyone has a_i and op(a_i) */
  void *tempBuf = ADTOOL_AMPI_allocateTempBuf(count,datatype,comm);
  if (rank==root) {
    ADTOOL_AMPI_adjointNullify(count, datatype, comm,
			       tempBuf, tempBuf, tempBuf);
    ADTOOL_AMPI_adjointIncrement(count, datatype, comm,
				 tempBuf, tempBuf, tempBuf, rbuf, idx);
    ADTOOL_AMPI_adjointNullify(count, datatype, comm,
			       rbuf, rbuf, rbuf);
  }
  rc=MPI_Bcast(tempBuf,
	       count,
	       datatype,
	       root,
	       comm);
  if (op==MPI_PROD) {
    ADTOOL_AMPI_adjointMultiply(count, datatype, comm,
				tempBuf, tempBuf, tempBuf, reduceResultBuf, idx);
    ADTOOL_AMPI_adjointDivide(count, datatype, comm,
			      tempBuf, tempBuf, tempBuf, prevValBuf, idx);
  }
  else {}
  ADTOOL_AMPI_adjointIncrement(count, datatype, comm,
			       sbuf, sbuf, sbuf, tempBuf, idx);
  ADTOOL_AMPI_releaseAdjointTempBuf(tempBuf);
  ADTOOL_AMPI_releaseAdjointTempBuf(reduceResultBuf);
  ADTOOL_AMPI_releaseAdjointTempBuf(prevValBuf);
  return rc;
}

derivedTypeData* getDTypeData() {
  static derivedTypeData* dat = NULL;
  if (dat==NULL) {
    derivedTypeData* newdat = malloc(sizeof(derivedTypeData));
    newdat->size = 4;
    newdat->pos = 0;
    newdat->num_actives = (int*)malloc((newdat->size)*sizeof(int));
    newdat->first_active_indices = (int*)malloc((newdat->size)*sizeof(int));
    newdat->last_active_indices = (int*)malloc((newdat->size)*sizeof(int));
    newdat->derived_types = (MPI_Datatype*)malloc((newdat->size)*sizeof(MPI_Datatype));
    newdat->counts = (int*)malloc((newdat->size)*sizeof(int));
    newdat->arrays_of_blocklengths = (int**)malloc((newdat->size)*sizeof(int*));
    newdat->arrays_of_displacements = (MPI_Aint**)malloc((newdat->size)*sizeof(MPI_Aint*));
    newdat->arrays_of_types = (MPI_Datatype**)malloc((newdat->size)*sizeof(MPI_Datatype*));
    newdat->mapsizes = (int*)malloc((newdat->size)*sizeof(int));
    newdat->packed_types = (MPI_Datatype*)malloc((newdat->size)*sizeof(MPI_Datatype));
    newdat->arrays_of_p_blocklengths = (int**)malloc((newdat->size)*sizeof(int*));
    newdat->arrays_of_p_displacements = (MPI_Aint**)malloc((newdat->size)*sizeof(MPI_Aint*));
    newdat->arrays_of_p_types = (MPI_Datatype**)malloc((newdat->size)*sizeof(MPI_Datatype*));
    newdat->p_mapsizes = (int*)malloc((newdat->size)*sizeof(int));
    dat = newdat;
  }
  return dat;
}

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
		 MPI_Datatype* packed_type) {
  if (dat==NULL) assert(0);
  int i;
  int num_actives=0;
  int fst_active_idx, fst_aidx_set=0, lst_active_idx;
  for (i=0;i<count;i++) {
    if (ADTOOL_AMPI_isActiveType(array_of_types[i])==AMPI_ACTIVE) {
      num_actives += array_of_blocklengths[i];
      if (!fst_aidx_set) {
	fst_active_idx = i;
	fst_aidx_set = 1;
      }
      lst_active_idx = i;
    }
  }
  if (!num_actives) return -1;
  int pos = dat->pos;
  if (pos >= dat->size) {
    dat->size *= 2;
    dat->num_actives = (int*)realloc((void*)dat->num_actives, (dat->size)*sizeof(int));
    dat->first_active_indices = (int*)realloc((void*)dat->first_active_indices, (dat->size)*sizeof(int));
    dat->last_active_indices = (int*)realloc((void*)dat->last_active_indices, (dat->size)*sizeof(int));
    dat->derived_types = (MPI_Datatype*)realloc((void*)dat->derived_types,
						(dat->size)*sizeof(MPI_Datatype));
    dat->counts = (int*)realloc((void*)dat->counts, (dat->size)*sizeof(int));
    dat->arrays_of_blocklengths = (int**)realloc((void*)dat->arrays_of_blocklengths,
						 (dat->size)*sizeof(int*));
    dat->arrays_of_displacements = (MPI_Aint**)realloc((void*)dat->arrays_of_displacements,
						       (dat->size)*sizeof(MPI_Aint*));
    dat->arrays_of_types = (MPI_Datatype**)realloc((void*)dat->arrays_of_types,
						   (dat->size)*sizeof(MPI_Datatype*));
    dat->mapsizes = (int*)realloc((void*)dat->mapsizes, (dat->size)*sizeof(int));
    dat->packed_types = (MPI_Datatype*)realloc((void*)dat->packed_types,
						(dat->size)*sizeof(MPI_Datatype));
    dat->arrays_of_p_blocklengths = (int**)realloc((void*)dat->arrays_of_p_blocklengths,
						 (dat->size)*sizeof(int*));
    dat->arrays_of_p_displacements = (MPI_Aint**)realloc((void*)dat->arrays_of_p_displacements,
						       (dat->size)*sizeof(MPI_Aint*));
    dat->arrays_of_p_types = (MPI_Datatype**)realloc((void*)dat->arrays_of_p_types,
						   (dat->size)*sizeof(MPI_Datatype*));
    dat->p_mapsizes = (int*)realloc((void*)dat->p_mapsizes, (dat->size)*sizeof(int));
  }
  dat->num_actives[pos] = num_actives;
  dat->first_active_indices[pos] = fst_active_idx;
  dat->last_active_indices[pos] = lst_active_idx;
  dat->derived_types[pos] = *newtype;
  dat->counts[pos] = count;
  dat->arrays_of_blocklengths[pos] = (int*)malloc(count*sizeof(int));
  memcpy((void*)dat->arrays_of_blocklengths[pos],(void*)array_of_blocklengths,count*sizeof(int));
  dat->arrays_of_displacements[pos] = (MPI_Aint*)malloc(count*sizeof(MPI_Aint));
  memcpy((void*)dat->arrays_of_displacements[pos],(void*)array_of_displacements,count*sizeof(MPI_Aint));
  dat->arrays_of_types[pos] = (MPI_Datatype*)malloc(count*sizeof(MPI_Datatype));
  memcpy((void*)dat->arrays_of_types[pos],(void*)array_of_types,count*sizeof(MPI_Datatype));
  dat->mapsizes[pos] = mapsize;
  dat->packed_types[pos] = *packed_type;
  dat->arrays_of_p_blocklengths[pos] = (int*)malloc(count*sizeof(int));
  memcpy((void*)dat->arrays_of_p_blocklengths[pos],(void*)array_of_p_blocklengths,count*sizeof(int));
  dat->arrays_of_p_displacements[pos] = (MPI_Aint*)malloc(count*sizeof(MPI_Aint));
  memcpy((void*)dat->arrays_of_p_displacements[pos],(void*)array_of_p_displacements,count*sizeof(MPI_Aint));
  dat->arrays_of_p_types[pos] = (MPI_Datatype*)malloc(count*sizeof(MPI_Datatype));
  memcpy((void*)dat->arrays_of_p_types[pos],(void*)array_of_p_types,count*sizeof(MPI_Datatype));
  dat->p_mapsizes[pos] = p_mapsize;
  dat->pos += 1;
  return pos;
}

int derivedTypeIdx(MPI_Datatype datatype) {
  int i;
  derivedTypeData* dtdata = getDTypeData();
  for (i=0;i<dtdata->size;i++) {
    if (dtdata->derived_types[i]==datatype) return i;
  }
  return -1;
}

int isDerivedType(int dt_idx) { return dt_idx!=-1; }

int AMPI_Type_create_struct (int count,
			     int array_of_blocklengths[],
			     MPI_Aint array_of_displacements[],
			     MPI_Datatype array_of_types[],
			     MPI_Datatype *newtype) {
  int i, rc;
  rc = MPI_Type_create_struct (count,
			       array_of_blocklengths,
			       array_of_displacements,
			       array_of_types,
			       newtype);
  if (!(rc==MPI_SUCCESS)) assert(0);
  MPI_Datatype packed_type;
  int array_of_p_blocklengths[count];
  MPI_Aint array_of_p_displacements[count];
  MPI_Datatype array_of_p_types[count];
  int s, is_active, mapsize=0, p_mapsize=0;
  for (i=0;i<count;i++) {
    is_active = ADTOOL_AMPI_isActiveType(array_of_types[i])==AMPI_ACTIVE;
    array_of_p_blocklengths[i] = array_of_blocklengths[i];
    array_of_p_displacements[i] = p_mapsize;
    array_of_p_types[i] = is_active ? MPI_DOUBLE : array_of_types[i];
    if (is_active) s = sizeof(double);
    else if (array_of_types[i]==MPI_DOUBLE) s = sizeof(double);
    else if (array_of_types[i]==MPI_INT) s = sizeof(int);
    else if (array_of_types[i]==MPI_FLOAT) s = sizeof(float);
    else if (array_of_types[i]==MPI_CHAR) s = sizeof(char);
    else if (array_of_types[i]==MPI_UB) {
      mapsize = (int)array_of_displacements[i];
      break;
    }
    else assert(0);
    p_mapsize += array_of_blocklengths[i]*s;
  }
  if (mapsize==0) {
    MPI_Aint lb,extent;
    MPI_Type_get_extent(*newtype,&lb,&extent);
    mapsize = (int)extent;
  }
  rc = MPI_Type_create_struct (count,
			       array_of_p_blocklengths,
			       array_of_p_displacements,
			       array_of_p_types,
			       &packed_type);
  if (!(rc==MPI_SUCCESS)) assert(0);
  derivedTypeData* dat = getDTypeData();  
  int pos = addDTypeData(dat,
			 count,
			 array_of_blocklengths,
			 array_of_displacements,
			 array_of_types,
			 mapsize,
			 array_of_p_blocklengths,
			 array_of_p_displacements,
			 array_of_p_types,
			 p_mapsize,
			 newtype,
			 &packed_type);
  /* DEBUGGING STUFF, REMOVE LATER*/
  if (0) {
    int i,j;
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    if (rank==0) {
      printf("So far (pos %d):\n",pos);
      printf("Size: %d\n",dat->size);
      for (i=0;i<=pos;i++) {
	printf("-------\nPos: %d\n",i);
	printf("Count: %d\n",dat->counts[i]);
	printf("Blocklengths: ");
	for (j=0;j<dat->counts[i];j++) printf("%d%c",(int)dat->arrays_of_blocklengths[i][j],j==dat->counts[i]-1?'\n':' ');
	printf("Displacements: ");
	for (j=0;j<dat->counts[i];j++) printf("%d%c",(int)dat->arrays_of_displacements[i][j],j==dat->counts[i]-1?'\n':' ');
	printf("Types: ");
	for (j=0;j<dat->counts[i];j++) printf("%d%c",(int)dat->arrays_of_types[i][j],j==dat->counts[i]-1?'\n':' ');
	printf("p_Blocklengths: ");
	for (j=0;j<dat->counts[i];j++) printf("%d%c",(int)dat->arrays_of_p_blocklengths[i][j],j==dat->counts[i]-1?'\n':' ');
	printf("p_Displacements: ");
	for (j=0;j<dat->counts[i];j++) printf("%d%c",(int)dat->arrays_of_p_displacements[i][j],j==dat->counts[i]-1?'\n':' ');
	printf("p_Types: ");
	for (j=0;j<dat->counts[i];j++) printf("%d%c",(int)dat->arrays_of_p_types[i][j],j==dat->counts[i]-1?'\n':' ');
      }
      printf("\n");
    }
  }
  return rc;
}

int AMPI_Type_commit (MPI_Datatype *datatype) {
  int dt_idx = derivedTypeIdx(*datatype);
  if (isDerivedType(dt_idx)) MPI_Type_commit(&(getDTypeData()->packed_types[dt_idx]));
  return MPI_Type_commit (datatype);
}
