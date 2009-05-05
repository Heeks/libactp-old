# Makefile for libactp
# written by Dan Heeks, January 2009, license: GPL version 3 http://www.gnu.org/licenses/gpl-3.0.txt

PREFIX	= /usr/local
HOME	= .
CXX     = g++
CC      = gcc
LD      = g++
LDFLAGS = -shared -rdynamic `python-config --ldflags`
LIBS    = -lstdc++ `python-config --libs`
CFLAGS  = -Wall -I/usr/include `python-config --includes` -I./ -I../freesteel/src -g -fPIC

LIBNAME	= actp
LIBOBJS	= Partition1.o S1.o PathX.o PathXboxed.o S1stockcircle.o S2weave.o SurfX.o SurfXboxed.o NormRay_gen.o CoreRoughGeneration.o S2weaveCell.o S2weaveCellLinearCut.o S2weaveCellLinearCutTraverse.o S2weaveCircle.o SLi_gen.o SurfXbuildcomponents.o SurfXSliceRay.o PythonStuff.o
LIBDIR	= .libs/
LIBOUT	= $(LIBDIR)$(LIBNAME).so

all:	$(LIBOUT)

$(LIBOUT): $(LIBOBJS)
	@-mkdirhier $(LIBDIR)
	$(LD) $(LDFLAGS) -fPIC -Wl,-soname,actp.so.0 $(LIBOBJS) -o $(LIBOUT) $(LIBS)

clean:
	@-rm -f $(LIBOBJS)
	@-rm -f $(LIBOUT)
	@-rmdir $(LIBDIR)

install: $(LIBOUT)
	strip $^
	chmod 644 $^
	install $^ $(PREFIX)/lib/python`python -c "import sys; print sys.version[:3]"`/site-packages/

test:
	python sample.py

Partition1.o: ../freesteel/src/bolts/Partition1.cpp
	$(CC) -c $? ${CFLAGS} -o $@

S1.o: ../freesteel/src/bolts/S1.cpp
	$(CC) -c $? ${CFLAGS} -o $@

PathX.o: ../freesteel/src/cages/PathX.cpp
	$(CC) -c $? ${CFLAGS} -o $@

PathXboxed.o: ../freesteel/src/cages/PathXboxed.cpp
	$(CC) -c $? ${CFLAGS} -o $@

S1stockcircle.o: ../freesteel/src/cages/S1stockcircle.cpp
	$(CC) -c $? ${CFLAGS} -o $@

S2weave.o: ../freesteel/src/cages/S2weave.cpp
	$(CC) -c $? ${CFLAGS} -o $@

SurfX.o: ../freesteel/src/cages/SurfX.cpp
	$(CC) -c $? ${CFLAGS} -o $@

SurfXboxed.o: ../freesteel/src/cages/SurfXboxed.cpp
	$(CC) -c $? ${CFLAGS} -o $@

NormRay_gen.o: ../freesteel/src/pits/NormRay_gen.cpp
	$(CC) -c $? ${CFLAGS} -o $@

CoreRoughGeneration.o: ../freesteel/src/pits/CoreRoughGeneration.cpp
	$(CC) -c $? ${CFLAGS} -o $@

S2weaveCell.o: ../freesteel/src/pits/S2weaveCell.cpp
	$(CC) -c $? ${CFLAGS} -o $@

S2weaveCellLinearCut.o: ../freesteel/src/pits/S2weaveCellLinearCut.cpp
	$(CC) -c $? ${CFLAGS} -o $@

S2weaveCellLinearCutTraverse.o: ../freesteel/src/pits/S2weaveCellLinearCutTraverse.cpp
	$(CC) -c $? ${CFLAGS} -o $@

S2weaveCircle.o: ../freesteel/src/pits/S2weaveCircle.cpp
	$(CC) -c $? ${CFLAGS} -o $@

SLi_gen.o: ../freesteel/src/pits/SLi_gen.cpp
	$(CC) -c $? ${CFLAGS} -o $@

SurfXbuildcomponents.o: ../freesteel/src/pits/SurfXbuildcomponents.cpp
	$(CC) -c $? ${CFLAGS} -o $@

SurfXSliceRay.o: ../freesteel/src/pits/SurfXSliceRay.cpp
	$(CC) -c $? ${CFLAGS} -o $@

PythonStuff.o: PythonStuff.cpp
	$(CC) -c $? ${CFLAGS} -o $@

MakeToolpath.o: MakeToolpath.cpp
	$(CC) -c $? ${CFLAGS} -o $@
