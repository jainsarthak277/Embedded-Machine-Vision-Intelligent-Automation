INCLUDE_DIRS = 
LIB_DIRS = 
CC=g++

CDEFS=
CFLAGS= -O0 -pg -g $(INCLUDE_DIRS) $(CDEFS)
LIBS= -lrt
CPPLIBS= -L/usr/lib -lopencv_core -lopencv_flann -lopencv_video

HFILES= 
CFILES= 
CPPFILES= faceDetect.cpp

SRCS= ${HFILES} ${CFILES}
CPPOBJS= ${CPPFILES:.cpp=.o}

all:	faceDetect

clean:
	-rm -f *.o *.d
	-rm -f faceDetect

distclean:
	-rm -f *.o *.d

run: faceDetect
	./faceDetect --cascade="haarcascade_frontalface_alt.xml" --nested-cascade="haarcascade_eye.xml" --scale=1.3 lena.jpg
	
faceDetect: faceDetect.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $@.o `pkg-config --libs opencv` $(CPPLIBS)

depend:

.c.o:
	$(CC) $(CFLAGS) -c $<

.cpp.o:
	$(CC) $(CFLAGS) -c $<
