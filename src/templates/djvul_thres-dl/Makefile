PROGNAME      = imthreshold-tdjvul
CPP           = g++
CFLAGS        = -DUNIX -O2 -Wall
LIB           = libimthreshold.so.0
LIBSRC        = imthreshold.cpp
LIBS          = -lfreeimage
SRCS          = djvul_thres.cpp

.PHONY: all
all: $(PROGNAME)

.PHONY: clean
clean:
	rm -f $(PROGNAME) $(LIB)

$(PROGNAME): $(LIB)
	$(CPP) $(CFLAGS) $(LIBS) $(LIB) $(SRCS) -s -o$(PROGNAME)

$(LIB):
	$(CPP) -shared $(CFLAGS) $(LIBS) -o $(LIB) $(LIBSRC) -s
