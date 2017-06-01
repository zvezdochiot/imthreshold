PROGNAME      = imthreshold-deskew imthreshold-rotate imthreshold-fadsmooth imthreshold-fautoinv imthreshold-fbgdiv imthreshold-fbgfglsep imthreshold-fdespeckle imthreshold-fdneuro imthreshold-fgreyworld imthreshold-fillumc imthreshold-finfo imthreshold-fkmeans imthreshold-flevelmean imthreshold-flevelsigma imthreshold-fpmean imthreshold-fretinex imthreshold-fselgauss imthreshold-fshrink imthreshold-funripple imthreshold-funsharp imthreshold-fwiener imthreshold-sbicub imthreshold-sbilin imthreshold-sbwmag2 imthreshold-shris imthreshold-snearest imthreshold-tabutaleb imthreshold-tbernsen imthreshold-tbht imthreshold-tbimod imthreshold-tdalg imthreshold-tdither imthreshold-tdjvul imthreshold-tent imthreshold-teqbright imthreshold-tgatos imthreshold-tgrad imthreshold-thalftone2 imthreshold-tjanni imthreshold-tkmeans imthreshold-tnib imthreshold-totsu imthreshold-trot imthreshold-tsauvola imthreshold-ttext imthreshold-ttsai imthreshold-twhiterohrer
CPP           = g++
CFLAGS        = -DUNIX -O2 -Wall
LIBS          = -lfreeimage
VER           = 0
VERB          = 20170601
COMMON        = libimthreshold.so.$(VER) libimthresholdfreeimage.so.$(VER)
PREFIX        = /usr/local
INCPREFIX     = $(PREFIX)/include
LIBPREFIX     = $(PREFIX)/lib/i386-linux-gnu
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

imthreshold-fadsmooth: fadsmooth.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-deskew: deskew.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-rotate: rotate.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-fautoinv: fautoinv.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-fbgdiv: fbgdiv.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-fbgfglsep: fbgfglsep.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-fdespeckle: fdespeckle.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-fdneuro: fdneuro.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-fgreyworld: fgreyworld.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-fillumc: fillumc.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-finfo: finfo.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-fkmeans: fkmeans.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-flevelmean: flevelmean.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-flevelsigma: flevelsigma.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-fpmean: fpmean.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-fretinex: fretinex.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-fselgauss: fselgauss.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-fshrink: fshrink.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-funripple: funripple.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-funsharp: funsharp.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-fwiener: fwiener.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-sbicub: sbicub.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-sbilin: sbilin.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-sbwmag2: sbwmag2.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-shris: shris.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-snearest: snearest.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-tabutaleb: tabutaleb.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-tbernsen: tbernsen.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-tbimod: tbimod.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-tbht: tbht.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-tdalg: tdalg.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-tdither: tdither.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-tdjvul: tdjvul.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-tent: tent.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-teqbright: teqbright.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-tgatos: tgatos.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-tgrad: tgrad.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-thalftone2: thalftone2.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-tjanni: tjanni.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-tkmeans: tkmeans.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-tnib: tnib.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-totsu: totsu.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-trot: trot.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-tsauvola: tsauvola.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-ttext: ttext.cpp $(COMMON)
	$(CPP) $(CFLAGS) $(LIBS) $^ -s -o $@

imthreshold-ttsai: ttsai.cpp $(COMMON)
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
