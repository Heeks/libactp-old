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

#ifndef PathXSeries__h
#define PathXSeries__h


//////////////////////////////////////////////////////////////////////
// a series of toolpaths
class PathXSeries		
{
public: 
	double z; 

	vector<P2> pths;  // individual 2D points ~ the actual path
	vector<int> brks; // breaks - indices where pths is non-consecutive 

	// 3D paths linking 2D paths at breaks
  // runs parallel to the brks array.  
	vector< vector<P3> > linkpths; 

	PathXSeries() 
		{;}; 
	PathXSeries(double lz) : 
		z(lz) {;} 


	void Add(const P2& pt)
	{
		pths.push_back(pt);
	}

	void Append(const vector<P2>& lpths)
	{
		pths.insert(pths.end(), lpths.begin(), lpths.end());
		Break();
	}

	void Break()
	{
		brks.push_back(pths.size());
		linkpths.push_back(vector<P3>());
	}

	void Pop_back()
	{
		if (brks.empty() || ((int)(pths.size()) != brks.back()))
			pths.pop_back();
	}


// accessor functions for post-processing
	int GetNbrks()
	{
		return brks.size(); 
	}

	int GetBrkIndex(int i)
	{
		return brks[i]; 
	}

	int GetNlnks(int i)
	{
		return linkpths[i].size(); 
	}

	double GetLinkX(int j, int i)
	{
		return linkpths[j][i].x; 
	}

	double GetLinkY(int j, int i)
	{
		return linkpths[j][i].y; 
	}

	double GetLinkZ(int j, int i)
	{
		return linkpths[j][i].z; 
	}

	int GetNpts()
	{
		return pths.size(); 
	}

	double GetX(int i)
	{
		return pths[i].u; 
	}
	double GetY(int i)
	{
		return pths[i].v; 
	}
}; 



//////////////////////////////////////////////////////////////////////
struct ckpline 
{
	int iseg; 
	mutable int idup; 
	double vmid; // the vrange is (vmid - vrad, vmid + vrad) 
	double vrad; 

	ckpline(int liseg, int lidup, double lvmid, double lvrad) : 
		iseg(liseg), idup(lidup), vmid(lvmid), vrad(lvrad) {;}; 
}; 

//////////////////////////////////////////////////////////////////////
struct pucketX
{
	vector<int> ckpoints; 
	vector<ckpline> cklines; 
}; 


//////////////////////////////////////////////////////////////////////
class PathXboxed
{
public: 
	PathXSeries* ppathx; 

	I1 gburg; 
	bool bGeoOutLeft; 
	bool bGeoOutRight; 

	Partition1 upart; 

	// simple buckets running parallel to the partitions.  
	vector<pucketX> puckets; 

	// integer places where the duplicate counters are looked up.  
	PathXSeries tsbound; 
	vector<int> idups; 
	mutable int maxidup; 

  PathXboxed() {}
  PathXboxed(PathXSeries* lppathx) : 
		ppathx(lppathx) {;}; 

	void BuildBoxes(const I1& lgburg, double boxwidth); 

		void PutSegment(int iseg, bool bFirst, bool bRemove); 
	void Add(const P2& p1); 
	void Break(); 
	void Pop_back(); 
}; 


#endif


