// FreesteelPython.h : Defines the entry point for the DLL application.

// Included by SWIG. Defines all exported objects

#include <vtkConeSource.h>
#include <vtkSphereSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRenderer.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkSTLReader.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkOpenGLPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkCellArray.h>

#include "bolts/bolts.h"
#include "cages/cages.h"
#include "pits/pits.h"

#include "visuals/fsvtkToolpathMapper.h"
//#include "visuals/gst.h"
#include "visuals/gstsurface.h"
#include "visuals/MakeToolpath.h"
#include "pits/CoreRoughGeneration.h"
#include "pits/toolshape.h"

///////////////////////////////////////////////////////////////////////////////

class VtkPolyDataSource {
protected:
  vtkPolyDataSource* source;
public:
  virtual ~VtkPolyDataSource() { }
  virtual vtkPolyDataSource* GetDataSource() { return source; }
};

class VtkCone : public VtkPolyDataSource {
public:
  VtkCone(double height = 2.0, double radius = 1.0, int resolution = 10) {
    vtkConeSource* source = vtkConeSource::New();
    source->SetHeight(height);
    source->SetRadius(radius);
    source->SetResolution(resolution);
    this->source = source;
  }
  virtual ~VtkCone() { source->Delete(); }
};

class VtkSphere : public VtkPolyDataSource {
public:
  VtkSphere(double radius = 1.0, int resolution = 10) {
    vtkSphereSource* source = vtkSphereSource::New();
    source->SetRadius(radius);
    source->SetThetaResolution(resolution);
    this->source = source;
  }
  virtual ~VtkSphere() { source->Delete(); }
};

///////////////////////////////////////////////////////////////////////////////

class VtkWindow
{
private:
  int window_id; // A HWND on Win32

  vtkRenderWindow *renderwindow;
  vtkRenderer* renderer;
  vtkRenderWindowInteractor* interactor;

protected:
  VtkWindow(int window_id = 0); // This class should not be instantiated directly!
  virtual ~VtkWindow();

public: 
  inline vtkRenderWindow* getRenderWindow() { return renderwindow; }
  inline vtkRenderer* getRenderer() { return renderer; }
  inline vtkRenderWindowInteractor* getRenderWindowInteractor() { return interactor; }

  void render() { renderwindow->Render(); }
  void start() { interactor->Start(); } // main loop - modal

  void setSize(int w, int h);
};

///////////////////////////////////////////////////////////////////////////////

class PolydataWindow : public VtkWindow
{
private:
  vtkPolyDataMapper *mapper;
  vtkActor *actor;
  vtkPolyData *nulldata;

public:
  PolydataWindow(int window_id = 0);
  virtual ~PolydataWindow();

  void display(VtkPolyDataSource* source = NULL);
};

///////////////////////////////////////////////////////////////////////////////

class FreesteelWindow : public VtkWindow
{
private:
  vector<GSTbase*> gstees;
  vector<SurfX *> surfaces;
  vector<PathXSeries* > toolpaths;

  GSTtoolpath* pthanimated;
	double totallen; // total length
	double animatedlast; // length to last slider position

public:
  FreesteelWindow(int window_id = 0);
  virtual ~FreesteelWindow();

  int add(GSTbase* gst);
  void showAll();

	int LoadSTL(const char* str);
	
  void addPathxSeries(PathXSeries* xser); 
	void PushTrianglesIntoSurface(int ix, SurfX* sx);

  void setAnimated(GSTtoolpath* path);
  void setProgress(double start /* Not implemented */, double end, bool onelevel = false);
};

///////////////////////////////////////////////////////////////////////////////

// NB: Not currently used 
class FreesteelWindow2 : public VtkWindow
{
private:
  GSTsurface* surface;
  GSTtoolpath* boundary;
  GSTtoolpath* toolpath;

public:
  FreesteelWindow2(int window_id = 0);
  virtual ~FreesteelWindow2();

  void clear();

  void setSurface(GSTsurface* surface);
  void setBoundary(const PathXSeries* boundpath);
  void setTool(const ToolShape* tool);
  void addToolpath(const PathXSeries* boundpath);

  void showAll();
};

// EOF ////////////////////////////////////////////////////////////////////////
