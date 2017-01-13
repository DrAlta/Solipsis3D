noinst_PROGRAMS = X3DBrowserGLUT

INCLUDES = -I$(top_srcdir)/include -I../../ $(X_CFLAGS)

X3DBrowserGLUT_SOURCES = \
	../X3DBrowserGLUT.cpp \
	../../X3DBrowserFunc.cpp 

X3DBrowserGLUT_LDADD = ../../../../lib/unix/libcx3d.a $(X_PRE_LIBS) -lX11 -lXmu -lXi -lXext $(X_EXTRA_LIBS) $(X_LIBS)
