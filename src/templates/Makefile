PROGNAME      = imthreshold-fselgauss
CPP           = g++
CFLAGS        = -DUNIX -O2 -Wall
LIBS          = -lfreeimage
COMMON        = imthreshold.cpp imthresholdfreeimage.cpp
SRCS          = fselgauss.cpp

.PHONY: all
all: $(PROGNAME)

.PHONY: clean
clean:
	rm -f $(PROGNAME)

$(PROGNAME):
	$(CPP) $(CFLAGS) $(LIBS) $(COMMON) $(SRCS) -s -o$(PROGNAME)
