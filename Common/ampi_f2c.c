
#include <stdlib.h>
#include <mpi.h>

#include "ampi/userIF/activity.h"
#include "ampi/userIF/pairedWith.h"
#include "ampi/userIF/request.h"
#include "ampi/userIF/nt.h"
#include "ampi/adTool/support.h"
#include "ampi/libCommon/modified.h"
#include "ampi/ampi.h"

AMPI_PairedWith pairedWithTable[] =
  {AMPI_RECV, AMPI_SEND, AMPI_IRECV_WAIT, AMPI_IRECV_WAITALL,
   AMPI_ISEND_WAIT, AMPI_ISEND_WAITALL, AMPI_BSEND, AMPI_RSEND,
   AMPI_ISEND, AMPI_IRECV, AMPI_WAIT} ;

MPI_Fint ampi_adouble_precision_;
MPI_Fint ampi_areal_;

void ampi_init_nt_(int* err_code) {
  /* [llh] Incoherent: why AMPI_Init_NT has "argc,argv" args, whereas fortran MPI_INIT
   * has only one "out" arg for the error code ? */
  *err_code = AMPI_Init_NT(0, 0);
  ampi_adouble_precision_=MPI_DOUBLE_PRECISION;
  ampi_areal_=MPI_REAL;
}

void ampi_finalize_nt_(int* err_code) {
  *err_code = AMPI_Finalize_NT();}

void ampi_comm_rank_(MPI_Fint *commF, int *rank, int* err_code) {
  MPI_Comm commC = MPI_Comm_f2c( *commF ) ;
  *err_code = MPI_Comm_rank(commC, rank);
}

void adtool_ampi_turn_(double *v, double *vb) {
  ADTOOL_AMPI_Turn(v, vb) ;
}

void fw_ampi_recv_(void* buf,
                   int *count,
                   MPI_Fint *datatypeF,
                   int *src,
                   int *tag,
                   int *pairedWithF,
                   int *commF,
                   int *status,
                   int *err_code) {
  MPI_Datatype datatype = MPI_Type_f2c(*datatypeF) ;
  AMPI_PairedWith pairedWith = pairedWithTable[*pairedWithF] ;
  MPI_Comm commC = MPI_Comm_f2c( *commF ) ;
  *err_code = FW_AMPI_Recv(buf, *count, datatype,
                           *src, *tag, pairedWith, commC,
                           (MPI_Status*)status);
}

void bw_ampi_recv_(void* buf,
                   int *count,
                   MPI_Fint *datatypeF,
                   int* src,
                   int* tag,
                   int* pairedWithF,
                   int* commF,
                   int* status,
                   int* err_code) {
  MPI_Datatype datatype = MPI_Type_f2c(*datatypeF) ;
  AMPI_PairedWith pairedWith = pairedWithTable[*pairedWithF] ;
  MPI_Comm commC = MPI_Comm_f2c( *commF ) ;
  *err_code = BW_AMPI_Recv(buf, *count, datatype,
                           *src, *tag, pairedWith, commC,
                           (MPI_Status*)status);
}

void fw_ampi_send_(void* buf, 
                   int *count, 
                   MPI_Fint *datatypeF, 
                   int *dest, 
                   int *tag,
                   int *pairedWithF,
                   int *commF,
                   int *err_code) {
  MPI_Datatype datatype = MPI_Type_f2c(*datatypeF) ;
  AMPI_PairedWith pairedWith = pairedWithTable[*pairedWithF] ;
  MPI_Comm commC = MPI_Comm_f2c( *commF ) ;
  *err_code = FW_AMPI_Send (buf, *count, datatype,
                            *dest, *tag, pairedWith, commC);
}

void bw_ampi_send_(void* buf,
                   int *count,
                   MPI_Fint *datatypeF,
                   int *dest, 
                   int *tag,
                   int *pairedWithF,
                   int *commF,
                   int *err_code) {
  MPI_Datatype datatype = MPI_Type_f2c(*datatypeF) ;
  AMPI_PairedWith pairedWith = pairedWithTable[*pairedWithF] ;
  MPI_Comm commC = MPI_Comm_f2c( *commF ) ;
  *err_code = BW_AMPI_Send (buf, *count, datatype,
                            *dest, *tag, pairedWith, commC);
}

void fw_ampi_irecv_(void* buf,
                    int *count,
                    MPI_Fint *datatypeF,
                    int *source,
                    int *tag,
                    int *pairedWithF,
                    int *commF,
                    int *request,
                    int *err_code){
  MPI_Datatype datatype = MPI_Type_f2c(*datatypeF) ;
  AMPI_PairedWith pairedWith = pairedWithTable[*pairedWithF] ;
  MPI_Comm commC = MPI_Comm_f2c( *commF ) ;
  *err_code = FW_AMPI_Irecv(buf, *count, datatype,
                            *source, *tag, pairedWith, commC,
                            (MPI_Request*)request);
}

void bw_ampi_irecv_(void* buf,
                    int *count,
                    MPI_Fint *datatypeF,
                    int *source,
                    int *tag,
                    int *pairedWithF,
                    int *commF,
                    int *request,
                    int *err_code) {
  MPI_Datatype datatype = MPI_Type_f2c(*datatypeF) ;
  AMPI_PairedWith pairedWith = pairedWithTable[*pairedWithF] ;
  MPI_Comm commC = MPI_Comm_f2c( *commF ) ;
  *err_code = BW_AMPI_Irecv(buf, *count, datatype,
                            *source, *tag, pairedWith, commC,
                            (MPI_Request*)request);
}

void fw_ampi_isend_(void* buf,
                    int *count,
                    MPI_Fint *datatypeF,
                    int *dest,
                    int *tag,
                    int *pairedWithF,
                    int *commF,
                    int *request,
                    int *err_code) {
  MPI_Datatype datatype = MPI_Type_f2c(*datatypeF) ;
  AMPI_PairedWith pairedWith = pairedWithTable[*pairedWithF] ;
  MPI_Comm commC = MPI_Comm_f2c( *commF ) ;
  *err_code = FW_AMPI_Isend(buf, *count, datatype,
                            *dest, *tag, pairedWith, commC,
                            (MPI_Request*)request);
}

void bw_ampi_isend_(void* buf,
                    int *count,
                    MPI_Fint *datatypeF,
                    int *dest,
                    int *tag,
                    int *pairedWithF,
                    int *commF,
                    int *request,
                    int *err_code) {
  MPI_Datatype datatype = MPI_Type_f2c(*datatypeF) ;
  AMPI_PairedWith pairedWith = pairedWithTable[*pairedWithF] ;
  MPI_Comm commC = MPI_Comm_f2c( *commF ) ;
  *err_code = BW_AMPI_Isend(buf, *count, datatype,
                            *dest, *tag, pairedWith, commC,
                            (MPI_Request*)request);
}

void fw_ampi_wait_(int *request, int *status, int* err_code) {
  *err_code = FW_AMPI_Wait((MPI_Request*)request,
                           (MPI_Status*)status);
}

void bw_ampi_wait_(int *request, int *status, int* err_code) {
  *err_code = BW_AMPI_Wait((MPI_Request*)request,
                           (MPI_Status*)status);
}
