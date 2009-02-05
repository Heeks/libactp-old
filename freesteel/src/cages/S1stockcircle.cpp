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
// this establishes the circrange
void CircCrossingStructure::ChopOutBoundary(const vector<P2>& bound)  
{
	// we now make the circrange thing (not quite the appropriate class)  
	circrange.SetNew(0.0, I1(0, 4), 3); 

	// special case of empty boundary returns everywhere is stock.  
	if (bound.empty())
	{
		circrange.Merge(I1(0.0, 4.0)); 
		return;
	}

	// the crossing points 
	vector< pair<double, bool> > hraypara; // along the horizontal ray (for if wholly in or out).  

	ASSERT(bound.front() == bound.back()); 
	P2 p1 = bound.front() - cpt; 
	double rad1sq = p1.Lensq(); 
	bool bradfirstin = (rad1sq < cradsq); 
	bool brad1in = bradfirstin; 

	for (unsigned int i = 1; i < bound.size(); i++) 
	{
		// move to next point in contour.  (done here due to the continue commands).  
		P2 p0 = p1; 
		double rad0sq = rad1sq; 
		bool brad0in = brad1in; 

		// the second point
		p1 = bound[i] - cpt; 
		rad1sq = p1.Lensq(); 
		// ensure total consistency  
		brad1in = (i != bound.size() - 1 ? (rad1sq < cradsq) : bradfirstin); 

		// crossing the horizontal axis 
		if ((p0.v < 0.0) != (p1.v < 0.0))  
		{
			double lam = p0.v / (p0.v - p1.v); 
			TOL_ZERO(Along(lam, p0.v, p1.v)); 
			double cu = Along(lam, p0.u, p1.u); 
			if (cu >= 0.0) 
				hraypara.push_back(pair<double, bool>(cu, (p1.v >= 0.0))); 
		}

		// discard if inside fully (by convexity).  
		if (brad0in && brad1in) 
			continue; 


		// find closest approach of line 
		P2 v = p1 - p0; 
		double vsq = v.Lensq(); 
		if (vsq == 0.0) 
			continue; 

		// discard if closest is too far.  
		double cdsq = Square(Dot(p0, APerp(v))) / vsq; 
		if (cdsq >= cradsq)
		{
			// only get away of both endpoints are not in circle.  
			if ((rad0sq >= cradsq) && (rad1sq >= cradsq)) 
				continue; 
			TOL_ZERO(max(cradsq - rad0sq, cradsq - rad1sq)); 
		}

		// find lambda of crossing points of line and circle.  
		double lamz = -Dot(p0, v) / vsq; 

		// discard if outside fully and closest point is beyond  
		if (!brad0in && !brad1in && !I1unit.Contains(lamz)) 
			continue; 

		TOL_ZERO(cdsq + Square(lamz) * vsq - p0.Lensq()); 
		double lampsq = (cradsq - cdsq) / vsq; 
		

		// line may cross the circle.  
		double lamp = sqrt(lampsq); 
		TOL_ZERO(AlongD(lamz + lamp, p0, p1).Len() - crad); 

		TOL_ZERO(min(I1unit.Distance(lamz + lamp), I1unit.Distance(lamz - lamp))); 

		// lower crossing
		if (!brad0in) 
		{
			double lam = I1unit.PushIntoSmall(lamz - lamp); 
			P2 rpt = Along(lam, p0, p1); 
			TOL_ZERO(rpt.Len() - crad); 
			cpara.push_back(CPara(rpt, rpt.DArg(), true)); 
		}
		
		// upper crossing
		if (!brad1in) 
		{
			double lam = I1unit.PushIntoSmall(lamz + lamp); 
			P2 rpt = Along(lam, p0, p1); 
			TOL_ZERO(rpt.Len() - crad); 
			cpara.push_back(CPara(rpt, rpt.DArg(), false)); 
		}
	}


	// we now have a series of points in cpara and hraypara
	sort(cpara.begin(), cpara.end()); 
	sort(hraypara.begin(), hraypara.end()); 


	// find if the point (crad, 0) is inside the contour (clockwise)
	// we measure closeness to the crossing for better comparison to 
	// the results from the circle intersection which will take priority.  
	double hrayinvaldist = 1.0; // default distance that's high enough.  
	while (!hraypara.empty()) 
	{
		// find the interval that most closely contains crad
		ASSERT(hraypara.back().second); 
		double hrayinhi = hraypara.back().first; 
		hraypara.pop_back(); 
		ASSERT(hraypara.empty() || !hraypara.back().second); 
		double hrayinlo = 0.0; 
		if (!hraypara.empty()) 
		{
			hrayinlo = hraypara.back().first; 
			hraypara.pop_back(); 
		}
		ASSERT(hrayinlo <= hrayinhi); 
		I1 rghrayinmcrad = I1(hrayinlo - crad, hrayinhi - crad); 
		if (rghrayinmcrad.lo >= 0.0) 
		{
			if (hrayinvaldist > rghrayinmcrad.lo)
				hrayinvaldist = rghrayinmcrad.lo; 
		}
		else if (rghrayinmcrad.hi >= 0.0) 
		{
			hrayinvaldist = min(rghrayinmcrad.lo, -rghrayinmcrad.hi); 
			break; 
		}
		else 
		{
			if (hrayinvaldist < -rghrayinmcrad.hi)
				hrayinvaldist = -rghrayinmcrad.hi; 
			break; 
		}
	}

	if (hrayinvaldist >= 0.0) 
		ASSERT(1); 
	else
		ASSERT(1); 



	// empty case, need to use the hray information  
	// if there is more than one contour, the hray value at cpt is meaningless 
	// because a circle just inside the circle may have been chopped out.  
	if (cpara.empty()) 
	{
		if (!(hrayinvaldist > 0.0))
			circrange.Merge(I1(0.0, 4.0)); 
		return; 
	}


	// loop through pairs in cpara, getting the range that goes over the start right.  
	// we keep the parts that are outside the stock, 
	// because we will union in the further cuts.  when it's from 0 to 4 there's no material left.  
	int kcp = 1; 
	ASSERT((cpara.size() % 2) == 0); 
	ASSERT(cpara.front().bClockwiseIn != cpara.back().bClockwiseIn); 

	// first segment 
	if (cpara.front().bClockwiseIn) 
	{
		circrange.Merge(I1(0.0, cpara.front().darg)); 
		kcp++; 
		ASSERT(hrayinvaldist <= 0.0); 
	}
	else
		ASSERT(hrayinvaldist >= 0.0); 

	// middle segments 
	while (kcp < (int)(cpara.size())) 
	{
		ASSERT(!cpara[kcp - 1].bClockwiseIn); 
		ASSERT(cpara[kcp].bClockwiseIn); 
		circrange.Merge(I1(cpara[kcp - 1].darg, cpara[kcp].darg)); 
		kcp += 2; 
	}

	// final segment
	ASSERT(!cpara.back().bClockwiseIn == (kcp == cpara.size())); 
	if (!cpara.back().bClockwiseIn) 
	{
		circrange.Merge(I1(cpara.back().darg, 4.0)); 
	}

	cpara.clear(); 
}



//////////////////////////////////////////////////////////////////////
void CircCrossingStructure::SetPrad(double lprad) 
{
	prad = lprad; 
	pradsq = Square(prad); 
	cradpprad = crad + prad; 
	cradppradsq = Square(cradpprad); 
	cradmpradsq = Square(crad - prad); 
}


static int tccount = 0; 
//////////////////////////////////////////////////////////////////////
void CircCrossingStructure::HackToolCircle(const P2& tpt) 
{
tccount++; 
	// maybe subtracting away a region will be better than Merge.  
	double vsq = tpt.Lensq();
	if (vsq >= cradppradsq) 
		return; 
	if (vsq <= cradmpradsq) 
	{
		// clear the whole range 
		if (prad > crad) 
			circrange.Minus(I1(0.0, 4.0)); 
		return; 
	}

	// the points of intersection will be: 
	// lam v +- mu APerp(v).  
	double lam = (crad != prad ? ((cradsq - pradsq) / vsq + 1) / 2 : 0.5); 
	double musq = cradsq / vsq - Square(lam); 
	double mu = PosSqrt(musq); 
	P2 vl = tpt * lam; 
	P2 vm = APerp(tpt) * mu; 

	P2 dcp2 = vl + vm; 
	P2 dcp1 = vl - vm; 

	TOL_ZERO(dcp2.Len() - crad); 
	TOL_ZERO(dcp1.Len() - crad); 
	TOL_ZERO((dcp2 - tpt).Len() - prad); 
	TOL_ZERO((dcp1 - tpt).Len() - prad); 

	double dc2 = dcp2.DArg(); 
	double dc1 = dcp1.DArg(); 

	#ifdef MDEBUG
		if (prad <= crad) 
			ASSERT((dc1 <= dc2) ? dc2 - dc1 <= 2.0 : dc2 - dc1 + 4 <= 2.0); 
	#endif


	// merge it in.  
	if (dc1 <= dc2) 
		circrange.Minus(I1(dc1, dc2)); 
	else
	{
		circrange.Minus(I1(0.0, dc2)); 
		circrange.Minus(I1(dc1, 4.0)); 
	}
}

//////////////////////////////////////////////////////////////////////
// this'll be a long one
void CircCrossingStructure::HackToolRectangle(const P2& p0, const P2& p1) 
{
	// the vector of the line 
	P2 v = p1 - p0; 
	double vsq = v.Lensq(); 
	ASSERT(vsq != 0.0); 
	
	// the rectangle joins the points p0/p1 +- APerp(v) crad / vlen.  

	// discard if totally misses the channel parallel and around the line.  
	double dp0cpv = Dot(p0, APerp(v)); 
	double cdsq = Square(dp0cpv) / vsq; 
	if (cdsq >= cradppradsq)
		return; 

	double vlen = sqrt(vsq); 

	// discard if misses in the perpendicular direction too, 
	// where the two planes are going forwards along v
	double dp0pv = Dot(p0, v); 
	double cdp0sq = Square(dp0pv) / vsq; 
	if ((dp0pv > 0.0) && (cdp0sq >= cradsq))  
		return; 
	double dp1pv = dp0pv + vsq; 
	TOL_ZERO(dp1pv - Dot(p1, v)); 
	double cdp1sq = Square(dp1pv) / vsq; 
	if ((dp1pv < 0.0) && (cdp1sq >= cradsq))  
		return; 


	double perpvfac = prad / vlen; 
	P2 vpcr = APerp(v) * perpvfac; 	
	P2 p0l = p0 + vpcr; 
	P2 p0r = p0 - vpcr; 
	P2 p1l = p1 + vpcr; 
	P2 p1r = p1 - vpcr; 

	double rad0lsq = p0l.Lensq(); 
	double rad0rsq = p0r.Lensq(); 
	double rad1lsq = p1l.Lensq(); 
	double rad1rsq = p1r.Lensq(); 

	bool brad0lin = (rad0lsq < cradsq); 
	bool brad0rin = (rad0rsq < cradsq); 
	bool brad1lin = (rad1lsq < cradsq); 
	bool brad1rin = (rad1rsq < cradsq); 

	// if fully in by convexity merge whole circle
	if (brad0lin && brad0rin && brad1lin && brad1rin) 
	{
		// or clear the whole thing
		circrange.Minus(I1(0.0, 4.0)); 
		return; 
	}

	// find lambda of crossing points of line and circle.  
	double lamz = -Dot(p0, v) / vsq; 

	// the lambda of the crossing points of the perpendicular lines and the circle 
	double lamz_p = -Dot(p0, APerp(v)) / vsq; 

	// discard if outside fully and closest point is beyond  
	// a double intersection on a raw face is not that worth finding.  
	if (!brad0lin && !brad0rin && !brad1lin && !brad1rin) 
	{
		if (!I1unit.Contains(lamz) && !I1(-perpvfac, perpvfac).Contains(lamz_p)) 
			return; 
	}


	double cdmd = (2 * dp0cpv * prad) / vlen; 


	// now we go round in order clockwise, entering and exiting the circle 
	// we can account for the circumpherence that's on the rectangle, and then invert it.  
	ASSERT(cpara.empty()); 


	// consider the p0r to p1r line  
	double cdrsq = cdsq - cdmd + pradsq; 
	TOL_ZERO(cdrsq - Square(Dot(p0r, APerp(v))) / vsq); 
	TOL_ZERO(cdrsq + Square(lamz) * vsq - p0r.Lensq()); 
	if ((brad0rin != brad1rin) || (!brad0rin && !brad1rin && (cdrsq < cradsq) && I1unit.Contains(lamz)))  
	{
		double lamrpsq = (cradsq - cdrsq) / vsq; 

		// line may cross the circle.  
		double lamrp = sqrt(lamrpsq); 
		TOL_ZERO(AlongD(lamz + lamrp, p0r, p1r).Len() - crad); 
		TOL_ZERO(min(I1unit.Distance(lamz + lamrp), I1unit.Distance(lamz - lamrp))); 

		// lower crossing
		if (!brad0rin) 
		{
			double lam = I1unit.PushIntoSmall(lamz - lamrp); 
			P2 rpt = Along(lam, p0r, p1r); 
			TOL_ZERO(rpt.Len() - crad); 
			cpara.push_back(CPara(rpt, rpt.DArg(), true)); 
		}
		
		// upper crossing
		if (!brad1rin) 
		{
			double lam = I1unit.PushIntoSmall(lamz + lamrp); 
			P2 rpt = Along(lam, p0r, p1r); 
			TOL_ZERO(rpt.Len() - crad); 
			cpara.push_back(CPara(rpt, rpt.DArg(), false)); 
		}
	}

	// consider the p1r to p1l line  
	// these crossings would be raw 
	TOL_ZERO(cdp1sq + Square(lamz_p) * vsq - p1.Lensq()); 
	if ((brad1rin != brad1lin) || (!brad1rin && !brad1lin && (cdp1sq < cradsq) && I1(-perpvfac, perpvfac).Contains(lamz_p)))  
	{
		double lam1psq = (cradsq - cdp1sq) / vsq; 

		// line may cross the circle.  
		double lam1p = sqrt(lam1psq); 
		TOL_ZERO((p1 + APerp(v) * (lamz_p + lam1p)).Len() - crad); 
		TOL_ZERO(min(I1(-perpvfac, perpvfac).Distance(lamz_p + lam1p), I1(-perpvfac, perpvfac).Distance(lamz_p - lam1p))); 

		// lower crossing
		if (!brad1rin) 
		{
			double lam = I1unit.PushIntoSmall(((lamz_p - lam1p) / perpvfac + 1) / 2); 
			P2 rpt = Along(lam, p1r, p1l); 
			TOL_ZERO(rpt.Len() - crad); 
			cpara.push_back(CPara(rpt, rpt.DArg(), true)); 
		}
		
		// upper crossing
		if (!brad1lin) 
		{
			double lam = I1unit.PushIntoSmall(((lamz_p + lam1p) / perpvfac + 1) / 2); 
			P2 rpt = Along(lam, p1r, p1l); 
			TOL_ZERO(rpt.Len() - crad); 
			cpara.push_back(CPara(rpt, rpt.DArg(), false)); 
		}
	}

	// consider the p1l to p0l line  
	double cdlsq = cdsq + cdmd + pradsq; 
	TOL_ZERO(cdlsq - Square(Dot(p0l, APerp(v))) / vsq); 
	TOL_ZERO(cdlsq + Square(lamz) * vsq - p0l.Lensq()); 
	if ((brad1lin != brad0lin) || (!brad1lin && !brad0lin && (cdlsq < cradsq) && I1unit.Contains(lamz))) 
	{
		// line may cross the circle.  
		double lamlpsq = (cradsq - cdlsq) / vsq; 
		double lamlp = -sqrt(lamlpsq); 
		TOL_ZERO(AlongD(lamz + lamlp, p0l, p1l).Len() - crad); 
		TOL_ZERO(min(I1unit.Distance(lamz + lamlp), I1unit.Distance(lamz - lamlp))); 

		// lower crossing
		if (!brad1lin) 
		{
			double lam = I1unit.PushIntoSmall(lamz - lamlp); 
			P2 rpt = Along(lam, p0l, p1l); 
			TOL_ZERO(rpt.Len() - crad); 
			cpara.push_back(CPara(rpt, rpt.DArg(), true)); 
		}
		
		// upper crossing
		if (!brad0lin) 
		{
			double lam = I1unit.PushIntoSmall(lamz + lamlp); 
			P2 rpt = Along(lam, p0l, p1l); 
			TOL_ZERO(rpt.Len() - crad); 
			cpara.push_back(CPara(rpt, rpt.DArg(), false)); 
		}
	}



	// consider the p0l to p0r line  
	// these crossings would be raw.  
	TOL_ZERO(cdp0sq + Square(lamz_p) * vsq - p0.Lensq()); 
	if ((brad0lin != brad0rin) || (!brad0lin && !brad0rin && (cdp0sq < cradsq) && I1(-perpvfac, perpvfac).Contains(lamz_p)))  
	{
		double lam0psq = (cradsq - cdp0sq) / vsq; 

		// line may cross the circle.  
		double lam0p = -sqrt(lam0psq); 
		TOL_ZERO((p0 + APerp(v) * (lamz_p + lam0p)).Len() - crad); 
		TOL_ZERO(min(I1(-perpvfac, perpvfac).Distance(lamz_p + lam0p), I1(-perpvfac, perpvfac).Distance(lamz_p - lam0p))); 

		// lower crossing
		if (!brad0lin) 
		{
			double lam = I1unit.PushIntoSmall(((lamz_p - lam0p) / perpvfac + 1) / 2); 
			P2 rpt = Along(lam, p0r, p0l); 
			TOL_ZERO(rpt.Len() - crad); 
			cpara.push_back(CPara(rpt, rpt.DArg(), true)); 
		}
		
		// upper crossing
		if (!brad0rin) 
		{
			double lam = I1unit.PushIntoSmall(((lamz_p + lam0p) / perpvfac + 1) / 2); 
			P2 rpt = Along(lam, p0r, p0l); 
			TOL_ZERO(rpt.Len() - crad); 
			cpara.push_back(CPara(rpt, rpt.DArg(), false)); 
		}
	}


	// we have all the intersections here 
	if (cpara.empty()) 
		return; 


	// we need to sort cpara.  
	// show it is out of order only once (it is properly clockwise) 
	#ifdef MDEBUG
		bool bOrder = false; 
		for (int ii = 1; ii < (int)cpara.size(); ii++) 
		{
			if (cpara[ii - 1].darg > cpara[ii].darg) 
			{
				ASSERT(!bOrder); 
				bOrder = true; 
			}
		}
	#endif

	// could use the knowledge from the above loop to work it out 
	sort(cpara.begin(), cpara.end()); 

	ASSERT((cpara.size() % 2) == 0); 
	ASSERT(cpara.front().bClockwiseIn != cpara.back().bClockwiseIn); 
	int k = 1; 
	if (cpara.front().bClockwiseIn) 
	{
		circrange.Minus(I1(0.0, cpara.front().darg)); 
		k++; 
	}
	while (k < (int)(cpara.size())) 
	{
		ASSERT(cpara[k].bClockwiseIn && !cpara[k - 1].bClockwiseIn); 
		circrange.Minus(I1(cpara[k - 1].darg, cpara[k].darg)); 
		k += 2; 
	}
	if (!cpara.back().bClockwiseIn) 
	{
		circrange.Minus(I1(cpara.back().darg, 4.0)); 
	}

	cpara.clear(); 
}



//////////////////////////////////////////////////////////////////////
void HackCCSx(CircCrossingStructure& ccs, const PathXSeries& paths) 
{
	// leaving out the toolcircle for the first point.  
	int j = 0;
	P2 p1 = paths.pths[0] - ccs.cpt; 
	for (int i = 1; i < (int)(paths.pths.size()); i++) 
	{
		// is our area full?  
		if (ccs.circrange.empty()) 
			break; 

		P2 p0 = p1; 
		p1 = paths.pths[i] - ccs.cpt; 

		if ((j == (int)(paths.brks.size())) || (i < paths.brks[j]))
			ccs.HackToolRectangle(p0, p1); 

		// advance through possible multiple markings on this segment.  
		else
		{
			ASSERT(i == paths.brks[j]); 
			do
				j++;
			while ((j < (int)(paths.brks.size())) && (i == paths.brks[j])); 
		}
		
		ccs.HackToolCircle(p1); 
	}
}

//////////////////////////////////////////////////////////////////////
void HackCCSx(CircCrossingStructure& ccs, const PathXboxed& pathxb) 
{
	// find conditions where we have to drop through to full-scan  
	I1 urg(ccs.cpt.u - ccs.cradpprad, ccs.cpt.u + ccs.cradpprad); 
	if (pathxb.puckets.empty() || (pathxb.bGeoOutLeft && (urg.lo < pathxb.gburg.lo)) || (pathxb.bGeoOutRight && (urg.hi > pathxb.gburg.hi)))  
	{
		HackCCSx(ccs, *pathxb.ppathx); 
		return; 
	}
	if (!urg.Intersect(pathxb.gburg)) 
		return; 

	// increment duplicates finder.  
	pathxb.maxidup++; 

	// work through the ranges 		
	pair<int, int> iurg = pathxb.upart.FindPartRG(urg); 
	for (int iu = iurg.first; iu <= iurg.second; iu++) 
	{
		const pucketX& pucx = pathxb.puckets[iu]; 
		for (int k = 0; k < (int)(pucx.cklines.size()); k++) 
		{
			if ((pucx.cklines[k].idup != -1) || (pucx.cklines[k].idup != pathxb.maxidup))
			{
				int iseg = pucx.cklines[k].iseg; 
				P2 p0 = pathxb.ppathx->pths[iseg - 1] - ccs.cpt; 
				P2 p1 = pathxb.ppathx->pths[iseg] - ccs.cpt; 
				ccs.HackToolRectangle(p0, p1); 
				ccs.HackToolCircle(p0); 

				// mark for duplicates 
				if (pucx.cklines[k].idup != -1)
				{
					ASSERT(pucx.cklines[k].idup < pathxb.maxidup); 
					pucx.cklines[k].idup = pathxb.maxidup; 
				}
			}
		}
	}
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// returns a series of points which interfere with the metal along the circumference.  
void CircleIntersectNew(vector<I1>& res, const P2& cpt, double crad, const PathXSeries& bound, const PathXboxed& pathxb, double prad)  
{
	CircCrossingStructure ccs(cpt, crad); 

	// only work on the one boundary for now.  
	// the boundary must come in as external c-clockwise.  
	ASSERT(!bound.brks.empty() && (bound.brks[0] == bound.pths.size())); 
	ccs.ChopOutBoundary(bound.pths); 

	// toolpath hacking.  
	ccs.SetPrad(prad); 

	HackCCSx(ccs, pathxb); 

	// convert this into a range of Dargs that model the material.  
	if (ccs.circrange.empty()) 
		ASSERT(1); 

	for (int i = 1; i < (int)(ccs.circrange.size()); i += 2) 
		res.push_back(I1(ccs.circrange[i - 1].w, ccs.circrange[i].w)); 
}



