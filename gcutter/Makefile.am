INCLUDES = 			\
	 -I$(top_builddir)	\
	 -I$(top_srcdir)	\
	 -I$(top_srcdir)/cutter

AM_CFLAGS = 			\
	$(CUTTER_CFLAGS)	\
	$(COVERAGE_CFLAGS)

EXTRA_DIST = gcut-marshalers.list
CLEANFILES = *.gcno *.gcda

marshal_sources =		\
	gcut-marshalers.c	\
	gcut-marshalers.h

MAINTAINERCLEANFILES =		\
	$(marshal_sources)

BUILT_SOURCES = $(marshal_sources)

noinst_LTLIBRARIES = libgcutter.la

gcutter_public_headers =		\
	gcut-assertions.h		\
	gcut-assertions-helper.h	\
	gcut-public.h			\
	gcut-test-utils.h		\
	gcut-value-equal.h		\
	gcut-list.h			\
	gcut-hash-table.h		\
	gcut-error.h			\
	gcut-enum.h			\
	gcut-types.h			\
	gcut-object.h			\
	gcut-string.h			\
	gcut-string-io-channel.h	\
	gcut-egg.h			\
	gcut-io.h			\
	gcut-key-file.h			\
	gcut-inspect.h			\
	gcut-data.h			\
	gcut-data-helper.h		\
	gcut-dynamic-data.h		\
	gcut-process.h			\
	gcut-event-loop.h		\
	gcut-glib-event-loop.h		\
	gcut-main.h

noinst_headers = 	\
	gbsearcharray.h

pkginclude_HEADERS = 	\
	gcutter.h

gcutter_includedir=$(pkgincludedir)/gcutter
gcutter_include_HEADERS = 		\
	$(gcutter_public_headers)

libgcutter_sources =			\
	gcut-assertions-helper.c	\
	gcut-data-helper.c		\
	gcut-dynamic-data.c		\
	gcut-egg.c			\
	gcut-enum.c			\
	gcut-error.c			\
	gcut-hash-table.c		\
	gcut-inspect.c			\
	gcut-io.c			\
	gcut-key-file.c			\
	gcut-list.c			\
	gcut-main.c			\
	gcut-object.c			\
	gcut-string.c			\
	gcut-string-io-channel.c	\
	gcut-types.c			\
	gcut-process.c			\
	gcut-value-equal.c		\
	gcut-event-loop.c		\
	gcut-glib-event-loop.c		\
	gcut-utils.c

libgcutter_la_SOURCES =			\
	$(gcutter_public_headers)	\
	$(noinst_headers)		\
	$(marshal_sources)		\
	$(libgcutter_sources)


gcut-marshalers.h : gcut-marshalers.list
	$(GLIB_GENMARSHAL) $(top_srcdir)/gcutter/gcut-marshalers.list --header --prefix=_gcut_marshal > $@

gcut-marshalers.c : gcut-marshalers.list
	$(GLIB_GENMARSHAL) $(top_srcdir)/gcutter/gcut-marshalers.list --header --body --prefix=_gcut_marshal > $@

echo-public-headers:
	@for header in $(gcutter_include_HEADERS); do	\
	  echo $(abs_srcdir)/$${header};		\
	done

echo-libgcutter-sources:
	@echo $(libgcutter_sources) gcut-marshalers.c
