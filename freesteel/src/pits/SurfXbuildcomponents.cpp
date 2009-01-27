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

#include <fstream>


//////////////////////////////////////////////////////////////////////
void SurfX::PushTriangle(const P3& p0, const P3& p1, const P3& p2)
{
	// does the triangle touch the region?  
	// this condition overestimates it.  
	// we can have triangles that go diagonally but miss it.  
	if (rangestate == 2)
	{
		if ((p0.x < gxrg.lo) && (p1.x < gxrg.lo) && (p2.x < gxrg.lo)) 
			return; 
		if ((p0.x > gxrg.hi) && (p1.x > gxrg.hi) && (p2.x > gxrg.hi)) 
			return; 
		if ((p0.y < gyrg.lo) && (p1.y < gyrg.lo) && (p2.y < gyrg.lo)) 
			return; 
		if ((p0.y > gyrg.hi) && (p1.y > gyrg.hi) && (p2.y > gyrg.hi)) 
			return; 
		if ((p0.z < gzrg.lo) && (p1.z < gzrg.lo) && (p2.z < gzrg.lo)) 
			return; 
		if ((p0.z > gzrg.hi) && (p1.z > gzrg.hi) && (p2.z > gzrg.hi)) 
			return; 
	}
	else
	{
		ASSERT(EqualOr(rangestate, 0, 1)); 
		bool bFirst = (rangestate == 0);
		gxrg.Absorb(p0.x, bFirst); 
		gyrg.Absorb(p0.y, bFirst); 
		gzrg.Absorb(p0.z, bFirst); 
		rangestate = 1; 
		gxrg.Absorb(p1.x, bFirst); 
		gyrg.Absorb(p1.y, bFirst); 
		gzrg.Absorb(p1.z, bFirst); 
		gxrg.Absorb(p2.x, bFirst); 
		gyrg.Absorb(p2.y, bFirst); 
		gzrg.Absorb(p2.z, bFirst); 
	}

	// push the triangle in.  
	lvd.push_back(p0); 
	lvd.push_back(p1); 
	lvd.push_back(p2); 
}; 

void SurfX::ReadStlFile(const char* filepath)
{
	// read the stl file
	ifstream ifs(filepath, ios::binary);
	if(!ifs)return;

	char solid_string[6] = "aaaaa";
	ifs.read(solid_string, 5);
	if(ifs.eof())return;
	if(strcmp(solid_string, "solid"))
	{
		// try binary file read

		// read the header
		char header[81];
		header[80] = 0;
		memcpy(header, solid_string, 5);
		ifs.read(&header[5], 75);

		unsigned int num_facets = 0;
		ifs.read((char*)(&num_facets), 4);

		float n[3];
		float x[3][3];

		for(unsigned int i = 0; i<num_facets; i++)
		{
			ifs.read((char*)(n), 12);
			ifs.read((char*)(x[0]), 36);
			short attr;
			ifs.read((char*)(&attr), 2);
			PushTriangle(P3(x[0][0], x[0][1], x[0][2]), P3(x[1][0], x[1][1], x[1][2]), P3(x[2][0], x[2][1], x[2][2]));
		}
	}
	else
	{
		// "solid" already found
		char str[1024] = "solid";
		ifs.getline(&str[5], 1024);

		float n[3];
		float x[3][3];
		char five_chars[6] = "aaaaa";

		int vertex = 0;

		while(!ifs.eof())
		{
			ifs.getline(str, 1024);

			int i = 0, j = 0;
			for(; i<5; i++, j++)
			{
				if(str[j] == 0)break;
				while(str[j] == ' ' || str[j] == '\t')j++;
				five_chars[i] = str[j];
			}
			if(i == 5)
			{
				if(!strcmp(five_chars, "verte"))
				{
					sscanf(str, " vertex %f %f %f", &(x[vertex][0]), &(x[vertex][1]), &(x[vertex][2]));
					vertex++;
					if(vertex > 2)vertex = 2;
				}
				else if(!strcmp(five_chars, "facet"))
				{
					sscanf(str, " facet normal %f %f %f", &(n[0]), &(n[1]), &(n[2]));
					vertex = 0;
				}
				else if(!strcmp(five_chars, "endfa"))
				{
					if(vertex == 2)
					{
						PushTriangle(P3(x[0][0], x[0][1], x[0][2]), P3(x[1][0], x[1][1], x[1][2]), P3(x[2][0], x[2][1], x[2][2]));
					}
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////
struct p3X_order
{
	bool operator()(const P3* a, const P3* b)
		{ return ((a->x < b->x) || ((a->x == b->x) && ((a->y < b->y) || ((a->y == b->y) && (a->z < b->z))))); }; 
};


//////////////////////////////////////////////////////////////////////
struct triangXr
{
	P3* a; 
	P3* b1; 
	P3* b2; 
	
	P3 tnorm;	// displacement of the triangle to hit the tool

	triangXr(P3& p0, P3& p1, P3& p2); 
}; 


/////////////////////////////////////////////////////////////////////
triangXr::triangXr(P3& p0, P3& p1, P3& p2) 
{
	a = &p0; 
	b1 = &p1; 
	b2 = &p2; 

	if (!p3X_order()(a, b1))
		swap(a, b1);
	if (!p3X_order()(b1, b2))
		swap(a, b2);

	P3 v1 = *b1 - *a;
	P3 v2 = *b2 - *a; 

	P3 ncross = -P3::CrossProd(v1, v2); 
	double fac = 1.0; 
	if (ncross.z < 0.0)
	{
		swap(b1, b2); 
		fac = -1.0;
	}
	double nclen = ncross.Len(); 
	if (nclen != 0.0)
		fac = fac / nclen; 
	tnorm = ncross * fac; 
}




/////////////////////////////////////////////////////////////////////
struct edgeXr
{
	P3* p0; 
	P3* p1; 
	int itR; // face index 
	int itL; 

	edgeXr(P3* lp0, P3* lp1, int it); 
};

//////////////////////////////////////////////////////////////////////
edgeXr::edgeXr(P3* lp0, P3* lp1, int it)  
{
	if (p3X_order()(lp0, lp1))
	{
		itR = it; 
		itL = -1; 
		p0 = lp0; 
		p1 = lp1; 
	}
	else 
	{
		itR = -1; 
		itL = it; 
		p0 = lp1; 
		p1 = lp0; 
	}
}


//////////////////////////////////////////////////////////////////////
edgeX::edgeX(P3* lp0, P3* lp1, triangX* ltpR, triangX* ltpL)  
{
	p0 = lp0; 
	p1 = lp1; 
	tpR = ltpR; 
	tpL = ltpL; 
}


//////////////////////////////////////////////////////////////////////
struct edgeXr_order
{
	bool operator()(const edgeXr* a, const edgeXr* b)
		{ return ((a->p0 < b->p0) || ((a->p0 == b->p0) && ((a->p1 < b->p1) || ((a->p1 == b->p1) && (a->itR < b->itR))))); }; 
};

//////////////////////////////////////////////////////////////////////
void triangX::SetEdge(edgeX* pe, struct triangXr& r)  
{
	if (((r.a == pe->p0) && (r.b1 == pe->p1)) || ((r.a == pe->p1) && (r.b1 == pe->p0)))  
	{
		ASSERT(ab1 == NULL); 
		ab1 = pe;
	}
	else if (((r.a == pe->p0) && (r.b2 == pe->p1)) || ((r.a == pe->p1) && (r.b2 == pe->p0)))  
	{
		ASSERT(ab2 == NULL); 
		ab2 = pe;
	}
	else if (((r.b1 == pe->p0) && (r.b2 == pe->p1)) || ((r.b1 == pe->p1) && (r.b2 == pe->p0)))  
	{
		ASSERT(b12 == NULL); 
		b12 = pe;  
	}
	else 
		ASSERT(0); 
}



//////////////////////////////////////////////////////////////////////
// this is the tooldef specific part  
void SurfX::BuildComponents()  
{
	// first sort all the points by increasing x
	int np = lvd.size(); // 3 times the number of triangles.  
	vector<P3*> p3X;
  int i; 
	for (i = 0; i < np; i++)
		p3X.push_back(&(lvd[i])); 
	sort(p3X.begin(), p3X.end(), p3X_order()); 

	// make the indexes into this array with duplicates removed 
	ltd.resize(np);
	for (i = 0; i < np; i++)
	{
		P3* pi = p3X[i]; 
		if (vdX.empty() || !(vdX.back() == *pi))
			vdX.push_back(*pi); 
		ltd[pi - &(lvd[0])] = (int)vdX.size() - 1; 
	}

	// kill the old arrays
	lvd.clear(); 
	p3X.clear(); 


	// build up oriented triangles with normals and remove degenerate ones
	int nt = np / 3; 
	vector<triangXr> ttx; 
	for (i = 0; i < nt; i++) 
		ttx.push_back(triangXr(vdX[ltd[i * 3]], vdX[ltd[i * 3 + 1]], vdX[ltd[i * 3 + 2]])); 

	// now make the array of linked edges
	vector<edgeXr> edXr; 
	for (i = 0; i < (int)ttx.size(); i++) 
	{
		edXr.push_back(edgeXr(ttx[i].a, ttx[i].b1, i)); 
		edXr.push_back(edgeXr(ttx[i].b1, ttx[i].b2, i)); 
		edXr.push_back(edgeXr(ttx[i].b2, ttx[i].a, i)); 
	}

	vector<edgeXr*> pedXr; 
	for (i = 0; i < (int)edXr.size(); i++) 
		pedXr.push_back(&(edXr[i])); 
	sort(pedXr.begin(), pedXr.end(), edgeXr_order()); 


	// build the final array of triangles into which the edges will point 
	for (i = 0; i < (int)ttx.size(); i++)
		trX.push_back(triangX(ttx[i].tnorm)); 

	// build the final array of edges with pointers into these triangles
	i = 0; 
	while (i < (int)pedXr.size()) 
	{
		// two edges can fuse into one with triangles on both sides
		if ((i + 1 < (int)pedXr.size()) && (pedXr[i]->p0 == pedXr[i + 1]->p0) && (pedXr[i]->p1 == pedXr[i + 1]->p1) && ((pedXr[i]->itL == -1) != (pedXr[i + 1]->itL == -1))) 
		{
			if (pedXr[i]->itL == -1)
				edX.push_back(edgeX(pedXr[i]->p0, pedXr[i]->p1, &(trX[pedXr[i]->itR]), &(trX[pedXr[i + 1]->itL]))); 
			else
				edX.push_back(edgeX(pedXr[i]->p0, pedXr[i]->p1, &(trX[pedXr[i + 1]->itR]), &(trX[pedXr[i]->itL]))); 
			i += 2; 
		}

		// one triangle sided edge
		else 
		{
			edX.push_back(edgeX(pedXr[i]->p0, pedXr[i]->p1, (pedXr[i]->itR != -1 ? &(trX[pedXr[i]->itR]) : NULL), (pedXr[i]->itL != -1 ? &(trX[pedXr[i]->itL]) : NULL))); 
			i++; 
		}
	}
	edXr.clear(); 
	pedXr.clear(); 

	// put the backpointers to the edges into the triangles
	for (i = 0; i < (int)edX.size(); i++)
	{
		if (edX[i].tpL != NULL)
			edX[i].tpL->SetEdge(&(edX[i]), ttx[edX[i].tpL - &(trX[0])]); 
		if (edX[i].tpR != NULL)
			edX[i].tpR->SetEdge(&(edX[i]), ttx[edX[i].tpR - &(trX[0])]); 
	}
	ttx.clear(); 
}

