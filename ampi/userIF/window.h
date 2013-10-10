#ifndef _AMPI_WINDOW_H_
#define _AMPI_WINDOW_H_

/**
 * \file 
 * the request in the AMPI context need to be used to track extra information;
 * the extra information cannot be exposed in Fortran77 but there can be a 
 * Fortran90 equivalent and it can be exposed in C to allow source transformation 
 * tools to use TBR analysis on the tracked information.
 */ 

#include "ampi/userIF/libConfig.h"

#define AMPI_WINDOW_STACK_CHUNK_SIZE 1000



typedef struct {
  void *origin_addr;
  int origin_count;
  MPI_Datatype origin_datatype;
  int target_rank;
  MPI_Aint target_disp;
  int target_count;
  MPI_Datatype target_datatype;
  void *idx;
} AMPI_WinRequest;

typedef struct {
    AMPI_WinRequest *v;
    int top;
    size_t size;
} AMPI_Win_stack;

void AMPI_WIN_STACK_push(AMPI_Win_stack *s, AMPI_WinRequest req);
AMPI_WinRequest AMPI_WIN_STACK_pop(AMPI_Win_stack *s);
void AMPI_WIN_STACK_stack_init(AMPI_Win_stack *s);
void AMPI_WIN_STACK_destroy(AMPI_Win_stack *s);
int AMPI_WIN_STACK_full(AMPI_Win_stack *s);
void AMPI_WIN_STACK_expand(AMPI_Win_stack *s);
void AMPI_WIN_STACK_shrink(AMPI_Win_stack *s);
int AMPI_WIN_STACK_empty(AMPI_Win_stack *s);

typedef struct {
  AMPI_Win_stack *req_stack;
  MPI_Win plainWindow;
} AMPI_Win;

/*  *//***/
/** \ref AMPI_Isend / \ref AMPI_Irecv  buf  parameter */
/*   */
/*void *buf;*/

/*  *//***/
/** The corresponding adjoint buffer*/
/*   */
/*void *adjointBuf ;*/

/*  *//***/
/** \ref AMPI_Isend / \ref AMPI_Irecv  count  parameter */
/*   */
/*int count;*/

/*  *//***/
/** \ref AMPI_Isend / \ref AMPI_Irecv  datatype  parameter */
/*   */
/*MPI_Datatype datatype;*/

/*  *//***/
/** \ref AMPI_Isend / \ref AMPI_Irecv  dst or src  parameter */
/*   */
/*int endPoint;*/

/*  *//***/
/** \ref AMPI_Isend / \ref AMPI_Irecv  tag parameter */
/*   */
/*int tag;*/

/*  *//***/
/** \ref AMPI_Isend / \ref AMPI_Irecv sets this*/
/*   */ 
/*enum AMPI_PairedWith_E pairedWith;*/

/*  *//***/
/** \ref AMPI_Isend / \ref AMPI_Irecv  comm  parameter */
/*   */
/*MPI_Comm comm;*/

/*  *//***/
/** temporary adjoint buffer; not to be traced*/
/*   */
/*void *adjointTempBuf;*/

/*  *//***/
/** the count of the adjoint buffer size in terms of the original data type; not to be traced;*/
/*   */
/*int adjointCount;*/

/*  *//***/
/** the "plain" request returned by MPI_Isend or MPI_Irecv resp; not to be traced*/
/*   */  
/*MPI_Request plainRequest;*/

/*  *//***/
/** the "plain" request returned by MPI_Isend or MPI_Irecv resp; */
/*   */  
/*MPI_Request tracedRequest;*/

/*  *//***/
/** \ref AMPI_Isend / \ref AMPI_Irecv sets this*/
/*   */ 
/*enum AMPI_Request_origin_E origin;*/

/*void *idx;*/

/*};*/

/*#ifdef AMPI_FORTRANCOMPATIBLE*/
/*typedef MPI_Request AMPI_Request;*/
/*#else */
/*typedef struct AMPI_Request_S AMPI_Request;*/
/*#endif */

#endif
