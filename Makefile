PNAME         = imthreshold
PROGNAME      = $(PNAME)-deskew $(PNAME)-rotate $(PNAME)-filter $(PNAME)-fautoinv $(PNAME)-fdespeckle $(PNAME)-finfo $(PNAME)-fpmean $(PNAME)-math $(PNAME)-separate $(PNAME)-size $(PNAME)-sbwmag2 $(PNAME)-scris $(PNAME)-shris $(PNAME)-tglobal $(PNAME)-tlocal $(PNAME)-tlayer $(PNAME)-tdjvul $(PNAME)-tgatos $(PNAME)-thalftone2 $(PNAME)-ttext $(PNAME)-twhiterohrer
CC            = gcc
CPP           = g++
CFLAGS        = -DUNIX -O2 -Wall -s
LIBS          = -lfreeimage
VER           = 0
VERB          = 20200419
PLIBF         = lib$(PNAME).so.$(VER)
PLIBFI        = lib$(PNAME)freeimage.so.$(VER)
PLIB          = $(PLIBF) $(PLIBFI)
PREFIX        = /usr/local
INCPREFIX     = $(PREFIX)/include
LIBPREFIX     = $(PREFIX)/lib
MANPREFIX     = $(PREFIX)/share/man/man1
DOCPREFIX     = $(PREFIX)/share/doc/$(PNAME)
INSTALL       = install
LN            = ln -fs

.PHONY: all clean install

all: $(PROGNAME)

clean:
	rm -f $(PROGNAME) lib$(PNAME).so* lib$(PNAME)freeimage.so*

$(PLIBF): src/lib/color.c src/lib/commom.c src/lib/denoise.c src/lib/despeckle.c src/lib/filter.c src/lib/math.c src/lib/pmean.c src/lib/rotate.c src/lib/separate.c src/lib/size.c src/lib/threshold.c
	$(CC) $(CFLAGS) -shared -Wl,-soname,$@ $^ -o $@
	chmod 644 $@
	mv $@ $@.$(VERB)
	ln -s $@.$(VERB) $@
	ln -s $@ libimthreshold.so

$(PLIBFI): src/imthresholdfreeimage.cpp $(PLIBF)
	$(CPP) $(CFLAGS) -shared -Wl,-soname,$@ $^ $(LIBS) -o $@
	chmod 644 $@
	mv $@ $@.$(VERB)
	ln -s $@.$(VERB) $@
	ln -s $@ libimthresholdfreeimage.so

$(PNAME)-deskew: src/deskew.cpp $(PLIB)
	$(CPP) $(CFLAGS) $^ $(LIBS) -o $@

$(PNAME)-rotate: src/rotate.cpp $(PLIB)
	$(CPP) $(CFLAGS) $^ $(LIBS) -o $@

$(PNAME)-filter: src/filter.cpp $(PLIB)
	$(CPP) $(CFLAGS) $^ $(LIBS) -o $@

$(PNAME)-fautoinv: src/fautoinv.cpp $(PLIB)
	$(CPP) $(CFLAGS) $^ $(LIBS) -o $@

$(PNAME)-fdespeckle: src/fdespeckle.cpp $(PLIB)
	$(CPP) $(CFLAGS) $^ $(LIBS) -o $@

$(PNAME)-finfo: src/finfo.cpp $(PLIB)
	$(CPP) $(CFLAGS) $^ $(LIBS) -o $@

$(PNAME)-fpmean: src/fpmean.cpp $(PLIB)
	$(CPP) $(CFLAGS) $^ $(LIBS) -o $@

$(PNAME)-math: src/math.cpp $(PLIB)
	$(CPP) $(CFLAGS) $^ $(LIBS) -o $@

$(PNAME)-separate: src/separate.cpp $(PLIB)
	$(CPP) $(CFLAGS) $^ $(LIBS) -o $@

$(PNAME)-size: src/size.cpp $(PLIB)
	$(CPP) $(CFLAGS) $^ $(LIBS) -o $@

$(PNAME)-sbwmag2: src/sbwmag2.cpp $(PLIB)
	$(CPP) $(CFLAGS) $^ $(LIBS) -o $@

$(PNAME)-scris: src/scris.cpp $(PLIB)
	$(CPP) $(CFLAGS) $^ $(LIBS) -o $@

$(PNAME)-shris: src/shris.cpp $(PLIB)
	$(CPP) $(CFLAGS) $^ $(LIBS) -o $@

$(PNAME)-tglobal: src/tglobal.cpp $(PLIB)
	$(CPP) $(CFLAGS) $^ $(LIBS) -o $@

$(PNAME)-tlocal: src/tlocal.cpp $(PLIB)
	$(CPP) $(CFLAGS) $^ $(LIBS) -o $@

$(PNAME)-tlayer: src/tlayer.cpp $(PLIB)
	$(CPP) $(CFLAGS) $^ $(LIBS) -o $@

$(PNAME)-tdjvul: src/tdjvul.cpp $(PLIB)
	$(CPP) $(CFLAGS) $^ $(LIBS) -o $@

$(PNAME)-tgatos: src/tgatos.cpp $(PLIB)
	$(CPP) $(CFLAGS) $^ $(LIBS) -o $@

$(PNAME)-thalftone2: src/thalftone2.cpp $(PLIB)
	$(CPP) $(CFLAGS) $^ $(LIBS) -o $@

$(PNAME)-ttext: src/ttext.cpp $(PLIB)
	$(CPP) $(CFLAGS) $^ $(LIBS) -o $@

$(PNAME)-twhiterohrer: src/twhiterohrer.cpp $(PLIB)
	$(CPP) $(CFLAGS) $^ $(LIBS) -o $@

install: $(PROGNAME)
	$(INSTALL) -d $(LIBPREFIX)
	$(INSTALL) -m 0644 lib$(PNAME).so.$(VER).$(VERB) $(LIBPREFIX)/lib$(PNAME).so.$(VER).$(VERB)
	$(LN)  lib$(PNAME).so.$(VER).$(VERB)  $(LIBPREFIX)/lib$(PNAME).so.$(VER)
	$(LN)  lib$(PNAME).so.$(VER)  $(LIBPREFIX)/lib$(PNAME).so
	$(INSTALL) -m 0644 lib$(PNAME)freeimage.so.$(VER).$(VERB) $(LIBPREFIX)/lib$(PNAME)freeimage.so.$(VER).$(VERB)
	$(LN)  lib$(PNAME)freeimage.so.$(VER).$(VERB)  $(LIBPREFIX)/lib$(PNAME)freeimage.so.$(VER)
	$(LN)  lib$(PNAME)freeimage.so.$(VER)  $(LIBPREFIX)/lib$(PNAME)freeimage.so
	$(INSTALL) -d $(INCPREFIX)
	$(INSTALL) -m 0644 src/imthreshold*.h $(INCPREFIX)
	$(INSTALL) -d $(PREFIX)/bin
	$(INSTALL) -m 0755 $(PROGNAME) $(PREFIX)/bin/
	$(INSTALL) -d $(MANPREFIX)
	$(INSTALL) -m 0644 man/man1/imthreshold*.1 $(MANPREFIX)
	$(INSTALL) -d $(DOCPREFIX)
	$(INSTALL) -m 0644 README.md doc/imthreshold/* $(DOCPREFIX)
