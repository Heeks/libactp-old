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

#ifndef S2WEAVECELL_H
#define S2WEAVECELL_H


//////////////////////////////////////////////////////////////////////
struct S2weaveCell
{
	class S2weave* ps2w; 

	// indexes for finding the cell.  
	// With subdivision the indexing will be more complex.  
	int iu; 
	int iv; 

	// cell boundary pointers (which get changed by splitting)
	const S1* pfulo; 
	const S1* pfuhi; 
	const S1* pfvlo; 
	const S1* pfvhi; 

	// further info about where the cell corners lie in the boundaries.  
	// cell bounds (for ease of viewing).  
	I1 clurg; 
	I1 clvrg; 

	// it goes bottom left, top left, top right, bottom right.  
	P2 GetCorner(int icn) const; 
	const S1* GetSide(int icn) const; // gets the following edge from the corner.  

    // the list of endpoints of fibres in this cell.
    // first int is the edge (left is first), second is the B1 entry in the array.
    vector< pair<int, B1*> > boundlist;
    bool bLDin;
    bool bLUin;
    bool bRUin;
    bool bRDin;

	// index into boundlist which marks connects the 
	// points between the boundary with lines, resolving ambiguities.  
	// not possible to control for order.  
	vector< pair<int, int> > bolistpairs; 


	P2 GetBoundPoint(int ibl); 
	bool GetBoundLower(int ibl); 


	// changing and construction functions 
		void ConstructCellBounds(); 
	    int CreateBoundList(); 
	void FindCellIndex(const P2& lptc); 
	void AdvanceCrossSide(int icn, const P2& cspt); 


	int GetBoundListPosition(int sic, const P2& ptb, bool bOnBoundOutside); 
};



#endif

