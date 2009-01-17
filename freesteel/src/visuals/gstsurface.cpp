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

#include "vtkActor.h"
#include "vtkActorCollection.h"
#include "vtkRenderer.h"
#include "vtkSTLReader.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkOpenGLPolyDataMapper.h"
#include "visuals/fsvtkToolpathMapper.h"
#include "vtkProperty.h"
#include "vtkCellArray.h"

#include "bolts/bolts.h"
#include "cages/cages.h"
#include "pits/pits.h"

#include "visuals/gstsurface.h"

void GSTbase::AddToRenderer(class vtkRenderer* ren)
{
    actors->InitTraversal();
    vtkActor* actor = actors->GetNextItem();
    while(actor != NULL)
    {
        ren->AddActor(actor);
        actor = actors->GetNextItem();
    }
}

bool GSTbase::GetVisibility()
{
    actors->InitTraversal();
    vtkActor* actor = actors->GetNextItem();
    return(actor == NULL ? false : actor->GetVisibility() != 0);
}

void GSTbase::SetVisibility(bool bVisible)
{
    actors->InitTraversal();
    vtkActor* actor = actors->GetNextItem();
    while(actor != NULL)
    {
        actor->SetVisibility(bVisible);
        actor = actors->GetNextItem();
    }
}

PathXSeries GSTbase::MakeRectBoundary(double lz) 
{
  PathXSeries px;

  px.z = lz;
  px.Add(P2(xrg.lo, yrg.lo)); 
  px.Add(P2(xrg.hi, yrg.lo)); 
  px.Add(P2(xrg.hi, yrg.hi)); 
  px.Add(P2(xrg.lo, yrg.hi)); 
  px.Add(P2(xrg.lo, yrg.lo)); 
  px.Break(); 

  return px;
}

/////////////////////////////////////////////////////////// 
void GSTsurface::LoadSTL(const char* fname) 
{
	stl->SetFileName(fname); 
	stl->Update(); 

	double* values = stl->GetOutput()->GetBounds(); 
    xrg = I1(values[0], values[1]); 
    yrg = I1(values[2], values[3]); 
    zrg = I1(values[4], values[5]); 

	stlMap->SetInput(stl->GetOutput());
        
        vo<vtkActor> actor;
	actor->SetMapper(&stlMap); 
	actor->GetProperty()->SetColor(1.0000, 0.3882, 0.2784);

        actors->AddItem(&actor);
}

void GSTtoolpath::SetDrawWhole()
{
  if( ftpolydataMap->pftpaths->size() == 0 || ftpolydataMap->pftpaths->back().linkpths.size() == 0)
    return;

	ftpolydataMap->pos.ipathx = ftpolydataMap->pftpaths->size() - 1;
	ftpolydataMap->poslast.ipathx = 0;
	ftpolydataMap->pos.ilink = ftpolydataMap->pftpaths->back().linkpths.size() - 1;
	ftpolydataMap->pos.isegOnLink = ftpolydataMap->pos.ilink >= 0 ? ftpolydataMap->pftpaths->back().linkpths[ftpolydataMap->pos.ilink].size() - 1 : -1;
	ftpolydataMap->pos.isegOnPath = ftpolydataMap->pftpaths->back().pths.size() - 1;
	if ((ftpolydataMap->pos.ilink >= 0) && !ftpolydataMap->pftpaths->back().linkpths[ftpolydataMap->pos.ilink].empty())
		ftpolydataMap->pos.ptOnLink = ftpolydataMap->pftpaths->back().linkpths[ftpolydataMap->pos.ilink].back();
	ftpolydataMap->pos.ptOnPath = ftpolydataMap->pftpaths->back().pths.back();

    ftpolydataMap->bAnimated = false;
    ftpolydataMap->bForward = false;
}



/////////////////////////////////////////////////////////// 
void GSTtoolpath::UpdateFromPax() 
{
	// set that input array into our mapper
	ftpolydataMap->pftpaths = &ftpaths;
	if ((toolshape.cornerrad > 0) || (toolshape.flatrad > 0))
	{
		ftpolydataMap->ptoolshape = &toolshape;
		ftpolydataMap->bound = &bound;
	}
            

	// Load the point, cell
	vo<vtkPoints> points;
	vo<vtkCellArray> lines;

	for (int ip = 0; ip < ftpaths.size(); ++ip)
	{
		const PathXSeries& ftpath = ftpaths[ip];
		const vector<int>& brks = ftpath.brks;
		const vector<P2>& pths = ftpath.pths;
		int i0 = 0;
		for (int j = 0; j < brks.size(); j++) 
		{
			int n = brks[j] - i0;
			lines->InsertNextCell(n);
			for(int i = i0; i < brks[j]; i++) 
			{
 				P3 p = ConvertGZ(pths[i], ftpath.z); 
				int id = points->InsertNextPoint(p.x, p.y, p.z); 
				lines->InsertCellPoint(id);
			}

			i0 = brks[j];
		}
	}
	ftpolydata->SetPoints(&points);
	ftpolydata->SetLines(&lines);


	ftpolydata->Update(); 
	double* values = ftpolydata->GetBounds();
	xrg = I1(values[0], values[1]);
	yrg = I1(values[2], values[3]);
	zrg = I1(values[4], values[5]); 

	ftpolydataMap->SetInput(&ftpolydata);

	vo<vtkActor> actor;
	actor->SetMapper(&ftpolydataMap);
	actors->AddItem(&actor);

	// set front and back points of the range that gets rendered
	SetDrawWhole();        
}

/////////////////////////////////////////////////////////// 
void GSTsurface::PushTrianglesIntoSurface(SurfX& sx) 
{
	// make the list of triangles
	vtkPolyData* surf = stl->GetOutput(); 

	vtkCellArray* poly  = surf->GetPolys();
	int p_c = poly->GetNumberOfCells();
	poly->InitTraversal();
	vtkIdType npts;
	vtkIdType* pts;
	while (poly->GetNextCell(npts, pts)) 
	{
	    double* values; 

		values = surf->GetPoint(pts[0]); 
		P3 p0(values[0], values[1], values[2]); 
	    values = surf->GetPoint(pts[1]); 
		P3 p1(values[0], values[1], values[2]); 
		values = surf->GetPoint(pts[2]); 
		P3 p2(values[0], values[1], values[2]); 

		sx.PushTriangle(p0, p1, p2); 
	}
}


