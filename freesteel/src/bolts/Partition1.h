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

#ifndef Partition1__h
#define Partition1__h

class Partition1 
{
	vector<double> b;
	bool bRegular; 

public: 
	int NumParts() const
		{ return (b.size() - 1); }
	I1 GetPart(int i) const 
		{ return I1(b[i], b[i + 1]); }

	int FindPart(double x) const; 
	pair<int, int> FindPartRG(const I1& xrg) const; 

	Partition1() 
		{;}; 
	Partition1(const I1& lrg, double w); 

	I1 Getrg() const 
		{ return I1(b.front(), b.back()); }  
};


#endif

