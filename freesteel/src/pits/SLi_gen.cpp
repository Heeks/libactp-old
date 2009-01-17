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
// very rough and brittle slicing which ignores miscalcs on triangle intersections for now  
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void SLi_gen::SetSlicePos(const P3& lp0, const P3& lp1) 
{
	p0 = lp0; 
	p1 = lp1; 
	P3 v01 = p1 - p0;
	v01n = v01 / v01.Len();

	p0p = p0 - v01n * Dot(v01n, p0); 
	TOL_ZERO(Dot(p0p, v01n)); 

	// find two perpendicular axes  
	perp1 = P3::CrossProd(P3(0,0,1), v01n); 
	if (perp1.Lensq() < 0.001)
		perp1 = P3::CrossProd((fabs(v01n.x) < fabs(v01n.y) ? P3(1,0,0) : P3(0,1,0)), v01n); 
	ASSERT(perp1.Lensq() != 0); 
	perp2 = P3::CrossProd(perp1, v01n); 

	TOL_ZERO(Dot(v01n, perp1)); 
	TOL_ZERO(Dot(v01n, perp2)); 


	axis = P2(Dot(perp1, p0), Dot(perp2, p1)); 

	// the array of intersections 
	inter.clear(); 
};


//////////////////////////////////////////////////////////////////////
void SLi_gen::SliceTriangle(const P3& a, const P3& b1, const P3& b2) 
{
	// resolve into transformed coordinates 
	P2 ta(Dot(perp1, a), Dot(perp2, a)); 
	P2 tb1(Dot(perp1, b1), Dot(perp2, b1)); 
	P2 tb2(Dot(perp1, b2), Dot(perp2, b2)); 

	P2 tv1 = tb1 - ta;
	P2 tv2 = tb2 - ta;

	P2 m = axis - ta; 

	// in 2D we need to solve the matrix equation 
	// ( tv1.u    tv2.u )   ( l1 )     ( m.u )
	// ( tv1.v    tv2.v ) * ( l2 )  =  ( m.v ).  

	// determinant
	double det = tv1.u * tv2.v - tv1.v * tv2.u; 
	if (det == 0.0)
		return; 

	double l1 = (tv2.v * m.u - tv2.u * m.v) / det; 
	if (l1 <= 0.0)
		return;
	double l2 = (-tv1.v * m.u + tv1.u * m.v) / det; 
	if ((l2 <= 0.0) || (l1 + l2 >= 1.0))
		return;

	// point of intersection 
	P3 p = a * (1.0 - l1 - l2) + b1 * l1 + b2 * l2; 

	double lamf = Dot(v01n, p); 

	TOL_ZERO((p0p + v01n * lamf - p).Len()); 

	inter.push_back(lamf); 
}

//////////////////////////////////////////////////////////////////////
void SLi_gen::Convert(vector<I1>& res, const I1& xrg, const I1& yrg, const I1& zrg)  
{
	// find the operating range 
	I1 rg(Dot(v01n, p0), Dot(v01n, p1)); 
	if ((v01n.x != 0.0) && !rg.Intersect(xrg / v01n.x)) 
		return; 
	if ((v01n.y != 0.0) && !rg.Intersect(yrg / v01n.y)) 
		return; 
	if ((v01n.z != 0.0) && !rg.Intersect(zrg / v01n.z)) 
		return; 

	sort(inter.begin(), inter.end()); 
	ASSERT(inter.empty() || (inter.front() <= inter.back())); 
	for (int i = 1; i < (int)inter.size(); i += 2) 
	{
		I1 lrg(inter[i - 1], inter[i]); 
		if (lrg.Intersect(rg)) 
			res.push_back(lrg); 
	}

	ASSERT(inter.size() % 2 == 0); 
	//if (inter.size() % 2 != 0)  
	//	res.push_back(I1(inter.back(), inter.back())); 
}


//////////////////////////////////////////////////////////////////////
void SurfX::SliceRay(SLi_gen& sgen)
{
	// triangles
	for (int i = 0; i < (int)trX.size(); i++)
		sgen.SliceTriangle(*(trX[i].b12->p0), *(trX[i].b12->p1), *(trX[i].ThirdPoint())); 

	if (1)
		return; 
}
