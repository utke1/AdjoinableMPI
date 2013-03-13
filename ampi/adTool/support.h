#ifndef _AMPI_ADTOOL_SUPPORT_H_
#define _AMPI_ADTOOL_SUPPORT_H_

#include <mpi.h>
#if defined(__cplusplus)
extern "C" {
#endif

#include "ampi/userIF/request.h"

/**
 * \file 
 * \brief methods that an AD tool needs to implement in order to use the implementation in Common
 */ 

/**
 * the implementation of pushing the required elements for send/recv
 * to the AD-tool-internal stack;
 * For souce transformation this may remain unimplemented provided all the parameters
 * are recovered by TBR and <tt>buf</tt> is mapped explicitly.
 * the operator overloading implementation maps <tt>buf</tt> to the adjoint address space.
 * The source transformation implementation ignores <tt>buf</tt> 
 */
void ADTOOL_AMPI_pushSRinfo(void* buf, 
			    int count,
			    MPI_Datatype datatype, 
			    int src, 
			    int tag,
			    enum AMPI_PairedWith_E pairedWith,
			    MPI_Comm comm);

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
			   enum AMPI_PairedWith_E* pairedWith,
			   MPI_Comm* comm,
			   void **idx);

/**
 * the implementation of pushing an operation code to the 
 * to the AD-tool-internal stack for an operator overloading tool;
 * the source transformation implementation will leave this empty;
 * this method is called in the respective <tt>FW_</tt>  variant 
 * implemented in <tt>Common</tt>
 */
void ADTOOL_AMPI_push_CallCode(enum AMPI_PairedWith_E thisCall);

/**
 * the implementation of popping an operation code from the 
 * to the AD-tool-internal stack for an operator overloading tool;
 * See comments of \ref ADTOOL_AMPI_push_CallCode.
 * the operator overloading tool needs to pop the code from its operation 
 * stack first and then call (with dummy parameters) the respect <tt>BW_</tt>
 * variant of the operatiorn represented by <tt>thisCall</tt> 
 */
void ADTOOL_AMPI_pop_CallCode(enum AMPI_PairedWith_E *thisCall);

/**
 * the implementation of pushing the required elements of an \ref  AMPI_Request_S
 * to the AD-tool-internal stack
 */
void ADTOOL_AMPI_push_AMPI_Request(struct AMPI_Request_S  *ampiRequest);

/**
 * the implementation of popping the required elements of an \ref  AMPI_Request_S
 * from the AD-tool-internal stack
 */
void ADTOOL_AMPI_pop_AMPI_Request(struct AMPI_Request_S  *ampiRequest);
  
/** Push the MPI_Request on the AD tool internal stack.
 * This is used as a key to the request bookkeeping 
to keep correspondence between the request Id of the FW sweep
 * to the request Id in BW sweep.
 * if we need to trace requests for a pure (operator overloading) trace evaluation
 * the Common implementation uses this to push the request 
 */
void ADTOOL_AMPI_push_request(MPI_Request request);

/**
 * the companion to \ref ADTOOL_AMPI_push_request
 */
MPI_Request ADTOOL_AMPI_pop_request();

/**
 * map active data to raw data; this is to be implemented for the forward 
 * execution by tools using association-by-address; 
 * for tools using association-by-name the same address should be returned;   
 */
void * ADTOOL_AMPI_rawData(void* activeData, int *size);
void ADTOOL_AMPI_writeData(void* activeData, int *size);

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
 * Adjoint nullify the values in adjointTarget.
 * \param adjointCount is the number of items in the buffer we will nullify
 * \param datatype the data type of the buffer to be nullified
 * \param comm the communicator to be passed to MPI_Abort for failures
 * \param target the buffer that comes from the fwd sweep.
 * \param adjointTarget the adjoint buffer to be nullified
 * \param checkAdjointTarget the adjoint buffer that comes from the bwd sweep. For runtime checking only.
 */ 
void ADTOOL_AMPI_adjointNullify(int adjointCount, MPI_Datatype datatype, MPI_Comm comm, void* target, void* adjointTarget, void* checkAdjointTarget);

#if defined(__cplusplus)
}
#endif

#endif

