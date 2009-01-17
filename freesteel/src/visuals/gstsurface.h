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
#ifndef GSTSURFACE_H
#define GSTSURFACE_H

//struct vector_PathX : vector<PathX> {;}; 

/////////////////////////////////////////////////////////// 
class GSTbase
{
public:
	vo<vtkActorCollection> actors;
	I1 xrg, yrg, zrg;

	void AddToRenderer(class vtkRenderer* ren);
	bool GetVisibility();
	void SetVisibility(bool bVisible);
  PathXSeries MakeRectBoundary(double lz);

	virtual ~GSTbase() {};
};

/////////////////////////////////////////////////////////// 
class GSTsurface : public GSTbase
{
public: 
	vo<vtkSTLReader> stl; 
	vo<vtkPolyDataMapper> stlMap; 

	void LoadSTL(const char* fname); 
	void PushTrianglesIntoSurface(SurfX& sx); 

	virtual ~GSTsurface() {};
}; 

/////////////////////////////////////////////////////////// 
class GSTtoolpath : public GSTbase
{
public: 
	vector<PathXSeries> ftpaths;

	vo<vtkPolyData> ftpolydata;
	vo<fsvtkToolpathMapper> ftpolydataMap;

	// our single tool definition 
	ToolShape toolshape;

	// the boundary we used to generate the toolpath
	PathXSeries bound;

  // Update polydata from the PathXSeries
	void UpdateFromPax();
//    void UpdateFromToolShape();
    void SetDrawWhole();

  void addPath(const PathXSeries* path) { ftpaths.push_back(*path); }

	virtual ~GSTtoolpath() {};
}; 

#endif
