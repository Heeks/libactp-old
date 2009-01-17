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
#ifndef ANIMATIONFRAME__H
#define ANIMATIONFRAME__H

class AnimationFrame: public wxFrame
{
public:
    AnimationFrame(MainFrame* mf, const wxString& title, const wxPoint& pos, const wxSize& size);

	void OnAnimateTP(wxScrollEvent& event);
	void OnIndexTP(wxScrollEvent& event);
	void OnAnimate(wxCommandEvent& event);
	void OnCalculateStock(wxCommandEvent& event);
	void OnExit(wxCommandEvent& event);

	void UpdateControls();

	void SetAnimated(class GSTtoolpath* gst);
	void ClearAnimated();
	
	enum Anistyle {FROMSTART, ONELEVEL};
	void SetStyle(Anistyle lst) { style = lst; }
	Anistyle GetStyle() const { return style; }

private:
	MainFrame* mainfr;
	wxSlider *sldTP;
	wxSlider *sldIndexLoadedTP;
	wxString status;

	class GSTtoolpath* pthanimated;
	double totallen; // total length
	double animatedlast; // length to last slider position

	enum Anistyle style;

	DECLARE_EVENT_TABLE()
};

enum
{
	SLIDER_ANIMATION = 1
};



enum
{
	CALC_STOCK = 400
};

void PostProcess(FILE* fpost, const vector<class PathXSeries>& pathxseries, const struct MachineParams& params);

#endif


