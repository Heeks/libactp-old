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

#ifndef S1_H
#define S1_H


//////////////////////////////////////////////////////////////////////
// an endpoint in the fibre.  
struct B1
{
	double w; 

	bool blower; 
	bool binterncellbound; 
	int contournumber; 

	mutable int cutcode; 

	B1() {;}; 
	B1(double lw, bool lblower, bool lbinterncellbound = false) : 
		w(lw), blower(lblower), binterncellbound(lbinterncellbound), contournumber(-1) {;}; 

	bool operator<(const B1& b) const 
		{ return (w < b.w); }; 
}; 

//////////////////////////////////////////////////////////////////////
// this is a fibre 
struct S1 : vector<B1> 
{
	double wp; // the perpendicular position.  
	I1 wrg; 

	int ftype; // 1 for ufibre, 2 for vfibre

	void Merge(const I1& rg); 
	void Merge(double rglo, bool binterncellboundlo, double rghi, bool binterncellboundhi); 
	void Minus(const I1& rg); 
	void Minus(double rglo, bool binterncellboundlo, double rghi, bool binterncellboundhi);

	pair<int, int> Loclohi(const I1& rg) const;


	void Invert(); 

	bool Check() const; 

	bool Contains(double lw) const; 
	I1 ContainsRG(double lw) const; 

	void SetNew(double lwp, const I1& lwrg, int lftype)
		{ wp = lwp; wrg = lwrg; ftype = lftype; clear(); }; 

	S1() {;}; 
	S1(double lwp, I1& lwrg, int lftype) 
		{ SetNew(lwp, lwrg, lftype); }; 

	void SetAllCutCodes(int lcutcode); 
}; 


#endif 

