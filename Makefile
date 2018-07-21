PNAME         = imthreshold
PROGNAME      = $(PNAME)-deskew $(PNAME)-rotate $(PNAME)-filter $(PNAME)-fautoinv $(PNAME)-fdespeckle $(PNAME)-finfo $(PNAME)-fpmean $(PNAME)-math $(PNAME)-separate $(PNAME)-size $(PNAME)-sbwmag2 $(PNAME)-scris $(PNAME)-shris $(PNAME)-tglobal $(PNAME)-tlocal $(PNAME)-tlayer $(PNAME)-tdalg $(PNAME)-tdjvul $(PNAME)-tgatos $(PNAME)-thalftone2 $(PNAME)-ttext $(PNAME)-twhiterohrer
CPP           = g++
CFLAGS        = -DUNIX -O2 -Wall -s
LIBS          = -lfreeimage
VER           = 0
VERB          = 20180618
COMMON        = lib$(PNAME).so.$(VER) lib$(PNAME)freeimage.so.$(VER)
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

lib$(PNAME).so.$(VER): src/imthreshold.cpp
	$(CPP) $(CFLAGS) -shared -Wl,-soname,$@ $< -o $@
	chmod 644 $@
	mv $@ $@.$(VERB)
	ln -s $@.$(VERB) $@
	ln -s $@ libimthreshold.so

lib$(PNAME)freeimage.so.$(VER): src/imthresholdfreeimage.cpp
	$(CPP) $(CFLAGS) $(LIBS) -shared -Wl,-soname,$@ $< -o $@
	chmod 644 $@
	mv $@ $@.$(VERB)
	ln -s $@.$(VERB) $@
	ln -s $@ libimthresholdfreeimage.so

$(PNAME)-deskew: src/deskew.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -o $@

$(PNAME)-rotate: src/rotate.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -o $@

$(PNAME)-filter: src/filter.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -o $@

$(PNAME)-fautoinv: src/fautoinv.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -o $@

$(PNAME)-fdespeckle: src/fdespeckle.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -o $@

$(PNAME)-finfo: src/finfo.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -o $@

$(PNAME)-fpmean: src/fpmean.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -o $@

$(PNAME)-math: src/math.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -o $@

$(PNAME)-separate: src/separate.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -o $@

$(PNAME)-size: src/size.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -o $@

$(PNAME)-sbwmag2: src/sbwmag2.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -o $@

$(PNAME)-scris: src/scris.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -o $@

$(PNAME)-shris: src/shris.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -o $@

$(PNAME)-tglobal: src/tglobal.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -o $@

$(PNAME)-tlocal: src/tlocal.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -o $@

$(PNAME)-tlayer: src/tlayer.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -o $@

$(PNAME)-tdalg: src/tdalg.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -o $@

$(PNAME)-tdjvul: src/tdjvul.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -o $@

$(PNAME)-tgatos: src/tgatos.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -o $@

$(PNAME)-thalftone2: src/thalftone2.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -o $@

$(PNAME)-ttext: src/ttext.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -o $@

$(PNAME)-twhiterohrer: src/twhiterohrer.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -o $@

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
