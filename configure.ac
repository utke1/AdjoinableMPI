dnl Process this file with autoconf to produce a configure script.

dnl Disable caching: 
define([AC_CACHE_LOAD], )dnl
define([AC_CACHE_SAVE], )dnl

AC_PREREQ(2.61)
AC_INIT([AMPI], [0.1])
dnl macros in config/*.m4
AC_CONFIG_MACRO_DIR([config])
dnl helper tools are in config
AC_CONFIG_AUX_DIR([config])
dnl check if at least one source file is there ...
AC_CONFIG_SRCDIR([ampi/ampi.h])
AC_CANONICAL_TARGET

dnl ---- Automake ----
dnl foreign   -> do no create the standard GNU standard files NEWS README AUTHORS ChangeLog
dnl no-define -> do not define PACKAGE and VERSION
AM_INIT_AUTOMAKE([foreign no-define])
dnl Do not show compilation command by default
AM_SILENT_RULES([yes])

# dump the configuration options here:
AC_CONFIG_HEADERS([config.h])
AH_TOP([
#ifndef AMPI_CONFIG_H
#define AMPI_CONFIG_H
])
AH_BOTTOM([
#endif 
])

AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_RANLIB

AC_LANG([C])
AC_LANG([C++])

AMPI_SET_MPICC

AC_SUBST(fortranCompatible)

AC_ARG_ENABLE(fortranCompatible,
[AS_HELP_STRING([--enable-fortranCompatible],
                [adjust the signatures to be callable from Fortran])],
[case "${enableval}" in
  yes)
    fortranCompatible='#define AMPI_FORTRANCOMPATIBLE'
    ;;
  no)
    fortranCompatible='#undef AMPI_FORTRANCOMPATIBLE'
    ;;
  *)
     AC_MSG_ERROR(bad value ${fortranCompatible} for --enable-fortranCompatible) ;;
esac],
[fortranCompatible='#undef AMPI_FORTRANCOMPATIBLE'])


AC_CONFIG_FILES([Makefile
		 ampi/Makefile
		 ampi/userIF/libConfig.h
                 PlainC/Makefile])
AC_SUBST(MPICC)
AC_SUBST(MPI_INC_DIR)
AC_SUBST(MPI_LIB_DIR)
AC_SUBST(MPI_LIBS)
AC_SUBST(MPI_FLAGS)

AC_CANONICAL_BUILD
AC_CANONICAL_HOST

AC_OUTPUT
AMPI_REPORT

dnl use config.status instead of reconfigure if configure hasn't changed
AC_CONFIG_COMMANDS([outmsg],[chmod +x autogen.sh])

