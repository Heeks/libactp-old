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



//////////////////////////////////////////////////////////////////////
SurfX::SurfX(const I1& lgxrg, const I1& lgyrg, const I1& lgzrg) : 
	gxrg(lgxrg), gyrg(lgyrg), gzrg(lgzrg), rangestate(2)  
{;}; 

//////////////////////////////////////////////////////////////////////
SurfX::SurfX() : 
	rangestate(0)  
{;}; 



//////////////////////////////////////////////////////////////////////
// the first two come from b12
P3* triangX::ThirdPoint()
{
	return (((ab1->p0 != b12->p0) && (ab1->p0 != b12->p1)) ? ab1->p0 : ab1->p1); 
}

//////////////////////////////////////////////////////////////////////
P3* triangX::ThirdPoint(edgeX* pe)  
{
	ASSERT((pe == ab1) || (pe == ab2) || (pe == b12));  
	if (pe == b12)
		return ThirdPoint(); 
	return (((b12->p0 != pe->p0) && (b12->p0 != pe->p1)) ? b12->p0 : b12->p1); 
}









