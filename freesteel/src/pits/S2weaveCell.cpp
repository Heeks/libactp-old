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
int FindCellParal(const vector<S1>& wfibs, double lw) 
{
	int res; 
	for (res = 1; res < (int)(wfibs.size()); res++) 
		if (wfibs[res].wp > lw) 
			break; 
	ASSERT((wfibs[res - 1].wp <= lw) && (wfibs[res].wp > lw)); 
	return res; 
}

//////////////////////////////////////////////////////////////////////
void S2weaveCell::FindCellIndex(const P2& lptc)  
{
	// set the index for this cell 
	iu = FindCellParal(ps2w->ufibs, lptc.u); 
	iv = FindCellParal(ps2w->vfibs, lptc.v); 

	ASSERT(I1(ps2w->ufibs[iu - 1].wp, ps2w->ufibs[iu].wp).Contains(lptc.u)); 
	ASSERT(I1(ps2w->vfibs[iv - 1].wp, ps2w->vfibs[iv].wp).Contains(lptc.v)); 

	// rebuild the rest of the cell info
	ConstructCellBounds(); 
	CreateBoundList();
}



//////////////////////////////////////////////////////////////////////
void S2weaveCell::ConstructCellBounds() 
{
	// this to be moved into the advance cross side stuff 
	// for better bonding.  
	pfulo = &(ps2w->ufibs[iu - 1]); 
	pfuhi = &(ps2w->ufibs[iu]); 
	pfvlo = &(ps2w->vfibs[iv - 1]); 
	pfvhi = &(ps2w->vfibs[iv]); 

	clurg.SetRan(pfulo->wp, pfuhi->wp); 
	clvrg.SetRan(pfvlo->wp, pfvhi->wp); 

    boundlist.clear();
	bolistpairs.clear(); 
}


//////////////////////////////////////////////////////////////////////
// it goes bottom left, top left, top right, bottom right.  
P2 S2weaveCell::GetCorner(int icn) const 
{
	bool buhi = ((icn & 2) != 0); 
	bool bvhi = (((icn + 1) & 2) != 0); 
	P2 res((buhi ? clurg.hi : clurg.lo), (bvhi ? clvrg.hi : clvrg.lo)); 
	return res; 
}


//////////////////////////////////////////////////////////////////////
const S1* S2weaveCell::GetSide(int icn) const 
{
	if ((icn & 2) == 0) 
		return ((icn & 1) == 0 ? pfulo : pfvhi); 
	return ((icn & 1) == 0 ? pfuhi : pfvlo); 
}


//////////////////////////////////////////////////////////////////////
// for now we assume it's all a regular grid.  
void S2weaveCell::AdvanceCrossSide(int icn, const P2& cspt)  
{
#ifdef MDEBUG
	double wvval = ((icn & 1) == 0 ? cspt.v : cspt.u);
	double mvval = ((icn & 1) != 0 ? cspt.v : cspt.u); 
#endif

	if (icn == 0) 
	{
		iu--; 
		ASSERT(iu > 0);
		ASSERT(mvval == clurg.lo); 
		ASSERT(clvrg.Contains(wvval)); 
	}
	else if (icn == 2) 
	{
		iu++; 
		ASSERT(iu < (int)(ps2w->ufibs.size()));
		ASSERT(mvval == clurg.hi); 
		ASSERT(clvrg.Contains(wvval)); 
	}
	else if (icn == 3) 
	{
		iv--; 
		ASSERT(iv > 0);
		ASSERT(mvval == clvrg.lo); 
		ASSERT(clurg.Contains(wvval)); 
	}
	else if (icn == 1) 
	{
		iv++; 
		ASSERT(iv < (int)(ps2w->vfibs.size()));
		ASSERT(mvval == clvrg.hi); 
		ASSERT(clurg.Contains(wvval)); 
	}
	else
		ASSERT(0); 

	// rebuild the rest of the cell info
	ConstructCellBounds(); 
	CreateBoundList();
}





//////////////////////////////////////////////////////////////////////
int S2weaveCell::GetBoundListPosition(int sic, const P2& ptb, bool bOnBoundOutside)  
{
	if (boundlist.empty()) 
		return -1; 
	int res = 0; 
	bool bgoingup = ((sic & 2) == 0); 
	bool binV = ((sic & 1) == 0); 

	double wb = (binV ? ptb.v : ptb.u); 
	ASSERT(GetSide(sic)->wp == (binV ? ptb.u : ptb.v)); 

	for ( ; res < (int)(boundlist.size()); res++) 
	{
		if (boundlist[res].first == sic)
		{
			// handle the coincident cases with warning and properly 
			if (boundlist[res].second->w == wb) 
			{
				ASSERT(bOnBoundOutside); 
				if (!GetBoundLower(res)) 
				{
					res++; 
					if (res == (int)boundlist.size()) 
						res = 0; 
				}
				else
					ASSERT(1); // rare case of doubling back from a corner through the previous cell, which we must see.  
				return res; 
			}
			if (bgoingup ? (boundlist[res].second->w >= wb) : (boundlist[res].second->w <= wb))  
			{
				ASSERT(!bOnBoundOutside); 
				return res; 
			}
		}
		else if (boundlist[res].first > sic)
			break; 
	}
	if (res == (int)boundlist.size()) 
		res = 0; 
	ASSERT(!bOnBoundOutside); 
	return res; 
}


//////////////////////////////////////////////////////////////////////
P2 S2weaveCell::GetBoundPoint(int ibl)  
{
	bool binV = ((boundlist[ibl].first & 1) == 0); 
	double wb = boundlist[ibl].second->w; 
	double wp = GetSide(boundlist[ibl].first)->wp; 
	return (binV ? P2(wp, wb) : P2(wb, wp)); 
}


//////////////////////////////////////////////////////////////////////
bool S2weaveCell::GetBoundLower(int ibl) 
{
	// this takes account of the sides 2 and 3 going in reverse.  
	return (((boundlist[ibl].first & 2) == 0) == boundlist[ibl].second->blower); 
}






//////////////////////////////////////////////////////////////////////
// we have some const_casts here so we can get at the 
bool AddBoundListMatches(vector< pair<int, B1*> >& boundlist, const S1& fw, const I1& rg, int edgno, bool bGoingDown, bool bStartIn) 
{
    ASSERT(((edgno & 2) != 0) == bGoingDown);
    pair<int, int> ilr = fw.Loclohi(rg);
    
    // pull-back from any boundaries that just cross.
    if ((ilr.first <= ilr.second) && fw[ilr.first].blower && (fw[ilr.first].w == rg.lo))
        ilr.first++;
    if ((ilr.first <= ilr.second) && !fw[ilr.second].blower && (fw[ilr.second].w == rg.hi))
        ilr.second--;
        
    // deal with the inside/outside info
    ASSERT(fw.empty() || fw.front().blower);
    ASSERT(fw.empty() || !fw.back().blower);
    bool bLeftIn = (ilr.first == 0 ? false : fw[ilr.first - 1].blower);
    bool bRightIn = (ilr.second == (int)(fw.size()) - 1 ? false : !fw[ilr.second + 1].blower);
    ASSERT(bStartIn == (bGoingDown ? bRightIn : bLeftIn));
    bool bEndIn = (bGoingDown ? bLeftIn : bRightIn);

    // feed in the indexes of the entries
    if (!bGoingDown)
    {
        for (int i = ilr.first; i <= ilr.second; i++)
        {
            boundlist.push_back(pair<int, B1*>(edgno, const_cast<B1*>(&(fw[i]))));
            ASSERT(rg.Contains(fw[i].w));
        }
    }
    else
    {
        for (int i = ilr.second; i >= ilr.first; i--)
        {
            boundlist.push_back(pair<int, B1*>(edgno, const_cast<B1*>(&(fw[i]))));
            ASSERT(rg.Contains(fw[i].w));
        }
    }

    return bEndIn;
}

//////////////////////////////////////////////////////////////////////
int S2weaveCell::CreateBoundList() 
{
    ASSERT(boundlist.empty());
    ASSERT(bolistpairs.empty());

    DEBUG_ONLY(bLDin = pfvlo->Contains(clurg.lo)); // final parameter only used for debug checking.
    bLUin = AddBoundListMatches(boundlist, *pfulo, clvrg, 0, false, bLDin);
    bRUin = AddBoundListMatches(boundlist, *pfvhi, clurg, 1, false, bLUin);
    bRDin = AddBoundListMatches(boundlist, *pfuhi, clvrg, 2, true, bRUin);
    bLDin = AddBoundListMatches(boundlist, *pfvlo, clurg, 3, true, bRDin);
    

	// for now, default resolve ambiguities, keeping the inside region connected.  
	// (not as likely if there has been a flat-rad offset where the region will always have some radius, but the spaces may be narrow).  
	DEBUG_ONLY(bool binD = bLDin); 
	int ib = boundlist.size() - 1; 
	for (int ibl = 0; ibl < (int)(boundlist.size()); ibl++) 
	{
		ASSERT(binD == !GetBoundLower(ibl)); 
		if (GetBoundLower(ibl)) 
		{
			ASSERT(!GetBoundLower(ib)); 
			bolistpairs.push_back(pair<int, int>(ib, ibl)); 
		}
		ib = ibl; 
		DEBUG_ONLY(binD = !binD); 
	}
	ASSERT(binD == bLDin); 
	ASSERT(bolistpairs.size() * 2 == boundlist.size()); 

    return bolistpairs.size();
}


