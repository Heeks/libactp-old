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

#ifndef MAINFRAME_H
#define MAINFRAME_H

class AnimationFrame;

// Define a new frame type
class MainFrame: public wxFrame
{
public:
    MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
	~MainFrame(); 

	void MakeMenues();

    void FileOpen(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnMachiningRaster(wxCommandEvent& event);
    void OnMachiningSlice(wxCommandEvent& event);
    void OnMachiningAreaclear(wxCommandEvent& event);
    void OnMachiningBoundingBox(wxCommandEvent& event);
    void OnViewMaximize(wxCommandEvent& event);
    void OnViewX(wxCommandEvent& event);
    void OnViewY(wxCommandEvent& event);
    void OnViewCentre(wxCommandEvent& event);
    void OnActors(wxCommandEvent& event);
    void OnUpdateActors(wxMenuEvent& event);
    void OnAnimate(wxCommandEvent& event);
    void OnUpdateAnimate(wxCommandEvent& event);
    void UpdateAnimate();
    void UpdateActors();
    void OnKeyDown(wxKeyEvent &event);
    void OnReplayFromStart(wxCommandEvent& event);
    void OnReplayOneLevel(wxCommandEvent& event);

	void OnActivate(wxActivateEvent& event);

	wxMenu *actors_menu;
        wxMenu *animate_menu;
        int idAnimated;

	class GeometryStuffTogether* GetGst() { return gst; };
	AnimationFrame* aniframe;

private:
    
    void CentreView(I1& xrg, I1& yrg, I1& zrg);
    void MaximizeView();
    void MachiningSlice(class GSTsurface *gstsurf, double zval, double toolrad, double     resolution);

    class GSTsurface* SelectedSurface() const;
    class GSTtoolpath* SelectedBoundary() const;

private:
  // Data members
    class wxVTKRenderWindowInteractor* iren; 
    class GeometryStuffTogether* gst; 
    
    DECLARE_EVENT_TABLE()
};

// Menubar IDs
enum
{
	MENU_ACTORS = 100
};

// Menu IDs
enum
{
    DIALOGS_FILE_OPEN = 1,
    DIALOG_VIEW_MAXIMIZE,
	DIALOG_VIEW_X, 
	DIALOG_VIEW_Y,
    DIALOG_VIEW_CENTRE,
    DIALOG_SURFACE_BOUNDING_BOX,
	DIALOG_SLICE,
	DIALOG_AREACLEAR,

	MENU_ANIMATE,
	DIALOG_BEGIN_ACTORS
};

enum
{
    MNU_FROMSTART=500,
    MNU_ONELEVEL,
    DIALOG_BEGIN_ANIMATE
};

#endif

