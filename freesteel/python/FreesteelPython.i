//swig -c++ -includeall -python -outdir "$(OutDir)" -I../src -I../src/bolts -I"$(PYTHON_INCLUDE)"  -I"$(VTK)"  -I"$(VTK)/Common" -I"$(VTK)/Filtering" -I"$(VTK)/Graphics" -I"$(VTK)/Rendering" -I"$(VTK)/IO" -I"$(VTK)/Hybrid" -I"$(VTK)/Imaging" -I"$(VTK)/Patented" "$(InputPath)"
//swig -c++ -python -outdir "$(OutDir)" -I../src "$(InputPath)"

/* File : FreesteelPython.i */
%module FreesteelPython

%{
/* Include in the generated wrapper file */
#include "FreesteelPython.h"
%}

%include stl.i
using namespace std;
//%import stl.i

%include "bolts/I1.h"
%include "bolts/P2.h"
%include "bolts/P3.h"

%include "bolts/smallfuncs.h"
%template(EqualP2) Equal<P2>;
%template(EqualP3) Equal<P3>;

%template(vecb1) std::vector<B1 >;
%include "bolts/S1.h"
%include "bolts/Partition1.h"
%include "bolts/vo.h"

%include "visuals/gstsurface.h"
//%template(vecGSTbasePtr) vector<GSTbase *>;
%template(vecPathXSeries) vector<PathXSeries>;
//%include "visuals/gst.h"
%include "pits/toolshape.h"
%include "pits/CoreRoughGeneration.h"
%template(vecP2) vector<P2>;
%template(vecint) vector<int>;
%include "cages/pathxseries.h"
%include "cages/SurfX.h"
%include "cages/SurfXboxed.h"
%include "cages/S2weave.h"
%include "cages/Area2_gen.h"
%include "visuals/fsvtkToolpathMapper.h"
%include "visuals/MakeToolpath.h"

%include "FreesteelPython.h"

