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

#ifndef P3__h
#define P3__h

//////////////////////////////////////////////////////////////////////
struct P3	// 3D point
{
	double x, y, z; 
	bool operator==(const P3& b) const
		{ return ((x == b.x) && (y == b.y) && (z == b.z)); }
	P3()
		{;}
	P3(double* a)
		{ x = a[0]; y = a[1]; z = a[2]; } 
	P3(double lx, double ly, double lz)
		{ x = lx; y = ly; z = lz; } 
	P3(const P3& a)
		{ x = a.x; y = a.y; z = a.z; } 

	P3 operator-(const P3& a) const
		{ return P3(x - a.x, y - a.y, z - a.z); } 
	P3 operator+(const P3& a) const
		{ return P3(x + a.x, y + a.y, z + a.z); } 
	P3 operator-() const
		{ return P3(-x, -y, -z); }
	P3 operator*(double f) const
		{ return P3(x * f, y * f, z * f); } 
	P3 operator/(double f) const
		{ return P3(x / f, y / f, z / f); } 

	double Lensq() const
		{ return x * x + y * y + z * z; } 
	double Len() const
		{ return sqrt(Lensq()); } 

	static P3 CrossProd(const P3& a, const P3& b)
		{ return P3((a.y * b.z - a.z * b.y), 
					-(a.x * b.z - a.z * b.x), 
					(a.x * b.y - a.y * b.x)); }
};

#endif
