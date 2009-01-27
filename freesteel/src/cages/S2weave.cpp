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
void S2weave::SetShape(const I1& lurg, const I1& lvrg, double res) 
{
	ASSERT(ufibs.empty()); 
	ASSERT(vfibs.empty()); 

	urg = lurg; 
	vrg = lvrg; 

	int nufib = (int)(urg.Leng() / res + 2);
	int nvfib = (int)(vrg.Leng() / res + 2); 

	// generate the fibres
	for (int i = 0; i <= nufib; i++) 
		ufibs.push_back(S1(urg.Along((double)i / nufib), vrg, 1)); 
	for (int j = 0; j <= nvfib; j++) 
		vfibs.push_back(S1(vrg.Along((double)j / nvfib), urg, 2)); 

	firstcontournumber = 0; 
	lastcontournumber = firstcontournumber - 1; 
}


//////////////////////////////////////////////////////////////////////
P2 S2weaveB1iter::GetPoint()  
{
	return (ftype == 1 ? P2(wp, w) : P2(w, wp)); 
}


//////////////////////////////////////////////////////////////////////
int FindInwards(const vector<S1>& wfibs, double lw, bool blower, double lwp, double lwpend, bool bedge) 
{
	// just do this as an inefficient loop for now.  
	if (blower)
	{
		for (int i = 0; i < (int)wfibs.size(); i++) 
		{
			if (wfibs[i].wp > lwpend) 
				break; 
			if (bedge ? (wfibs[i].wp >= lwp) : (wfibs[i].wp > lwp)) 
				if (wfibs[i].Contains(lw)) 
					return i; 
		}
		return -1; 
	}
	else
	{
		for (int i = (int)wfibs.size() - 1; i >= 0; i--) 
		{
			if (wfibs[i].wp < lwpend) 
				break; 
			if (bedge ? (wfibs[i].wp <= lwp) : (wfibs[i].wp < lwp)) 
				if (wfibs[i].Contains(lw)) 
					return i; 
		}
		return -1; 
	}
}


//////////////////////////////////////////////////////////////////////
// this is the real code 
void S2weave::Advance(S2weaveB1iter& al)
{
	bool bedge = true; 
	double wend; 
	while (true)
	{
		
		I1 frg = (al.ftype == 1 ? ufibs : vfibs)[al.ixwp].ContainsRG(al.w); 
		wend = (al.blower ? frg.hi : frg.lo); 
		int lixwp = FindInwards((al.ftype == 1 ? vfibs : ufibs), al.wp, al.blower, al.w, wend, bedge); 

		// hit an end.  
		if (lixwp == -1) 
			break; 

		// we always turn perpendicular
		al.w = al.wp; 
		al.ftype = (al.ftype == 1 ? 2 : 1); 
		al.ixwp = lixwp; 
		al.wp = (al.ftype == 2 ? vfibs[al.ixwp].wp : ufibs[al.ixwp].wp); 
		if (al.ftype == 1)
			al.blower = !al.blower; 

		bedge = false; 
	}

	// we've hit an endpoint, go to it and reverse 
	al.w = wend; 
	al.blower = !al.blower; 
}


//////////////////////////////////////////////////////////////////////
void S2weave::TrackContour(vector<P2>& pth, S2weaveB1iter al)  
{
	lastcontournumber++; 
	ASSERT(pth.empty()); 
	while (ContourNumber(al) < firstcontournumber) 
	{
		ContourNumber(al) = lastcontournumber; 
		pth.push_back(al.GetPoint()); 
		Advance(al); 
	}
	pth.push_back(al.GetPoint()); 
	ASSERT(ContourNumber(al) == lastcontournumber); 
}




//////////////////////////////////////////////////////////////////////
int& S2weave::ContourNumber(S2weaveB1iter& al)  
{
	S1& wfib = (al.ftype == 1 ? ufibs : vfibs)[al.ixwp]; 
	for (int i = (al.blower ? 0 : 1); i < (int)wfib.size(); i += 2) 
		if (wfib[i].w == al.w) 
			return wfib[i].contournumber; 
	static int balls = 1; 
	ASSERT(0); 
	return balls; 
}

//////////////////////////////////////////////////////////////////////
void S1::SetAllCutCodes(int lcutcode) 
{
	for (unsigned int i = 0; i < size(); i++) 
		operator[](i).cutcode = lcutcode; 
}

//////////////////////////////////////////////////////////////////////
void S2weave::SetAllCutCodes(int lcutcode)  
{
	for (unsigned int iu = 0; iu < ufibs.size(); iu++) 
		ufibs[iu].SetAllCutCodes(lcutcode); 
	for (unsigned int iv = 0; iv < vfibs.size(); iv++) 
		vfibs[iv].SetAllCutCodes(lcutcode); 
}

void S2weave::Invert()
{
    for (unsigned int iu = 0; iu < ufibs.size(); iu++)
        ufibs[iu].Invert();

    for (unsigned int iv = 0; iv < vfibs.size(); iv++)
        vfibs[iv].Invert();
}




