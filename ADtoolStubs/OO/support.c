#include <stdlib.h>
#include <assert.h>
#include <mpi.h>
#include "ampi/adTool/support.h"

void ADTOOL_AMPI_pushBcastInfo(void* buf,
			       int count,
			       MPI_Datatype datatype,
			       int root,
			       MPI_Comm comm) {
}

void ADTOOL_AMPI_popBcastInfo(void** buf,
			      int* count,
			      MPI_Datatype* datatype,
			      int* root,
			      MPI_Comm* comm,
			      void **idx) {
}

void ADTOOL_AMPI_pushDoubleArray(void* buf,
				 int count) {
}

void ADTOOL_AMPI_popDoubleArray(double* buf,
				int* count) {
}

void ADTOOL_AMPI_pushReduceInfo(void* sbuf,
				void* rbuf,
				void* resultData,
				int pushResultData, /* push resultData if true */
				int count,
				MPI_Datatype datatype,
				MPI_Op op,
				int root,
				MPI_Comm comm) {
}
void ADTOOL_AMPI_popReduceCountAndType(int* count,
				       MPI_Datatype* datatype) {
}

void ADTOOL_AMPI_popReduceInfo(void** sbuf,
			       void** rbuf,
			       void** prevData,
			       void** resultData,
			       int* count,
			       MPI_Op* op,
			       int* root,
			       MPI_Comm* comm,
			       void **idx) {
}

void ADTOOL_AMPI_pushSRinfo(void* buf, 
			    int count,
			    MPI_Datatype datatype, 
			    int src, 
			    int tag,
			    AMPI_PairedWith pairedWith,
			    MPI_Comm comm) { 
}

void ADTOOL_AMPI_popSRinfo(void** buf,
			   int* count,
			   MPI_Datatype* datatype, 
			   int* src, 
			   int* tag,
			   AMPI_PairedWith* pairedWith,
			   MPI_Comm* comm,
			   void **idx) { 
}

void ADTOOL_AMPI_pushGSinfo(int commSizeForRootOrNull,
                            void *rbuf,
                            int rcnt,
                            MPI_Datatype rtype,
                            void *buf,
                            int count,
                            MPI_Datatype type,
                            int  root,
                            MPI_Comm comm) {
}

void ADTOOL_AMPI_popGScommSizeForRootOrNull(int *commSizeForRootOrNull) {
}

void ADTOOL_AMPI_popGSinfo(int commSizeForRootOrNull,
                           void **rbuf,
                           int *rcnt,
                           MPI_Datatype *rtype,
                           void **buf,
                           int *count,
                           MPI_Datatype *type,
                           int *root,
                           MPI_Comm *comm) {
}

void ADTOOL_AMPI_pushGSVinfo(int commSizeForRootOrNull,
                             void *rbuf,
                             int *rcnts,
                             int *displs,
                             MPI_Datatype rtype,
                             void *buf,
                             int  count,
                             MPI_Datatype type,
                             int  root,
                             MPI_Comm comm) {
}

void ADTOOL_AMPI_popGSVinfo(int commSizeForRootOrNull,
                            void **rbuf,
                            int *rcnts,
                            int *displs,
                            MPI_Datatype *rtype,
                            void **buf,
                            int *count,
                            MPI_Datatype *type,
                            int *root,
                            MPI_Comm *comm) {
}

void ADTOOL_AMPI_push_CallCode(enum AMPI_PairedWith_E thisCall) { 
}

void ADTOOL_AMPI_pop_CallCode(enum AMPI_PairedWith_E *thisCall) { 
}

void ADTOOL_AMPI_push_AMPI_Request(struct AMPI_Request_S  *ampiRequest) { 
}

void ADTOOL_AMPI_pop_AMPI_Request(struct AMPI_Request_S  *ampiRequest) { 
}

void ADTOOL_AMPI_push_request(MPI_Request request) { 
} 

MPI_Request ADTOOL_AMPI_pop_request() { 
  return 0;
}

void ADTOOL_AMPI_push_comm(MPI_Comm comm) {
}

MPI_Comm ADTOOL_AMPI_pop_comm() {
  return 0;
}

void* ADTOOL_AMPI_rawData(void* activeData, int *size) { 
  return activeData;
}

void* ADTOOL_AMPI_rawDataV(void* activeData, int *counts, int* displs) {
  return activeData;
}
void * ADTOOL_AMPI_rawData_DType(void* indata, void* outdata, int* count, int idx) {
  return indata;
}
void * ADTOOL_AMPI_unpackDType(void* indata, void* outdata, int* count, int idx) {
  return indata;
}

void * ADTOOL_AMPI_rawAdjointData(void* activeData) { 
  return activeData;
}

void ADTOOL_AMPI_mapBufForAdjoint(struct AMPI_Request_S  *ampiRequest,
				  void* buf) { 
}

void ADTOOL_AMPI_setBufForAdjoint(struct AMPI_Request_S  *ampiRequest,
				  void* buf) { 
  /* do nothing */
}

void ADTOOL_AMPI_getAdjointCount(int *count,
				 MPI_Datatype datatype) { 
}

void ADTOOL_AMPI_setAdjointCount(struct AMPI_Request_S  *ampiRequest) { 
  /* for now we keep the count as is but for example in vector mode one would have to multiply by vector length */
}
void ADTOOL_AMPI_setAdjointCountAndTempBuf(struct AMPI_Request_S *ampiRequest) { 
  ADTOOL_AMPI_setAdjointCount(ampiRequest);
  ampiRequest->adjointTempBuf =
    ADTOOL_AMPI_allocateTempBuf(ampiRequest->adjointCount,
                                ampiRequest->datatype,
                                ampiRequest->comm) ;
  assert(ampiRequest->adjointTempBuf);
}

void* ADTOOL_AMPI_allocateTempBuf(int adjointCount, MPI_Datatype datatype, MPI_Comm comm) {
  size_t s=0;
  int dt_idx = derivedTypeIdx(datatype);
  if (datatype==MPI_DOUBLE)
    s=sizeof(double);
  else if (datatype==MPI_FLOAT)
    s=sizeof(float);
  else if (isDerivedType(dt_idx))
    s = getDTypeData()->p_mapsizes[dt_idx];
  else
    MPI_Abort(comm, MPI_ERR_TYPE);
  return (void*)malloc(adjointCount*s);
}

void ADTOOL_AMPI_releaseAdjointTempBuf(void *tempBuf) { 
  free(tempBuf) ;
}

void ADTOOL_AMPI_adjointIncrement(int adjointCount, MPI_Datatype datatype, MPI_Comm comm, void* target, void* adjointTarget, void* checkAdjointTarget, void *source, void *idx) { 
}

void ADTOOL_AMPI_adjointMultiply(int adjointCount, MPI_Datatype datatype, MPI_Comm comm, void* target, void* adjointTarget, void* checkAdjointTarget, void *source, void *idx) {
}

void ADTOOL_AMPI_adjointDivide(int adjointCount, MPI_Datatype datatype, MPI_Comm comm, void* target, void* adjointTarget, void* checkAdjointTarget, void *source, void *idx) {
}

void ADTOOL_AMPI_adjointEquals(int adjointCount, MPI_Datatype datatype, MPI_Comm comm, void* target, void* adjointTarget, void* checkAdjointTarget, void *source1, void *source2, void *idx) {
}

void ADTOOL_AMPI_adjointNullify(int adjointCount, MPI_Datatype datatype, MPI_Comm comm, void* target, void* adjointTarget, void* checkAdjointTarget) { 
}

void ADTOOL_AMPI_writeData(void *buf,int *count) { }

void ADTOOL_AMPI_writeDataV(void* activeData, int *counts, int* displs) {}

AMPI_Activity ADTOOL_AMPI_isActiveType(MPI_Datatype datatype) {
  return AMPI_PASSIVE;
}

void ADTOOL_AMPI_setupTypes() {};

MPI_Datatype ADTOOL_AMPI_FW_rawType(MPI_Datatype datatype) {
  return datatype;
}

MPI_Datatype ADTOOL_AMPI_BW_rawType(MPI_Datatype datatype) {
  return datatype;
}
