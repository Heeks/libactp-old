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

#include "bolts/bolts.h"

#include "MainFrame.h"
#include "AnimationFrame.h"

#include "MyApp.h"


// `Main program' equivalent, creating windows and returning main app mainfr
bool MyApp::OnInit()
{
  mainfr = new MainFrame(_T("freesteel"), wxPoint(20, 20), wxSize(800, 600));

  // Make a menubar
  this->MakeMenues();

  // Show the mainfr
  mainfr->Show(TRUE);

  animfr = new AnimationFrame(mainfr, _T("Animation"), wxPoint(25, 25), wxSize(400, 100));
  animfr->Show(TRUE);

  mainfr->aniframe = animfr;


  SetTopWindow(mainfr);
  
  return TRUE;
}

void MyApp::MakeMenues()
{
  wxMenu *file_menu = new wxMenu;
  file_menu->Append(DIALOGS_FILE_OPEN,  _T("&Open file\tCtrl-O"));
  file_menu->Append(wxID_EXIT, _T("E&xit\tAlt-X"));

  // machining menu
  wxMenu *view_menu = new wxMenu;
  view_menu->Append(DIALOG_VIEW_MAXIMIZE, _T("&Maximize\tM"));
  view_menu->Append(DIALOG_VIEW_CENTRE, _T("&Centre\tShift-T"));
  view_menu->Append(DIALOG_VIEW_X, _T("Along &X\tX"));
  view_menu->Append(DIALOG_VIEW_Y, _T("Along &Y\tY"));

  // actors menu
  mainfr->actors_menu = new wxMenu;
  
  // toolpath menu
  mainfr->animate_menu = new wxMenu;
  
  // machining menu
  wxMenu *machining_menu = new wxMenu;
  machining_menu->Append(DIALOG_SURFACE_BOUNDING_BOX, _T("&Bounding box"));
  machining_menu->Append(DIALOG_SLICE, _T("&Slice"));
  machining_menu->Append(DIALOG_AREACLEAR, _T("&Core Roughing"));

  mainfr->animate_menu->Append(MNU_FROMSTART, _T("&From Start"), _T("Replay from start of toolpath"), wxITEM_CHECK);
  mainfr->animate_menu->Append(MNU_ONELEVEL, _T("&One Level"), _T("Replay per level of toolpath"), wxITEM_CHECK);
  mainfr->animate_menu->Append(-1, _T(""), _T(""), wxITEM_SEPARATOR);


  wxMenuBar *menu_bar = new wxMenuBar;
  menu_bar->Append(file_menu, _T("&File"));
  menu_bar->Append(view_menu, _T("&View"));
  menu_bar->Append(mainfr->actors_menu, _T("&Actors"));
  menu_bar->Append(mainfr->animate_menu, _T("&Replay"));
  menu_bar->Append(machining_menu, _T("&Machining"));
  mainfr->SetMenuBar(menu_bar);

  mainfr->Centre(wxBOTH);
}
