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

#ifndef S2WEAVECELLLINEARCUT_H
#define S2WEAVECELLLINEARCUT_H


//////////////////////////////////////////////////////////////////////
struct S2weaveCellLinearCut : S2weaveCell
{
	// values defining the cut, in terms of a point that 
	// may not be in the cell.  
	P2 ptcst; // start point 
	P2 vbearing; 
	P2 apvb; // APerp(vbearing) 

	double ptcDapvb; // Dot(apvb, ptcst)
	int vbqdrant; 

	
	// the two cut positions within the square 	
	double lamcb; // position of incoming cut (negative if ptcst is in current cell) 
	int sicnb; 
	P2 ptcb;	// approx = ptcst + vbearing * lamcb.  On GetSide(sicnb).  
	int iblb; // bound list position of cut (bool is if exactly on bound) 

	// outgoing cell cut  
	double lamcf; 
	int sicnf; 
	P2 ptcf; 
	int iblf; 

	// indexes into bolistpairs and says whether the crossing is from left (false) or from right 
	// bolistpair has the area on the right hand side.  
	vector< pair<int, bool> > bolistcrossings; 

		void SetCellCutBack(); 
		void SetCellCutFore(bool bOnBoundF); 
		void FindBolistCrossings(); 
		
	void SetCellCut(const P2& lptcst, bool bOnBoundB, bool bOnBoundF, const P2& lvbearing); // first point of cut 
	void AdvanceThroughForeCut(bool bOnBoundF); // advance into next cell of cut.  
								  // sicnf determins which side it is.  
	
	// returns lamalong 
	double Getbolistcrossing(double& lambb, P2& ptcross, int ibb);  
}; 


//////////////////////////////////////////////////////////////////////
struct S2weaveCellLinearCutTraverse : S2weaveCellLinearCut
{
	// position along the line from ptcst 
	// will be between lamcb and lamcf and the point 
	double lamcp; 
	P2 ptcp; 

	// if true then half the cut information is bogus 
	// and subverted to contour following.  
	bool bOnContour; 	
	bool bContouribfvisited; // used to tell whether to update the cutcode which determins the state of the whole edge.  

	// this gives value of next intersection point where we will 
	// hit the boundline of the area.  We could be on it if lamcpbb == lamcp 
	int ibb; 	// indexes bolistcrossings
	int ib; // bolistcrossings[ibb].first (active when bOnContour, overriding ibb)  
	double lamcpbb; // > lamcp and marks the place where we will encounter this line.  
	double lambb; // along the crossing defined by ibb and ib 
	P2 ptcpbb; // when bOnContour true this is the point which marks where we are.  


	// these are different cases for the start of a cut, one works 
	// geometrically and the other topologically, although the first 
	// would be fairer to assume we always begin away from the area boundary.  
		void Findibbfore(int libb); 
	void SetCellCutBegin(const P2& lptcst, const P2& lvbearing); 
	bool SetCellCutContinue(const P2& lvbearing); 

	void AdvanceThroughForeCutTraverse(bool bOnBoundF); // advance into next cell of cut.  
	void AdvanceToLamPos(double llamcp); 
	void AdvanceToContourCut(); 
	void AdvanceAlongContourToLamPos(double llambn); 
	void AdvanceAlongContourAcrossCell(); 

	// functions used in tracking core-roughing
	bool OnContourFollowBearing(double dch, double folldist); 
	double FollowBearing(const P2& lvbearing, double folldist); 
}; 	 


//////////////////////////////////////////////////////////////////////
struct BCellIndex
{
	int iu; 
	int iv; 
	int ib; 
	double lambb; 

	P2 ptcp; 
	P2 vptcp; 

	BCellIndex(S2weaveCellLinearCutTraverse& lwc, const P2& lvptcp = P2(0.0, 0.0)) : 
		iu(lwc.iu), iv(lwc.iv), ib(lwc.ib), lambb(lwc.lambb), ptcp(lwc.ptcp), vptcp(lvptcp)   
		{ ASSERT((ib != -1) == (vptcp.Lensq() == 0.0)); }; 
};




#endif



