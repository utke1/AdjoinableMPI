#ifndef _AMPI_ADTOOL_SUPPORT_H_
#define _AMPI_ADTOOL_SUPPORT_H_

#include <mpi.h>
#if defined(__cplusplus)
extern "C" {
#endif

#include "ampi/userIF/request.h"
#include "ampi/userIF/activity.h"
#include "ampi/userIF/modified.h"

/**
 * \file 
 * \brief methods that an AD tool needs to implement in order to use the implementation in Common
 */ 

/**
 * The implementation of pushing the required elements for Bcast calls.
 * Might rework for conciseness. Wrote this to avoid pushing too much stuff with _pushSRinfo.
 */
void ADTOOL_AMPI_pushBcastInfo(void* buf,
			       int count,
			       MPI_Datatype datatype,
			       int root,
			       MPI_Comm comm);
typedef void (ADTOOL_AMPI_pushBcastInfoF) (void*,int, MPI_Datatype, int, MPI_Comm);


/**
 * Popping the required elements for Bcast calls.
 */
void ADTOOL_AMPI_popBcastInfo(void** buf,
			      int* count,
			      MPI_Datatype* datatype,
			      int* root,
			      MPI_Comm* comm,
			      void **idx);
typedef void (ADTOOL_AMPI_popBcastInfoF) (void**, int*, MPI_Datatype*, int*, MPI_Comm*, void**);


/**
 * Pushing and popping a block of double values, specifically for reduction results.
 */
void ADTOOL_AMPI_pushDoubleArray(void* buf,
				 int count);
typedef void (ADTOOL_AMPI_pushDoubleArrayF) (void*, int);

void ADTOOL_AMPI_popDoubleArray(double* buf,
				int* count);
typedef void (ADTOOL_AMPI_popDoubleArrayF) (double*, int*);

/**
 * The implementation of pushing the required elements for Reduce calls.
 * Might rework for conciseness. Note that we require a separate TAPE_AMPI_push_MPI_Op
 * function to push the reduce operation. I defined _push_MPI_Op in
 * AdjoinableMPI/Tape/support.c w/ header AdjoinableMPI/ampi/tape/support.h.
 */
void ADTOOL_AMPI_pushReduceInfo(void* sbuf,
				void* rbuf,
				void* resultData,
				int pushResultData,
				int count,
				MPI_Datatype datatype,
				MPI_Op op,
				int root,
				MPI_Comm comm);
typedef void (ADTOOL_AMPI_pushReduceInfoF) (void*, void*, void*, int, int, MPI_Datatype, MPI_Op, int, MPI_Comm);

/**
 * Popping the required elements for Reduce calls.
 */
void ADTOOL_AMPI_popReduceCountAndType(int* count,
				       MPI_Datatype* datatype);
typedef void (ADTOOL_AMPI_popReduceCountAndTypeF) (int*, MPI_Datatype*);

void ADTOOL_AMPI_popReduceInfo(void** sbuf,
			       void** rbuf,
			       void** prevData,
			       void** resultData,
			       int* count,
			       MPI_Op* op,
			       int* root,
			       MPI_Comm* comm,
			       void **idx);
typedef void (ADTOOL_AMPI_popReduceInfoF) (void**, void**, void**, void**, int*, MPI_Op*, int*, MPI_Comm*, void **);


/**
 * the implementation of pushing the required elements for send/recv
 * to the AD-tool-internal stack;
 * For source transformation this may remain unimplemented provided all the parameters
 * are recovered by TBR and <tt>buf</tt> is mapped explicitly.
 * the operator overloading implementation maps <tt>buf</tt> to the adjoint address space.
 * The source transformation implementation ignores <tt>buf</tt> 
 */
void ADTOOL_AMPI_pushSRinfo(void* buf, 
			    int count,
			    MPI_Datatype datatype, 
			    int src, 
			    int tag,
			    AMPI_PairedWith pairedWith,
			    MPI_Comm comm);
typedef void (ADTOOL_AMPI_pushSRinfoF) (void*, int, MPI_Datatype, int, int, AMPI_PairedWith, MPI_Comm);

/**
 * the implementation of popping the required elements for send/recv
 * from the AD-tool-internal stack;
 * See comments of \ref ADTOOL_AMPI_pushSRinfo.
 */
void ADTOOL_AMPI_popSRinfo(void** buf,
			   int* count,
			   MPI_Datatype* datatype, 
			   int* src, 
			   int* tag,
			   AMPI_PairedWith* pairedWith,
			   MPI_Comm* comm,
			   void **idx);
typedef void (ADTOOL_AMPI_popSRinfoF) (void**, int*, MPI_Datatype*, int*, int*, AMPI_PairedWith*, MPI_Comm*, void**);

/**
 * the implementation of pushing the required elements for gather/scatter
 * to the AD-tool-internal stack;
 * the implementation rationale follows \ref  ADTOOL_AMPI_pushSRinfo
 * NOTE: for non-root ranks the root specific parameters are ignored
 * which implies in particular that the pointers passed may not be valid
 * therefore we use commSizeForRootOrNull to discriminate
 * \param commSizeForRootOrNull is the communicator size for rank root or 0
 * \param rbuf the buffer on rank root
 * \param rcnt the count on rank root 
 * \param rtype the data type on rank root
 * \param buf the buffer on non-root ranks
 * \param count the counter for buf on non-root ranks
 * \param type the data type on non-root ranks
 * \param root the root rank
 * \param comm the communicator
 */
void ADTOOL_AMPI_pushGSinfo(int commSizeForRootOrNull,
			    void *rbuf,
			    int rcnt,
			    MPI_Datatype rtype,
			    void *buf,
			    int count,
			    MPI_Datatype type,
			    int  root,
			    MPI_Comm comm);
typedef void (ADTOOL_AMPI_pushGSinfoF) (int, void*, int, MPI_Datatype, void*, int, MPI_Datatype, int, MPI_Comm);

/**
 * this must be called before \ref ADTOOL_AMPI_popGSinfo and \ref ADTOOL_AMPI_popGSVinfo
 * \param commSizeForRootOrNull this is popped so that we may allocate buffers for
 * rcnts and displs in the subsequent call to \ref ADTOOL_AMPI_popGSVinfo
 */
void ADTOOL_AMPI_popGScommSizeForRootOrNull(int *commSizeForRootOrNull);
typedef void (ADTOOL_AMPI_popGScommSizeForRootOrNullF) (int*);

/**
 * the implementation of popping the required elements for gather/scatter
 * from the AD-tool-internal stack;
 * see comments of \ref ADTOOL_AMPI_pushGSinfo;
 * following the note there we will not be setting the values for root specific
 * arguments on non-root ranks
 * \param commSizeForRootOrNull retrieved via \ref ADTOOL_AMPI_popGSVcommSizeForRootOrNull
 * \param rbuf the buffer on rank rook, set if commSizeForRootOrNull>0
 * \param rcnt the size  for rank root, set if commSizeForRootOrNull>0
 * \param rtype the data type for rank root, set if commSizeForRootOrNull>0
 * \param buf the buffer for all ranks
 * \param count the count for all ranks
 * \param type the type for all ranks
 * \param root the root rank
 * \param comm the communicator for all ranks
 */
void ADTOOL_AMPI_popGSinfo(int commSizeForRootOrNull,
			   void **rbuf,
			   int *rcnt,
			   MPI_Datatype *rtype,
			   void **buf,
			   int *count,
			   MPI_Datatype *type,
			   int *root,
			   MPI_Comm *comm);
typedef void (ADTOOL_AMPI_popGSinfoF) (int, void**, int*, MPI_Datatype*, void**, int*, MPI_Datatype*, int*, MPI_Comm*);

/**
 * the implementation of pushing the required elements for gatherv/scatterv
 * to the AD-tool-internal stack;
 * the implementation rationale follows \ref  ADTOOL_AMPI_pushSRinfo
 * NOTE: for non-root ranks the root specific parameters are ignored
 * which implies in particular that the pointers passed may not be valid
 * therefore we use commSizeForRootOrNull to discriminate
 * \param commSizeForRootOrNull is the communicator size for rank root or 0
 * \param rbuf the buffer on rank root
 * \param rcnts the counters per rank on rank root
 * \param displs the displacements for rbuf on rank root
 * \param rtype the data type on rank root
 * \param buf the buffer on non-root ranks
 * \param count the counter for buf on non-root ranks
 * \param type the data type on non-root ranks
 * \param root the root rank
 * \param comm the communicator
 */
void ADTOOL_AMPI_pushGSVinfo(int commSizeForRootOrNull,
                             void *rbuf,
                             int *rcnts,
                             int *displs,
                             MPI_Datatype rtype,
                             void *buf,
                             int  count,
                             MPI_Datatype type,
                             int  root,
                             MPI_Comm comm);
typedef void (ADTOOL_AMPI_pushGSVinfoF) (int, void*, int*, int*, MPI_Datatype, void*, int, MPI_Datatype, int, MPI_Comm);

/**
 * the implementation of popping the required elements for gatherv/scatterv
 * from the AD-tool-internal stack;
 * see comments of \ref ADTOOL_AMPI_pushGSVinfo;
 * following the note there we will not be setting the values for root specific
 * arguments on non-root ranks
 * \param commSizeForRootOrNull retrieved via \ref ADTOOL_AMPI_popGSVcommSizeForRootOrNull
 * \param rbuf the buffer on rank rook, set if commSizeForRootOrNull>0
 * \param rcnts the array of size commSizeForRootOrNull for rank root, set if commSizeForRootOrNull>0
 * \param displs the array of size commSizeForRootOrNull  for rank root, set if commSizeForRootOrNull>0
 * \param rtype the data type for rank root, set if commSizeForRootOrNull>0
 * \param buf the buffer for all ranks
 * \param count the count for all ranks
 * \param type the type for all ranks
 * \param root the root rank
 * \param comm the communicator for all ranks
 */
void ADTOOL_AMPI_popGSVinfo(int commSizeForRootOrNull,
                            void **rbuf,
                            int *rcnts,
                            int *displs,
                            MPI_Datatype *rtype,
                            void **buf,
                            int *count,
                            MPI_Datatype *type,
                            int *root,
                            MPI_Comm *comm);
typedef void (ADTOOL_AMPI_popGSVinfoF) (int, void**, int*, int*, MPI_Datatype*, void**, int*, MPI_Datatype*, int*, MPI_Comm*);

/**
 * the implementation of pushing an operation code to the 
 * to the AD-tool-internal stack for an operator overloading tool;
 * the source transformation implementation will leave this empty;
 * this method is called in the respective <tt>FW_</tt>  variant 
 * implemented in <tt>Common</tt>
 */
void ADTOOL_AMPI_push_CallCode(enum AMPI_PairedWith_E thisCall);
typedef void (ADTOOL_AMPI_push_CallCodeF) (enum AMPI_PairedWith_E);

/**
 * the implementation of popping an operation code from the 
 * to the AD-tool-internal stack for an operator overloading tool;
 * See comments of \ref ADTOOL_AMPI_push_CallCode.
 * the operator overloading tool needs to pop the code from its operation 
 * stack first and then call (with dummy parameters) the respect <tt>BW_</tt>
 * variant of the operatiorn represented by <tt>thisCall</tt> 
 */
void ADTOOL_AMPI_pop_CallCode(enum AMPI_PairedWith_E *thisCall);
typedef void (ADTOOL_AMPI_pop_CallCodeF) (enum AMPI_PairedWith_E*);
  


/**
 * the implementation of pushing the required elements of an \ref  AMPI_Request_S
 * to the AD-tool-internal stack
 */
void ADTOOL_AMPI_push_AMPI_Request(struct AMPI_Request_S  *ampiRequest);
typedef void (ADTOOL_AMPI_push_AMPI_RequestF) (struct AMPI_Request_S*);

/**
 * the implementation of popping the required elements of an \ref  AMPI_Request_S
 * from the AD-tool-internal stack
 */
void ADTOOL_AMPI_pop_AMPI_Request(struct AMPI_Request_S  *ampiRequest);
typedef void (ADTOOL_AMPI_pop_AMPI_RequestF) (struct AMPI_Request_S*);

  
/** 
 * Push the MPI_Request on the AD tool internal stack.
 * This is used as a key to the request bookkeeping 
 * to keep correspondence between the request Id of the FW sweep
 * to the request Id in BW sweep.
 * if we need to trace requests for a pure (operator overloading) trace evaluation
 * the Common implementation uses this to push the request 
 */
void ADTOOL_AMPI_push_request(MPI_Request request);
typedef void (ADTOOL_AMPI_push_requestF) (MPI_Request);

/**
 * the companion to \ref ADTOOL_AMPI_push_request
 */
MPI_Request ADTOOL_AMPI_pop_request();
typedef MPI_Request (ADTOOL_AMPI_pop_requestF) ();

void ADTOOL_AMPI_push_comm(MPI_Comm comm);

/**
 * the companion to \ref ADTOOL_AMPI_push_request
 */
MPI_Comm ADTOOL_AMPI_pop_comm();

/**
 * map active data to raw data; this is to be implemented for the forward 
 * execution by tools using association-by-address; 
 * for tools using association-by-name the same address should be returned;   
 */
void * ADTOOL_AMPI_rawData(void* activeData, int *size);

/**
 * map active data to raw data; functionality similar to \ref ADTOOL_AMPI_rawData
 * except it is handling vector buffers with arrays of counts and displacements as
 * used in \ref MPI_Gatherv or \ref MPI_Scatterv
 */
void * ADTOOL_AMPI_rawDataV(void* activeData, int *counts, int* displs);

/**
 * serialize user-defined struct for sending in forward execution in
 * association-by-address tools
 */
void * ADTOOL_AMPI_rawData_DType(void* indata, void* outdata, int* count, int idx);

/**
 * unpack serialized user-defined struct data into its original form
 */
void * ADTOOL_AMPI_unpackDType(void* indata, void* outdata, int* count, int idx);

/** \todo add description
 *
 */
void ADTOOL_AMPI_writeData(void* activeData, int *size);

/** \todo add description
 *
 */
void ADTOOL_AMPI_writeDataV(void* activeData, int *counts, int* displs);

/**
 * map active data to adjoint data; this is to be implemented for the backward
 * execution by tools using association-by-address; 
 * for tools using association-by-name the same address should be returned;   
 */
void * ADTOOL_AMPI_rawAdjointData(void* activeData);

/**
 * Declares correspondence between a buffer and its counterpart adjoint buffer
 * Adds correspondence into the request-to-buffer association list
 * This is necessary for association-by-name transfo tools.
 * should be done upon turn from FW sweep to BW sweep.
 * \param buf the original, non-differentiated buffer.
 * \param adjointBuf the corresponding adjoint buffer.
 */
void ADTOOL_AMPI_Turn(void* buf, void* adjointBuf) ;

/**
 * set it on the request; 
 * \param buf is forward sweep buffer (for source transformation tools)
 * \param ampiRequest is the request to be pushed and popped for the adjoint communication
 */
void ADTOOL_AMPI_mapBufForAdjoint(struct AMPI_Request_S  *ampiRequest,
				  void* buf);

/**
 * an operator overloading tool should not do anything in the implementation but see \ref ADTOOL_AMPI_mapBufForAdjoint;
 * a source transformation tool would receive the adjoint buffer as an argument
 * and set it on the request; 
 * \param buf is the adjoint buffer (for source transformation tools)
 * \param ampiRequest is the request to be used during the adjoint communication
 */
void ADTOOL_AMPI_setBufForAdjoint(struct AMPI_Request_S  *ampiRequest,
				  void* buf);

/**  
 * this method resets \param count member to represent the buffer in terms of 
 * elements of the original MPI \param datatype; this is of particular interest for vector mode; 
 * \todo needs to be expanded for the case where the adjoint data is not represented by the same MPI data type as the 
 * original program data
 */
void ADTOOL_AMPI_getAdjointCount(int *count,
				 MPI_Datatype datatype);

/**  
 * \param ampiRequest in this instance this method resets the <tt>adjointCount</tt> member; 
 */
void ADTOOL_AMPI_setAdjointCount(struct AMPI_Request_S  *ampiRequest);

/**  
 * calls \ref ADTOOL_AMPI_setAdjointCount and sets up a temporary buffer into which the adjoint data is received, see e.g. \ref BW_AMPI_Wait
 * \param ampiRequest is the request instance that is modified
 */
void ADTOOL_AMPI_setAdjointCountAndTempBuf(struct AMPI_Request_S *ampiRequest);

/**
 * Allocates a temporary buffer needed to receive adjoint
 * data before adding it to the adjoint variable
 */
void* ADTOOL_AMPI_allocateTempBuf(int adjointCount, MPI_Datatype dataType, MPI_Comm comm) ;

/**  
 * releases the temporary buffer (allocated by \ref ADTOOL_AMPI_setAdjointCountAndTempBuf)  into which the adjoint data was received 
 */
void ADTOOL_AMPI_releaseAdjointTempBuf(void *tempBuf);

/**
 * Adjoint increment the values in adjointTarget.
 * \param adjointCount is the number of items in the buffer we will increment
 * \param datatype the data type of the buffer to be incremented
 * \param comm the communicator to be passed to MPI_Abort for failures
 * \param target the buffer that comes from the fwd sweep.
 * \param adjointTarget the adjoint buffer to be incremented
 * \param checkAdjointTarget the adjoint buffer that comes from the bwd sweep. For runtime checking only.
 * \param source the adjoint value that must be added into the adjoint buffer.
 * \param idx tape index for each element of the non contiguous buffer
 */
void ADTOOL_AMPI_adjointIncrement(int adjointCount, MPI_Datatype datatype, MPI_Comm comm, void* target, void* adjointTarget, void* checkAdjointTarget, void *source, void *idx);
/**
 * Adjoint multiply the values in adjointTarget by source.
 */
void ADTOOL_AMPI_adjointMultiply(int adjointCount, MPI_Datatype datatype, MPI_Comm comm, void* target, void* adjointTarget, void* checkAdjointTarget, void *source, void *idx);
/**
 * Adjoint divide the values in adjointTarget by source.
 */
void ADTOOL_AMPI_adjointDivide(int adjointCount, MPI_Datatype datatype, MPI_Comm comm, void* target, void* adjointTarget, void* checkAdjointTarget, void *source, void *idx);

/**
 * Return equality result between the values in adjointTarget and source.
 */
void ADTOOL_AMPI_adjointEquals(int adjointCount, MPI_Datatype datatype, MPI_Comm comm, void* target, void* adjointTarget, void* checkAdjointTarget, void *source1, void *source2, void *idx);

/**
 * Adjoint nullify the values in adjointTarget.
 * \param adjointCount is the number of items in the buffer we will nullify
 * \param datatype the data type of the buffer to be nullified
 * \param comm the communicator to be passed to MPI_Abort for failures
 * \param target the buffer that comes from the fwd sweep.
 * \param adjointTarget the adjoint buffer to be nullified
 * \param checkAdjointTarget the adjoint buffer that comes from the bwd sweep. For runtime checking only.
 */ 
void ADTOOL_AMPI_adjointNullify(int adjointCount, MPI_Datatype datatype, MPI_Comm comm, void* target, void* adjointTarget, void* checkAdjointTarget);

/**
 * initialize predefined active types
 */
void ADTOOL_AMPI_setupTypes();

/**
 * Take datatype for forward mode, return datatype for transfer.
 */
MPI_Datatype ADTOOL_AMPI_FW_rawType(MPI_Datatype datatype);

/**
 * Take datatype for reverse mode, return datatype for transfer.
 */
MPI_Datatype ADTOOL_AMPI_BW_rawType(MPI_Datatype datatype);

#ifdef AMPI_FORTRANCOMPATIBLE
/**
 * fortran routine to figure out what the proper types are on the fortran side
 * \param adouble returns the integer representation for the fortran version of AMPI_ADOUBLE_PRECISION
 * \param real returns the integer representation for the fortran version of AMPI_AREAL
 */
void adtool_ampi_fortransetuptypes_(MPI_Fint* adouble, MPI_Fint* areal);
#endif

/**
 * test types for activity
 * \param datatype any data type but particularly also the active data type(s) created by the tool (see \ref AMPI_ADOUBLE etc.)
 * \returns the respective enum value based on the type's activity
 */
AMPI_Activity ADTOOL_AMPI_isActiveType(MPI_Datatype datatype);


struct ADTOOL_AMPI_FPCollection{
  ADTOOL_AMPI_pushBcastInfoF *pushBcastInfo_fp;
  ADTOOL_AMPI_popBcastInfoF *popBcastInfo_fp;
  ADTOOL_AMPI_pushDoubleArrayF *pushDoubleArray_fp;
  ADTOOL_AMPI_popDoubleArrayF *popDoubleArray_fp;
  ADTOOL_AMPI_pushReduceInfoF *pushReduceInfo_fp;
  ADTOOL_AMPI_popReduceCountAndTypeF *popReduceCountAndType_fp;
  ADTOOL_AMPI_popReduceInfoF *popReduceInfo_fp;
  ADTOOL_AMPI_pushSRinfoF *pushSRinfo_fp;
  ADTOOL_AMPI_popSRinfoF *popSRinfo_fp;
  ADTOOL_AMPI_pushGSinfoF *pushGSinfo_fp;
  ADTOOL_AMPI_popGScommSizeForRootOrNullF *popGScommSizeForRootOrNull_fp;
  ADTOOL_AMPI_popGSinfoF *popGSinfo_fp;
  ADTOOL_AMPI_pushGSVinfoF *pushGSVinfo_fp;
  ADTOOL_AMPI_popGSVinfoF *popGSVinfo_fp;
  ADTOOL_AMPI_push_CallCodeF *push_CallCode_fp;
  ADTOOL_AMPI_pop_CallCodeF *pop_CallCode_fp;
  ADTOOL_AMPI_push_AMPI_RequestF *push_AMPI_Request_fp;
  ADTOOL_AMPI_pop_AMPI_RequestF *pop_AMPI_Request_fp;
  ADTOOL_AMPI_push_requestF *push_request_fp;
  ADTOOL_AMPI_pop_requestF *pop_request_fp;
};

/**
 * the single instance of ADTOOL_AMPI_FPCollection
 */
extern struct ADTOOL_AMPI_FPCollection ourADTOOL_AMPI_FPCollection;


#if defined(__cplusplus)
}
#endif

#endif

