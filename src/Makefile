PROGNAME      = imthreshold-deskew imthreshold-rotate imthreshold-filter imthreshold-fautoinv imthreshold-fbgfglsep imthreshold-fdespeckle imthreshold-finfo imthreshold-flevell imthreshold-fpmean imthreshold-size imthreshold-sbwmag2 imthreshold-scris imthreshold-shris imthreshold-tglobal imthreshold-tlocal imthreshold-tdalg imthreshold-tdjvul imthreshold-tgatos imthreshold-thalftone2 imthreshold-ttext imthreshold-twhiterohrer
CPP           = g++
CFLAGS        = -DUNIX -O2 -Wall
LIBS          = -lfreeimage
VER           = 0
VERB          = 20180103
COMMON        = libimthreshold.so.$(VER) libimthresholdfreeimage.so.$(VER)
PREFIX        = /usr/local
INCPREFIX     = $(PREFIX)/include
LIBPREFIX     = $(PREFIX)/lib
MANPREFIX     = $(PREFIX)/share/man/man1
INSTALL       = install
LN            = ln -fs

.PHONY: all clean install

all: $(PROGNAME)

clean:
	rm -f $(PROGNAME) libimthreshold.so* libimthresholdfreeimage.so*

libimthreshold.so.$(VER): imthreshold.cpp
	$(CPP) $(CFLAGS) -shared $< -s -o $@
	chmod 644 $@
	mv $@ $@.$(VERB)
	ln -s $@.$(VERB) $@
	ln -s $@ libimthreshold.so

libimthresholdfreeimage.so.$(VER): imthresholdfreeimage.cpp
	$(CPP) $(CFLAGS) $(LIBS) -shared $< -s -o $@
	chmod 644 $@
	mv $@ $@.$(VERB)
	ln -s $@.$(VERB) $@
	ln -s $@ libimthresholdfreeimage.so

imthreshold-deskew: deskew.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-rotate: rotate.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-filter: filter.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-fautoinv: fautoinv.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-fbgfglsep: fbgfglsep.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-fdespeckle: fdespeckle.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-finfo: finfo.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-flevell: flevell.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-fpmean: fpmean.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-size: size.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-sbwmag2: sbwmag2.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-scris: scris.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-shris: shris.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-tglobal: tglobal.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-tlocal: tlocal.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-tdalg: tdalg.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-tdjvul: tdjvul.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-tgatos: tgatos.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-thalftone2: thalftone2.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-ttext: ttext.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-twhiterohrer: twhiterohrer.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

install: $(PROGNAME)
	$(INSTALL) -d $(LIBPREFIX)
	$(INSTALL) -m 0644 libimthreshold.so.$(VER).$(VERB) $(LIBPREFIX)/libimthreshold.so.$(VER).$(VERB)
	$(LN)  $(LIBPREFIX)/libimthreshold.so.$(VER).$(VERB)  $(LIBPREFIX)/libimthreshold.so.$(VER)
	$(LN)  $(LIBPREFIX)/libimthreshold.so.$(VER)  $(LIBPREFIX)/libimthreshold.so
	$(INSTALL) -m 0644 libimthresholdfreeimage.so.$(VER).$(VERB) $(LIBPREFIX)/libimthresholdfreeimage.so.$(VER).$(VERB)
	$(LN)  $(LIBPREFIX)/libimthresholdfreeimage.so.$(VER).$(VERB)  $(LIBPREFIX)/libimthresholdfreeimage.so.$(VER)
	$(LN)  $(LIBPREFIX)/libimthresholdfreeimage.so.$(VER)  $(LIBPREFIX)/libimthresholdfreeimage.so
	$(INSTALL) -d $(INCPREFIX)
	$(INSTALL) -m 0644 imthreshold*.h $(INCPREFIX)
	$(INSTALL) -d $(PREFIX)/bin
	$(INSTALL) -m 0755 $(PROGNAME) $(PREFIX)/bin/
	$(INSTALL) -d $(MANPREFIX)
	$(INSTALL) -m 0644 ../man/man1/imthreshold*.1.gz $(MANPREFIX)
