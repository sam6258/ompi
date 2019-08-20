#
# Copyright (c) 2016-2018 Intel, Inc.  All rights reserved.
# Copyright (c) 2019      IBM Corporation.  All rights reserved.
# $COPYRIGHT$
#
# Additional copyrights may follow
#
# $HEADER$
#

sources = \
          regx_none_component.c \
          regx_none.h \
          regx_none.c

# Make the output library in this directory, and name it either
# mca_<type>_<name>.la (for DSO builds) or libmca_<type>_<name>.la
# (for static builds).

if MCA_BUILD_orte_regx_none_DSO
component_noinst =
component_install = mca_regx_none.la
else
component_noinst = libmca_regx_none.la
component_install =
endif

mcacomponentdir = $(ortelibdir)
mcacomponent_LTLIBRARIES = $(component_install)
mca_regx_none_la_SOURCES = $(sources)
mca_regx_none_la_LDFLAGS = -module -avoid-version
mca_regx_none_la_LIBADD = $(top_builddir)/orte/lib@ORTE_LIB_PREFIX@open-rte.la

noinst_LTLIBRARIES = $(component_noinst)
libmca_regx_none_la_SOURCES = $(sources)
libmca_regx_none_la_LDFLAGS = -module -avoid-version
