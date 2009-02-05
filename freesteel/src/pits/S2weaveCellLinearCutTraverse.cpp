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
void S2weaveCellLinearCutTraverse::Findibbfore(int libb) 
{
	for (ibb = libb; ibb < (int)bolistcrossings.size(); ibb++) 
	{
		if (!bolistcrossings[ibb].second) 
		{
			lamcpbb = Getbolistcrossing(lambb, ptcpbb, ibb); 
			if (lamcpbb >= 0.0) 
				break; 
		}
		#ifdef MDEBUG
		else
		{
			// facing a down crossing; we are inside the area, and shouldn't be since we can't model being here  
			lamcpbb = Getbolistcrossing(lambb, ptcpbb, ibb); 
			ASSERT(lamcpbb <= 0.0); 
		}
		#endif
	}
	if (ibb == (int)(bolistcrossings.size())) 
		ibb = -1; 
}


//////////////////////////////////////////////////////////////////////
void S2weaveCellLinearCutTraverse::SetCellCutBegin(const P2& lptcst, const P2& lvbearing) 
{
	bolistcrossings.clear(); 
	SetCellCut(lptcst, false, false, lvbearing); 
	FindBolistCrossings(); 
	lamcp = 0.0; 
	ptcp = ptcst; 
	bOnContour = false; 
	ib = -1; 

	// go through the crossings till we find the one we are facing 
	Findibbfore(0); 
}


//////////////////////////////////////////////////////////////////////
bool VecBearingInwardCell(int sic, const P2& vbearing) 
{
	if (sic == 0) 
		return (vbearing.u >= 0.0); 
	if (sic == 2) 
		return (vbearing.u <= 0.0); 
	if (sic == 1) 
		return (vbearing.v <= 0.0); 
	ASSERT(sic == 3); 
		return (vbearing.v >= 0.0); 
}

//////////////////////////////////////////////////////////////////////
// continue but change direction 
// this handles leaving a contour, and continuing (with a change of direction) while not on a contour.  
// we could break these cases apart.  
bool S2weaveCellLinearCutTraverse::SetCellCutContinue(const P2& lvbearing) 
{
	// record the edge point we are on currently  
	ASSERT(bOnContour == (ib != -1)); 
	bolistcrossings.clear(); 

	// the bool says if we are going from a position at the corner 
	// at the beginning of a cell (should have a flag for this too).  

	// Calculate if our current position means the cut forward or the 
	// cut backwards will coincide with a contour point on the cell 
	// boundary, because it's on it already.  
	bool bOnBoundB = false; 
	bool bOnBoundF = false; 
	//int pib = ib; // unused variable
	if (bOnContour && EqualOr(lambb, 0.0, 1.0))  
	{
		// the going out case is then we we are on the bound on the 
		// edge of a cell, and are heading back across the cell boundary.  
		int sic = (lambb == 0.0 ? boundlist[bolistpairs[ib].first].first : boundlist[bolistpairs[ib].second].first); 
		if (VecBearingInwardCell(sic, lvbearing)) 
			bOnBoundB = true; 
		else
			bOnBoundF = true; 
	}

	// should pass in actual boundary indexes rather than these bools.  
	SetCellCut(ptcp, bOnBoundB, bOnBoundF, lvbearing); 

	// make some assertions to verify what we should have set topologically 
	// in the function above (but can't work out).  
	//ASSERT(!bOnBoundB || boundlist[bolistpairs[pib]].first : boundlist[bolistpairs[ib]].second); 

	ptcp = ptcst; 
	FindBolistCrossings(); 

	// contour condition is removed; we're going in another direction.  
	// but first recreate the 
	if (bOnContour) 
	{
		// these conditions assume that the bearing is away from the contour.  

		// we are on corner going out from contour
		// if this is so and the iblb is right, then it won't be represented in the list of crossings 
		if (lambb == 0.0)
		{
			#ifdef MDEBUG
			if (!bOnBoundF) 
			{
				for (int libb = 0; libb < (int)(bolistcrossings.size()); libb++) 
					ASSERT(ib != bolistcrossings[libb].first); 
			}
			#endif
			Findibbfore(0); 
		}

		// we are on middle of contour when we change and go out.  
		// we should see it in the list of crossings and move on.  
		else
		{
			int libb = 0;
			for (; libb < (int)(bolistcrossings.size()); libb++) 
				if (ib == bolistcrossings[libb].first) 
					break; 
			ASSERT(libb < (int)(bolistcrossings.size())); 
			#ifdef MDEBUG
			pair<int, int> ccrD = bolistpairs[bolistcrossings[libb].first];  
			TOL_ZERO((Along(lambb, GetBoundPoint(ccrD.first), GetBoundPoint(ccrD.second)) - ptcpbb).Len()); 
			#endif

			// facing a down crossing means we're going outwards 
			ASSERT(bolistcrossings[libb].second); 
			Findibbfore(libb + 1); 
		}

		bOnContour = false; 
		ib = -1; 
	}
	else
		Findibbfore(0); 


	lamcp = 0.0; 

	// ibb is index into bolistcrossings, or -1 if none.  
	// point must be on bound or outside area.  
	#ifdef MDEBUG
	if (ibb != -1) 
	{
		pair<int, int> ccrD = bolistpairs[bolistcrossings[ibb].first];  
		TOL_ZERO((Along(lambb, GetBoundPoint(ccrD.first), GetBoundPoint(ccrD.second)) - ptcpbb).Len()); 
	}
	#endif

	return bOnBoundF; 
}


//////////////////////////////////////////////////////////////////////
void S2weaveCellLinearCutTraverse::AdvanceThroughForeCutTraverse(bool bOnBoundF) 
{
	ASSERT(!bOnContour); // not suitable for this function  

	ASSERT(ibb == -1); 
	lamcp = lamcf; 
	ptcp = ptcf; 

	AdvanceThroughForeCut(bOnBoundF); 
	ASSERT(lamcp == lamcb); 
	ASSERT(ptcp == ptcb); 

	FindBolistCrossings(); 

	// we are in clear space so we can assume the first crossing will be down 
	if (!bolistcrossings.empty()) 
	{
		ibb = bolistcrossings.size() - 1; 
		ASSERT(!bolistcrossings[ibb].second); 
		lamcpbb = Getbolistcrossing(lambb, ptcpbb, ibb); 
		ASSERT(lamcpbb >= lamcb); 
	}
	else 
		ibb = -1; 
}


//////////////////////////////////////////////////////////////////////
void S2weaveCellLinearCutTraverse::AdvanceToLamPos(double llamcp) 
{
	ASSERT(!bOnContour); 
	ASSERT(I1(lamcb, lamcf).Contains(lamcp)); 
	ASSERT(llamcp >= lamcb); 
	TOL_ZERO((ptcst + vbearing * lamcp - ptcp).Len()); 
	ASSERT((ibb == -1) || (lamcpbb > llamcp)); 

	lamcp = llamcp; 
	double clam = I1(lamcb, lamcf).InvAlong(lamcp); 
	ptcp = Along(clam, ptcb, ptcf); // or use an AlongAcc.  
	TOL_ZERO((ptcst + vbearing * lamcp - ptcp).Len()); 
}


//////////////////////////////////////////////////////////////////////
void S2weaveCellLinearCutTraverse::AdvanceToContourCut()  
{
	ASSERT(!bOnContour); 
	ASSERT(I1(lamcb, lamcf).Contains(lamcp)); 
	TOL_ZERO((ptcst + vbearing * lamcp - ptcp).Len()); 
	ASSERT((ibb != -1) && (lamcpbb > lamcp)); 
	ib = bolistcrossings[ibb].first; 

	lamcp = lamcpbb; 
	ptcp = ptcpbb; 

	bolistcrossings.clear(); 
	bOnContour = true; 
	bContouribfvisited = false; // we join in the middle.  

	TOL_ZERO((ptcst + vbearing * lamcp - ptcp).Len()); 
	pair<int, int> ccr = bolistpairs[ib];  
	TOL_ZERO((Along(lambb, GetBoundPoint(ccr.first), GetBoundPoint(ccr.second)) - ptcp).Len()); 

	// set the bearing now 
	P2 vnl = GetBoundPoint(ccr.second) - GetBoundPoint(ccr.first); 
	double clen = vnl.Len(); 
	vbearing = vnl / clen; 
	apvb = APerp(vbearing); 
}


//////////////////////////////////////////////////////////////////////
void S2weaveCellLinearCutTraverse::AdvanceAlongContourToLamPos(double llambn) 
{
	ASSERT(bOnContour); 
	ASSERT(bolistcrossings.empty()); 
	ASSERT(I1(lambb, 1.0).Contains(llambn)); 
	pair<int, int> ccr = bolistpairs[ib];  
	TOL_ZERO((Along(lambb, GetBoundPoint(ccr.first), GetBoundPoint(ccr.second)) - ptcpbb).Len()); 
	
	lambb = llambn; 

	ptcpbb = Along(lambb, GetBoundPoint(ccr.first), GetBoundPoint(ccr.second)); 
	ptcp = ptcpbb; 
}

//////////////////////////////////////////////////////////////////////
void S2weaveCellLinearCutTraverse::AdvanceAlongContourAcrossCell() 
{
	ASSERT(bOnContour); 
	ASSERT(bolistcrossings.empty()); 
	#ifdef MDEBUG
	pair<int, int> ccrD = bolistpairs[ib];  
	TOL_ZERO((Along(lambb, GetBoundPoint(ccrD.first), GetBoundPoint(ccrD.second)) - ptcpbb).Len()); 
	#endif


	int ibl = bolistpairs[ib].second; 
	ptcpbb = GetBoundPoint(ibl); 
	ptcp = ptcpbb; 
	ASSERT(GetBoundLower(ibl)); 
	lambb = 0.0; 

	// cross over into the next cell 
	int sicc = boundlist[ibl].first; 
	const B1* blcp = boundlist[ibl].second; 

	boundlist.clear(); 
	bolistpairs.clear(); 
	AdvanceCrossSide(sicc, ptcpbb); 

	// find the point in the boundlist which matches the one we are crossing over on.  
	for (ibl = 0; ibl < (int)(boundlist.size()); ibl++) 
		if (boundlist[ibl].second == blcp) 
			break; 
	ASSERT(ibl < (int)(boundlist.size())); 
	ASSERT(boundlist[ibl].first == ((sicc + 2) & 3)); 

	// find the pair which leads on from this point 
	for (ib = 0; ib < (int)(bolistpairs.size()); ib++) 
		if (bolistpairs[ib].first == ibl) 
			break; 
	ASSERT(ib < (int)(bolistpairs.size())); 

	ASSERT(ptcpbb == GetBoundPoint(bolistpairs[ib].first)); 
	// iblb will be set properly on the peeling off.  
}


//////////////////////////////////////////////////////////////////////
bool S2weaveCellLinearCutTraverse::OnContourFollowBearing(double dch, double folldist)  
{
	// change direction and then follow along.  
	// dch is relative to apvb 
	ASSERT(ib != -1); 
	ASSERT(APerp(vbearing) == apvb); 
	P2 Nvbearing = vbearing + apvb * dch; 

	// if we're on contour, decide whether to stick with it.  
	ASSERT(bOnContour); 

	pair<int, int> ccr = bolistpairs[ib];  
	P2 vnl = GetBoundPoint(ccr.second) - GetBoundPoint(ccr.first); 

	// choose to leave the contour because we're not pushed in.  
	if (Dot(APerp(vnl), Nvbearing) > 0.0) 
		return false; 

	double clen = vnl.Len(); 
	vbearing = vnl / clen; 
	apvb = APerp(vbearing); 
	double llambn = lambb + folldist / clen; 

	if (llambn <= 1.0) 
	{
		AdvanceAlongContourToLamPos(llambn); 
	}
	else
	{
#ifdef MDEBUG
		double res = (1.0 - lambb) * clen;  // unused variable
		ASSERT(res <= folldist); 
#endif

		// we will cross this cell boundary.  Mark it if it's entirely cleared from the start.  
		if (bContouribfvisited) 
		{
			int ibl = bolistpairs[ib].second; 
			ASSERT(GetBoundLower(ibl)); 
			ASSERT(boundlist[ibl].second->cutcode == -1); 
			boundlist[ibl].second->cutcode = 0; 
		}
		AdvanceAlongContourAcrossCell(); 

		// in the next cell, the start has been visited.  
		bContouribfvisited = true; 
	}
	return true; 
}


//////////////////////////////////////////////////////////////////////
double S2weaveCellLinearCutTraverse::FollowBearing(const P2& lvbearing, double folldist)  
{
	TOL_ZERO(lvbearing.Len() - 1.0); 

	// change the angle we are going in.  
	bool bOnBoundF = SetCellCutContinue(lvbearing); 
	ASSERT(!bOnContour); 

	while (true) 
	{
		// there's a cut ahead 
		if (ibb != -1) 
		{
			ASSERT(!bOnBoundF); 
			if (folldist < lamcpbb) 
			{
				AdvanceToLamPos(folldist); 
				return folldist; 
			}
			AdvanceToContourCut(); 
			return lamcpbb; 
		}

		// nothing lies ahead in the way, stop when we want to 
		if (folldist < lamcf) 
		{
			ASSERT(!bOnBoundF); 
			AdvanceToLamPos(folldist); 
			return folldist; 
		}
		AdvanceThroughForeCutTraverse(bOnBoundF); 
		bOnBoundF = false; 
	}
}

