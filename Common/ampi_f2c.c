
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

AMPI_Activity activityTable[] =
  {AMPI_PASSIVE, AMPI_ACTIVE} ;

MPI_Datatype datatypeTable[] =
  {NULL,
   MPI_BYTE,
   MPI_PACKED,
   MPI_UB,
   MPI_LB,
   MPI_CHARACTER,
   MPI_LOGICAL,
   MPI_INTEGER,
   MPI_INTEGER1,
   MPI_INTEGER2,
   MPI_INTEGER4,
   MPI_INTEGER8,
   NULL,/* <== PATCH!! MPI_INTEGER16*/
   MPI_REAL,
   MPI_REAL4,
   MPI_REAL8,
   MPI_REAL16,
   MPI_DOUBLE,/* <== PATCH!! MPI_DOUBLE_PRECISION*/
   MPI_COMPLEX,
   MPI_COMPLEX8,
   MPI_COMPLEX16,
   MPI_COMPLEX32,
   MPI_DOUBLE_COMPLEX,
   MPI_2REAL,
   MPI_2DOUBLE_PRECISION,
   MPI_2INTEGER,
   NULL,/* <== PATCH!! MPI_2COMPLEX */
   NULL,/* <== PATCH!! MPI_2DOUBLE_COMPLEX */
   NULL,/* <== PATCH!! MPI_REAL2*/
   NULL,/* <== PATCH!! MPI_LOGICAL1 */
   NULL,/* <== PATCH!! MPI_LOGICAL2 */
   NULL,/* <== PATCH!! MPI_LOGICAL4 */
   NULL,/* <== PATCH!! MPI_LOGICAL8 */
   MPI_WCHAR,
   MPI_CHAR,
   MPI_UNSIGNED_CHAR,
   MPI_SIGNED_CHAR,
   MPI_SHORT,
   MPI_UNSIGNED_SHORT,
   MPI_INT,
   MPI_UNSIGNED,
   MPI_LONG,
   MPI_UNSIGNED_LONG,
   MPI_LONG_LONG_INT,
   MPI_UNSIGNED_LONG_LONG,
   MPI_FLOAT,
   MPI_DOUBLE,
   MPI_LONG_DOUBLE,
   MPI_FLOAT_INT,
   MPI_DOUBLE_INT,
   NULL,/* <== PATCH!! MPI_LONGDBL_INT*/
   MPI_LONG_INT,
   MPI_2INT,
   MPI_SHORT_INT,
   NULL,/* <== PATCH!! MPI_CXX_BOOL*/
   NULL,/* <== PATCH!! MPI_CXX_CPLEX*/
   NULL,/* <== PATCH!! MPI_CXX_DBLCPLEX*/
   NULL,/* <== PATCH!! MPI_CXX_LDBLCPLEX*/
   MPI_INT8_T,
   MPI_UINT8_T,
   MPI_INT16_T,
   MPI_UINT16_T,
   MPI_INT32_T,
   MPI_UINT32_T,
   MPI_INT64_T,
   MPI_UINT64_T,
   MPI_AINT,
   MPI_OFFSET} ;


void ampi_init_nt_(int* err_code) {
  /* [llh] Incoherent: why AMPI_Init_NT has "argc,argv" args, whereas fortran MPI_INIT
   * has only one "out" arg for the error code ? */
  *err_code = AMPI_Init_NT(0, 0);
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
                   int *datatypeF,
                   int *isActiveF,
                   int *src,
                   int *tag,
                   int *pairedWithF,
                   int *commF,
                   int *status,
                   int *err_code) {
  MPI_Datatype datatype = datatypeTable[*datatypeF] ;
  AMPI_Activity isActive = activityTable[*isActiveF] ;
  AMPI_PairedWith pairedWith = pairedWithTable[*pairedWithF] ;
  MPI_Comm commC = MPI_Comm_f2c( *commF ) ;
  *err_code = FW_AMPI_Recv(buf, *count, datatype, isActive,
                           *src, *tag, pairedWith, commC,
                           (MPI_Status*)status);
}

void bw_ampi_recv_(void* buf,
                   int *count,
                   int *datatypeF,
                   int* isActiveF,
                   int* src,
                   int* tag,
                   int* pairedWithF,
                   int* commF,
                   int* status,
                   int* err_code) {
  MPI_Datatype datatype = datatypeTable[*datatypeF] ;
  AMPI_Activity isActive = activityTable[*isActiveF] ;
  AMPI_PairedWith pairedWith = pairedWithTable[*pairedWithF] ;
  MPI_Comm commC = MPI_Comm_f2c( *commF ) ;
  *err_code = BW_AMPI_Recv(buf, *count, datatype, isActive,
                           *src, *tag, pairedWith, commC,
                           (MPI_Status*)status);
}

void fw_ampi_send_(void* buf, 
                   int *count, 
                   int *datatypeF, 
                   int *isActiveF,
                   int *dest, 
                   int *tag,
                   int *pairedWithF,
                   int *commF,
                   int *err_code) {
  MPI_Datatype datatype = datatypeTable[*datatypeF] ;
  AMPI_Activity isActive = activityTable[*isActiveF] ;
  AMPI_PairedWith pairedWith = pairedWithTable[*pairedWithF] ;
  MPI_Comm commC = MPI_Comm_f2c( *commF ) ;
  *err_code = FW_AMPI_Send (buf, *count, datatype, isActive,
                            *dest, *tag, pairedWith, commC);
}

void bw_ampi_send_(void* buf,
                   int *count,
                   int *datatypeF,
                   int *isActiveF,
                   int *dest, 
                   int *tag,
                   int *pairedWithF,
                   int *commF,
                   int *err_code) {
  MPI_Datatype datatype = datatypeTable[*datatypeF] ;
  AMPI_Activity isActive = activityTable[*isActiveF] ;
  AMPI_PairedWith pairedWith = pairedWithTable[*pairedWithF] ;
  MPI_Comm commC = MPI_Comm_f2c( *commF ) ;
  *err_code = BW_AMPI_Send (buf, *count, datatype, isActive,
                            *dest, *tag, pairedWith, commC);
}

void fw_ampi_irecv_(void* buf,
                    int *count,
                    int *datatypeF,
                    int *isActiveF,
                    int *source,
                    int *tag,
                    int *pairedWithF,
                    int *commF,
                    int *request,
                    int *err_code){
  MPI_Datatype datatype = datatypeTable[*datatypeF] ;
  AMPI_Activity isActive = activityTable[*isActiveF] ;
  AMPI_PairedWith pairedWith = pairedWithTable[*pairedWithF] ;
  MPI_Comm commC = MPI_Comm_f2c( *commF ) ;
  *err_code = FW_AMPI_Irecv(buf, *count, datatype, isActive,
                            *source, *tag, pairedWith, commC,
                            (MPI_Request*)request);
}

void bw_ampi_irecv_(void* buf,
                    int *count,
                    int *datatypeF,
                    int *isActiveF,
                    int *source,
                    int *tag,
                    int *pairedWithF,
                    int *commF,
                    int *request,
                    int *err_code) {
  MPI_Datatype datatype = datatypeTable[*datatypeF] ;
  AMPI_Activity isActive = activityTable[*isActiveF] ;
  AMPI_PairedWith pairedWith = pairedWithTable[*pairedWithF] ;
  MPI_Comm commC = MPI_Comm_f2c( *commF ) ;
  *err_code = BW_AMPI_Irecv(buf, *count, datatype, isActive,
                            *source, *tag, pairedWith, commC,
                            (MPI_Request*)request);
}

void fw_ampi_isend_(void* buf,
                    int *count,
                    int *datatypeF,
                    int *isActiveF,
                    int *dest,
                    int *tag,
                    int *pairedWithF,
                    int *commF,
                    int *request,
                    int *err_code) {
  MPI_Datatype datatype = datatypeTable[*datatypeF] ;
  AMPI_Activity isActive = activityTable[*isActiveF] ;
  AMPI_PairedWith pairedWith = pairedWithTable[*pairedWithF] ;
  MPI_Comm commC = MPI_Comm_f2c( *commF ) ;
  *err_code = FW_AMPI_Isend(buf, *count, datatype, isActive,
                            *dest, *tag, pairedWith, commC,
                            (MPI_Request*)request);
}

void bw_ampi_isend_(void* buf,
                    int *count,
                    int *datatypeF,
                    int *isActiveF,
                    int *dest,
                    int *tag,
                    int *pairedWithF,
                    int *commF,
                    int *request,
                    int *err_code) {
  MPI_Datatype datatype = datatypeTable[*datatypeF] ;
  AMPI_Activity isActive = activityTable[*isActiveF] ;
  AMPI_PairedWith pairedWith = pairedWithTable[*pairedWithF] ;
  MPI_Comm commC = MPI_Comm_f2c( *commF ) ;
  *err_code = BW_AMPI_Isend(buf, *count, datatype, isActive,
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
