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

#ifndef FSTOOLPATHMAPPER_H
#define FSTOOLPATHMAPPER_H

#include "bolts/bolts.h"
#include "cages/cages.h"
#include <vector>

class ToolShape;

struct AnimatedPos
{
	int ipathx; // the path we are on
	P3 ptOnLink;
	int ilink;
	int isegOnLink;

	bool bOnPath; // true if the current position is on a toolpath and potentially cutting
	P2 ptOnPath;
	int isegOnPath;
};

/**
  This class maps the toolpath to the display, and supports showing only an 
  interval of the toolpath, as needed for animation. This is done without 
  copying the actual data.
*/

class fsvtkToolpathMapper : public vtkOpenGLPolyDataMapper
{
public:
		static fsvtkToolpathMapper *New();

		vector<PathXSeries>* pftpaths;
		ToolShape* ptoolshape;
		PathXSeries* bound;
		I1 xrg, yrg;
		S2weave stockweave;


	// this mark out a visible interval of the ftpaths.  
	// going to
		AnimatedPos pos;

        // starting from 
		AnimatedPos poslast;

        bool bAnimated, bForward;
        bool bDrawStock;
        
		int Draw(vtkRenderer *aren, vtkActor *act);
        void DrawStock();
        void DrawFibre(S1& fib, I1& wrg, double z);
        void CalculateStock(double zstock);

protected:
		fsvtkToolpathMapper();
		~fsvtkToolpathMapper();

private:
		fsvtkToolpathMapper(const fsvtkToolpathMapper&);  // Not implemented.
		void operator=(const fsvtkToolpathMapper&);  // Not implemented.

		int DrawPathSegment();
		int DrawToolShape(const P3& pos);
};

#endif
