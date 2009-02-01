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

#ifndef macros__h
#define macros__h

#ifdef WIN32
#pragma warning(disable:4390) // Empty statements in code - caused by ASSERT statements in release
#endif

//////////////////////////////////////////////////////////////////////
//#define MDEBUG 1

#ifdef MDEBUG

void OutputDebugStringG(const char* str);
void OutputDebugStringG(const char* str0, const char* strf, int line1); 

#define MDTOL 0.001

#define DEBUG_ONLY(X) X
#define ASSERT(X) do { if (!(X))  OutputDebugStringG("Assert failed: ", __FILE__, __LINE__); } while (false) 
#define TOL_ZERO(X) do { if (fabs(X) > MDTOL)  OutputDebugStringG("Tol-Zero failed: ", __FILE__, __LINE__); } while (false) 
#define IF_TOL_ZERO(B, X) do { if ((B) && (fabs(X) > MDTOL))  OutputDebugStringG("If-Tol-Zero failed: ", __FILE__, __LINE__); } while (false) 

#else

#define MDTOL DO_NOT_USE_THIS_VALUE_IN_NONDEBUG_BUILD

#define DEBUG_ONLY(X)
#define ASSERT(X)
#define TOL_ZERO(X)
#define IF_TOL_ZERO(B, X)

#endif



#endif
