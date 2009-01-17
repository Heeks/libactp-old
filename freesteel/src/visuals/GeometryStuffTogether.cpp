////////////////////////////////////////////////////////////////////////////////
// FreeSteel -- Computer Aided Manufacture Algorithms
// Copyright (C) 2004  Julian Todd and Martin Dunschen.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// See fslicense.txt and gpl.txt for further details
////////////////////////////////////////////////////////////////////////////////

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkFloatArray.h"
#include "vtkLine.h"
#include "vtkPolyLine.h"
#include "vtkDecimatePro.h"
#include "vtkProperty.h"
#include "vtkCamera.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkOpenGLPolyDataMapper.h"
#include "visuals/fsvtkToolpathMapper.h"
#include "vtkProperty.h"
#include "vtkCellArray.h"
#include "vtkSTLReader.h"

#include "bolts/bolts.h"
#include "cages/cages.h"
#include "pits/pits.h"
#include "visuals/MakeToolpath.h"
#include "visuals/gst.h"
#include "visuals/gstsurface.h"


/////////////////////////////////////////////////////////// 
void GeometryStuffTogether::MakeRectBoundary(const I1& xrg, const I1& yrg, double z) 
{
	GSTtoolpath *gstbound = new GSTtoolpath();
	gstees.push_back(gstbound);

	gstbound->ftpaths.push_back(PathXSeries());
	gstbound->ftpaths.back().z = z;
	gstbound->ftpaths.back().Add(P2(xrg.lo, yrg.lo)); 
	gstbound->ftpaths.back().Add(P2(xrg.hi, yrg.lo)); 
	gstbound->ftpaths.back().Add(P2(xrg.hi, yrg.hi)); 
	gstbound->ftpaths.back().Add(P2(xrg.lo, yrg.hi)); 
	gstbound->ftpaths.back().Add(P2(xrg.lo, yrg.lo)); 
	gstbound->ftpaths.back().Break(); 

	gstbound->UpdateFromPax();  

	gstbound->AddToRenderer(&ren1); 
}

/////////////////////////////////////////////////////////// 
GeometryStuffTogether::GeometryStuffTogether(vtkRenderWindow* lrenWin) 
{
	renWin = lrenWin; 
	ren1->GetActiveCamera()->ParallelProjectionOn(); 
	ren1->SetBackground(0.1, 0.2, 0.4);
	renWin->AddRenderer(&ren1);
	renWin->SetSize(600, 500);
}

GeometryStuffTogether::~GeometryStuffTogether()
{
	for (int i = 0; i < gstees.size(); ++i)
		delete gstees[i];
}
