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

#ifndef bolts__h
#define bolts__h

#include <math.h>

#include <iostream>
#include <algorithm>
#include <vector>
#include <list>

// min and max on VC6.0 and earlier
#if (defined(WIN32) && (_MSC_VER < 1300) ) 
template <class _Tp>
inline const _Tp& (min)(const _Tp& __a, const _Tp& __b) { return __b < __a ? __b : __a; }
template <class _Tp>
inline const _Tp& (max)(const _Tp& __a, const _Tp& __b) {  return  __a < __b ? __b : __a; }
#endif


#include "debugfuncs.h"

#include "P2.h"
#include "P3.h"
#include "I1.h"
#include "smallfuncs.h"

using namespace std;
#include "S1.h"
#include "Partition1.h"
#include "vo.h"


#endif
