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

template<class V>
struct vo
{
	V* v; 
	vo(bool bInit = true) : 
		v(bInit ? V::New() : NULL) {;}; 
	~vo() 
		{ if (v != NULL)  v->Delete(); }; 
	V* operator->() 
		{ return v; }; 
	V* operator&() 
		{ return v; }; 
	void Renew() 
		{ if (v != NULL)  v->Delete(); 
		  v = V::New(); }; 
}; 

template<class V>
struct vectorvo 
{
	vector<V*> vv; 

#ifndef SWIG
	V* operator[](int i) 
		{ return vv[i]; }; 
#endif

  void resize(int n) 
		{ ASSERT(vv.empty()); 
		  vv.resize(n); 
		  for (int i = 0; i < n; i++) 
			vv[i] = V::New(); 
		}
	~vectorvo() 
		{ for (int i = 0; i < vv.size(); i++) 
			vv[i]->Delete(); 
		}; 
}; 

