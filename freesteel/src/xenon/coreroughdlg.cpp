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


#include "coreroughdlg.h"


BEGIN_EVENT_TABLE(CCoreRoughDlg, wxDialog)
    EVT_BUTTON(wxID_OK, CCoreRoughDlg::OnOk)
END_EVENT_TABLE()

bool ReadDouble(const wxTextCtrl* ctrl, double& val);

// dialog constructor
CCoreRoughDlg::CCoreRoughDlg()
: wxDialog((wxDialog *)NULL, -1, _T("Coreroughing"), wxPoint(10, 10), wxSize(200, 200 + wxDefaultSize.y))
{

    // create and position controls in the dialog
    wxBoxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);

    // text controls
    wxSize edtsize(50, 25);
    
	wxStaticText* stcr = new wxStaticText(this, -1, wxString("Corner rad"), wxPoint(10,30));
	wxStaticText* stfr = new wxStaticText(this, -1, wxString("Flat rad"), wxPoint(10,60));
	wxStaticText* stsd = new wxStaticText(this, -1, wxString("Step down"), wxPoint(10,90));
	wxStaticText* stsi = new wxStaticText(this, -1, wxString("Step in"), wxPoint(10,120));

    cornerradctrl = new wxTextCtrl(this, -1, wxString(""), wxPoint(70,30), edtsize, wxTE_RIGHT);
    flatradctrl = new wxTextCtrl(this, -1, wxString(""), wxPoint(70,60), edtsize, wxTE_RIGHT);
    stepdownctrl = new wxTextCtrl(this, -1, wxString(""), wxPoint(70,90), edtsize, wxTE_RIGHT);
    stepinctrl = new wxTextCtrl(this, -1, wxString(""), wxPoint(70,120), edtsize, wxTE_RIGHT);
        
    // ok button
    OK = new wxButton( this, wxID_OK, "OK", wxPoint(10, 150 - wxDefaultSize.y), wxDefaultSize);
	OK->SetDefault();
 
    // cancel button
    Cancel = new wxButton( this, wxID_CANCEL, "Cancel", wxPoint(100, 150 - wxDefaultSize.y), wxDefaultSize);

    wxBoxSizer *sizercr = new wxBoxSizer(wxHORIZONTAL);
    sizercr->Add(stcr, 1, wxEXPAND | wxALL, 5);
    sizercr->Add(cornerradctrl, 1, wxEXPAND | wxALL, 5);
    sizerTop->Add(sizercr, 1, wxEXPAND | wxALL, 5);

    wxBoxSizer *sizerfr = new wxBoxSizer(wxHORIZONTAL);
    sizerfr->Add(stfr, 1, wxEXPAND | wxALL, 5);
    sizerfr->Add(flatradctrl, 1, wxEXPAND | wxALL, 5);
    sizerTop->Add(sizerfr, 1, wxEXPAND | wxALL, 5);

    wxBoxSizer *sizersd = new wxBoxSizer(wxHORIZONTAL);
    sizersd->Add(stsd, 1, wxEXPAND | wxALL, 5);
    sizersd->Add(stepdownctrl, 1, wxEXPAND | wxALL, 5);
    sizerTop->Add(sizersd, 1, wxEXPAND | wxALL, 5);

    wxBoxSizer *sizersi = new wxBoxSizer(wxHORIZONTAL);
    sizersi->Add(stsi, 1, wxEXPAND | wxALL, 5);
    sizersi->Add(stepinctrl, 1, wxEXPAND | wxALL, 5);
    sizerTop->Add(sizersi, 1, wxEXPAND | wxALL, 5);

    wxBoxSizer *sizerButtons = new wxBoxSizer(wxHORIZONTAL);
    sizerButtons->Add(OK, 1, wxEXPAND | wxALL, 5);
    sizerButtons->Add(Cancel, 1, wxEXPAND | wxALL, 5);
    sizerTop->Add(sizerButtons, 1, wxEXPAND | wxALL, 5);

    SetAutoLayout(TRUE);
    SetSizer(sizerTop);

    sizerTop->SetSizeHints(this);
    sizerTop->Fit(this);
}

void CCoreRoughDlg::OnOk(wxCommandEvent &event)
{
    // read data from controls
    
    double cr, fr, sd, si;
    bool bSuccess = ReadDouble(cornerradctrl, cr) && ReadDouble(flatradctrl, fr) && ReadDouble(stepdownctrl, sd) && ReadDouble(stepinctrl, si);
		
    
    if (bSuccess)
    {
		cornerrad = cr;
		flatrad = fr;
		stepdown = sd;
		stepin = si;
    
	    // skip leaves the event to be handled by MainFrame
        event.Skip();
    }

	UpdateControls();   
}


CCoreRoughDlg::~CCoreRoughDlg()
{
	// delete all controls
    delete cornerradctrl;
    delete flatradctrl;
    delete stepdownctrl;
    delete stepinctrl;

	delete OK;
	delete Cancel;
}

bool RunCoreRoughDlg(double& cr, double& fr, double& sd, double& si)
{
    CCoreRoughDlg dlg;
    dlg.cornerrad = cr;
    dlg.flatrad = fr;
    dlg.stepdown = sd;
    dlg.stepin = si;

	dlg.UpdateControls();
    if (dlg.ShowModal() == wxID_OK)
    {
		cr = dlg.cornerrad ;
		fr = dlg.flatrad;
		sd = dlg.stepdown;
		si = dlg.stepin;
       
       return true;
    }  
    
    return false;
}
