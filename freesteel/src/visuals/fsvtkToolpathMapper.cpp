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

#include "vtkOpenGLPolyDataMapper.h"
#include "visuals/fsvtkToolpathMapper.h"

#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkCommand.h"
#include "vtkDataArray.h"
#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"
#include "vtkOpenGLRenderer.h"
#include "vtkPlane.h"
#include "vtkPlaneCollection.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkPolygon.h"
#include "vtkProperty.h"
#include "vtkTimerLog.h"
#include "vtkTriangle.h"
#include "vtkOpenGLRenderWindow.h"
#include "vtkSTLReader.h"
#include "bolts/bolts.h"
#include "cages/cages.h"
#include "pits/pits.h"

#include "visuals/gstsurface.h"


#ifndef VTK_IMPLEMENT_MESA_CXX
  #if defined(__APPLE__) && (defined(VTK_USE_CARBON) || defined(VTK_USE_COCOA))
    #include <OpenGL/gl.h>
  #else
    #include <GL/gl.h>
  #endif
#endif



#ifndef VTK_IMPLEMENT_MESA_CXX
vtkStandardNewMacro(fsvtkToolpathMapper);
#endif

// Construct empty object.
fsvtkToolpathMapper::fsvtkToolpathMapper() : 
	pftpaths(NULL), ptoolshape(NULL), bound(NULL)
{
	ImmediateModeRenderingOn();
        bDrawStock = false;
        bAnimated = false;
        bForward = false;
}

// Destructor (don't call ReleaseGraphicsResources() since it is virtual
fsvtkToolpathMapper::~fsvtkToolpathMapper()
{
}

int fsvtkToolpathMapper::Draw(vtkRenderer *aren, vtkActor *act) 
{
	// the old renderer.  
	//vtkOpenGLPolyDataMapper::Draw(aren, act); 

	if (pftpaths == NULL) 
		return 1; 

    if (bAnimated && bForward)
    {
        return DrawPathSegment(); 
    }
    else
        return DrawPathSegment(); 
}

/////////////////////////////////////////////////////////////////////////////////
// draw from last drawn toolpath(lxp), last drawn segment(lxs), up to last point
// (lptpath) to path ixp, segment ixs, point pt;
int fsvtkToolpathMapper::DrawPathSegment()
{
	glDisable(GL_LIGHTING);

	ASSERT(pftpaths	!= NULL);
	for (int ip = poslast.ipathx; ip <= pos.ipathx; ++ip)
	{
		PathXSeries& pathxs = (*pftpaths)[ip];
		bool bFinal = (ip == pos.ipathx) && bAnimated;

		int j = 0; 
		if (!pathxs.pths.empty())
		{
			glColor3d(0.09,	0.91, 0.92);
			glBegin(GL_LINE_STRIP);
			glVertex3d(pathxs.pths[0].u, pathxs.pths[0].v, pathxs.z);	
		}
		for (int i = 1; (i < (bFinal ? pos.isegOnPath : pathxs.pths.size())); i++) 
		{
			if ((j == pathxs.brks.size()) || (i < pathxs.brks[j]))
				;

			// advance through possible multiple markings on this segment.  
			else
			{
				glEnd();
				glBegin(GL_LINE_STRIP);

				ASSERT(i == pathxs.brks[j]); 
				do
				{
					j++;
				}
				while ((j < pathxs.brks.size()) && (i == pathxs.brks[j])); 
			}
			glVertex3d(pathxs.pths[i].u, pathxs.pths[i].v, pathxs.z);	
		}

		if (bFinal && !pathxs.pths.empty())
			glVertex3d(pos.ptOnPath.u, pos.ptOnPath.v, pathxs.z);	

		if (!pathxs.pths.empty())
			glEnd();

			
		// draw the	retracts	
		ASSERT(pathxs.linkpths.size() == pathxs.brks.size());
		glColor3d(0.09,	0.91, 0.02); 
		for (int jl = 0; (jl < (bFinal ? pos.ilink : pathxs.brks.size())); jl++) 
		{
			const vector<P3>& lnkpth = pathxs.linkpths[jl];
			if (!lnkpth.empty())
			{
				glBegin(GL_LINE_STRIP);
				for (int ilp = 0; ilp < lnkpth.size(); ++ilp)
					glVertex3d(lnkpth[ilp].x, lnkpth[ilp].y, lnkpth[ilp].z);	
				glEnd();
			}
		}
		if (bFinal && (pos.ilink < pathxs.brks.size()))
		{
			const vector<P3>& lnkpth = pathxs.linkpths[pos.ilink];
			if (!lnkpth.empty())
			{
				glBegin(GL_LINE_STRIP);
				ASSERT(pos.isegOnLink <= lnkpth.size());
				for (int ilp = 0; ilp < pos.isegOnLink; ++ilp)
					glVertex3d(lnkpth[ilp].x, lnkpth[ilp].y, lnkpth[ilp].z);	
				glVertex3d(pos.ptOnLink.x, pos.ptOnLink.y, pos.ptOnLink.z);	
				glEnd();
			}
		}
	}

	if (bAnimated && ptoolshape)
	{
		if (pos.bOnPath)
			ptoolshape->Draw(ConvertGZ(pos.ptOnPath, (*pftpaths)[pos.ipathx].z), pftpaths, pos.ipathx, pos.isegOnPath,  bound->pths);
		else
			ptoolshape->Draw(pos.ptOnLink);
	}

	if (bDrawStock)
	{
		DrawStock();
	}

	return 1; 
}

void fsvtkToolpathMapper::CalculateStock(double zstock)
{
	double zpath = (*pftpaths)[pos.ipathx].z;
    double rad = ptoolshape->RadAtHeight(zstock - zpath);
    
    stockweave.SetShape(xrg, yrg, .5);
    HackAreaOffset(stockweave, *bound, 0);

    // inverse weave
    stockweave.Invert();

    // hack in toolpath up to position
	// hack in up to point in animation, but exclude any retract
	/*
	for (int ix = 0; ix < pos.ipathx; ++ix)
	{
		const PathXSeries& pathxs = (*pftpaths)[ix];
		HackToolpath(stockweave, pathxs, pathxs.pths.size(), P2(), rad); 
	}
	*/ 
	
	ASSERT(pos.ipathx < pftpaths->size());  
    HackToolpath(stockweave, (*pftpaths)[pos.ipathx], pos.isegOnPath, pos.ptOnPath, rad);    

    // inverse weave
    stockweave.Invert();
}

///////////////////////////////////////////////////////////
void fsvtkToolpathMapper::DrawFibre(S1& fib, I1& wrg, double z)
{
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < fib.size(); i++)
    {
        if ((i % 2) == 0)
        {
            glEnd();
            glBegin(GL_LINE_STRIP);
        }
        //P2 p = ((fib.value_type == 1) ? P2(fib[i].w, fib.wp) : P2(fib.wp, fib[i].w));
        P2 p = ((fib.ftype == 2) ? P2(fib[i].w, fib.wp) : P2(fib.wp, fib[i].w));
        glVertex3d(p.u, p.v, z);
    }

    glEnd();
}

void fsvtkToolpathMapper::DrawStock()
{
    glDisable(GL_LIGHTING);
    glColor3d(0.91, 0.09, 0.92);

    
	// do the fibres in each direction
    for (int iu = 0; iu < stockweave.ufibs.size(); iu++)
        DrawFibre(stockweave.ufibs[iu], stockweave.vrg, (*pftpaths)[pos.ipathx].z);
    for (int iv = 0; iv < stockweave.vfibs.size(); iv++)
        DrawFibre(stockweave.vfibs[iv], stockweave.urg, (*pftpaths)[pos.ipathx].z);
}

///////////////////////////////////////////////////////////
void BadGenBound(PathX& pxstockbound)
{
    for (int j = -10; j < 10; j++)
    {
        double th = M2PI * j / 30;
        pxstockbound.pth.push_back(P2(1.5, 1.5) + P2(cos(th), sin(th)) * 1.5);
    }
    pxstockbound.pth.push_back(pxstockbound.pth.back() - P2(1.3, 0.0));
    pxstockbound.pth.push_back(pxstockbound.pth.front() + P2(0.0, 1.0));
    pxstockbound.pth.push_back(pxstockbound.pth.front());
}

/*
///////////////////////////////////////////////////////////
void MakeStock(PathX& px)
{
    S2stock s2s;
    PathX pxstockbound;
    BadGenBound(pxstockbound);

    I1 xrg(-1, 4);
    I1 yrg(-1, 4);

    s2s.SetShape(xrg, yrg, 0, 0.05, &pxstockbound);
    double trad = 0.187;
    if (!px.pth.empty())
        s2s.HackToolpath(&px, trad);
}
*/


/*
int fsvtkToolpathMapper::DrawToolShape(const P3& pos)
{
    if ((ptsslices == NULL) || (ptsslices->empty()))
        return 1;

    glDisable(GL_LIGHTING);
    glColor3d(1.0, 1.0, 1.0);

    for (int i = 0; i < ptsslices->size(); ++i)
    {
        glBegin(GL_LINE_STRIP);
        vector<P3>& slice = (*ptsslices)[i];
        for (int j = 0; j < slice.size(); ++j)
            glVertex3d(slice[j].x + pos.x, slice[j].y + pos.y, slice[j].z + pos.z);

        glEnd();
    }

    return 1;
}
*/


