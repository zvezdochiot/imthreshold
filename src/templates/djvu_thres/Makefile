PROGNAME      = imthreshold-tdjvu
CPP           = g++
CFLAGS        = -DUNIX -O2 -Wall
LIBS          = -lfreeimage
SRCS          = djvu_thres.cpp

.PHONY: all
all: $(PROGNAME)

.PHONY: clean
clean:
	rm -f $(PROGNAME)

$(PROGNAME):
	$(CPP) $(CFLAGS) $(LIBS) $(SRCS) -s -o$(PROGNAME)
