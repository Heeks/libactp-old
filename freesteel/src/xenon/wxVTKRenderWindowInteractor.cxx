/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: wxVTKRenderWindowInteractor.cxx,v $
  Language:  C++
  Date:      $Date: 2004/04/26 10:38:36 $
  Version:   $Revision: 1.6 $

  Copyright (c) 1993-2002 Ken Martin, Will Schroeder, Bill Lorensen 
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "wxVTKRenderWindowInteractor.h"

#if defined(WIN32) && defined(VTK_BUILD_SHARED_LIBS)
// #error Only use this setting when you build this class into a shared library, if thats true, remove this line
#endif


#if (VTK_MAJOR_VERSION >= 4 && VTK_MINOR_VERSION > 0)
#  include <vtkCommand.h>
#else
#  include <vtkInteractorStyle.h>
#endif //(VTK_MAJOR_VERSION >= 4 && VTK_MINOR_VERSION > 0)

//Keep this for compatibilty reason, this was introduced in wxGTK 2.4.0
#if (!wxCHECK_VERSION(2, 4, 0))
wxWindow* wxGetTopLevelParent(wxWindow *win)
{
    while ( win && !win->IsTopLevel() )
         win = win->GetParent();
    return win;
}
#endif

#ifdef WXMAKINGDLL
 #pragma message("making")
#elif defined(WXUSINGDLL)
 #pragma message("using")
#endif

#if defined(__WXMSW__) || defined (__WXMAC__) || defined (__WXCOCOA__)
IMPLEMENT_DYNAMIC_CLASS(wxVTKRenderWindowInteractor, wxWindow)
#else
IMPLEMENT_DYNAMIC_CLASS(wxVTKRenderWindowInteractor, wxGLCanvas)
#endif  //__WXMSW__

//---------------------------------------------------------------------------
#if defined(__WXMSW__) || defined (__WXMAC__) || defined (__WXCOCOA__)
BEGIN_EVENT_TABLE(wxVTKRenderWindowInteractor, wxWindow)
#else
BEGIN_EVENT_TABLE(wxVTKRenderWindowInteractor, wxGLCanvas)
#endif //__WXMSW__
  //refresh window by doing a Render
  EVT_PAINT       (wxVTKRenderWindowInteractor::OnPaint)
  EVT_ERASE_BACKGROUND(wxVTKRenderWindowInteractor::OnEraseBackground)
  EVT_MOTION      (wxVTKRenderWindowInteractor::OnMotion)

  //Bind the events to the event converters
  EVT_LEFT_DOWN   (wxVTKRenderWindowInteractor::OnButtonDown)
  EVT_MIDDLE_DOWN (wxVTKRenderWindowInteractor::OnButtonDown)
  EVT_RIGHT_DOWN  (wxVTKRenderWindowInteractor::OnButtonDown)
  EVT_LEFT_UP     (wxVTKRenderWindowInteractor::OnButtonUp)
  EVT_MIDDLE_UP   (wxVTKRenderWindowInteractor::OnButtonUp)
  EVT_RIGHT_UP    (wxVTKRenderWindowInteractor::OnButtonUp)
  EVT_MOUSEWHEEL  (wxVTKRenderWindowInteractor::OnMouseWheel)
#if !(VTK_MAJOR_VERSION == 3 && VTK_MINOR_VERSION == 1)
  EVT_ENTER_WINDOW(wxVTKRenderWindowInteractor::OnEnter)
  EVT_LEAVE_WINDOW(wxVTKRenderWindowInteractor::OnLeave)
// If we use EVT_KEY_DOWN instead of EVT_CHAR, capital versions
// of all characters are always returned.  EVT_CHAR also performs
// other necessary keyboard-dependent translations.
// EVT_KEY_DOWN    (wxVTKRenderWindowInteractor::OnKeyDown)
  EVT_CHAR        (wxVTKRenderWindowInteractor::OnKeyDown)
  EVT_KEY_UP      (wxVTKRenderWindowInteractor::OnKeyUp)
#endif
  EVT_TIMER       (ID_wxVTKRenderWindowInteractor_TIMER, wxVTKRenderWindowInteractor::OnTimer)
  EVT_SIZE        (wxVTKRenderWindowInteractor::OnSize)
END_EVENT_TABLE()

//---------------------------------------------------------------------------
#if defined(__WXMSW__) || defined (__WXMAC__) || defined (__WXCOCOA__)
wxVTKRenderWindowInteractor::wxVTKRenderWindowInteractor() : wxWindow()
#else
wxVTKRenderWindowInteractor::wxVTKRenderWindowInteractor() : wxGLCanvas()
#endif //__WXMSW__
      , vtkRenderWindowInteractor()
      , ActiveButton(wxEVT_NULL)
      , RenderAllowed(0)
      , Stereo(0)
      , Handle(0)
      , Created(true)
      , RenderWhenDisabled(1)
      , UseCaptureMouse(0)
{
  timer = new wxTimer(this, ID_wxVTKRenderWindowInteractor_TIMER);
  vtkRenderWindowInteractor::SetRenderWindow(vtkRenderWindow::New());
  this->RenderWindow->Delete();
}
//---------------------------------------------------------------------------
wxVTKRenderWindowInteractor::wxVTKRenderWindowInteractor(wxWindow *parent,
                                                         wxWindowID id,
                                                         const wxPoint &pos,
                                                         const wxSize &size,
                                                         long style,
                                                         const wxString &name)
#if defined(__WXMSW__) || defined (__WXMAC__) || defined (__WXCOCOA__)
      : wxWindow(parent, id, pos, size, style, name)
#else
      : wxGLCanvas(parent, id, pos, size, style, name)
#endif
      , vtkRenderWindowInteractor()
      , ActiveButton(wxEVT_NULL)
      , RenderAllowed(0)
      , Stereo(0)
      , Handle(0)
      , Created(true)
      , RenderWhenDisabled(1)
      , UseCaptureMouse(0)
{
  timer = new wxTimer(this, ID_wxVTKRenderWindowInteractor_TIMER);
  vtkRenderWindowInteractor::SetRenderWindow(vtkRenderWindow::New());
  this->RenderWindow->Delete();
}
//---------------------------------------------------------------------------
wxVTKRenderWindowInteractor::~wxVTKRenderWindowInteractor()
{
  if(this->RenderWindow != NULL)
  {
    this->RenderWindow->UnRegister(this);
  }
  delete timer;
}
//---------------------------------------------------------------------------
wxVTKRenderWindowInteractor * wxVTKRenderWindowInteractor::New()
{
	// we don't make use of the objectfactory, because we're not registered
  return new wxVTKRenderWindowInteractor;
}
//---------------------------------------------------------------------------
void wxVTKRenderWindowInteractor::Initialize()
{
  // enable everything and start rendering
  Enable();
  RenderWindow->Start();

  // this is initialized
  Initialized = 1;
}
//---------------------------------------------------------------------------
void wxVTKRenderWindowInteractor::Enable()
{
  // if already enabled then done
  if (Enabled)
    return;

  // that's it
  Enabled = 1;
  Modified();
}
//---------------------------------------------------------------------------
void wxVTKRenderWindowInteractor::Disable()
{
  // if already disabled then done
  if (!Enabled)
    return;

  // that's it (we can't remove the event handler like it should be...)
  Enabled = 0;
  Modified();
}
//---------------------------------------------------------------------------
void wxVTKRenderWindowInteractor::Start()
{
  // the interactor cannot control the event loop
  vtkErrorMacro(<<"wxVTKRenderWindowInteractor::Start() \
    interactor cannot control event loop.");
}
//---------------------------------------------------------------------------
void wxVTKRenderWindowInteractor::UpdateSize(int x, int y)
{
  if( RenderWindow )
  {
    // if the size changed tell render window
    if (( (x != Size[0]) || (y != Size[1]) ))
    {
      Size[0] = x;
      Size[1] = y;
      // and our RenderWindow's size
//      RenderWindow->SetSize(x, y);
    }
  }
}
//---------------------------------------------------------------------------
int wxVTKRenderWindowInteractor::CreateTimer(int WXUNUSED(timertype))
{
  // it's a one shot timer
  if (!timer->Start(10, TRUE))
    assert(false);

  return 1;
  
}
//---------------------------------------------------------------------------
int wxVTKRenderWindowInteractor::DestroyTimer()
{
  // do nothing
  return 1;
}
//---------------------------------------------------------------------------
void wxVTKRenderWindowInteractor::OnTimer(wxTimerEvent& WXUNUSED(event))
{
  if (!Enabled)
    return;
    
#if (VTK_MAJOR_VERSION >= 4 && VTK_MINOR_VERSION > 0)
    // new style
    InvokeEvent(vtkCommand::TimerEvent, NULL);
#else
    // old style
    InteractorStyle->OnTimer();
#endif
}
//---------------------------------------------------------------------------
long wxVTKRenderWindowInteractor::GetHandle()
{
  //helper function to hide the MSW vs GTK stuff
  long handle_tmp = 0;

#ifdef __WXMSW__
    handle_tmp = (long)this->GetHWND();
#endif //__WXMSW__

#ifdef __WXMAC__
#ifdef VTK_USE_COCOA
    handle_tmp = (long)objc_msgSend(GetNSView(), "window");
#endif //VTK_USE_COCOA
#ifdef VTK_USE_CARBON
    handle_tmp = (long)this->MacGetRootWindow();
#endif //VTK_USE_CARBON
#endif //__WXMAC__

    // Find and return the actual X-Window.
#ifdef __WXGTK__
    if (m_wxwindow) {
#ifdef __WXGTK20__
        handle_tmp = (long) GDK_WINDOW_XWINDOW(GTK_PIZZA(m_wxwindow)->
          bin_window);
#else
        GdkWindowPrivate* bwin = reinterpret_cast<GdkWindowPrivate*>(
          GTK_PIZZA(m_wxwindow)->bin_window);
        if (bwin) {
            handle_tmp = (long)bwin->xwindow;
        }
#endif //__WXGTK20__
    }
#endif //__WXGTK__

#ifdef __WXMOTIF__
    handle_tmp = (long)this->GetXWindow();
#endif

  return handle_tmp;
}
//---------------------------------------------------------------------------
void wxVTKRenderWindowInteractor::OnPaint(wxPaintEvent& WXUNUSED(event))
{
  //must always be here
  wxPaintDC pDC(this);

  //do it here rather than in the cstor: this is safer.
  if(!Handle)
  {
    Handle = GetHandle();
    RenderWindow->SetWindowId(reinterpret_cast<void *>(Handle));
  }
  // get vtk to render to the wxWindows
  Render();
}
//---------------------------------------------------------------------------
void wxVTKRenderWindowInteractor::OnEraseBackground(wxEraseEvent &event)
{
  //turn off background erase to reduce flickering on MSW
  event.Skip(false);
}
//---------------------------------------------------------------------------
void wxVTKRenderWindowInteractor::OnSize(wxSizeEvent &event)
{
  int w, h;
  GetClientSize(&w, &h);
  UpdateSize(w, h);

  if (!Enabled) 
    {
    return;
    }

#if (VTK_MAJOR_VERSION >= 4 && VTK_MINOR_VERSION > 0)
  InvokeEvent(vtkCommand::ConfigureEvent, NULL);
#endif
  //this will check for Handle
  Render();
}
//---------------------------------------------------------------------------
void wxVTKRenderWindowInteractor::OnMotion(wxMouseEvent &event)
{
 if (!Enabled) 
    {
    return;
    }
#if (VTK_MAJOR_VERSION >= 4 && VTK_MINOR_VERSION > 0)
  SetEventInformationFlipY(event.GetX(), event.GetY(), 
    event.ControlDown(), event.ShiftDown(), '\0', 0, NULL);

  InvokeEvent(vtkCommand::MouseMoveEvent, NULL);
#else
  InteractorStyle->OnMouseMove(event.ControlDown(), event.ShiftDown(),
    event.GetX(), Size[1] - event.GetY() - 1);
#endif
}
//---------------------------------------------------------------------------
#if !(VTK_MAJOR_VERSION == 3 && VTK_MINOR_VERSION == 1)
void wxVTKRenderWindowInteractor::OnEnter(wxMouseEvent &event)
{
  if (!Enabled) 
    {
    return;
    }

#if (VTK_MAJOR_VERSION >= 4 && VTK_MINOR_VERSION > 0)
    // new style
  SetEventInformationFlipY(event.GetX(), event.GetY(), 
      event.ControlDown(), event.ShiftDown(), '\0', 0, NULL);

  InvokeEvent(vtkCommand::EnterEvent, NULL);
#else
    // old style
  InteractorStyle->OnEnter(event.ControlDown(), event.ShiftDown(),
      event.GetX(), Size[1] - event.GetY() - 1);  
#endif
}
//---------------------------------------------------------------------------
void wxVTKRenderWindowInteractor::OnLeave(wxMouseEvent &event)
{
  if (!Enabled) 
    {
    return;
    }

#if (VTK_MAJOR_VERSION >= 4 && VTK_MINOR_VERSION > 0)
    // new style
  SetEventInformationFlipY(event.GetX(), event.GetY(), 
      event.ControlDown(), event.ShiftDown(), '\0', 0, NULL);

  InvokeEvent(vtkCommand::LeaveEvent, NULL);
#else
    // old style
  InteractorStyle->OnLeave(event.ControlDown(), event.ShiftDown(),
      event.GetX(), Size[1] - event.GetY() - 1);  
#endif
}
//---------------------------------------------------------------------------
void wxVTKRenderWindowInteractor::OnKeyDown(wxKeyEvent &event)
{
  if (!Enabled) 
    {
    return;
    }

#if (VTK_MAJOR_VERSION >= 4 && VTK_MINOR_VERSION > 0)
    // new style
  int keycode = event.GetKeyCode();
  char key = '\0';
  if (keycode < 256)
  {
    // TODO: Unicode in non-Unicode mode ??
    key = (char)keycode;
  }

  SetEventInformationFlipY(event.GetX(), event.GetY(), 
    event.ControlDown(), event.ShiftDown(), key, 0, NULL);

  InvokeEvent(vtkCommand::KeyPressEvent, NULL);
  InvokeEvent(vtkCommand::CharEvent, NULL);
#else
  InteractorStyle->OnKeyDown(event.ControlDown(), event.ShiftDown(), 
    event.GetKeyCode(), 1);
#endif
  event.Skip();
}
//---------------------------------------------------------------------------
void wxVTKRenderWindowInteractor::OnKeyUp(wxKeyEvent &event)
{
  if (!Enabled) 
    {
    return;
    }

#if (VTK_MAJOR_VERSION >= 4 && VTK_MINOR_VERSION > 0)
    // new style
  int keycode = event.GetKeyCode();
  char key = '\0';
  if (keycode < 256)
  {
    // TODO: Unicode in non-Unicode mode ??
    key = (char)keycode;
  }

  SetEventInformationFlipY(event.GetX(), event.GetY(), 
    event.ControlDown(), event.ShiftDown(), key, 0, NULL);
  InvokeEvent(vtkCommand::KeyReleaseEvent, NULL);
#else
  InteractorStyle->OnKeyUp(event.ControlDown(), event.ShiftDown(), 
    event.GetKeyCode(), 1);
#endif
  event.Skip();
}
#endif //!(VTK_MAJOR_VERSION == 3 && VTK_MINOR_VERSION == 1)
//---------------------------------------------------------------------------
void wxVTKRenderWindowInteractor::OnButtonDown(wxMouseEvent &event)
{
  if (!Enabled || (ActiveButton != wxEVT_NULL))
    {
    return;
    }
  ActiveButton = event.GetEventType();

#if (VTK_MAJOR_VERSION >= 4 && VTK_MINOR_VERSION > 0)
  SetEventInformationFlipY(event.GetX(), event.GetY(), 
    event.ControlDown(), event.ShiftDown(), '\0', 0, NULL);
#endif

  if(event.RightDown())
  {
#if (VTK_MAJOR_VERSION >= 4 && VTK_MINOR_VERSION > 0)
    // new style
    InvokeEvent(vtkCommand::RightButtonPressEvent, NULL);
#else            
    // old style
    InteractorStyle->OnRightButtonDown(event.ControlDown(), event.ShiftDown(),
      event.GetX(), Size[1] - event.GetY() - 1);
#endif
  }
  else if(event.LeftDown())
  {
#if (VTK_MAJOR_VERSION >= 4 && VTK_MINOR_VERSION > 0)
    // new style
    InvokeEvent(vtkCommand::LeftButtonPressEvent, NULL);
#else            
    // old style
    InteractorStyle->OnLeftButtonDown(event.ControlDown(),  event.ShiftDown(),
      event.GetX(), Size[1] - event.GetY() - 1);
#endif
  }
  else if(event.MiddleDown())
  {
#if (VTK_MAJOR_VERSION >= 4 && VTK_MINOR_VERSION > 0)
    // new style
    InvokeEvent(vtkCommand::MiddleButtonPressEvent, NULL);
#else            
    // old style
    InteractorStyle->OnMiddleButtonDown(event.ControlDown(), event.ShiftDown(),
      event.GetX(), Size[1] - event.GetY() - 1);
#endif
  }
  //save the button and capture mouse until the button is released
  //Only if :
  //1. it is possible (WX_USE_X_CAPTURE)
  //2. user decided to.
  if ((ActiveButton != wxEVT_NULL) && WX_USE_X_CAPTURE && UseCaptureMouse)
  {
    CaptureMouse();
  }
}
//---------------------------------------------------------------------------
void wxVTKRenderWindowInteractor::OnButtonUp(wxMouseEvent &event)
{
  //EVT_xxx_DOWN == EVT_xxx_UP - 1
  //This is only needed if two mouse buttons are pressed at the same time.
  //In wxWindows 2.4 and later: better use of wxMOUSE_BTN_RIGHT or 
  //wxEVT_COMMAND_RIGHT_CLICK
  if (!Enabled || (ActiveButton != (event.GetEventType()-1))) 
    {
    return;
    }

#if (VTK_MAJOR_VERSION >= 4 && VTK_MINOR_VERSION > 0)
  SetEventInformationFlipY(event.GetX(), event.GetY(), 
    event.ControlDown(), event.ShiftDown(), '\0', 0, NULL);
#endif
  
  if(ActiveButton == wxEVT_RIGHT_DOWN)
  {
#if (VTK_MAJOR_VERSION >= 4 && VTK_MINOR_VERSION > 0)
    // new style
    InvokeEvent(vtkCommand::RightButtonReleaseEvent, NULL);
#else            
    // old style
    InteractorStyle->OnRightButtonUp(event.ControlDown(), event.ShiftDown(),
      event.GetX(), Size[1] - event.GetY() - 1);
#endif
  }
  else if(ActiveButton == wxEVT_LEFT_DOWN)
  {
#if (VTK_MAJOR_VERSION >= 4 && VTK_MINOR_VERSION > 0)
    // new style
    InvokeEvent(vtkCommand::LeftButtonReleaseEvent, NULL);
#else            
    // old style
    InteractorStyle->OnLeftButtonUp(event.ControlDown(), event.ShiftDown(),
      event.GetX(), Size[1] - event.GetY() - 1);
#endif
  }
  else if(ActiveButton == wxEVT_MIDDLE_DOWN)
  {
#if (VTK_MAJOR_VERSION >= 4 && VTK_MINOR_VERSION > 0)
    // new style
    InvokeEvent(vtkCommand::MiddleButtonReleaseEvent, NULL);
#else            
    // old style
    InteractorStyle->OnMiddleButtonUp(event.ControlDown(), event.ShiftDown(),
      event.GetX(), Size[1] - event.GetY() - 1);
#endif
  }
  //if the ActiveButton is realeased, then release mouse capture
  if ((ActiveButton != wxEVT_NULL) && WX_USE_X_CAPTURE && UseCaptureMouse)
  {
    ReleaseMouse();
  }
  ActiveButton = wxEVT_NULL;
}
//---------------------------------------------------------------------------
void wxVTKRenderWindowInteractor::OnMouseWheel(wxMouseEvent &event)
{
  if (!Enabled) 
    {
    return;
    }

  SetEventInformationFlipY(event.GetX(), event.GetY(), 
    event.ControlDown(), event.ShiftDown(), '\0', 0, NULL);

//  if(event.GetWheelRotation() > 0 )
//    InvokeEvent(vtkCommand::MouseWheelForwardEvent, NULL);
//  else
//    InvokeEvent(vtkCommand::MouseWheelBackwardEvent, NULL);
}
//---------------------------------------------------------------------------
void wxVTKRenderWindowInteractor::Render()
{
  RenderAllowed = 1;
  if (!RenderWhenDisabled)
    {
    //the user doesn't want us to render when the toplevel frame
    //is disabled - first find the top level parent
    wxWindow *topParent = wxGetTopLevelParent(this);
    if (topParent)
      {
      //if it exists, check whether it's enabled
      //if it's not enabeld, RenderAllowed will be false
      RenderAllowed = topParent->IsEnabled();
      }
    }

  if (RenderAllowed)
    {
    if(Handle && (Handle == GetHandle()) )
      {
      RenderWindow->Render();
      }
#if (VTK_MAJOR_VERSION >= 4 && VTK_MINOR_VERSION > 2)
    else if(GetHandle())
      {
      //this means the user has reparented us; let's adapt to the
      //new situation by doing the WindowRemap dance
      //store the new situation
      Handle = GetHandle();
      RenderWindow->SetNextWindowId(reinterpret_cast<void *>(Handle));
      RenderWindow->WindowRemap();
      RenderWindow->Render();
      }
#endif
    }
}
//---------------------------------------------------------------------------
void wxVTKRenderWindowInteractor::SetRenderWhenDisabled(int newValue)
{
  //Change value of __RenderWhenDisabled ivar.
  //If __RenderWhenDisabled is false (the default), this widget will not
  //call Render() on the RenderWindow if the top level frame (i.e. the
  //containing frame) has been disabled.

  //This prevents recursive rendering during wxSafeYield() calls.
  //wxSafeYield() can be called during the ProgressMethod() callback of
  //a VTK object to have progress bars and other GUI elements updated -
  //it does this by disabling all windows (disallowing user-input to
  //prevent re-entrancy of code) and then handling all outstanding
  //GUI events.
        
  //However, this often triggers an OnPaint() method for wxVTKRWIs,
  //resulting in a Render(), resulting in Update() being called whilst
  //still in progress.

  RenderWhenDisabled = (bool)newValue;
}
//---------------------------------------------------------------------------
//
// Set the variable that indicates that we want a stereo capable window
// be created. This method can only be called before a window is realized.
//
void wxVTKRenderWindowInteractor::SetStereo(int capable)
{
  if (Stereo != capable)
    {
    Stereo = capable;
    RenderWindow->StereoCapableWindowOn();
    RenderWindow->SetStereoTypeToCrystalEyes();
    Modified();
    }
}
