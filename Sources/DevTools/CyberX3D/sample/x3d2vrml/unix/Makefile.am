noinst_PROGRAMS = x3d2vrml

INCLUDES = -I$(top_srcdir)/include $(X_CFLAGS)

x3d2vrml_SOURCES = \
	../x3d2vrml.cpp

x3d2vrml_LDADD = ../../../lib/unix/libcx3d.a $(X_PRE_LIBS) -lX11 -lXmu -lXi -lXext $(X_EXTRA_LIBS) $(X_LIBS)


