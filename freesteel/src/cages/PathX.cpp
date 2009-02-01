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
void Ray_gen2::HoldFibre(S1* lpfib) 
{	
	pfib = lpfib; 
	ASSERT(scuts.empty()); 
}; 

//////////////////////////////////////////////////////////////////////
void Ray_gen2::ReleaseFibre()  
{
	if (!scuts.empty())
	{
		sort(scuts.begin(), scuts.end()); 
		ASSERT((scuts.size() % 2) == 0); 
		for (unsigned int i = 1; i < scuts.size(); i += 2) 
		{
			ASSERT(scuts[i - 1].blower && !scuts[i].blower); 
			pfib->Merge(scuts[i - 1].w, true, scuts[i].w, true); 
		}
		scuts.clear(); 
	}
	DEBUG_ONLY(pfib = NULL); 
}



//////////////////////////////////////////////////////////////////////
void Ray_gen2::LineCut(const P2& a, const P2& b)  
{
	// requires cclockwise paths
	if ((a.u < 0.0) != (b.u < 0.0)) 
	{
		double al = a.u / (a.u - b.u); 
		double lw = Along(al, a.v, b.v); 
		bool lblower = ((a.u < 0.0) != (pfib->ftype == 1)); // account for reflection 
scuts.push_back(B1(lw, !lblower)); 
	}
}






//////////////////////////////////////////////////////////////////////
// the semi-circle cap is on b
void Ray_gen2::DiscSliceCapN(const P2& a, const P2& b)  
{
	P2 d = b - a; 
	double dlen = d.Len(); 
	
	// find the cut alongs on the rectangle.  
	double lamm; 
	double lamd; 
	if (d.u != 0.0)
	{
		lamm = -a.u / d.u; 
		lamd = raddisc * d.v / (dlen * d.u); 
	}
	else
	{
		ASSERT(a.u == b.u); 
		if (fabs(a.u) >= raddisc) 
			return; 

		lamm = 0.5; 
		lamd = 1; 
	}
	double lamdp = fabs(lamd); 
	

	// off bottom entirely
	if (lamm + lamdp < 0.0) 
		return; 

	// find the intersections with the edges of the rectangle  

	// bottom end 
	double lvlo; 
	double llamlo = lamm - lamdp; 
	bool binterncellboundlo; 
	if (llamlo < 0.0) 
	{
		if (d.u != 0.0) 
		{
			ASSERT(lamdp != 0.0); 
			double mu = -a.u / d.v; 
			TOL_ZERO((I1(-1, 1) * (raddisc / dlen)).Distance(mu)); 
			lvlo = a.v - d.u * mu; 
		}
		else
			lvlo = a.v; 
		binterncellboundlo = true; 
	}
	else if (llamlo <= 1.0)
	{
		lvlo = a.v + d.v * llamlo - d.u * raddisc / dlen * (lamd < 0.0 ? -1 : 1); 
		binterncellboundlo = false; 
	}

	// now work at the top end of the piece, the one with the dome.  
	double lvhi; 
	double llamhi = lamm + lamdp; 
	if (llamhi > 1.0) 
	{
		double bdsq = raddiscsq - Square(b.u); 
		if (bdsq <= 0.0) 
		{
			ASSERT(llamlo > 1.0); 
			return; 
		}
		double bd = sqrt(bdsq) * (d.v > 0.0 ? 1 : -1); 
		lvhi = b.v + bd; 

		if (llamlo >= 1.0) 
			lvlo = b.v - bd; 
	}
		
	else 
	{
		ASSERT(llamhi >= 0.0); 
		lvhi = a.v + d.v * llamhi + d.u * raddisc / dlen * (lamd < 0.0 ? -1 : 1); 
	}

	// put the range in
	if (lvlo <= lvhi)
	{
		ASSERT(d.v >= 0.0); 
		pfib->Merge(lvlo, binterncellboundlo, lvhi, false); 
	}
	else
	{
		ASSERT(d.v <= 0.0); 
		pfib->Merge(lvhi, false, lvlo, binterncellboundlo); 
	}
}




//////////////////////////////////////////////////////////////////////
void HackAreaOffset(Ray_gen2& rgen2, const PathXSeries paths)  
{
	int j = 0;
	P2 tb; 
	bool bFirstPoint = true; 
	for (int i = 0; i < (int)(paths.pths.size()); i++) 
	{
		P2 ta = tb; 
		tb = rgen2.Transform(paths.pths[i]); 

		if ((j == (int)(paths.brks.size())) || (i < paths.brks[j]))
		{
			if (!bFirstPoint) 
			{
				rgen2.LineCut(ta, tb); 
				rgen2.DiscSliceCapN(ta, tb); 
			}
			else 
				bFirstPoint = false; 
		}

		// advance through possible multiple markings on this segment.  
		else
		{
			ASSERT(i == paths.brks[j]); 
			do
				j++;
			while ((j < (int)(paths.brks.size())) && (i == paths.brks[j])); 

			bFirstPoint = true; 
		}
	}
	ASSERT(rgen2.pfib->Check()); 
}


//////////////////////////////////////////////////////////////////////
void HackAreaOffset(S2weave& wve, const PathXSeries& paths, double rad)
{
    Ray_gen2 ryg2(rad);

    for (int iu = 0; iu < (int)wve.ufibs.size(); iu++)
    {
        ryg2.HoldFibre(&wve.ufibs[iu]);
        HackAreaOffset(ryg2, paths);
        ryg2.ReleaseFibre();
    }

    for (int iv = 0; iv < (int)wve.vfibs.size(); iv++)
    {
        ryg2.HoldFibre(&wve.vfibs[iv]);
        HackAreaOffset(ryg2, paths);
        ryg2.ReleaseFibre();
    }
}

//////////////////////////////////////////////////////////////////////
void HackToolpath(Ray_gen2& rgen2, const PathXSeries& pathxs, int iseg, const P2& ptpath)
{
	int j = 0;
	P2 tb; 
	bool bFirstPoint = true; 
	for (int i = 0; i < iseg; i++) 
	{
		P2 ta = tb; 
		tb = rgen2.Transform(pathxs.pths[i]); 

		if ((j == (int)(pathxs.brks.size())) || (i < pathxs.brks[j]))
		{
			if (!bFirstPoint) 
				rgen2.DiscSliceCapN(ta, tb); 
			else 
				bFirstPoint = false; 
		}

		// advance through possible multiple markings on this segment.  
		else
		{
			ASSERT(i == pathxs.brks[j]); 
			do
				j++;
			while ((j < (int)(pathxs.brks.size())) && (i == pathxs.brks[j])); 

			bFirstPoint = true; 
		}
	}
	
	if (iseg < (int)(pathxs.pths.size()))
	{
		ASSERT(!bFirstPoint);
		P2 ta = tb;
		tb = rgen2.Transform(ptpath); 
		rgen2.DiscSliceCapN(ta, tb); 
	}
	
	ASSERT(rgen2.pfib->Check()); 
}

//////////////////////////////////////////////////////////////////////
void HackToolpath(S2weave& wve, const PathXSeries& pathxs, int iseg, const P2& ptpath, double rad)
{
    Ray_gen2 ryg2(rad);

    for (int iu = 0; iu < (int)wve.ufibs.size(); iu++)
    {
        ryg2.HoldFibre(&wve.ufibs[iu]);
        HackToolpath(ryg2, pathxs, iseg, ptpath);
        ryg2.ReleaseFibre();
    }

    for (int iv = 0; iv < (int)wve.vfibs.size(); iv++)
    {
        ryg2.HoldFibre(&wve.vfibs[iv]);
        HackToolpath(ryg2, pathxs, iseg, ptpath);
        ryg2.ReleaseFibre();
    }
}






