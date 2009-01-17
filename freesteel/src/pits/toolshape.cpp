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
#include "vtkFloatArray.h"
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

////////////////////////////////////////////////////////////////////////////////
double ToolShape::RadAtHeight(double lz)
{
    if (lz == 0.0)
        return flatrad;
    ASSERT(lz > 0.0);
    double zbc = cornerrad - lz;
    if (zbc <= 0.0)
        return flatrad + cornerrad;

    double crsq = Square(cornerrad) - Square(zbc);
    return flatrad + PosSqrt(crsq);
}

////////////////////////////////////////////////////////////////////////////////
void ToolShape::ConstructToolShape(vector< vector <P3> >& slcs)
{
    ASSERT(slcs.empty());
    slcs.resize(ntoolslices);

    for (int i = 0; i < ntoolslices; i++)
    {
        double lz = sliceheight * i;
        double rad = RadAtHeight(lz);

        slcs[i].resize(nang + 1);
        for (int j = 0; j < nang; j++)
        {
            double thet = j * M2PI / nang;
            P2 rp = P2(cos(thet), sin(thet)) * rad;
            slcs[i][j] = ConvertGZ(rp, lz);
        }
        slcs[i][nang] = slcs[i][0];
    }
}


////////////////////////////////////////////////////////////////////////////////
void ToolShape::ConstructToolShape(vtkPolyData* ppd, vtkFloatArray* pdcontactsca)
{
    vo<vtkPoints> points;
    vo<vtkCellArray> lines;

    for (int i = 0; i < ntoolslices; i++)
    {
        double lz = sliceheight * i;
        double rad = RadAtHeight(lz);

        lines->InsertNextCell(nang + 1);
        int idfirst;
        for (int j = 0; j < nang; j++)
        {
            double thet = j * M2PI / nang;
            P2 rp = P2(cos(thet), sin(thet)) * rad;
            int id = points->InsertNextPoint(rp.u, rp.v, lz);
            if (j == 0)
                idfirst = id;
            lines->InsertCellPoint(id);
        }
        lines->InsertCellPoint(idfirst);
    }

    ppd->SetPoints(&points);
    ppd->SetLines(&lines);

    // setup the float array
    if (pdcontactsca != NULL)
    {
        ASSERT(points->GetNumberOfPoints() == ntoolslices * nang);
        pdcontactsca->SetNumberOfValues(ntoolslices * nang);

        for (int i1 = 0; i1 < ntoolslices; i1++)
        {
            for (int j = 0; j < nang; j++)
                pdcontactsca->SetValue(i1 * nang + j, ((j + i1) % nang) * 1.0 / nang);
        }

        ppd->GetPointData()->SetScalars(pdcontactsca);
    }
}

////////////////////////////////////////////////////////////////////////////////
void ToolShape::Draw(const P3& iptpath, vector<PathXSeries>* pftpaths, int ipath, int iseg, const vector<P2>& bound)
{
    glDisable(GL_LIGHTING);

    for (int i = 0; i < ntoolslices; i++)
    {
        double lz = sliceheight * i;
        double rad = RadAtHeight(lz);

        // model the crossing on this slice
        CircCrossingStructure ccs(ConvertLZ(iptpath), rad);
        ccs.ChopOutBoundary(bound); // creates circrange

        for (int ip = 0; ip <= ipath; ++ip)
        {
			PathXSeries& pathxs = (*pftpaths)[ip];
			
            double relz = lz + (*pftpaths)[ipath].z - pathxs.z;
            if (relz < 0.0)
                continue;
            double relrad = RadAtHeight(relz);

            vector<P2>& path = pathxs.pths;
            ccs.SetPrad(relrad); // the cutting radius

            bool bAnim = (ip == ipath);
            int iterm = bAnim ? (iseg + 1) : (path.size());
            P2 p1 = path.front() - ccs.cpt;
            for (int i = 1; i < iterm; i++)
            {
                // is our cleared?
                if (ccs.circrange.empty())
                    break;

                P2 p0 = p1;
                p1 = ((bAnim && (i == iseg)) ? ConvertLZ(iptpath) : path[i]) - ccs.cpt;

                ccs.HackToolRectangle(p0, p1);
                if (!bAnim || (i != iseg))
                    ccs.HackToolCircle(p1);
            }
        }
		
		DrawColourRange(iptpath, lz, rad, ccs.circrange);
	}
}

void ToolShape::Draw(const P3& iptpath)
{
    for (int i = 0; i < ntoolslices; i++)
    {
        double lz = sliceheight * i;
        double rad = RadAtHeight(lz);

		DrawColourRange(iptpath, lz, rad, S1());
	}
}


void ToolShape::DrawColourRange(const P3& iptpath, double lz, double rad, const S1& srg)
{
	glColor3d(1.0, 1.0, 1.0);
	glBegin(GL_LINE_STRIP);
	bool bInside = false;
	P2 rplast(rad, 0);
	for (int j = 0; j < nang; j++)
	{
		double thet = j * M2PI / nang;
		P2 rp = P2(cos(thet), sin(thet)) * rad;
		double thdarg = rad == 0.0 ? 0.0 : rp.DArg();

		bool bInNow = srg.Contains(thdarg);
		if (bInNow != bInside)
		{
			double lthdarg = bInNow ? thdarg : (rad == 0.0 ? 0.0 : rplast.DArg());
                
			I1 rg = srg.ContainsRG(lthdarg);
			lthdarg = rg.PushInto(lthdarg);
			double lam = rg.Leng() == 0.0 ? 0.0 : rg.InvAlong(lthdarg);
			ASSERT(I1unit.Contains(lam));
			P3 pt = ConvertGZ(Along(lam, rplast, rp), lz) + iptpath;
			glVertex3d(pt.x, pt.y, pt.z);
                    
			glEnd();
			bInNow ? glColor3d(1.0, 0.0, 0.0) : glColor3d(1.0, 1.0, 1.0);
			glBegin(GL_LINE_STRIP);
			glVertex3d(pt.x, pt.y, pt.z);
		}
		
		P3 pt = P3(rp.u, rp.v, lz) + iptpath;
		glVertex3d(pt.x, pt.y, pt.z);            

		bInside = bInNow;
		rplast = rp;

	}

	glVertex3d(rad + iptpath.x, iptpath.y, lz + iptpath.z);
	glEnd();
}





