PROGNAME      = imthreshold-feqv5
CPP           = g++
CFLAGS        = -DUNIX -O2 -Wall
LIBS          = -lfreeimage
SRCS          = eqv5.cpp

.PHONY: all
all: $(PROGNAME)

.PHONY: clean
clean:
	rm -f $(PROGNAME)

$(PROGNAME):
	$(CPP) $(CFLAGS) $(LIBS) $(SRCS) -s -o$(PROGNAME)
