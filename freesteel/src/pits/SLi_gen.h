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

#ifndef SLI_GEN_H
#define SLI_GEN_H



//////////////////////////////////////////////////////////////////////
// normalized ray (where the triangle points have been transformed)  
class NormRay_gen
{
public:
	// the endpoints of the ray are at (0,0,zrg.lo) and (0, 0, zrg.hi)
	I1 zrg; 

	// the ball size
	double radball; 
	double radballsq; 

	// result value (a range), validity from return function.  
	double reslo; 
	bool binterncellboundlo; // tells it's the flat end of the cylinder, which must be covered by a ball.  
	double reshi; 
	bool binterncellboundhi; 

	// puts zrg over the res range.  
	bool TrimToZrg(); 

	NormRay_gen(double lradball, const I1& lzrg) : 
		zrg(lzrg), radball(lradball), radballsq(radball * radball) {;}; 

	// primary validity comes from return value  
	bool BallSlice(const P3& a); 
	bool BallSlice(const P3& a, const P3& b); 
	bool BallSlice(const P3& a, const P3& b1, const P3& b2, const P3& xprod); 
};


//////////////////////////////////////////////////////////////////////
class Ray_gen : public NormRay_gen
{
public: 
	double z; 
	S1* pfib; 

	Ray_gen(double lradball, const I1& lwrg) : 
		NormRay_gen(lradball, lwrg) {;}; 

	void HoldFibre(S1* lpfib, double lz); 

	P3 Transform(const P3& p) 
		{ return (pfib->ftype == 1 ? P3(p.x - pfib->wp, p.z - radball - z, p.y) : P3(p.z - radball - z, p.y - pfib->wp, p.x)); }  


	void BallSlice(const P3& a); 
	void BallSlice(const P3& a, const P3& b); 
	void BallSlice(const P3& a, const P3& b1, const P3& b2); 
}; 








//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// ray holding for flat cases  
class Ray_gen2
{
public:
	S1* pfib; 
	vector<B1> scuts; // local for cutting and offsetting.  

	// the endpoints of the ray are at (0,0,zrg.lo) and (0, 0, zrg.hi)
	I1 urg; 

	// the disc size
	double raddisc; 
	double raddiscsq; 

	// result value (a range), validity from return function.  
	void HoldFibre(S1* lpfib); 
	void ReleaseFibre(); 


	Ray_gen2(double lraddisc) : 
		raddisc(lraddisc), raddiscsq(Square(raddisc)) {;}; 

	void DiscSliceCapN(const P2& a, const P2& b); 
	void LineCut(const P2& a, const P2& b); // fills in the scuts

	P2 Transform(const P2& p) 
		{ return (pfib->ftype == 1 ? P2(p.u - pfib->wp, p.v) : P2(p.v - pfib->wp, p.u)); }  
};






//////////////////////////////////////////////////////////////////////
class SLi_gen
{
public:
	P3 p0;
	P3 p1;
	P3 v01n;
	P3 perp1;
	P3 perp2;
	P2 axis;

	P3 p0p; 

	// position of intersections 
	vector<double> inter; 

	void SetSlicePos(const P3& lp0, const P3& lp1); 

	void SliceTriangle(const P3& a, const P3& b1, const P3& b2); 

	void Convert(vector<I1>& res, const I1& xrg, const I1& yrg, const I1& zrg); 
};



#endif


