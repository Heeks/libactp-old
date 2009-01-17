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



//////////////////////////////////////////////////////////////////////
Partition1::Partition1(const I1& lrg, double w) : 
	bRegular(true) 
{
	int n = (int)(lrg.Leng() / w) + 1; 
	for (int i = 0; i <= n; i++)
		b.push_back(lrg.Along((double)i / n)); 
	ASSERT(GetPart(0).Leng() <= w); 
};

//////////////////////////////////////////////////////////////////////
int Partition1::FindPart(double x) const 
{
	ASSERT(Getrg().Contains(x)); 
	if (bRegular)
	{
		int i = (int)(Getrg().InvAlong(x) * (NumParts() + 1)); 
		if (i > NumParts() - 1)
			i = NumParts() - 1; 
		else if (i < 0)
			i = 0; 
		else if (b[i] > x)
			i--; 
		else if (b[i + 1] <= x)
			i++; 
		ASSERT(GetPart(i).Contains(x)); 
		return i; 
	}

	// otherwise some sort of binary search 
	ASSERT(0); 
	return 0; 	
}

//////////////////////////////////////////////////////////////////////
// this can be optimized when binary searching 
pair<int, int> Partition1::FindPartRG(const I1& xrg) const  
{
	return pair<int, int>(FindPart(xrg.lo), FindPart(xrg.hi)); 
}

