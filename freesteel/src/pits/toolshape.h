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

#ifndef TOOLSHAPE_H
#define TOOLSHAPE_H
//////////////////////////////////////////////////////////////////////
class ToolShape	
{
public:
	double flatrad; // shaftrad - cornerrad 
	double cornerrad; 

	double sliceheight; 
	int ntoolslices; 

	int nang; 

	ToolShape()
        {
            flatrad = 0;
            cornerrad = 0;
        }; 

	ToolShape(double lflatrad, double lcornerrad, double shaftlength, double lsliceheight) : 
		flatrad(lflatrad), cornerrad(lcornerrad), sliceheight(lsliceheight), ntoolslices((int)((cornerrad + sliceheight) / sliceheight))
			{ nang = 31;}; 

	double RadAtHeight(double lz); 

	void ConstructToolShape(vector< vector <P3> >& slcs);
	void ConstructToolShape(class vtkPolyData* ppd, class vtkFloatArray* pdcontactsca); 

	void Draw(const P3& iptpath, vector<PathXSeries>* pftpaths, int ipath, int iseg, const vector<P2>& bound);
	void Draw(const P3& pt);
	void DrawColourRange(const P3& iptpath, double z, double rad, const S1& rg);

}; 


#endif


