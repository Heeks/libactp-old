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
//////////////////////////////////////////////////////////////////////






//////////////////////////////////////////////////////////////////////
void Ray_gen::BallSlice(const P3& a) 
{
	if (NormRay_gen::BallSlice(Transform(a)))
		pfib->Merge(reslo, binterncellboundlo, reshi, binterncellboundhi); 
}; 


//////////////////////////////////////////////////////////////////////
void Ray_gen::BallSlice(const P3& a, const P3& b) 
{
	P3 ta = Transform(a); 
	P3 tb = Transform(b); 
	bool bres = (ta.z < tb.z ? NormRay_gen::BallSlice(ta, tb) : NormRay_gen::BallSlice(tb, ta)); 
	if (bres) 
		pfib->Merge(reslo, binterncellboundlo, reshi, binterncellboundhi); 
}; 


//////////////////////////////////////////////////////////////////////
void Ray_gen::BallSlice(const P3& a, const P3& b1, const P3& b2)  
{
	P3 ta = Transform(a); 
	P3 tb1 = Transform(b1); 
	P3 tb2 = Transform(b2); 

	// not quite saving the value I'd hoped here
	P3 xprod = P3::CrossProd(tb1 - ta, tb2 - ta); 

	bool bres = (xprod.z >= 0.0 ? NormRay_gen::BallSlice(ta, tb1, tb2, xprod) : NormRay_gen::BallSlice(ta, tb2, tb1, -xprod)); 
	if (bres) 
		pfib->Merge(reslo, binterncellboundlo, reshi, binterncellboundhi); 
}



//////////////////////////////////////////////////////////////////////
void SurfX::SliceFibre(Ray_gen& rgen)  
{
	// points 
	for (int ip = 0; ip < (int)vdX.size(); ip++)
		rgen.BallSlice(vdX[ip]); 

    // edges
	for (int ie = 0; ie < (int)edX.size(); ie++)
		rgen.BallSlice(*(edX[ie].p0), *(edX[ie].p1)); 

	// faces
	for (int ic = 0; ic < (int)trX.size(); ic++)
		rgen.BallSlice(*(trX[ic].b12->p0), *(trX[ic].b12->p1), *(trX[ic].ThirdPoint())); 
}



//////////////////////////////////////////////////////////////////////
void SurfXboxed::SliceFibreBox(int iu, int iv, Ray_gen& rgen) 
{
	bucketX& bu = buckets[iu][iv]; 

	for (int ip = 0; ip < (int)bu.ckpoints.size(); ip++)
		rgen.BallSlice(*(bu.ckpoints[ip])); 

	for (int ie = 0; ie < (int)bu.ckedges.size(); ie++)
		rgen.BallSlice(*(bu.ckedges[ie].edx->p0), *(bu.ckedges[ie].edx->p1)); 

	for (int ic = 0; ic < (int)bu.cktriangs.size(); ic++) 
		rgen.BallSlice(*(bu.cktriangs[ic].trx->b12->p0), *(bu.cktriangs[ic].trx->b12->p1), *(bu.cktriangs[ic].trx->ThirdPoint())); 
}


//////////////////////////////////////////////////////////////////////
void SurfXboxed::SliceUFibre(Ray_gen& rgen)  
{
	ASSERT(rgen.pfib->ftype == 1); 

	// case of drop down to the underlying surfx -- could also do if outside the region.  
	// some detection of region limits and anything on the far side of them is needed.  
	if (buckets.empty()) 
	{
		psurfx->SliceFibre(rgen); 
		return; 
	}

	// make the urange strip we scan within 
	double r = rgen.radball + searchbox_epsilon; 
	I1 urg = I1(rgen.pfib->wp - r, rgen.pfib->wp + r); 
	if (urg.Intersect(gbxrg)) 
	{
		pair<int, int> iurg = xpart.FindPartRG(urg); 

		// could loop in a more optimal order 
		for (int iu = iurg.first; iu <= iurg.second; iu++) 
		{
			I1 vrg = rgen.pfib->wrg.Inflate(r); 
			if (vrg.Intersect(gbyrg)) 
			{
				pair<int, int> ivrg = yparts[iu].FindPartRG(vrg); 

				for (int iv = ivrg.first; iv <= ivrg.second; iv++) 
					SliceFibreBox(iu, iv, rgen); 
			}
		}
	}
}


//////////////////////////////////////////////////////////////////////
void SurfXboxed::SliceVFibre(Ray_gen& rgen) 
{
	ASSERT(rgen.pfib->ftype == 2); 

	// case of drop down to the underlying surfx -- could also do if outside the region.  
	// some detection of bGeoOut region limits and anything on the far side of them is needed.  
	if (buckets.empty()) 
	{
		psurfx->SliceFibre(rgen); 
		return; 
	}

	// make the vrange strip we scan within 
	double r = rgen.radball + searchbox_epsilon; 
	I1 urg = rgen.pfib->wrg.Inflate(r); 
	if (urg.Intersect(gbxrg)) 
	{
		pair<int, int> iurg = xpart.FindPartRG(urg); 

		// could loop in a more optimal order 
		for (int iu = iurg.first; iu <= iurg.second; iu++) 
		{
			I1 vrg = I1(rgen.pfib->wp - r, rgen.pfib->wp + r); 
			if (vrg.Intersect(gbyrg)) 
			{
				pair<int, int> ivrg = yparts[iu].FindPartRG(vrg); 

				for (int iv = ivrg.first; iv <= ivrg.second; iv++) 
					SliceFibreBox(iu, iv, rgen); 
			}
		}
	}
}




//////////////////////////////////////////////////////////////////////
void Area2_gen::FindInterior(SurfX& sx)  
{
	SLi_gen sgen; 
	vector<I1> res; 

	for (int iu = 0; iu < (int)ufibs.size(); iu++) 
	{
		sgen.SetSlicePos(P3(ufibs[iu].wp, vrg.lo, z), P3(ufibs[iu].wp, vrg.hi, z)); 
		sx.SliceRay(sgen); 
		sgen.Convert(res, urg, vrg, sx.gzrg); 

		while (!res.empty())
		{
			ufibs[iu].Merge(res.back());
			res.pop_back();
		}
	}

	for (int iv = 0; iv < (int)vfibs.size(); iv++) 
	{
		sgen.SetSlicePos(P3(urg.lo, vfibs[iv].wp, z), P3(urg.hi, vfibs[iv].wp, z)); 
		sx.SliceRay(sgen); 
		sgen.Convert(res, urg, urg, sx.gzrg); 

		while (!res.empty())
		{
			vfibs[iv].Merge(res.back());
			res.pop_back();
		}
	}
}



//////////////////////////////////////////////////////////////////////
void Area2_gen::SetSurfaceTop(SurfXboxed* lpsxb, double lr) 
{
	r = lr; 
	psxb = lpsxb; 
	z = psxb->psurfx->gzrg.hi; 
}


//////////////////////////////////////////////////////////////////////
void Area2_gen::HackDowntoZ(float lz) 
{
	ASSERT(lz <= z); 
	z = lz; 

	Ray_gen uryg(r, vrg);
	for (int iu = 0; iu < (int)ufibs.size(); iu++) 
	{
		uryg.HoldFibre(&ufibs[iu], z); 
		psxb->SliceUFibre(uryg); 
	}

	Ray_gen vryg(r, urg); 
	for (int iv = 0; iv < (int)vfibs.size(); iv++) 
	{
		vryg.HoldFibre(&vfibs[iv], z); 
		psxb->SliceVFibre(vryg); 
	}
}



//////////////////////////////////////////////////////////////////////





//////////////////////////////////////////////////////////////////////
void Area2_gen::MakeContours(PathXSeries& ftpaths)  
{
	firstcontournumber = lastcontournumber + 1; 

	S2weaveB1iter alscan; 
	alscan.ftype = 1; 
	for (alscan.ixwp = 0; alscan.ixwp < (int)ufibs.size(); alscan.ixwp++) 
	{
		alscan.wp = ufibs[alscan.ixwp].wp; 
		for (int i = 0; i < (int)ufibs[alscan.ixwp].size(); i++) 
		{
			alscan.w = ufibs[alscan.ixwp][i].w; 
			alscan.blower = ufibs[alscan.ixwp][i].blower; 

			if (ContourNumber(alscan) < firstcontournumber) 
			{
				vector<P2> contour;
				TrackContour(contour, alscan); 
				ftpaths.Append(contour);
				ftpaths.z = z;
			}
		}
	}
}
	



