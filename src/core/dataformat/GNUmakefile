#
# This is an example GNUmakefile for my packages
#
PACKAGE_NAME = dataformat

# specific names for this package
SOURCES = $(wildcard *.cxx)
FMWK_HEADERS = 
HEADERS = $(filter-out $(FMWK_HEADERS), $(wildcard *.h))
HEADERS_DEST = larcv/core/$(PACKAGE_NAME)

# include options for this package
INCFLAGS  = -I.                       #Include itself
INCFLAGS += -I$(LARCV_INCDIR)

# platform-specific options
OSNAME          = $(shell uname -s)
HOST            = $(shell uname -n)
OSNAMEMODE      = $(OSNAME)

include $(LARCV_BASEDIR)/Makefile/Makefile.${OSNAME}

INCFLAGS += -I/opt/local/include

LDFLAGS += -L$(LARCV_LIBDIR) -lbase -L/opt/local/lib -lhdf5_cpp -lhdf5 -lhdf5_hl
# call the common GNUmakefile
include $(LARCV_BASEDIR)/Makefile/GNUmakefile.CORE

pkg_build: # nothing extra to do in this package
pkg_clean: # nothing extra to do in this package

