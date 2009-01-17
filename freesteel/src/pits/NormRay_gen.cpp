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
void Ray_gen::HoldFibre(S1* lpfib, double lz) 
{
	pfib = lpfib; 
	ASSERT(zrg == pfib->wrg); 
	z = lz; 
}; 

//////////////////////////////////////////////////////////////////////
bool NormRay_gen::TrimToZrg()  
{
	if (reslo < zrg.lo)
	{
		reslo = zrg.lo; 
		binterncellboundlo = false; 
	}
	if (reshi > zrg.hi)
	{
		reshi = zrg.hi; 
		binterncellboundhi = false; 
	}
	return (reslo <= reshi); 
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// single point 
bool NormRay_gen::BallSlice(const P3& a)  
{
	if (zrg.Distance(a.z) >= radball) 
		return false; 

	double ausq = radballsq - ConvertLZ(a).Lensq(); 
	if (ausq < 0.0) 
		return false; 

	double au = sqrt(ausq); 

	reslo = a.z - au; 
	binterncellboundlo = false; 
	reshi = a.z + au; 
	binterncellboundhi = false; 

	return TrimToZrg(); 
};


//////////////////////////////////////////////////////////////////////
// a line segment 
// intersect cylinder around line segment with z-axis
bool NormRay_gen::BallSlice(const P3& a, const P3& b)  
{
	ASSERT(a.z <= b.z); // must be this way round.  
	if ((b.z + radball < zrg.lo) || (a.z - radball > zrg.hi)) 
		return false; 

	// considered in 2D, this is a line segment that passes within radball of the origin.  
	P3 v = b - a; 

	double vfsq = ConvertLZ(v).Lensq(); 
	double vzsq = Square(v.z); 
	double vsq = vfsq + vzsq; 

	double afDvf = Dot(ConvertLZ(a), ConvertLZ(v)); 
	double aDv = afDvf + a.z * v.z; 
	double aDvsq = Square(aDv);

    double aDpv = Dot(ConvertLZ(a), APerp(ConvertLZ(v)));
    double lcsq = Square(aDpv) / vsq;
    if (lcsq >= radballsq)
        return false;

  


	// special case of horizontal edge
    // would like to make this case continuous in its calculation.
	if ((v.z == 0.0) || (fabs(v.z) < 1e-8))
	{
		double lam = -afDvf / vfsq; 
		if (!I1unit.Contains(lam)) 
			return false; 

		P2 la = ConvertLZ(a) + ConvertLZ(v) * lam; 
		TOL_ZERO(Dot(la, ConvertLZ(v)));
        TOL_ZERO(lcsq - la.Lensq());
		double ausq = radballsq - la.Lensq(); 
		if (ausq < 0.0) 
			return false; 

		double au = sqrt(ausq); 
		reslo = a.z - au; 
		binterncellboundlo = false; 
		reshi = a.z + au; 
		binterncellboundhi = false; 
		return TrimToZrg();   
	}

	// special case of vertical edge; the line only passes through the raw faces.  
	if ((vfsq == 0.0) || (vfsq < 1e-20))
	{
        double adsq = ConvertLZ(a).Lensq();
        if (adsq >= radballsq)
            return false;
            
        reslo = a.z; 
		binterncellboundlo = true; 
		reshi = b.z; 
		binterncellboundhi = true; 
		return TrimToZrg(); 
	}
	

	double qa = vsq * vfsq; 
	double qb2 = afDvf * vsq; 

	ASSERT(qa > 0.0); 

	double qc = -vzsq * radballsq + vzsq * a.Lensq() - 2 * a.z * v.z * aDv + aDvsq; 
	double lamsq = Square(qb2) - qa * qc; 
	if (lamsq < 0.0)
		return false; 
	double lamr = sqrt(lamsq) / qa; 


	double lammid = -afDvf / vfsq; 
	I1 llam(lammid - lamr, lammid + lamr); 

	TOL_ZERO(qa * Square(llam.lo) + 2 * qb2 * llam.lo + qc); 

	#ifdef MDEBUG
    if (fabs(v.z) > 1e-3)
    {
		double lz = (aDv + llam.lo * vsq) / v.z; 
		P3 vv = a + v * llam.lo - P3(0, 0, lz); 
		TOL_ZERO(Dot(vv, v)); 
		TOL_ZERO(vv.Len() - radball); 
	}
	#endif

	// truncate at the ends of the cylinder  
	if (!llam.Intersect(I1unit)) 
		return false; 



	// lo end on the z-axis
	binterncellboundlo = (llam.lo == 0.0); 
	if  (!binterncellboundlo)
	{
		P2 la = ConvertLZ(a) + ConvertLZ(v) * llam.lo; 
		double ausq = radballsq - la.Lensq(); 
		double vzzm = a.z + v.z * llam.lo; 
		double vzzu = PosSqrt(ausq); 
		reslo = vzzm - vzzu; 
		TOL_ZERO((aDv + llam.lo * vsq) - reslo * v.z); 
	}
	else 
		reslo = aDv / v.z; 



	// hi end on the z-axis
	binterncellboundhi = (llam.hi == 1.0); 
	if  (!binterncellboundhi)  
	{
		P2 la = ConvertLZ(a) + ConvertLZ(v) * llam.hi; 
		double ausq = radballsq - la.Lensq(); 
		double vzzm = a.z + v.z * llam.hi; 
		double vzzu = PosSqrt(ausq); 
		reshi = vzzm + vzzu; 
		TOL_ZERO((aDv + llam.hi * vsq) - reshi * v.z); 
	}

	// just find the value in the plane of the disc 
	else 
		reshi = (aDv + vsq) / v.z; 

	// alters res, and returns if intersection is non-empty.  
	return TrimToZrg(); 
}



//////////////////////////////////////////////////////////////////////
bool NormRay_gen::BallSlice(const P3& a, const P3& b1, const P3& b2, const P3& xprod)  
{
	ASSERT(xprod.z >= 0.0); 

	P3 norm = xprod / xprod.Len(); 
	P3 rnorm = norm * radball; 

	// cull out by height.  
	I1 tz; 
	tz.Combine(a.z, b1.z, b2.z); 
	if ((tz.hi + radball < zrg.lo) || (tz.lo - radball > zrg.hi)) 
		return false; 


	// the triangle prism is two offset triangles joined by three 
	// straight lines, and we locate the origin in among this.  
	// In 2D this forms a pentagon

	// get the directions on the three prism edges 
	P2 pnorm = APerp(ConvertLZ(norm)); 
	double pnDa = DotLZ(pnorm, a); 
	double pnDb1 = DotLZ(pnorm, b1); 
	double pnDb2 = DotLZ(pnorm, b2); 


	// define the plus-minus edges along the two triangles.  
	P2 ab1 = ConvertLZ(b1) - ConvertLZ(a); 
	P2 pab1 = CPerp(ab1); 
	double zpab1 = DotLZ(pab1, a); 
	double npab1 = DotLZ(pab1, rnorm); 

	P2 b1b2 = ConvertLZ(b2) - ConvertLZ(b1); 
	P2 pb1b2 = CPerp(b1b2); 
	double zpb1b2 = DotLZ(pb1b2, b1); 
	double npb1b2 = DotLZ(pb1b2, rnorm); 

	P2 b2a = ConvertLZ(a) - ConvertLZ(b2); 
	P2 pb2a = CPerp(b2a); 
	double zpb2a = DotLZ(pb2a, b2); 
	double npb2a = DotLZ(pb2a, rnorm); 


	// a pair of these n-values have the same sign, 
	// and the sign determins if it is a top or bottom triangle forming the pentagon.  
	ASSERT(!((npab1 < 0.0) && (npb1b2 < 0.0) && (npb2a < 0.0)) && !((npab1 > 0.0) && (npb1b2 > 0.0) && (npb2a > 0.0)));  

	int swa12; 
	bool bTop = (npb1b2 > 0.0); 
	if ((npab1 > 0.0) == bTop) 
		swa12 = 1; 
	else if (bTop == (npb2a > 0.0)) 
		swa12 = 2; 
	else
	{
		ASSERT((npb2a > 0.0) == (npab1 > 0.0)); 
		swa12 = 0; 
		bTop = (npb2a > 0.0); 
	}

	// check this has identified the between-ness case 
	#ifdef MDEBUG
	{
		I1 DpnD = I1::SCombine(pnDa, pnDb1, pnDb2); 
		ASSERT(DpnD.Contains(swa12 == 0 ? pnDa : (swa12 == 1 ? pnDb1 : pnDb2))); 
	}
	#endif

	// decide if we are in the pentagon.  
	// fill in face which we have 
	// could have filled these already with pointer references.  
	bool& binterncellboundhl = (bTop ? binterncellboundhi : binterncellboundlo); 
	double& reshl =  (bTop ? reshi : reslo); 
	binterncellboundhl = false; 
	int tsign = (bTop ? 1 : -1); 

	// outside the b1b2 face 
	if (zpb1b2 + npb1b2 * tsign < 0.0)
	{
		if (swa12 != 0)  
			return false; 
		// get another chance if before the second edge  
		ASSERT(npb1b2 * tsign <= 0.0); 
		if (zpb1b2 - npb1b2 * tsign < 0.0)
			return false; 

		// between the sides 
		ASSERT(I1::SCombine(pnDb1, pnDb2).Contains(pnDa)); 
		if (((pnDb1 < 0.0) && (pnDb2 < 0.0)) || ((pnDb1 > 0.0) && (pnDb2 > 0.0)))  
			return false; 
		binterncellboundhl = true; 
	}
	
	if (zpb2a + npb2a * tsign < 0.0)
	{
		if (swa12 != 1)  
			return false; 
		// get another chance if before the second edge  
		ASSERT(npb2a * tsign <= 0.0); 
		if (zpb2a - npb2a * tsign < 0.0)
			return false; 

		// between the sides 
		ASSERT(I1::SCombine(pnDb2, pnDa).Contains(pnDb1)); 
		if (((pnDb2 < 0.0) && (pnDa < 0.0)) || ((pnDb2 > 0.0) && (pnDa > 0.0)))  
			return false; 
		binterncellboundhl = true; 
	}

	if (zpab1 + npab1 * tsign < 0.0)
	{
		if (swa12 != 2)  
			return false; 
		// get another chance if before the second edge  
		ASSERT(npab1 * tsign <= 0.0); 
		if (zpab1 - npab1 * tsign < 0.0)
			return false; 

		// between the sides 
		ASSERT(I1::SCombine(pnDa, pnDb1).Contains(pnDb2)); 
		if (((pnDa < 0.0) && (pnDb1 < 0.0)) || ((pnDa > 0.0) && (pnDb1 > 0.0)))  
			return false; 
		binterncellboundhl = true; 
	}



	// pentagon done.  Now we know the line intersects on a topside and a bottom side.  
	double aDn = Dot(a, norm); 

	// fill in the pentagon, two polygon side, which we know.  
	if (!binterncellboundhl)  
	{
		reshl = (aDn + radball * tsign) / norm.z; 
	}
	else if (swa12 == 0) 
	{
		double rnfac = -zpb1b2 / npb1b2; 
		double b1b2fac = -pnDb1 / (pnDb2 - pnDb1); 
		TOL_ZERO((ConvertLZ(b1 + rnorm * rnfac) + b1b2 * b1b2fac).Len()); 
		reshl = b1.z + rnorm.z * rnfac + (b2.z - b1.z) * b1b2fac; 
	}
	else if (swa12 == 1) 
	{
		double rnfac = -zpb2a / npb2a; 
		double b2afac = -pnDb2 / (pnDa - pnDb2); 
		TOL_ZERO((ConvertLZ(b2 + rnorm * rnfac) + b2a * b2afac).Len()); 
		reshl = b2.z + rnorm.z * rnfac + (a.z - b2.z) * b2afac; 
	}
	else 
	{
		ASSERT(swa12 == 2); 
		double rnfac = -zpab1 / npab1; 
		double ab1fac = -pnDa / (pnDb1 - pnDa); 
		TOL_ZERO((ConvertLZ(a + rnorm * rnfac) + ab1 * ab1fac).Len()); 
		reshl = a.z + rnorm.z * rnfac + (b1.z - a.z) * ab1fac; 
	}





	// now do the other side; we know which faces we must see, 
	// so need only check the sidedness we are wrt edges of the three way 
	// vertex in the middle.  
	int tsigno = -tsign; 
	double& reslh = (bTop ? reslo : reshi); 

	// are we outside the triangle?  
	bool& binterncellboundlh = (bTop ? binterncellboundlo : binterncellboundhi); 
	binterncellboundlh = false; 
	if ((swa12 != 0) && (zpb1b2 + npb1b2 * tsigno < 0.0))
		binterncellboundlh = true; 
	if ((swa12 != 1) && (zpb2a + npb2a * tsigno < 0.0)) 
		binterncellboundlh = true; 
	if ((swa12 != 2) && (zpab1 + npab1 * tsigno < 0.0)) 
		binterncellboundlh = true; 

	// on the internal facing edge 
	if (binterncellboundlh) 
	{
		// find which of two faces we are in.  
		int swas; 
		if (swa12 == 0)
			swas = (pnDa * tsigno < 0.0 ? 1 : 2); 
		else if (swa12 == 1)
			swas = (pnDb1 * tsigno < 0.0 ? 2 : 0); 
		else if (swa12 == 2)
			swas = (pnDb2 * tsigno < 0.0 ? 0 : 1); 

		// now find intersection on this particular face  
		if (swas == 0) 
		{
			double rnfac = -zpb1b2 / npb1b2; 
			double b1b2fac = -pnDb1 / (pnDb2 - pnDb1); 
			TOL_ZERO((ConvertLZ(b1 + rnorm * rnfac) + b1b2 * b1b2fac).Len()); 
			reslh = b1.z + rnorm.z * rnfac + (b2.z - b1.z) * b1b2fac; 
		}
		else if (swas == 1) 
		{
			double rnfac = -zpb2a / npb2a; 
			double b2afac = -pnDb2 / (pnDa - pnDb2); 
			TOL_ZERO((ConvertLZ(b2 + rnorm * rnfac) + b2a * b2afac).Len()); 
			reslh = b2.z + rnorm.z * rnfac + (a.z - b2.z) * b2afac; 
		}
		else 
		{
			ASSERT(swas == 2); 
			double rnfac = -zpab1 / npab1; 
			double ab1fac = -pnDa / (pnDb1 - pnDa); 
			TOL_ZERO((ConvertLZ(a + rnorm * rnfac) + ab1 * ab1fac).Len()); 
			reslh = a.z + rnorm.z * rnfac + (b1.z - a.z) * ab1fac; 
		}
	}		


	// in triangle, do calculation.  
	else
	{
		ASSERT((zpab1 + npab1 * tsigno >= 0.0) && (zpb1b2 + npb1b2 * tsigno >= 0.0) && (zpb2a + npb2a * tsigno >= 0.0)); 
		reslh = (aDn + radball * tsigno) / norm.z; 
	}


	return TrimToZrg(); 
}


