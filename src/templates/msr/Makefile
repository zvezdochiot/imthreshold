PROGNAME      = imthreshold-fmsr
CPP           = g++
CFLAGS        = -DUNIX -O2 -Wall
LIBS          = -lfreeimage -lfftw3 -lm
SRCS          = msr.cpp MSR_original_lib.c

.PHONY: all
all: $(PROGNAME)

.PHONY: clean
clean:
	rm -f $(PROGNAME)

$(PROGNAME):
	$(CPP) $(CFLAGS) $(LIBS) $(SRCS) -s -o$(PROGNAME)
