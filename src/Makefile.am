bin_PROGRAMS = wmlj

INCLUDES = @GTK_CFLAGS@ @CURL_CFLAGS@

CFLAGS = @CFLAGS@ -Wall -g

wmlj_SOURCES = \
	about.c about.h dlg.c dlg.h foodock.c foodock.h menu.c menu.h \
	network.c network.h rc.c rc.h settings.c settings.h wmlj.c wmlj.h

wmlj_LDADD = @GTK_LIBS@ @CURL_LIBS@
