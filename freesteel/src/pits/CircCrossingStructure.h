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

#ifndef CircCrossingStructure__h
#define CircCrossingStructure__h

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
struct CPara
{
	P2 pt; 
	double darg; 
	bool bClockwiseIn; // clockwise of this point is inside.  

	CPara(const P2& lpt, double ldarg, bool lbClockwiseIn) : 
		pt(lpt), darg(ldarg), bClockwiseIn(lbClockwiseIn) {;}; 

	bool operator<(const CPara& oth) const
		{ return (darg < oth.darg); }; 
}; 

//////////////////////////////////////////////////////////////////////
struct CircCrossingStructure
{
	// the circle position.  
	P2 cpt; 
	double crad; 
	double cradsq; 

	// this marks through the range from 0 to 4 in DArg-space.  
	S1 circrange; 

	// we may in future make a radial series of S1s from the centre so we 
	// can model the thickness of the chip.  

	CircCrossingStructure(const P2& lcpt, double lcrad) : 
		cpt(lcpt), crad(lcrad), cradsq(Square(lcrad)) {;}; 

	// should be a set of boundaries 
	void ChopOutBoundary(const vector<P2>& bound); // creates circrange

	vector<CPara> cpara; // around the circle

	// toolpath hacking.  
	double prad; 
	double pradsq; 
	double cradpprad; 
	double cradppradsq; 
	double cradmpradsq; 

	void SetPrad(double lprad); // precalculations 
	void HackToolCircle(const P2& tpt); 
	void HackToolRectangle(const P2& tpt0, const P2& tpt1); 
};


#endif



