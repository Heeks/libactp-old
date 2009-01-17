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

#ifndef I1__h
#define I1__h

//////////////////////////////////////////////////////////////////////
struct I1	// Interval
{
	double lo;
	double hi;

	I1()
		{;}
	I1(double llo, double lhi)
		{ lo = llo; hi = lhi; 
		  ASSERT(lo <= hi); } 
	void SetRan(double llo, double lhi)
		{ lo = llo; hi = lhi; 
		  ASSERT(lo <= hi); } 

	void SetLo(double a) 
		{ ASSERT((lo <= hi) && (a <= hi)); 
		  lo = a; } 
	void SetHi(double a) 
		{ ASSERT((lo <= hi) && (lo <= a)); 
		  hi = a; } 

	void Absorb(double a)
		{ if (a < lo)  lo = a; 
		  else if (a > hi)  hi = a; }
	bool Absorb(double a, bool bFirst)
		{ if (bFirst || (a < lo))  lo = a; 
		  if (bFirst || (a > hi))  hi = a; 
		  return false; }  // return value for syntax to reset a bFirst variable. 
	void Absorb(const I1& v)
		{ if (v.lo < lo)  lo = v.lo; 
		  else if (v.hi > hi)  hi = v.hi; } 
	bool Absorb(const I1& v, bool bFirst)
		{ if (bFirst || (v.lo < lo))  lo = v.lo; 
		  if (bFirst || (v.hi > hi))  hi = v.hi; 
		  return false; } // return value for syntax to reset a bFirst variable. 

	void Combine(double a, double b)
		{ if (a < b)
			{ lo = a; hi = b; } 
		  else
			{ lo = b; hi = a; } 
		}  
	void Combine(double a, double b, double c)
		{ Combine(a, b); Absorb(c); } 
	static I1 SCombine(double a, double b) 
		{ I1 res; res.Combine(a, b); return res; }; 
	static I1 SCombine(double a, double b, double c) 
		{ I1 res; res.Combine(a, b, c); return res; }; 


	I1 Inflate(double x) const 
		{ return I1(lo - x, hi + x); } 
	bool Intersect(const I1& v) 
		{ if (v.lo > lo)  lo = v.lo; 
		  if (v.hi < hi)  hi = v.hi; 
		  return lo <= hi; }

	bool Contains(double x) const 
		{ return ((lo <= x) && (x <= hi)); } 
	bool ContainsWithin(double x, double e) const 
		{ return ((lo - e <= x) && (x <= hi + e)); } 
	bool Contains(const I1& v) const 
		{ return ((lo <= v.lo) && (v.hi <= hi)); } 

	double Leng() const
		{ return hi - lo; }; 
	double Along(double lam) const
		{ return lo * (1.0 - lam) + hi * lam; }; 
	double Half() const
		{ return (lo + hi) * 0.5; }; 
	double InvAlong(double x) const
		{ ASSERT((Leng() != 0.0) && Contains(x)); return (x - lo) / Leng(); }; 
	double Distance(double x) const 
		{ return (x > lo ? (x < hi ? 0.0 : x - hi) : lo - x); }; 
	double PushInto(double x) const 
		{ return (x < lo ? lo : (x > hi ? hi : x)); }; 
	double PushIntoSmall(double x) const 
		{ TOL_ZERO(Distance(x)); 
		  return PushInto(x); }; 

	I1 operator+(double d) const
		{ return I1(lo + d, hi + d); } 
	I1 operator-(double d) const
		{ return I1(lo - d, hi - d); } 

	I1 operator*(double d) const 
		{ return (d > 0 ? I1(lo * d, hi * d) : I1(hi * d, lo * d)); }; 
	I1 operator/(double d) const 
		{ return operator*(1 / d); }; 

	bool operator==(const I1& b) const 
		{ return ((lo == b.lo) && (hi == b.hi)); } 
}; 


// this should be a const global variable 
#define I1unit I1(0.0, 1.0) 

#endif
