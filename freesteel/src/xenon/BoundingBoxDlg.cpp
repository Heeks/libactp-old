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
#include "wxhelpers.h"

#include "vtkPolyData.h"

#include "bolts/bolts.h"


#include "BoundingBoxDlg.h"


BEGIN_EVENT_TABLE(CBoundingBoxDlg, wxDialog)
    EVT_BUTTON(wxID_OK, CBoundingBoxDlg::OnOk)
END_EVENT_TABLE()

// dialog constructor
CBoundingBoxDlg::CBoundingBoxDlg()
: wxDialog((wxDialog *)NULL, -1, _T("Bounding Box"), wxPoint(10, 10), wxSize(200, 200 + wxDefaultSize.y))
{

    // create and position controls in the dialog

    // text controls
    wxSize edtsize(80, 25);
    
    xminbox = new wxTextCtrl(this, -1, wxString(""), wxPoint(10,30), edtsize, wxTE_RIGHT);
    xmaxbox = new wxTextCtrl(this, -1, wxString(""), wxPoint(100,30), edtsize, wxTE_RIGHT);
    yminbox = new wxTextCtrl(this, -1, wxString(""), wxPoint(10,50), edtsize, wxTE_RIGHT);
    ymaxbox = new wxTextCtrl(this, -1, wxString(""), wxPoint(100,50), edtsize, wxTE_RIGHT);
    zminbox = new wxTextCtrl(this, -1, wxString(""), wxPoint(10,70), edtsize, wxTE_RIGHT);
    zmaxbox = new wxTextCtrl(this, -1, wxString(""), wxPoint(100,70), edtsize, wxTE_RIGHT);
        
    // ok button
    OK = new wxButton( this, wxID_OK, "OK", wxPoint(10, 150 - wxDefaultSize.y), wxDefaultSize);
	OK->SetDefault();
 
    // cancel button
    Cancel = new wxButton( this, wxID_CANCEL, "Cancel", wxPoint(100, 150 - wxDefaultSize.y), wxDefaultSize);
}

void CBoundingBoxDlg::OnOk(wxCommandEvent &event)
{
    // read data from controls
    
    I1 lxrg, lyrg, lzrg;
    bool bSuccess = ReadDouble(xminbox, lxrg.lo) && ReadDouble(xmaxbox, lxrg.hi) && 
					ReadDouble(yminbox, lyrg.lo) && ReadDouble(ymaxbox, lyrg.hi) && 
					ReadDouble(zminbox, lzrg.lo) && ReadDouble(zmaxbox, lzrg.hi); 
		
    
    if ((bSuccess) && (lxrg.lo <= lxrg.hi) && (lyrg.lo <= lyrg.hi) && (lzrg.lo <= lzrg.hi))
    {
        Set(lxrg, lyrg, lzrg);
    
	    // skip leaves the event to be handled by MainFrame
        event.Skip();
    }

	UpdateControls();   
}


CBoundingBoxDlg::~CBoundingBoxDlg()
{
	// delete all controls
    delete xminbox;
    delete xmaxbox;
    delete yminbox;
    delete ymaxbox;
    delete zminbox;
    delete zmaxbox;

	delete OK;
	delete Cancel;
}

bool RunBoundingBoxDlg(I1& xrg, I1& yrg, I1& zrg)
{
    CBoundingBoxDlg dlg;
    dlg.Set(xrg, yrg, zrg);    
    if (dlg.ShowModal() == wxID_OK)
    {
       xrg = dlg.GetXrg();
       yrg = dlg.GetYrg();
       zrg = dlg.GetZrg();
       
       return true;
    }  
    
    return false;
}

