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
#ifndef COREROUGHDLG_h
#define COREROUGHDLG_H

class CCoreRoughDlg : public wxDialog
{
public:

    CCoreRoughDlg();
    ~CCoreRoughDlg();
    void UpdateControls();

    double cornerrad;
    double flatrad;
    double stepdown;
    double stepin;

private:

    wxTextCtrl *cornerradctrl;
    wxTextCtrl *flatradctrl;
    wxTextCtrl *stepdownctrl;
    wxTextCtrl *stepinctrl;

	wxButton *OK, *Cancel;

private:
    void OnOk(wxCommandEvent &event);
    DECLARE_EVENT_TABLE()

};

inline void CCoreRoughDlg::UpdateControls()
{
    cornerradctrl->Clear();
    flatradctrl->Clear();
    stepdownctrl->Clear();
    stepinctrl->Clear();

    *cornerradctrl << cornerrad;
    *flatradctrl << flatrad;
    *stepdownctrl << stepdown;
    *stepinctrl << stepin;
}

bool RunCoreRoughDlg(double& cr, double& fr, double& sd, double& si);

#endif

