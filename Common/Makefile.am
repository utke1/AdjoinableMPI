CC=@MPICC@
AM_CXXFLAGS = -Wall -I$(top_srcdir)
AM_CFLAGS = -Wall -Wno-unknown-pragmas -std=c89 -I$(top_srcdir)
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

lib_LIBRARIES = libampiCommon.a

libampiCommon_adir = $(includedir)

libampiCommon_a_SOURCES = \
modified.c \
nt.c \
passThrough.c \
st.c
