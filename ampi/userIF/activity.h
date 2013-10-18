#ifndef _AMPI_ACTIVITY_H_
#define _AMPI_ACTIVITY_H_

/**
 * \file 
 * \ingroup UserInterfaceHeaders
 * enumeration to distinguish between active and passive variants of MPI_Datatype parameters passed to AMPI routines 
 */ 

#include "ampi/userIF/libConfig.h"

/** \ingroup UserInterfaceDeclarations
 * @{
 */
enum AMPI_Activity_E { 
  AMPI_PASSIVE=0,
  AMPI_ACTIVE=1
};

#ifdef AMPI_FORTRANCOMPATIBLE
typedef int AMPI_Activity;
#else 
typedef enum AMPI_Activity_E AMPI_Activity;
#endif 
/** @} */
#endif
