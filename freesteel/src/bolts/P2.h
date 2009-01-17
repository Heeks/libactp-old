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

#ifndef P2__h
#define P2__h

#include "debugfuncs.h"

//////////////////////////////////////////////////////////////////////
// gotta find the real places for this, or use a const value 
#define MPI 3.14159265358979
#define M2PI (2*MPI)


//////////////////////////////////////////////////////////////////////
struct P2	// 2D point
{
	double u, v; 

	P2()
		{;}
	P2(double lu, double lv)
		{ u = lu; v = lv; } 
	P2(const P2& a)
		{ u = a.u; v = a.v; } 
	void SetVal(double lu, double lv)
		{ u = lu; v = lv; } 

	P2 operator-(const P2& a) const
		{ return P2(u - a.u, v - a.v); } 
	P2 operator+(const P2& a) const
		{ return P2(u + a.u, v + a.v); } 
	P2 operator*(double f) const
		{ return P2(u * f, v * f); } 
	P2 operator/(double f) const
		{ return P2(u / f, v / f); } 

	const P2& operator+=(const P2& a)
		{ SetVal(u + a.u, v + a.v); return *this; } 

	double Lensq() const
		{ return u * u + v * v; } 
	double Len() const
		{ return sqrt(Lensq()); } 
	double DArg() const; 
	double Arg() const; 

	static P2 InvDarg(double a);

	bool operator==(const P2& b) const
		{ return ((u == b.u) && (v == b.v)); }
	bool operator!=(const P2& b) const
		{ return ((u != b.u) || (v != b.v)); } 
};


//////////////////////////////////////////////////////////////////////
inline double P2::DArg() const
{
	ASSERT(Lensq() != 0.0F);
	double res;
	
	// guard against float exceptions
	if ((u == 0.0) && (v == 0.0)) 
	{
		ASSERT(0); 
		return(0.0); 
	}

	if (v >= 0.0)
	{
		if (u >= 0.0)
			res = v / (u + v);
		else
			res = 1.0 - u / (-u + v);
	}
	else
	{
		if (u < 0.0)
			res = 2.0 - v / (-u - v);
		else
		{
			res = 3.0 + u / (u - v);
			if (res == 4.0)	
				res = 0.0;
		}
	}

	ASSERT((res >= 0.0) && (res < 4.0));
	return(res);
}

//////////////////////////////////////////////////////////////////////
inline P2 P2::InvDarg(double a) 
{
	if (a == 4.0) 
		a = 0.0; 
	ASSERT((0.0 <= a) && (a < 4.0));
	P2 res((a < 2.0 ? (1.0 - a) : (a - 3.0)), (a < 3.0 ? ((a > 1.0) ? (2.0 - a) : a) : (a - 4.0)));

	TOL_ZERO(res.DArg() - a); 
	return res; 
}

//////////////////////////////////////////////////////////////////////
inline double P2::Arg() const
{
	// Use atan2 instead of old Arg code - check that they give the same result for now
	double res = atan2(v, u); 
	if (res < 0.0F)  
		res += 2 * M2PI;
    return res;
}


//////////////////////////////////////////////////////////////////////
inline P2 CPerp(const P2& a)
{
	return P2(a.v, -a.u); 
}

//////////////////////////////////////////////////////////////////////
inline P2 APerp(const P2& a)
{
	return P2(-a.v, a.u); 
}

#endif
