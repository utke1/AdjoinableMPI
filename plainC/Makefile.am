CC=@MPICC@
AM_CXXFLAGS = -Wall -I$(top_srcdir)
AM_CFLAGS = -Wall -Wno-unknown-pragmas -std=c89 -I$(top_srcdir)
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

lib_LIBRARIES = libampiPlainC.a

libAMPI_adir = $(includedir)

libAMPI_a_HEADERS=\
                  ../ampi/ampi.h

libAMPI_a_SOURCES = \
                    $(libAMPI_a_HEADERS) \
                    ampi.c 
