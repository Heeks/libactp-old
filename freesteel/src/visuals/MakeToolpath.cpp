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

#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkActor.h"
#include "vtkActorCollection.h"
#include "vtkSTLReader.h"
#include "vtkPolyDataMapper.h"
#include "vtkOpenGLPolyDataMapper.h"

#include "bolts/bolts.h"
#include "cages/cages.h"
#include "pits/pits.h"
#include "visuals/MakeToolpath.h"
#include "visuals/fsvtkToolpathMapper.h"
#include "visuals/gstsurface.h"





/////////////////////////////////////////////////////////// 
void MakeCorerough(vector<PathXSeries>& vpathseries, SurfX& sx, const PathXSeries&  bound, const MachineParams& params) 
{
	// boxed surfaces 
	SurfXboxed sxb(&sx); 
	sxb.BuildBoxes(10.0); 

	// interior close to tool, or absolute intersections with triangle faces
	double areaoversize = (params.toolcornerrad + params.toolflatrad) * 2 + 13; 

	Area2_gen a2g; 
	a2g.SetShape(sx.gxrg.Inflate(areaoversize), sx.gyrg.Inflate(areaoversize), params.triangleweaveres); 
	a2g.SetSurfaceTop(&sxb, params.toolcornerrad); 

	Area2_gen a2gfl; 
	a2gfl.SetShape(sx.gxrg.Inflate(areaoversize), sx.gyrg.Inflate(areaoversize), params.flatradweaveres);

	double hz = sx.gzrg.hi - params.stepdown / 2; 
double htopz = sx.gzrg.lo + 5;
	a2g.z = sx.gzrg.hi - params.stepdown / 2;
	while (hz > htopz)
	{
		vpathseries.push_back(PathXSeries());
		// make the core roughing algorithm thing
		CoreRoughGeneration crg(&vpathseries.back(), sx.gxrg.Inflate(10), sx.gyrg.Inflate(10)); 
		// the stock boundary 
	//	crg.tsbound.paths.insert(crg.tsbound.paths.end(), bound.ftpaths.paths.begin(), bound.ftpaths.paths.end()); 
		crg.tsbound.Append(bound.pths); 

		// the material boundary weave used in the core roughing.  
		crg.pa2gg = (params.toolflatrad != 0.0 ? &a2gfl : &a2g); 
		crg.trad = params.toolcornerrad * 0.9 + params.toolflatrad; // the clearing radius 
		crg.wc.ps2w = crg.pa2gg; 

		PathXSeries blpaths;

		// hack against the surfaces 
		a2g.HackDowntoZ(hz); 
		a2g.MakeContours(blpaths); 

		// hack by the flatrad.  
		if (params.toolflatrad != 0.0) 
		{
			HackAreaOffset(a2gfl, blpaths, params.toolflatrad); 
			a2gfl.z = a2g.z; 

			// make it again so we can see
			blpaths = PathXSeries(); 
			a2gfl.MakeContours(blpaths); 
		}

		crg.GrabberAlg(params); 


//		vpathseries.push_back(blpaths);
//break;
		hz -= params.stepdown; 
	}
}


/////////////////////////////////////////////////////////// 
void CoreRoughGeneration::AddPoint(const P2& ppt)  
{
	if ((ppt - wc.ptcp).Len() < 1e-5) 
		ASSERT(1); 
	if (ppt != wc.ptcp)
	{
		ASSERT(pathxb.ppathx->pths.empty() || (pathxb.ppathx->pths.back() != ppt)); 
		pathxb.Add(ppt); 
	}
}


/////////////////////////////////////////////////////////// 
void BuildRetract(vector<P3>& lnkpth, const P3& pts, const P3& pte, const MachineParams& params)
{
	ASSERT((params.retractzheight > pts.z) && (params.retractzheight > pte.z));
	lnkpth.push_back(pts);
	lnkpth.push_back(ConvertCZ(pts, params.retractzheight));
	lnkpth.push_back(ConvertCZ(pte, params.retractzheight));
	lnkpth.push_back(pte);
}

/////////////////////////////////////////////////////////// 
void BuildCurl(vector<P2>& lnkpth, const P2& pts, const P2& dirs, const MachineParams& params, bool bCurlIn)
{
	TOL_ZERO(dirs.Len() - 1.0);

	double asradstep = params.leadoffsamplestep / params.leadoffrad;

	// centre
	P2 cts = pts + APerp(dirs) * params.leadoffrad;
	double adiff = params.leadofflen / params.leadoffrad;
	double ae, as;
	if (bCurlIn)
	{
		ae = (cts - pts).Arg();
		as = ae - adiff;;
	}
	else
	{
		as = (cts - pts).Arg();
		ae = as + adiff;
	}
	

	P2 pt = cts - P2(cos(as), sin(as)) * params.leadoffrad;
	lnkpth.push_back(pt);
	as += asradstep;
	while (true)
	{
		P2 ptnew = cts - P2(cos(as), sin(as)) * params.leadoffrad;
		if (as > ae)
		{
			P2 ptlast = cts - P2(cos(ae), sin(ae)) * params.leadoffrad;
			lnkpth.push_back(ptlast);
			break;
		}

		lnkpth.push_back(ptnew);
		pt = ptnew;
		as += asradstep;
	}
}

void BuildLink(vector<P2>& lnkpth, const P2& pts, const P2& dirs, const P2& pte, const P2& dire, const MachineParams& params)
{
	TOL_ZERO(dirs.Len() - 1.0);
	TOL_ZERO(dire.Len() - 1.0);

	double asradstep = params.leadoffsamplestep / params.leadoffrad;

	// centre
	P2 cts = pts + APerp(dirs) * params.leadoffrad;
	P2 cte = pte + APerp(dire) * params.leadoffrad;

	// tangent dir
	P2 tdir = cte - cts;
	double tdirlen = tdir.Len();
	P2 tdirp = APerp(tdir) * params.leadoffrad / tdirlen;

	// tangent points on arcs
	P2 tps = cts - tdirp;
	P2 tpe = dire == P2(0, 0) ? pte : (cte - tdirp);

	// loop from start point to tangent point
	double as = (cts - pts).Arg();
	if (as > M2PI)
		as -= M2PI;
	double at = (cts - tps).Arg();
	if (at > M2PI)
		at -= M2PI;
	if (as > at)
		at += M2PI;

	while (as <= at)
	{
		P2 pt = cts - P2(cos(as), sin(as)) * params.leadoffrad;
		lnkpth.push_back(pt);
		as += asradstep;
	}
	if (lnkpth.back() != tps)
		lnkpth.push_back(tps);

	// loop from tangent point to end point
	if (dire != P2(0, 0))
	{
		as = (cte - tpe).Arg();
		if (as > M2PI)
			as -= M2PI;
		at = (cte - pte).Arg();
		if (at > M2PI)
			at -= M2PI;
		if (as > at)
			at += M2PI;

		while(as <= at)
		{
			lnkpth.push_back(cte - P2(cos(as), sin(as)) * params.leadoffrad);
			as += asradstep;
		}
	}
	if (lnkpth.back() != pte)
		lnkpth.push_back(pte);
}

void BuildLinkZ(vector<P3>& lnkpth, const vector<P2>& lnk2D, double z, const MachineParams& params) 
{
	// total length
	double totallen = 0;
	int ix = 1;
	while (ix < lnk2D.size())
	{
		totallen += (lnk2D[ix] - lnk2D[ix - 1]).Len();
		++ix;
	}

	double leadofflen = params.leadofflen;
	if (totallen < (2.0 * params.leadofflen))
		leadofflen = 0.5 * totallen;

	vector<P3> lnkStart;
	int ixstart = 1;
	double len = 0;
	lnkStart.push_back(ConvertGZ(lnk2D[0], z));
	while (ixstart < lnk2D.size())
	{
		len += (lnk2D[ixstart] - lnk2D[ixstart - 1]).Len();
		if (len > leadofflen)
			break;

		double dz = len * params.leadoffdz / leadofflen;
		lnkStart.push_back(ConvertGZ(lnk2D[ixstart], z + dz));
		++ixstart;
	}
	ASSERT((lnkStart.back().z > z) && (lnkStart.back().z <= params.leadoffdz + z));
	if (ixstart < lnk2D.size())
	{
		len += (lnk2D[ixstart] - lnk2D[ixstart - 1]).Len();
		double dz = len * params.leadoffdz / leadofflen;
		double lam = params.leadoffdz / dz;
		P2 pt = Along(lam, lnk2D[ixstart - 1], lnk2D[ixstart]);
		lnkStart.push_back(ConvertGZ(pt, z + params.leadoffdz));
	}

	vector<P3> lnkEnd;
	int ixend = lnk2D.size() - 2;
	len = 0;
	lnkEnd.push_back(ConvertGZ(lnk2D.back(), z));
	while (ixend > ixstart)
	{
		len += (lnk2D[ixend] - lnk2D[ixend + 1]).Len();
		if (len > leadofflen)
			break;

		double dz = len * params.leadoffdz / leadofflen;
		lnkEnd.push_back(ConvertGZ(lnk2D[ixend], z + dz));
		--ixend;
	}
	ASSERT((lnkEnd.back().z > z) && (lnkEnd.back().z <= params.leadoffdz + z));
	if (ixend >= ixstart)
	{
		len += (lnk2D[ixend] - lnk2D[ixend + 1]).Len();
		double dz = len * params.leadoffdz / leadofflen;
		double lam = params.leadoffdz / dz;
		P2 pt = Along(lam, lnk2D[ixend + 1], lnk2D[ixend]);
		lnkEnd.push_back(ConvertGZ(pt, z + params.leadoffdz));
	}

	// now put it all together
	lnkpth.insert(lnkpth.end(), lnkStart.begin(), lnkStart.end());

	int ie;
	for (ie = ixstart; ie <= ixend; ++ie)
		lnkpth.push_back(ConvertGZ(lnk2D[ie], (z + params.leadoffdz)));

	ie = lnkEnd.size() - 1;
	while (ie >= 0)
		lnkpth.push_back(lnkEnd[ie--]);
}


/////////////////////////////////////////////////////////// 
// returns index to which we can go until we interfere with stock or contour
// wclink passed by value to get a local copy
int CoreRoughGeneration::TrackLink(const vector<P2>& lnk2D, S2weaveCellLinearCutTraverse wclink, bool bFromEnd, const MachineParams& params)
{
	bool bOnStock = false;
	int ix = bFromEnd ? (lnk2D.size() - 2) : 1;
	for (; (bFromEnd ? (ix > 0) : (ix < lnk2D.size() - 1)); ix += (bFromEnd ? -1 : 1))
	{
//		TOL_ZERO((lnk2D[bFromEnd ? (ix) : (ix - 1)] - wclink.ptcp).Len());
		P2 Nvbearing = bFromEnd ? (lnk2D[ix - 1] - lnk2D[ix]) : (lnk2D[ix] - lnk2D[ix - 1]); 
		double Nvd = Nvbearing.Len(); 

		double lNvd = wclink.FollowBearing(Nvbearing / Nvd, Nvd); 
		if (wclink.bOnContour)
			return (bFromEnd ? ix - 1 : ix); 
		else
		{
			// does it cut any stock
			vector<I1> res;
			P2 vec = (lnk2D[ix] - lnk2D[ix - 1]);
			double len = vec.Len();
			P2 nvec = vec / len;
			P2 pt = lnk2D[ix - 1];
			while ((len > params.leadoffsamplestep) && res.empty())
			{
				pt = pt + nvec * params.leadoffsamplestep;
				CircleIntersectNew(res, pt, trad, tsbound, pathxb, trad); 
				len -= params.leadoffsamplestep;
			}

			if (res.empty())
				CircleIntersectNew(res, lnk2D[ix], trad, tsbound, pathxb, trad); 
			if (!res.empty())
			{
				bOnStock = true;
				return (bFromEnd ? ix - 1 : ix);
			}
		}

		ASSERT(lNvd == Nvd); 
	}

	return lnk2D.size();
}

/////////////////////////////////////////////////////////// 
void CoreRoughGeneration::GrabberAlg(const MachineParams& params)
{
	// Generate the toolpath thing 
	countfreespacesteps = 0; 
	pa2gg->SetAllCutCodes(-1);   
	FindGoStart(); 
	
	// put the first position in into our set of links, 
	// for we will return here.  
	bcellixs.push_back(BCellIndex(wc, P2(0.0, 1.0))); 

	S2weaveCellLinearCutTraverse wclink;

	// loop looking for different places 
	// the reset of start is second part of loop 
	while (true) 
	{
		// track along happily until we reach an ending of some sort 
		double dch = (wc.bOnContour ? -params.dchangrightoncontour: 0.0); 
		while (true)
		{
			// too far in free space, find what's happened
			if (countfreespacesteps == 39) 
			{
				// remove the tail  
				ASSERT(1); 

				// this is dreck, but might get some demos.  
				while (countfreespacesteps-- > 0) 
					pathxb.Pop_back(); 

				break; 
			}

			// the point we are at currently  
			P2 ppt = wc.ptcp; 

			// move along to next point 
			if (wc.bOnContour)
			{
				if (!wc.OnContourFollowBearing(dch, params.samplestep)) 
				{
					// if we are curving in then revert to straight line 
					if (dch <= 0.0) 
						dch = 0.0; 

					// leave contour (mark this place) 
					bcellixs.push_back(BCellIndex(wc)); 
	
					// special follow bearing away from contour 
					// first move is special case 
					// change direction and then follow along.  
					// dch is relative to apvb 
					ASSERT(APerp(wc.vbearing) == wc.apvb); 
					P2 Nvbearing = wc.vbearing + wc.apvb * dch; 
	
					wc.FollowBearing(Nvbearing / Nvbearing.Len(), params.samplestep); 
					ASSERT(!wc.bOnContour); 
				}
				else
					ASSERT(wc.bOnContour); 
			}

			// free-form follow  
			else 
			{
				// change direction and then follow along.  
				// dch is relative to apvb 
				ASSERT(APerp(wc.vbearing) == wc.apvb); 
				P2 Nvbearing = wc.vbearing + wc.apvb * dch; 
	
				wc.FollowBearing(Nvbearing / Nvbearing.Len(), params.samplestep); 
			}
				

			// only put in unique points (a min distance might be included).  
			AddPoint(ppt); 
			// if we have arrived following the contour at an edge where 
			// the cutpoint has been done, then quit 
			if (wc.bOnContour) 
			{
				// we're on a contour; has it already been cleared?  
				int ibl = wc.bolistpairs[wc.ib].second; 
				ASSERT(wc.GetBoundLower(ibl)); 

				// it has, so end this path now.  
				if (wc.boundlist[ibl].second->cutcode != -1) 
				{
					pathxb.Add(wc.ptcp); 
					break; 
				}
			}

			// change direction according to the amount of material cutting.  
			dch = ChangeBearing(wc.ptcp, wc.vbearing, params); 
		}

		wclink = wc; 
		pathxb.Break();

		// build the entire location for this new starting point 
		// at the entry to a cell on a contour.  
		// (or at an entry in space which is in contact with something).  
		while (!bcellixs.empty())
		{
			if (RestartAtBCI(bcellixs.back(), params, (bcellixs.size() == 1))) 
				break; 
			bcellixs.pop_back(); 
		}

		// no more starts; leave 
		if (bcellixs.empty()) 
			break; 
		bcellixs.pop_back(); 

		// need to build a linking motion from wclink.ptcp to wc.ptcp
		// S2weaveCellLinearCutTraverse wclink = wc; 
		vector<P2> lnk2D;
		P2 ptOut = wclink.ptcp;
		P2 drOut = wclink.vbearing;
		P2 ptIn = wc.ptcp;
		P2 drIn = wc.vbearing;
		if (drIn == P2(0, 0))
			drIn = P2(0, 1.0);
		BuildLink(lnk2D, ptOut, drOut, ptIn, drIn, params);

		// test link, returns how far we could track along 
		// link until we make contact with contours or stock
		int itracked = TrackLink(lnk2D, wclink, false, params); 

		ASSERT(pathxb.ppathx->linkpths.size() == pathxb.ppathx->brks.size()); 
		vector<P3>& lnkpth = pathxb.ppathx->linkpths.back();
		if (itracked < (int)lnk2D.size()) 
		{
			// retract, but try using curls
			vector<P2> curlout;
			BuildCurl(curlout, ptOut, drOut, params, false);
			int resout = TrackLink(curlout, wclink, false, params);
			bool bUseOut = (resout == (int)curlout.size()); 

			vector<P2> curlin;
			BuildCurl(curlin, ptIn, drIn, params, true);
			int resin = TrackLink(curlin, wc, true, params);
			bool bUseIn = (resin == (int)curlin.size()); 

			P3 ptStartRetract = bUseOut ? ConvertGZ(curlout.back(), pathxb.ppathx->z + params.leadoffdz) : ConvertGZ(wclink.ptcp, pathxb.ppathx->z);
			P3 ptEndRetract = bUseIn ? ConvertGZ(curlin.front(), pathxb.ppathx->z + params.leadoffdz) : ConvertGZ(wc.ptcp, pathxb.ppathx->z);

			if (bUseOut)
			{
				double z = pathxb.ppathx->z;
				double zstep = params.leadoffsamplestep * params.leadoffdz / params.leadofflen;
				for (int is = 0; is < (int)curlout.size() - 1; ++is)
				{
					lnkpth.push_back(ConvertGZ(curlout[is], z));
					z += zstep;
				}
				lnkpth.push_back(ConvertGZ(curlout.back(), pathxb.ppathx->z + params.leadoffdz));
			}
			BuildRetract(lnkpth, ptStartRetract, ptEndRetract, params);
			if (bUseIn)
			{
				double z = pathxb.ppathx->z + params.leadoffdz;
				double zstep = params.leadoffsamplestep * params.leadoffdz / params.leadofflen;
				for (int ie = 0; ie < curlin.size() - 1; ++ie)
				{
					lnkpth.push_back(ConvertGZ(curlin[ie], z));
					z -= zstep;
				}
				lnkpth.push_back(ConvertGZ(curlin.back(), pathxb.ppathx->z));
			}
		}
		else
			BuildLinkZ(lnkpth, lnk2D, pathxb.ppathx->z, params);

		// we've connected to the start
		if (bcellixs.empty()) 
			break; 
	}
}


/////////////////////////////////////////////////////////// 
bool CoreRoughGeneration::RestartAtBCI(BCellIndex& bci, const MachineParams& params, bool bConnectAtStart)  
{
	wc.iu = bci.iu; 
	wc.iv = bci.iv; 
	wc.ConstructCellBounds(); 
	wc.CreateBoundList(); 

	wc.ib = bci.ib; 
	if (bci.ib != -1)  
	{
		ASSERT(!bConnectAtStart); 
		wc.bOnContour = true; 	
		wc.bContouribfvisited = true; 

		wc.lambb = 0.0; 
		int ibl = wc.bolistpairs[wc.ib].first; 
		ASSERT(!wc.GetBoundLower(ibl)); 
		wc.ptcpbb = wc.GetBoundPoint(ibl); 

		P2 Nvbearing = wc.GetBoundPoint(wc.bolistpairs[wc.ib].second) - wc.GetBoundPoint(ibl); 
		wc.vbearing = Nvbearing / Nvbearing.Len(); 

		wc.ptcp = wc.ptcpbb; 
		ASSERT(wc.ptcp == bci.ptcp); 
	}

	else
	{
		wc.bOnContour = false; 	
		wc.ptcp = bci.ptcp; 
		wc.vbearing = bci.vptcp; 

		// check if this point is really going into new stuff.  
		P2 tept = wc.ptcp + wc.vbearing * params.samplestep; 
		vector<I1> lccpath; 
		CircleIntersectNew(lccpath, tept, trad, tsbound, pathxb, trad); 
		if (!bConnectAtStart) 
		{
			if (lccpath.empty()) 
				return false; 
		}
		else
			ASSERT(lccpath.empty()); 
	}

	wc.apvb = APerp(wc.vbearing); 
	ASSERT(pathxb.ppathx->z == pa2gg->z);
	bPrevPointDoubleRange = false; 
	countfreespacesteps = 0; 
	return true; 
}


/////////////////////////////////////////////////////////// 
CoreRoughGeneration::CoreRoughGeneration(PathXSeries* px, const I1& lxrg, const I1& lyrg) : 
	machxrg(lxrg), machyrg(lyrg), pathxb(px) 
{
	pathxb.BuildBoxes(machxrg, 2.0); 
}



/////////////////////////////////////////////////////////// 
void CoreRoughGeneration::FindGoStart()  
{
	// start with a point on the boundary of the stock  
	P2 a0 = tsbound.pths[0]; 
	P2 a1 = tsbound.pths[1]; 
	P2 v = a1 - a0; 
	P2 vn = v / v.Len(); 

	P2 tvec = APerp(vn); 
	P2 a = a0 + CPerp(vn) * trad; 

	pathxb.ppathx->z = pa2gg->z;

	countfreespacesteps	= 0; 
	bPrevPointDoubleRange = false; 
	
	// if model edge is not -1 then this is important.  
    wc.FindCellIndex(a); 
	wc.SetCellCutBegin(a, tvec); 
}



/////////////////////////////////////////////////////////// 
double CoreRoughGeneration::ChangeBearing(const P2& pt, const P2& ltvec, const MachineParams& params) 
{
	P2 tvec = ltvec + CPerp(ltvec) * params.sidecutdisplch; 

	// the vector we can turn towards.  
	P2 tvecright = CPerp(tvec); 

	// find the part of the tool which is touching the stock.  
	vector<I1> lccpath; 
	CircleIntersectNew(lccpath, pt, trad, tsbound, pathxb, trad); 
	ASSERT(lccpath.empty() || ((lccpath.front().lo == 0.0) == (lccpath.back().hi == 4.0))); 

	// control for the change of direction depending on how much stock we are touching.  
	double dch; 
	if (!lccpath.empty())  
	{
		countfreespacesteps = 0; 
		double datvec = tvec.DArg(); 

		// find the index of range which contains this 
		int g; 
		for (g = lccpath.size() - 1; g >= 0; g--) 
			if (lccpath[g].lo <= datvec) 
				break; 
		double ghi; 
		if (g == -1) 
		{
			g = lccpath.size() - 1; 
			ghi = lccpath[g].hi - 4.0; 
		}
		else 
			ghi = lccpath[g].hi; 

		// the point we aim for 
		P2 gpt = P2::InvDarg(ghi < 0.0 ? ghi + 4 : ghi); 

		// protect division by zero case 
		double dchnum = -Dot(tvec, APerp(gpt)); 
		double dchden = Dot(tvecright, APerp(gpt)); 
		double ldch = (dchden != 0.0 ? dchnum / dchden : (ghi >= datvec ? -99.0 : 99.0)); 

		// if the front of the tool is going into the stock, 
		// go to the left to go outwards 
		if (ghi >= datvec) 
		{
			ASSERT(ldch <= MDTOL); 
			dch = max(ldch, params.dchangleft); 
		}

		// if the front of the tool is going out of the stock, 
		// turn right to go more into it.  
		else if (Dot(gpt, tvec) > 0.0) 
		{
			ASSERT(ldch >= -MDTOL); 
			dch = min(ldch, (wc.bOnContour ? params.dchangrightoncontour : params.dchangright)); 
		}

		// touching at some place almost behind, turn to the right anyway
		else
			dch = (wc.bOnContour ? params.dchangrightoncontour : params.dchangright); 


		// decide if we have freshly broken through
		if (lccpath.size() > (((lccpath.front().lo == 0.0) && (lccpath.back().hi == 4.0)) ? 2 : 1)) 
		{
			// we only respond to the first one.  
			if (!bPrevPointDoubleRange)
			{
				// only care about breakthroughs that are not along boundary.  
				if (wc.ib == -1) 
				{
					// go to previous segment 
					int pg = (g == 0 ? lccpath.size() - 1 : g - 1); 
					P2 pgpt = P2::InvDarg(lccpath[pg].hi); 
					if (Dot(tvecright, pgpt) > 0.0) 
						 bcellixs.push_back(BCellIndex(wc, pgpt / pgpt.Len()));  
				}
				bPrevPointDoubleRange = true; 
			}
		}
		else 
			bPrevPointDoubleRange = false; 
	}

	// not touching any stock: spiral ever outwards to the left.  
	else
	{
		if (wc.bOnContour) 
			dch = params.dchangrightoncontour; 
		else 
			dch = params.dchangefreespace * 10 / (10 + countfreespacesteps); 
		countfreespacesteps++; 
		bPrevPointDoubleRange = false; 
	}

	// change the angle we are going in.  
	return -dch; 
}


