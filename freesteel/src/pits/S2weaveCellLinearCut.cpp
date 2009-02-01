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
void S2weaveCellLinearCut::SetCellCut(const P2& lptcst, bool bOnBoundB, bool bOnBoundF, const P2& lvbearing)  
{
	ptcst = lptcst; 
	ASSERT(clurg.Contains(ptcst.u)); 
	ASSERT(clvrg.Contains(ptcst.v)); 

	vbearing = lvbearing; 
    vbqdrant = (((vbearing.u >= 0.0) == (vbearing.v >= 0.0)) ? 1 : 0) | (vbearing.v >= 0.0 ? 2 : 0); 
	apvb = APerp(vbearing); 
	ptcDapvb = Dot(ptcst, apvb); 

	SetCellCutBack(); 
	iblb = GetBoundListPosition(sicnb, ptcb, bOnBoundB); 
	SetCellCutFore(bOnBoundF); 
}

//////////////////////////////////////////////////////////////////////
void S2weaveCellLinearCut::SetCellCutFore(bool bOnBoundF)  
{
	// the near and far corners wrt the cut line
	ASSERT(Dot(GetCorner(vbqdrant), apvb) <= ptcDapvb); 
	ASSERT(Dot(GetCorner(vbqdrant + 2), apvb) >= ptcDapvb); 

	P2 poutdiag = GetCorner(vbqdrant + 3);
	double poutapvb = Dot(poutdiag, apvb);
	bool bOutCDiagFarSide = (poutapvb <= ptcDapvb);

	// vertical change case
	if (bOutCDiagFarSide == ((vbqdrant & 1) == 1)) 
	{
		lamcf = (poutdiag.v - ptcst.v) / vbearing.v;
        double wdpc = clurg.PushIntoSmall(ptcst.u + vbearing.u * lamcf); 

		ptcf = P2(wdpc, poutdiag.v);

		bool bDown = ((vbqdrant & 2) == 0);
		sicnf = (bDown ? 3 : 1); 
	}

	// horizontal change case
	else
	{
		lamcf = (poutdiag.u - ptcst.u) / vbearing.u; 
		double wdpc = clvrg.PushIntoSmall(ptcst.v + vbearing.v * lamcf); 

		ptcf = P2(poutdiag.u, wdpc);

		bool bLeft = (((vbqdrant + 1) & 2) != 0); 
		sicnf = (bLeft ? 0 : 2); 
	}

	// usually used with a valid back 
	ASSERT(lamcf >= lamcb); 
	ASSERT(sicnf != sicnb); 

	// check points on line-cut.  
	TOL_ZERO(Dot(ptcf, apvb) - ptcDapvb); 
	TOL_ZERO((ptcst + vbearing * lamcf - ptcf).Len()); 
	TOL_ZERO(Dot(ptcb, apvb) - ptcDapvb); 
	TOL_ZERO((ptcst + vbearing * lamcb - ptcb).Len()); 

	iblf = GetBoundListPosition(sicnf, ptcf, bOnBoundF); 
}


//////////////////////////////////////////////////////////////////////
void S2weaveCellLinearCut::SetCellCutBack()  
{
	// the near and far corners wrt the cut line
	ASSERT(Dot(GetCorner(vbqdrant), apvb) <= ptcDapvb); 
	ASSERT(Dot(GetCorner(vbqdrant + 2), apvb) >= ptcDapvb); 

	P2 pindiag = GetCorner(vbqdrant + 1);
	double pinapvb = Dot(pindiag, apvb);
	bool bInCDiagFarSide = (pinapvb <= ptcDapvb);

	// horizontal change case
	if (bInCDiagFarSide == ((vbqdrant & 1) == 1)) 
	{
		lamcb = (pindiag.u - ptcst.u) / vbearing.u; 
		double wdpc = clvrg.PushIntoSmall(ptcst.v + vbearing.v * lamcb); 

		ptcb = P2(pindiag.u, wdpc);

		bool bLeft = (((vbqdrant + 1) & 2) == 0);
		sicnb = (bLeft ? 0 : 2); 
	}

	// vertical change case
	else
	{
		lamcb = (pindiag.v - ptcst.v) / vbearing.v;
        double wdpc = clurg.PushIntoSmall(ptcst.u + vbearing.u * lamcb); 

		ptcb = P2(wdpc, pindiag.v);

		bool bDown = ((vbqdrant & 2) != 0);
		sicnb = (bDown ? 3 : 1); 
	}

	ASSERT(lamcb <= 0.0); // how it's used  
}

//////////////////////////////////////////////////////////////////////
void S2weaveCellLinearCut::AdvanceThroughForeCut(bool bOnBoundF)  
{
	bolistcrossings.clear(); 
	AdvanceCrossSide(sicnf, ptcf); 

	sicnb = ((sicnf + 2) & 3); 
	lamcb = lamcf; 
	ptcb = ptcf; 
	iblb = GetBoundListPosition(sicnb, ptcb, bOnBoundF); // this should be special to force consistency with cross-over 

	SetCellCutFore(false); 
}


//////////////////////////////////////////////////////////////////////
double S2weaveCellLinearCut::Getbolistcrossing(double& lambb, P2& ptcross, int ibb)  
{
	int ib = bolistcrossings[ibb].first; 
	pair<int, int> ibp = bolistpairs[ib]; 
	double lamc0 = Dot(apvb, GetBoundPoint(ibp.first)); 
	double lamc1 = Dot(apvb, GetBoundPoint(ibp.second)); 
	ASSERT(bolistcrossings[ibb].second ? ((lamc0 >= ptcDapvb) && (lamc1 <= ptcDapvb)) : ((lamc0 <= ptcDapvb) && (lamc1 >= ptcDapvb))); 

	lambb = (ptcDapvb - lamc0) / (lamc1 - lamc0); 
	ptcross = Along(lambb, GetBoundPoint(ibp.first), GetBoundPoint(ibp.second)); 

	TOL_ZERO(Dot(apvb, ptcross) - ptcDapvb); 
	double lamalong = Dot(ptcross - ptcst, vbearing); 
	TOL_ZERO(vbearing.Len() - 1.0); 
	TOL_ZERO((ptcst + vbearing * lamalong - ptcross).Len()); 
	TOL_ZERO(fabs(lamalong) - (ptcross - ptcst).Len()); 

	return lamalong; 
}


//////////////////////////////////////////////////////////////////////
void S2weaveCellLinearCut::FindBolistCrossings()  
{
	// this code can deal with an arbitrary number of crossings 
	// of the cutline, although the expected number is almost always 1, 
	// and rarely gets above 2.  

	// the crossings of the lines (if the input is not crossed up) 
	// can be deduced topologically, but many geomebtric checks are 
	// put into the debug area.  

	ASSERT(bolistcrossings.empty()); 
	if (bolistpairs.empty()) 
		return; 

	// everything is on one side of the line, so no 
	// chance of connecting.  This avoids also the 
	// flaw in the alg below which could place 
	// all the boundlists on the wrong side of the cutline  
	if (iblb == iblf) 
		return; 

	// three layers of dereferencing 
	// vector< pair<int, B1*> > boundlist;
	// vector< pair<int, int> > bolistpairs; 
	// vector< pair<int, bool> > bolistcrossings; 

	// ptcb to ptcf 
	// iblb and iblf determins what we are between 

	// when we work backwards popping from topbos, the values 
	// in bolistcrossings will be in the right order.  

	// vector< pair<int, int> > bolistpairs; 
	int ib = iblf; 
	DEBUG_ONLY(double lamcutposD); 
	while (ib != iblb) 
	{
		// get the direction. 
		bool bDownCut = GetBoundLower(ib); 

		// find other end of the cut 
		int i = bolistpairs.size() - 1; 
		for ( ; i >= 0; i--) 
			if ((bDownCut ? bolistpairs[i].second : bolistpairs[i].first) == ib) 
				break; 
		ASSERT(i >= 0); 
		int ibth = (bDownCut ? bolistpairs[i].first : bolistpairs[i].second); 
		
		// we have other end.  Is this a crossing or a loop on this side?  
		bool bloopcut; 
		if (iblf < iblb) 
		{
			ASSERT((ibth > ib) || (ibth < iblf)); 
			bloopcut = ((ibth >= iblf) && (ibth < iblb)); 
		}			
		else 
		{
			#ifdef MDEBUG
			if (iblf <= ib) 
				ASSERT((ibth > ib) || (ibth < iblf)); 
			else
				ASSERT((ibth > ib) && (ibth < iblf)); 
			#endif
			ASSERT((ibth > ib) || (ibth < iblf)); 
			bloopcut = ((ibth >= iblf) || (ibth < iblb)); 
		}

		#ifdef MDEBUG
		double Dlamc0 = Dot(apvb, GetBoundPoint(ib)); 
		double Dlamc1 = Dot(apvb, GetBoundPoint(ibth)); 
		#endif

		// go to other end of the loop 
		if (bloopcut) 
		{
			// check the loop and all it contains is on the lower side of the cut 
			#ifdef MDEBUG
			ASSERT((Dlamc0 <= ptcDapvb) && (Dlamc1 <= ptcDapvb)); 
			int Dlib = ib + 1; 
			if (Dlib == boundlist.size()) 
				Dlib = 0; 
			while (Dlib != ibth) 
			{
				bool DlbDownCut = GetBoundLower(Dlib); 
				int Dli = bolistpairs.size() - 1; 
				for ( ; Dli >= 0; i--) 
					if ((DlbDownCut ? bolistpairs[Dli].second : bolistpairs[Dli].first) == Dlib) 
						break; 
				ASSERT(Dli >= 0); 
				int Dlibth = (bDownCut ? bolistpairs[Dli].first : bolistpairs[Dli].second); 
				if (ib < ibth) 
					ASSERT((Dlibth > ib) && (Dlibth < ib)); 
				else
					ASSERT((Dlibth > ib) || (Dlibth < ib)); 

				// advance to next boundlist element.  
				Dlib++; 
				if (Dlib == boundlist.size()) 
					Dlib = 0; 
			}
			#endif
			
			ib = ibth; 
		}

		// we have a proper cut 
		else 
		{
			// check values and order, that we are retreating along the line, 
			// and that the cuts are alternating.  
			#ifdef MDEBUG
			ASSERT((Dlamc0 <= ptcDapvb) && (Dlamc1 >= ptcDapvb)); 
			double Dlamc = (ptcDapvb - Dlamc0) / (Dlamc1 - Dlamc0); 
			P2 Dptc = Along(Dlamc, GetBoundPoint(ib), GetBoundPoint(ibth)); 
			TOL_ZERO(Dot(apvb, Dptc) - ptcDapvb); 
			double NlamcutposD = (ptcst - Dptc).Len(); 
			if (!bolistcrossings.empty()) 
			{
				ASSERT(NlamcutposD <= lamcutposD); 
				ASSERT(bolistcrossings.back().second == !bDownCut); 
			}
			lamcutposD = NlamcutposD; 
			#endif

			bolistcrossings.push_back(pair<int, bool>(i, bDownCut)); 
		}

		// advance to next boundlist element.  
		ib++; 
		if (ib == (int)(boundlist.size())) 
			ib = 0; 
	}

	// bolistcrossings is the list of crossing lines in reverse order from b to f  

	// check directions known from the entry and exit cases 
	ASSERT(GetBoundLower(iblb) == !bolistcrossings.back().second); 
	ASSERT(GetBoundLower(iblf) == bolistcrossings.front().second); 
}


