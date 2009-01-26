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

#include "wxVTKRenderWindowInteractor.h"
#include "vtkCylinderSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkOpenGLPolyDataMapper.h"

#include "vtkActor.h"
#include "vtkActorCollection.h"
#include "vtkRenderer.h"
#include "vtkRendererCollection.h"
#include "vtkRenderWindow.h"
#include "vtkInteractorObserver.h"
#include "vtkSTLReader.h"
#include "vtkLineSource.h"
#include "vtkFloatArray.h"
#include "vtkLine.h"
#include "vtkPolyLine.h"
#include "vtkProperty.h"
#include "vtkCamera.h"
#include "vtkPolyData.h"
#include "vtkClipPolyData.h"
#include "vtkPointData.h"
#include "vtkInteractorStyleSwitch.h"

#include "bolts/bolts.h"
#include "cages/cages.h"
#include "pits/pits.h"
#include "visuals/MakeToolpath.h"
#include "visuals/fsvtkToolpathMapper.h"
#include "visuals/gstsurface.h"
#include "visuals/gst.h"

#include "BoundingBoxDlg.h"
#include "coreroughdlg.h"
#include "MainFrame.h"
#include "AnimationFrame.h"
#include "MyApp.h"

static int MAX_ACTORS = 20;
#define VTK_WINDOW 501

//#include <pthread.h>


IMPLEMENT_APP(MyApp)

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(DIALOGS_FILE_OPEN,             MainFrame::FileOpen)
    EVT_MENU(wxID_EXIT,                     MainFrame::OnExit)
    
    EVT_MENU(DIALOG_VIEW_MAXIMIZE,	    	MainFrame::OnViewMaximize)
    EVT_MENU(DIALOG_VIEW_X,	    			MainFrame::OnViewX)
    EVT_MENU(DIALOG_VIEW_Y,	    			MainFrame::OnViewY)
    EVT_MENU(DIALOG_VIEW_CENTRE,	    	MainFrame::OnViewCentre)

    EVT_MENU(DIALOG_SURFACE_BOUNDING_BOX,	MainFrame::OnMachiningBoundingBox)
    EVT_MENU(DIALOG_SLICE,			MainFrame::OnMachiningSlice)
    EVT_MENU(DIALOG_AREACLEAR,		        MainFrame::OnMachiningAreaclear)
    EVT_MENU(MNU_FROMSTART,		        MainFrame::OnReplayFromStart)
    EVT_MENU(MNU_ONELEVEL,		        MainFrame::OnReplayOneLevel)
	
	// not very good, but I cant work out how to trigger an update when the menu "Actors" is highlighted
	// this event is called everytime you activate the menubar
    EVT_MENU_OPEN(				MainFrame::OnUpdateActors) 
    EVT_MENU_RANGE(DIALOG_BEGIN_ACTORS, DIALOG_BEGIN_ACTORS + MAX_ACTORS, 
	                                        MainFrame::OnActors)
    EVT_MENU_RANGE(DIALOG_BEGIN_ANIMATE, DIALOG_BEGIN_ANIMATE + MAX_ACTORS,
                                                    MainFrame::OnAnimate)
    EVT_KEY_DOWN(                                   MainFrame::OnKeyDown)

    // needs to handle events for vtkWindow activate/close/focus
END_EVENT_TABLE()


// frame constructor
MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame(NULL, -1, title, pos, size)
{
    CreateStatusBar();
    iren = new wxVTKRenderWindowInteractor(this, VTK_WINDOW);
    gst = new GeometryStuffTogether(iren->GetRenderWindow());

    idAnimated = -1;
}


MainFrame::~MainFrame()
{
	delete gst; 
	delete iren;
}


void MainFrame::FileOpen(wxCommandEvent& WXUNUSED(event) )
{
    wxFileDialog dialog (this, _T(""), _T(""), _T(""), _T("STL files (*.STL)|*.stl|XML files (*.XML)|*.xml"));

    dialog.SetFilename("D.STL");

    if (dialog.ShowModal() == wxID_OK)    
	{                      
		wxString fn(dialog.GetPath().c_str());
		if (fn.Matches("*.xml"))
		{

			this->aniframe->UpdateControls();
		}
		else
		{
			// make the surface 
			const char *fn = dialog.GetPath().c_str();
			GSTsurface *gstsurface = new GSTsurface();
			gst->gstees.push_back(gstsurface);
			gstsurface->LoadSTL(fn); 
			gstsurface->AddToRenderer(&gst->ren1); 

			// make a bounding box for it
			gst->MakeRectBoundary(gstsurface->xrg, gstsurface->yrg, gstsurface->zrg.hi + 1.0);  
		}

                MaximizeView();

		gst->renWin->Render();
                UpdateActors();
                UpdateAnimate();
    }
}

void MainFrame::OnExit(wxCommandEvent& WXUNUSED(event) )
{
    Close(TRUE);
}


void MainFrame::OnViewMaximize(wxCommandEvent& WXUNUSED(event) )
{
    MaximizeView();
    gst->renWin->Render();
}

void MainFrame::OnViewX(wxCommandEvent& WXUNUSED(event) )
{
    I1 xrg, yrg, zrg;
    CentreView(xrg, yrg, zrg);
    gst->ren1->GetActiveCamera()->SetFocalPoint(xrg.Half(), yrg.Half(), zrg.Half());
    gst->ren1->GetActiveCamera()->SetPosition(xrg.hi + 100, yrg.Half(), zrg.Half());
    gst->renWin->Render();
}

void MainFrame::OnViewY(wxCommandEvent& WXUNUSED(event) )
{
    I1 xrg, yrg, zrg;
    CentreView(xrg, yrg, zrg);
    gst->ren1->GetActiveCamera()->SetFocalPoint(xrg.Half(), yrg.Half(), zrg.Half());
    gst->ren1->GetActiveCamera()->SetPosition(xrg.Half(), yrg.lo - 100, zrg.Half());
    gst->renWin->Render();
}

void MainFrame::OnViewCentre(wxCommandEvent& WXUNUSED(event) )
{
    I1 xrg, yrg, zrg;
    CentreView(xrg, yrg, zrg);
    gst->renWin->Render();
}

void MainFrame::OnKeyDown(wxKeyEvent &event)
{
    int keycode = event.GetKeyCode();
    char key = '\0';
    if (keycode < 256)
    {
        // TODO: Unicode in non-Unicode mode ??
        key = (char)keycode;
    }
}    

void MainFrame::OnActors(wxCommandEvent& event)
{
	int nID = event.GetId() - DIALOG_BEGIN_ACTORS;
	gst->gstees[nID]->SetVisibility(!gst->gstees[nID]->GetVisibility());

	gst->renWin->Render();
}

void MainFrame::OnAnimate(wxCommandEvent& event)
{
    // toggle previous animated to non-animated
    wxMenuItem* itemlast = NULL;
    if (idAnimated >= 0)
    {
        itemlast = animate_menu->FindItem(idAnimated + DIALOG_BEGIN_ANIMATE);
        ASSERT(itemlast != NULL);
        itemlast->Check(0);
    }

    // selection same as before?
    int nID = event.GetId() - DIALOG_BEGIN_ANIMATE;
    if (idAnimated == nID)
    {
        // switch allanimation off
        ASSERT(!itemlast->IsChecked());
        aniframe->ClearAnimated();
        idAnimated = -1;
        return;
    }

    wxMenuItem* item = animate_menu->FindItem(event.GetId());
    
    item->Check(1);

    int i = 0, ix = 0;
    while (i < gst->gstees.size())
    {
        if (dynamic_cast<GSTtoolpath* >(gst->gstees[i]))
        {
            GSTtoolpath* gsttp = static_cast<GSTtoolpath* >(gst->gstees[i]);

            if ((gsttp->toolshape.cornerrad != 0) || (gsttp->toolshape.flatrad != 0))
            {
                if (ix == nID)
                {
                    aniframe->SetAnimated(dynamic_cast<GSTtoolpath* >(gst->gstees[i]));
                    idAnimated = ix;
                    break;
                }
                ++ix;
            }
        }
        ++i;
    }

    if (i == gst->gstees.size())
        aniframe->ClearAnimated();    
}

void MainFrame::OnUpdateAnimate(wxCommandEvent& event)
{
    UpdateAnimate();
}

void MainFrame::UpdateAnimate()
{
    animate_menu->FindItem(MNU_FROMSTART)->Check(aniframe->GetStyle() == AnimationFrame::FROMSTART);
    animate_menu->FindItem(MNU_ONELEVEL)->Check(aniframe->GetStyle() == AnimationFrame::ONELEVEL);

    // remove all menu items
    int ndelete = animate_menu->GetMenuItemCount() - 3;
    ASSERT(ndelete >= 0);
    int i;
    for (i = 0; i < ndelete; ++i)
        animate_menu->Delete(DIALOG_BEGIN_ANIMATE + i);

    int ix = 0;
    for (i = 0; i < gst->gstees.size(); ++i)
    {
        if (dynamic_cast<GSTtoolpath* >(gst->gstees[i]))
        {
            GSTtoolpath* gsttp = static_cast<GSTtoolpath* >(gst->gstees[i]);

            if ((gsttp->toolshape.cornerrad != 0) || (gsttp->toolshape.flatrad != 0))
            {
                wxString strEty;
                strEty << ix;

                wxMenuItem *item = new wxMenuItem(animate_menu, ix + DIALOG_BEGIN_ANIMATE, _T("Toolpath &") + strEty, _T(""), wxITEM_CHECK);
                item->Check(0);
                animate_menu->Append(item);
                ++ix;
            }
        }
    }
}

void MainFrame::OnMachiningBoundingBox(wxCommandEvent& WXUNUSED(event) )
{
/*
    // find bounding box range of this data. 
    I1 xrg, yrg, zrg;
    BoundingBox(gst.stl->GetOutput(), xrg, yrg, zrg);
    RunBoundingBoxDlg(xrg, yrg, zrg);
    
//	gst.iren->GetRenderWindow()->Render();
*/
}	


void MainFrame::OnMachiningSlice(wxCommandEvent& WXUNUSED(event) )
{
    UpdateActors();
}

GSTsurface* MainFrame::SelectedSurface() const
{
    int ix = 0;

    while (ix < gst->gstees.size())
    {
        GSTbase *gb = gst->gstees[ix];
        GSTsurface *gstsurf = dynamic_cast<GSTsurface *>(gb);
        if ((gstsurf != NULL) && (gstsurf->GetVisibility()))
            return gstsurf;

        ++ix;
    }

    return NULL;
}

GSTtoolpath* MainFrame::SelectedBoundary() const
{
    int ix = 0;

    while (ix < gst->gstees.size())
    {
        GSTbase *gb = gst->gstees[ix];
        GSTtoolpath *gstbdy = dynamic_cast<GSTtoolpath *>(gb);
        if ((gstbdy != NULL) && (gstbdy->GetVisibility()))
            return gstbdy;

        ++ix;
    }

    return NULL;
}

struct croin
{
    GSTtoolpath *gsttpath;
    GSTsurface *gstsurf;
    GSTtoolpath *gstbound;
    double sd, cr, fr, si;

    MainFrame* mainfr;
};

void MainFrame::OnMachiningAreaclear(wxCommandEvent& WXUNUSED(event) )
{
	double cr = 3.;
	double fr = 0.;
	double sd = 15.;
	double si = cr / 2.;
	if (RunCoreRoughDlg(cr, fr, sd, si))
	{
		GSTsurface *gstsurf = SelectedSurface();
		GSTtoolpath *gstbound = SelectedBoundary();
		ASSERT((gstsurf != NULL) && (gstbound != NULL));

		if (!(gstsurf || gstbound))
			return;

		MachineParams params;
	// linking parameters
		params.leadoffdz = 0.1; 
		params.leadofflen = 1.1;
		params.leadoffrad = 2.0;
		params.retractzheight = gstsurf->zrg.hi + 5.0;
		params.leadoffsamplestep = 0.6;

	// cutting parameters
		params.toolcornerrad = cr;
		params.toolflatrad = fr;
		params.samplestep = 0.4;
		params.stepdown = sd;
		params.clearcuspheight = sd / 3.0;

	// weave parameters
		params.triangleweaveres = 0.51;
		params.flatradweaveres = 0.71;

	// stearing parameters
	// fixed values controlling the step-forward of the tool and 
	// changes of direction.  
		params.dchangright = 0.17; 
		params.dchangrightoncontour = 0.37; 
		params.dchangleft = -0.41; 
		params.dchangefreespace = -0.6; 
		params.sidecutdisplch = 0.0;
		params.fcut = 1000;
		params.fretract = 5000;
		params.thintol = 0.0001;



		GSTtoolpath* gsttpath = new GSTtoolpath;
		gst->gstees.push_back(gsttpath);
		
		// define the empty surface
		SurfX sx(gstsurf->xrg.Inflate(2), gstsurf->yrg.Inflate(2), gstsurf->zrg); 
		gstsurf->PushTrianglesIntoSurface(sx); 
		sx.BuildComponents(); // compress thing 

		ASSERT(gstbound->ftpaths.size() == 1);
		MakeCorerough(gsttpath->ftpaths, sx, gstbound->ftpaths[0], params);

		// write result to a file
		FILE* fpost = fopen("freesteel.tp", "w");
		ASSERT(fpost);
		PostProcess(fpost, gsttpath->ftpaths, params);
		fclose(fpost);


		gsttpath->toolshape = ToolShape(params.toolflatrad, params.toolcornerrad, params.toolcornerrad, params.toolcornerrad / 10.0);
		gsttpath->bound.Append(gstbound->ftpaths[0].pths);
		gsttpath->UpdateFromPax();
		gsttpath->AddToRenderer(&gst->ren1);

		UpdateActors();
		UpdateAnimate();
	}
}

void MainFrame::CentreView(I1 &xrg, I1 &yrg, I1 &zrg)
{
    for (size_t i = 0; i < gst->gstees.size(); ++i)
    {
        xrg.Absorb(gst->gstees[i]->xrg, i == 0);
        yrg.Absorb(gst->gstees[i]->yrg, i == 0);
        zrg.Absorb(gst->gstees[i]->zrg, i == 0);
    }

    gst->ren1->GetActiveCamera()->SetFocalPoint(xrg.Half(), yrg.Half(), zrg.Half());
}

void MainFrame::MaximizeView()
{
    I1 xrg, yrg, zrg;
    CentreView(xrg, yrg, zrg);

    gst->ren1->GetActiveCamera()->SetFocalPoint(xrg.Half(), yrg.Half(), zrg.Half());
    gst->ren1->GetActiveCamera()->SetPosition(xrg.Half(), yrg.Half(), zrg.hi + 100);
    
    // scale to fill window
    ASSERT(gst->ren1->GetActiveCamera()->GetParallelProjection() != 0);

    double scale = gst->ren1->GetActiveCamera()->GetParallelScale();
    gst->ren1->GetActiveCamera()->SetParallelScale(max(xrg.Leng(), yrg.Leng())); 
}

void MainFrame::UpdateActors()
{
	// remove all menu items
	int ndelete = actors_menu->GetMenuItemCount();
	int i;
	for (i = 0; i < ndelete; ++i)
		actors_menu->Delete(DIALOG_BEGIN_ACTORS + i);

        int icsrf = 0, icbdy = 0, icpth = 0;
        for (i = 0; i < gst->gstees.size(); ++i)
        {
            wxString strEty;

            wxMenuItem *item = NULL;
            if (dynamic_cast<GSTsurface* >(gst->gstees[i]))
            {
                strEty << icsrf;
                item = new wxMenuItem(actors_menu, i + DIALOG_BEGIN_ACTORS, _T("Surface &") + strEty + _T("\tShift-") + strEty, _T(""), wxITEM_CHECK);
                ++icsrf;
            }
            else if (dynamic_cast<GSTtoolpath* >(gst->gstees[i]))
            {
                GSTtoolpath* gsttp = static_cast<GSTtoolpath* >(gst->gstees[i]);
                bool bIsBoundary = (gsttp->toolshape.cornerrad == 0) && (gsttp->toolshape.flatrad == 0);
                strEty << (bIsBoundary ? icbdy : icpth);

                item = new wxMenuItem(actors_menu, i + DIALOG_BEGIN_ACTORS, (bIsBoundary ? _T("Boundary &") : _T("Toolpath &")) + strEty + _T("\tShift-") + strEty, _T(""), wxITEM_CHECK);
                bIsBoundary ? ++icbdy : ++icpth;
            }
            else
                ASSERT(0);
            
            actors_menu->Append(item);
            item->Check(gst->gstees[i]->GetVisibility());
        }
}

void MainFrame::OnUpdateActors(wxMenuEvent& event)
{
	UpdateActors();
}

void MainFrame::OnReplayFromStart(wxCommandEvent& event)
{
    aniframe->SetStyle(AnimationFrame::FROMSTART);
    UpdateAnimate();
}

void MainFrame::OnReplayOneLevel(wxCommandEvent& event)
{
    aniframe->SetStyle(AnimationFrame::ONELEVEL);
    UpdateAnimate();
}

