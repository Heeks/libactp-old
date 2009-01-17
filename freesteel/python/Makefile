NAME=FreesteelPython
SWIG=swig
CXX=g++

SRCDIR=../src ../src/bolts ../src/cages ../src/pits ../src/visuals
BOLTS=bolts.h debugfuncs.h I1.h P2.h P3.h Partition1.h Partition1.cpp S1.h smallfuncs.h vo.h
CAGES=Area2_gen.h cages.h PathX.h PathX.cpp PathXboxed.cpp pathxseries.h S1stockcircle.cpp S2weave.cpp S2weave.h SurfX.h SurfX.cpp SurfXboxed.cpp SurfXboxed.h
PITS=CircCrossingStructure.h NormRay_gen.cpp pits.h S2weaveCell.cpp S2weaveCell.h S2weaveCellLinearCut.cpp S2weaveCellLinearCut.h S2weaveCellLinearCutTraverse.cpp S2weaveCircle.cpp SLi_gen.cpp SLi_gen.h SurfXbuildcomponents.cpp SurfXSliceRay.cpp toolshape.cpp toolshape.h
VISUALS=fsvtkToolpathMapper.cpp fsvtkToolpathMapper.h GeometryStuffTogether.cpp gst.h gstsurface.cpp gstsurface.h MakeToolpath.cpp MakeToolpath.h
SRC=$(NAME)cpp $(NAME).h $(NAME)_wrap.cxx

$(NAME)_wrap.o: $(NAME)_wrap.cxx
	$(CXX) -c -o $@ $(NAME)_wrap.cxx

$(NAME)_wrap.cxx: $(NAME).i
	$(SWIG) -c++ -python  -I../src $(NAME).i
#swig -c++ -python -outdir "$(OutDir)" -I../src "$(InputPath)"
