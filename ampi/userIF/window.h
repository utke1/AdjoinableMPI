#ifndef _AMPI_WINDOW_H_
#define _AMPI_WINDOW_H_

/**
 * \file 
 * \ingroup UserInterfaceHeaders
 * the request in the AMPI context need to be used to track extra information;
 * the extra information cannot be exposed in Fortran77 but there can be a 
 * Fortran90 equivalent and it can be exposed in C to allow source transformation 
 * tools to use TBR analysis on the tracked information.
 */ 

#include <stddef.h>
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
    MPI_Aint num_reqs;
} AMPI_Win_stack;

void AMPI_WIN_STACK_push(AMPI_Win_stack *s, AMPI_WinRequest req);
AMPI_WinRequest AMPI_WIN_STACK_pop(AMPI_Win_stack *s);
void AMPI_WIN_STACK_stack_init(AMPI_Win_stack *s);
void AMPI_WIN_STACK_destroy(AMPI_Win_stack *s);
int AMPI_WIN_STACK_full(AMPI_Win_stack *s);
void AMPI_WIN_STACK_expand(AMPI_Win_stack *s);
void AMPI_WIN_STACK_shrink(AMPI_Win_stack *s);
int AMPI_WIN_STACK_empty(AMPI_Win_stack *s);

/** \ingroup UserInterfaceDeclarations
 * @{
 */

typedef struct {
  void *map;
  void *base;
  MPI_Aint size;
  AMPI_Win_stack *req_stack;
  MPI_Win plainWindow;
} AMPI_Win;

/** @} */

void AMPI_WIN_sync(AMPI_Win win);


#endif
