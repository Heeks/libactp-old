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
bool S1::Check() const
{
	if (size() % 2 != 0) 
		return false;
        int i;
	for (i = 1; i < (int)size(); i++) 
		ASSERT(operator[](i - 1).w <= operator[](i).w); 
	for (i = 1; i < (int)size(); i += 2) 
		ASSERT(operator[](i - 1).blower && !operator[](i).blower); 
	return true; 
}


//////////////////////////////////////////////////////////////////////
void S1::Merge(const I1& rg)  
{
	Merge(rg.lo, false, rg.hi, false);   
}

//////////////////////////////////////////////////////////////////////
void S1::Minus(const I1& rg)  
{
	Minus(rg.lo, false, rg.hi, false);   
}

//////////////////////////////////////////////////////////////////////
pair<int, int> S1::Loclohi(const I1& rg) const
{
	pair<int, int> res; 
	for (res.first = 0; res.first < (int)size(); res.first++) 
		if (operator[](res.first).w >= rg.lo)
			break; 

	if (res.first < (int)size())  
	{
		for (res.second = (int)size() - 1; res.second >= res.first; res.second--) 
			if (operator[](res.second).w <= rg.hi)
				break; 
	}
	else 
		res.second = res.first - 1; 
		 
	#ifdef MDEBUG
	ASSERT(res.first <= res.second + 1); 

	if (res.first < (int)size()) 
		ASSERT(rg.lo <= operator[](res.first).w); 
	if (res.first - 1 >= 0) 
		ASSERT(rg.lo > operator[](res.first - 1).w); 

	if ((res.second >= 0) && (res.second < (int)size())) 
		ASSERT(rg.hi >= operator[](res.second).w); 
	if (res.second + 1 < (int)size()) 
		ASSERT(rg.hi < operator[](res.second + 1).w); 
	#endif

	return res; 
}


//////////////////////////////////////////////////////////////////////
// we could have many speeding up cases  
void S1::Merge(double rglo, bool binterncellboundlo, double rghi, bool binterncellboundhi)  
{
	pair<int, int> ilr = Loclohi(I1(rglo, rghi));
	int il = ilr.first; 
	int ir = ilr.second; 

	// off the end
	if (il == (int)size())  
	{
		push_back(B1(rglo, true, binterncellboundlo)); 
		push_back(B1(rghi, false, binterncellboundhi)); 
		ASSERT(Check()); 
		return; 
	}

	// no boundaries between the range.  
	if (il > ir) 
	{
		if (operator[](il).blower)
		{
			insert(begin() + il, 2, B1(rghi, false, binterncellboundhi)); 
			operator[](il) = B1(rglo, true, binterncellboundlo); 
			ASSERT(Check()); 
		}
		return; 
	}

	// we have il to ir inclusive within the range 
	if (!operator[](ir).blower) 
	{
		operator[](ir) = B1(rghi, false, binterncellboundhi); 
		ir--; 
	}
	if (operator[](il).blower)
	{
		operator[](il) = B1(rglo, true, binterncellboundlo); 
		il++; 
	}

	if (il <= ir) 
		erase(begin() + il, begin() + ir + 1); 
	ASSERT(Check()); 
}; 

//////////////////////////////////////////////////////////////////////
void S1::Minus(double rglo, bool binterncellboundlo, double rghi, bool binterncellboundhi)  
{
	pair<int, int> ilr = Loclohi(I1(rglo, rghi));
	int il = ilr.first; 
	int ir = ilr.second; 

	if (il == (int)size())  
		return; 

	if (ir < il) 
	{
		if (operator[](il).blower)
			return;

		insert(begin() + il, B1(rghi, true, binterncellboundhi)); 
		insert(begin() + il, B1(rglo, false, binterncellboundlo)); 
		ASSERT(Check()); 
		return; 
	}

	// we have il to ir inclusive within the range 
	if (!operator[](il).blower) 
	{
		operator[](il) = B1(rglo, false, binterncellboundlo); 
		++il;
	}
	if (operator[](ir).blower)
	{
		operator[](ir) = B1(rghi, true, binterncellboundlo); 
		--ir;
	}

	if (il <= ir) 
		erase(begin() + il, begin() + ir + 1); 
	ASSERT(Check()); 
}; 


//////////////////////////////////////////////////////////////////////
bool S1::Contains(double lw) const 
{
	for (int i = 1; i < (int)size(); i += 2) 
		if ((operator[](i - 1).w <= lw) && (operator[](i).w >= lw)) 
			return true; 
	return false; 
}

//////////////////////////////////////////////////////////////////////
I1 S1::ContainsRG(double lw) const 
{
	for (int i = 1; i < (int)size(); i += 2) 
		if ((operator[](i - 1).w <= lw) && (operator[](i).w >= lw)) 
			return I1(operator[](i - 1).w, operator[](i).w); 

	ASSERT(0); 
	return I1unit; 
}

//////////////////////////////////////////////////////////////////////
void S1::Invert() 
{
	if (empty()) 
	{
		push_back(B1(wrg.lo, true)); 
		push_back(B1(wrg.hi, false)); 
		ASSERT(Check()); 
		return; 
	}

	// invert the flags 
	for (unsigned int i = 0; i < size(); i++) 
		operator[](i).blower = !operator[](i).blower; 

	// the front condition  
	if (front().w == wrg.lo)
	{
		ASSERT(!front().blower); 
		erase(begin()); 
	}
	else
		insert(begin(), B1(wrg.lo, true)); 

	// the back condition 
	if (back().w == wrg.hi) 
	{
		ASSERT(back().blower); 
		pop_back(); 
	}
	else
		push_back(B1(wrg.hi, false)); 

	ASSERT(Check()); 
}

