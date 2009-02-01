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

#include "bolts/bolts.h"
#include "cages/cages.h"
#include "pits/pits.h"


//////////////////////////////////////////////////////////////////////
void PathXboxed::BuildBoxes(const I1& lgburg, double boxwidth)  
{
	gburg = lgburg; 
	bGeoOutLeft = false; 
	bGeoOutRight = false; 
	upart = Partition1(gburg, boxwidth); 
	puckets.resize(upart.NumParts()); 

	ASSERT(idups.empty()); 
	maxidup = 0; 

	// assume that this path is empty and we are adaptively growing it.  
	// in future we will do this properly.  
	ASSERT(ppathx->pths.empty()); 
}


//////////////////////////////////////////////////////////////////////
void PathXboxed::Break()  
{
//	ppathx->brks.push_back(ppathx->pths.size());
	ppathx->Break();
}

//////////////////////////////////////////////////////////////////////
double PTcrossU(double lu, P2& p0, P2& p1) 
{
	ASSERT(p0.u <= p1.u); 
	if (lu <= p0.u) 
		return p0.v; 
	if (lu >= p1.u) 
		return p1.v; 

	double lamu = InvAlong(lu, p0.u, p1.u); 
	return Along(lamu, p0.v, p1.v); 
}


//////////////////////////////////////////////////////////////////////
// we should be putting segments expanded by their prad into here
// and doing the points and the rectangles in separate box components.  
void PathXboxed::PutSegment(int iseg, bool bFirst, bool bRemove) 
{
	// do the point addition 
	// if we get the next part of the arc, we can narrow it down a lot.  
	P2& pp = ppathx->pths[iseg]; 
	if (pp.u < gburg.lo) 
		bGeoOutLeft = true; 
	if (pp.u > gburg.hi) 
		bGeoOutRight = true; 
	else 
	{
		ASSERT(gburg.Contains(pp.u)); 
		int iu = upart.FindPart(pp.u); 
		ASSERT(iu >= 0 && iu < (int)puckets.size());
		puckets[iu].ckpoints.push_back(iseg); 
	}

	// quit if no line to be added with this.  
	if (bFirst) 
		return; 

	ASSERT(iseg != 0); 

	// iseg should not be in brk
	bool bincx = (ppathx->pths[iseg - 1].u <= pp.u); 
	P2& p0 = (bincx ? ppathx->pths[iseg - 1] : pp); 
	P2& p1 = (bincx ? pp : ppathx->pths[iseg - 1]); 
	I1 urg(p0.u, p1.u); 
	if (!urg.Intersect(gburg)) 
		return; 

	pair<int, int> iurg = upart.FindPartRG(urg); 

	// take away this index from each of the strips 
	if (bRemove)
	{
		for (int iu = iurg.first; iu <= iurg.second; iu++) 
		{
			if (puckets[iu].cklines.back().iseg == iseg) 
				puckets[iu].cklines.pop_back(); 
			else
			{
				ASSERT(0); // get it out somewhere in the middle / must have been sorted into it.  
			}
		}

		// could handle the leaking of idups values, but won't.  
		return; 
	}


	// decide if we will find duplicates.  
	int idup = -1; 
	if(iurg.first != iurg.second) 
	{
		idup = idups.size(); 
		idups.push_back(0); 
	}

	// loop across the strips now.  
	double v1 = PTcrossU(upart.GetPart(iurg.first).lo, p0, p1);  
	for (int iu = iurg.first; iu <= iurg.second; iu++) 
	{
		double v0 = v1; 
		v1 = PTcrossU(upart.GetPart(iu).hi, p0, p1);  
		puckets[iu].cklines.push_back(ckpline(iseg, idup, Half(v0, v1), fabs(v1 - v0) / 2)); 
	}
}



//////////////////////////////////////////////////////////////////////
void PathXboxed::Add(const P2& p1) 
{
	// if this is the starting point then nothing to do.  
	bool bFirst = (ppathx->pths.empty() || (!ppathx->brks.empty() && (ppathx->brks.back() == (int)(ppathx->pths.size())))); 
	ppathx->pths.push_back(p1); 
	PutSegment(ppathx->pths.size() - 1, bFirst, false);  
}


//////////////////////////////////////////////////////////////////////
void PathXboxed::Pop_back()  
{
	ASSERT(1); // nothing for now.  
}

