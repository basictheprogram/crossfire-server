SUBDIRS = include
plugindir=$(pkglibdir)/plugins
noinst_HEADERS = include/citylife.h	../common/include/plugin_common.h
plugin_LTLIBRARIES = citylife.la
citylife_la_SOURCES = citylife.c\
	../common/plugin_common.c \
	../../common/cf_random.c

citylife_la_LIBADD =
citylife_la_LDFLAGS = -module -avoid-version -rdynamic
AM_CPPFLAGS = -I$(srcdir)/include -I$(srcdir)/../common/include -I$(top_srcdir)/include

