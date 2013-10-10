lib_LTLIBRARIES = libampiCommon.la

libampiCommon_la_CPPFLAGS = -I@top_builddir@ -I@abs_top_srcdir@

libampiCommon_la_SOURCES = \
modified.c \
nt.c \
passThrough.c \
window.c \
st.c

if ISFORTRANCOMPATIBLE
libampiCommon_la_SOURCES += ampi_f2c.c
endif
