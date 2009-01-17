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
#include "wx/wx.h"
void OutputDebugStringG(const char* str)
{
    wxLogError(wxT(str));
}

void OutputDebugStringG(const char* str0, const char* strf, int line1) 
{
    wxString strline1;
    strline1 << line1;
	wxString ws = wxString(str0) + wxString(" file: ") + wxString(strf) + wxString(" line: ") + strline1; 
    wxLogError(ws);
}

bool ReadDouble(const wxTextCtrl* ctrl, double& val)
{
    wxString sval = ctrl->GetValue();
	return sval.ToDouble(&val);
}

