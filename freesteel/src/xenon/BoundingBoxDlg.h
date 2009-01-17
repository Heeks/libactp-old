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
#ifndef BOUNDINGBOXDLG_H
#define BOUNDINGBOXDLG_H

class CBoundingBoxDlg : public wxDialog
{
public:

    CBoundingBoxDlg();
    ~CBoundingBoxDlg();

    void Set(const I1& lxrg, const I1& lyrg, const I1& lzrg);
    
    const I1& GetXrg() const { return xrg; };
    const I1& GetYrg() const { return yrg; };
    const I1& GetZrg() const { return zrg; };
    
private:
    I1 xrg, yrg, zrg;
    
    wxTextCtrl *xminbox;
    wxTextCtrl *xmaxbox;
    wxTextCtrl *yminbox;
    wxTextCtrl *ymaxbox;
    wxTextCtrl *zminbox;
    wxTextCtrl *zmaxbox;

	wxButton *OK, *Cancel;

private:
    void OnOk(wxCommandEvent &event);
    DECLARE_EVENT_TABLE()

    void UpdateControls();
};

inline void CBoundingBoxDlg::Set(const I1& lxrg, const I1& lyrg, const I1& lzrg)
{
    xrg = lxrg;
    yrg = lyrg;
    zrg = lzrg;
    
    UpdateControls();    
}

inline void CBoundingBoxDlg::UpdateControls()
{
	xminbox->Clear();
	xmaxbox->Clear();
	yminbox->Clear();
	ymaxbox->Clear();
	zminbox->Clear();
	zmaxbox->Clear();

    *xminbox << xrg.lo;
    *xmaxbox << xrg.hi;
    *yminbox << yrg.lo;
    *ymaxbox << yrg.hi;
    *zminbox << zrg.lo;
    *zmaxbox << zrg.hi;
}

void BoundingBox(vtkPolyData* vtkdat, I1& xrg, I1& yrg, I1& zrg);
bool RunBoundingBoxDlg(I1& xrg, I1& yrg, I1& zrg);

#endif

