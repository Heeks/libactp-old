# Makefile
# written by Dan Heeks, January 2009, license: GPL version 3 http://www.gnu.org/licenses/gpl-3.0.txt

CCFLAGS=-Wall -fPIC -g -I/usr/include/python2.5 -I./ -I../freesteel/src

OBJECTS=Partition1.o S1.o PathX.o PathXboxed.o S1stockcircle.o S2weave.o SurfX.o SurfXboxed.o NormRay_gen.o CoreRoughGeneration.o S2weaveCell.o S2weaveCellLinearCut.o S2weaveCellLinearCutTraverse.o S2weaveCircle.o SLi_gen.o SurfXbuildcomponents.o SurfXSliceRay.o PythonStuff.o

SHARED_LIBRARY=./actp.so

${SHARED_LIBRARY}: ${OBJECTS}
	gcc -shared -fPIC -Wl,-soname,actp.so.0 -o ${SHARED_LIBRARY} ${OBJECTS} -lstdc++ -lpython2.5

Partition1.o: ../freesteel/src/bolts/Partition1.cpp
	gcc -c $? ${CCFLAGS} -o $@

S1.o: ../freesteel/src/bolts/S1.cpp
	gcc -c $? ${CCFLAGS} -o $@

PathX.o: ../freesteel/src/cages/PathX.cpp
	gcc -c $? ${CCFLAGS} -o $@

PathXboxed.o: ../freesteel/src/cages/PathXboxed.cpp
	gcc -c $? ${CCFLAGS} -o $@

S1stockcircle.o: ../freesteel/src/cages/S1stockcircle.cpp
	gcc -c $? ${CCFLAGS} -o $@

S2weave.o: ../freesteel/src/cages/S2weave.cpp
	gcc -c $? ${CCFLAGS} -o $@

SurfX.o: ../freesteel/src/cages/SurfX.cpp
	gcc -c $? ${CCFLAGS} -o $@

SurfXboxed.o: ../freesteel/src/cages/SurfXboxed.cpp
	gcc -c $? ${CCFLAGS} -o $@

NormRay_gen.o: ../freesteel/src/pits/NormRay_gen.cpp
	gcc -c $? ${CCFLAGS} -o $@

CoreRoughGeneration.o: ../freesteel/src/pits/CoreRoughGeneration.cpp
	gcc -c $? ${CCFLAGS} -o $@

S2weaveCell.o: ../freesteel/src/pits/S2weaveCell.cpp
	gcc -c $? ${CCFLAGS} -o $@

S2weaveCellLinearCut.o: ../freesteel/src/pits/S2weaveCellLinearCut.cpp
	gcc -c $? ${CCFLAGS} -o $@

S2weaveCellLinearCutTraverse.o: ../freesteel/src/pits/S2weaveCellLinearCutTraverse.cpp
	gcc -c $? ${CCFLAGS} -o $@

S2weaveCircle.o: ../freesteel/src/pits/S2weaveCircle.cpp
	gcc -c $? ${CCFLAGS} -o $@

SLi_gen.o: ../freesteel/src/pits/SLi_gen.cpp
	gcc -c $? ${CCFLAGS} -o $@

SurfXbuildcomponents.o: ../freesteel/src/pits/SurfXbuildcomponents.cpp
	gcc -c $? ${CCFLAGS} -o $@

SurfXSliceRay.o: ../freesteel/src/pits/SurfXSliceRay.cpp
	gcc -c $? ${CCFLAGS} -o $@

PythonStuff.o: PythonStuff.cpp
	gcc -c $? ${CCFLAGS} -o $@

MakeToolpath.o: MakeToolpath.cpp
	gcc -c $? ${CCFLAGS} -o $@

clean:
	-rm ${SHARED_LIBRARY} ${OBJECTS}


