PROGNAME      = imthreshold-fgreysep
CPP           = g++
CFLAGS        = -DUNIX -O2 -Wall
LIBS          = -lfreeimage
SRCS          = greysep.cpp

.PHONY: all
all: $(PROGNAME)

.PHONY: clean
clean:
	rm -f $(PROGNAME)

$(PROGNAME):
	$(CPP) $(CFLAGS) $(LIBS) $(SRCS) -s -o$(PROGNAME)
