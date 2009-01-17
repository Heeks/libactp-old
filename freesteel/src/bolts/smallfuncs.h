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

#ifndef smallfuncs__h
#define smallfuncs__h




//////////////////////////////////////////////////////////////////////
inline P3 ConvertGZ(const P2& a, double lz)
{
	return P3(a.u, a.v, lz); 
} 

//////////////////////////////////////////////////////////////////////
inline P3 ConvertCZ(const P3& a, double lz)
{
	return P3(a.x, a.y, lz); 
} 

//////////////////////////////////////////////////////////////////////
inline P2 ConvertLZ(const P3& a)
{
	return P2(a.x, a.y); 
} 

//////////////////////////////////////////////////////////////////////
inline double Square(double a)
{
	return a * a;
}

//////////////////////////////////////////////////////////////////////
inline double Half(double a, double b)
{
	return (a + b) / 2; 
}
//////////////////////////////////////////////////////////////////////
inline P2 Half(const P2& a, const P2& b)
{
	return P2((a.u + b.u) / 2, (a.v + b.v) / 2);
}
//////////////////////////////////////////////////////////////////////
inline P3 Half(const P3& a, const P3& b)
{
	return P3((a.x + b.x) / 2, (a.y + b.y) / 2, (a.z + b.z) / 2);
}

//////////////////////////////////////////////////////////////////////
inline double Along(double lam, double a, double b)
{
	ASSERT(I1unit.Contains(lam)); 
	return (a * (1.0 - lam) + b * lam); 
}

//////////////////////////////////////////////////////////////////////
inline P2 Along(double lam, P2 a, P2 b)
{
	ASSERT(I1unit.Contains(lam)); 
    return (a * (1.0 - lam) + b * lam);
}

//////////////////////////////////////////////////////////////////////
inline P3 Along(double lam, const P3& a, const P3& b)
{
	ASSERT(I1unit.Contains(lam)); 
    return (a * (1.0 - lam) + b * lam);
}
//////////////////////////////////////////////////////////////////////
// use in tol-zero checks without the unit restriction.  
#ifdef MDEBUG
inline P2 AlongD(double lam, P2 a, P2 b)
{
    return (a * (1.0 - lam) + b * lam);
}
#endif

//////////////////////////////////////////////////////////////////////
inline double Dot(const P2& a, const P2& b)
{
	return a.u * b.u + a.v * b.v; 
} 

//////////////////////////////////////////////////////////////////////
inline double InvAlong(P2 al, P2 a, P2 b)
{
    TOL_ZERO(Dot((al - a), (b - a)));
    double lam = (al - a).Len() /(b - a).Len();
    TOL_ZERO((al - Along(lam, a, b)).Len());

    return lam;
}

inline double InvAlong(double x, double a, double b) 
{
	ASSERT(a != b); 
	ASSERT(((x >= a) || (x >= b)) && ((x <= a) || (x <= b))); 
	return (x - a) / (b - a); 
}


//////////////////////////////////////////////////////////////////////
inline double Dot(const P3& a, const P3& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z; 
} 

//////////////////////////////////////////////////////////////////////
inline double DotLZ(const P2& a, const P3& b)
{
	return a.u * b.x + a.v * b.y; 
} 


//////////////////////////////////////////////////////////////////////
inline double PosSqrt(double a)
{
	if (a > 0.0)
		return sqrt(a); 
	TOL_ZERO(a); 
	return 0.0; 
}

//////////////////////////////////////////////////////////////////////
inline double Distsq(const P2& p, const I1& urg, const I1& vrg)  
{
	return P2(urg.Distance(p.u), vrg.Distance(p.v)).Len(); 
}




//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
template<class T>
inline bool Equal(const T& a1, const T& a2, const T& a3) 
{
	return ((a1 == a2) && (a1 == a3)); 
}

//////////////////////////////////////////////////////////////////////
template<class T>
inline bool EqualOr(const T& a, const T& b1, const T& b2) 
{
	return ((a == b1) || (a == b2)); 
}


//////////////////////////////////////////////////////////////////////
// math headers
//////////////////////////////////////////////////////////////////////

#endif
