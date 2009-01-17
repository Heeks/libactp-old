// FreesteelPython.cpp : Defines the entry point for the DLL application.

#include "FreesteelPython.h"

#include <vtkInteractorStyleTrackballCamera.h>

///////////////////////////////////////////////////////////////////////////////

#ifdef WIN32

#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#include <windows.h>

HINSTANCE hInstance = (HINSTANCE)INVALID_HANDLE_VALUE;

BOOL APIENTRY DllMain( HINSTANCE hModule, 
                       DWORD  fdwReason, 
                       LPVOID lpReserved
					 )
{
  switch(fdwReason) {
  case DLL_PROCESS_ATTACH:
    hInstance = hModule;
    return TRUE;
  case DLL_PROCESS_DETACH:
    hInstance = (HINSTANCE)INVALID_HANDLE_VALUE;
    return TRUE;
  default:
    return FALSE;
  }
}

#endif // WIN32

///////////////////////////////////////////////////////////////////////////////

VtkWindow::VtkWindow(int window_id) 
{
//  if( window_id == 0 ) {
//    // TODO: Create a new top-level window
//    renderer = NULL;
//    renderwindow = NULL;
//    interactor = NULL;
//    return;
//  }

  // Create the basic parts of a pipeline and connect them
  renderer = vtkRenderer::New();
  renderwindow = vtkRenderWindow::New();
  renderwindow->AddRenderer(renderer);

  // setup the parent window
  renderwindow->SetParentId((void*)window_id);
  interactor = vtkRenderWindowInteractor::New();

  vtkInteractorStyleTrackballCamera* track = vtkInteractorStyleTrackballCamera::New();
  interactor->SetInteractorStyle(track);
  interactor->SetRenderWindow(renderwindow);

  // set trackball camera mode
  interactor->SetInteractorStyle(vtkInteractorStyleTrackballCamera::New());

  renderer->SetBackground(0.0,0.2,0.5);
  renderwindow->SetSize(400, 400);
}

VtkWindow::~VtkWindow()
{
  if( renderwindow ) renderwindow->Delete();
  if( renderer )     renderer->Delete();
  if( interactor )   interactor->Delete();
}

void VtkWindow::setSize(int w, int h) {
  if( renderwindow == NULL )
    return;
  renderwindow->SetSize(w, h);
}

///////////////////////////////////////////////////////////////////////////////

PolydataWindow::PolydataWindow(int window_id)
  : VtkWindow(window_id)
{
  nulldata = vtkPolyData::New();

  mapper = vtkPolyDataMapper::New();
  mapper->SetInput(nulldata);

  actor = vtkActor::New();
  actor->SetMapper(this->mapper);

  getRenderer()->AddActor(this->actor);

  render(); // Re-render display
}

PolydataWindow::~PolydataWindow()
{
  if( nulldata ) nulldata->Delete();
  if( mapper )   mapper->Delete();
  if( actor )    actor->Delete();
}

void PolydataWindow::display(VtkPolyDataSource* source) {
  if( mapper == NULL )
    return;

  if( source == NULL )
    mapper->SetInput(nulldata);
  else
    mapper->SetInput(source->GetDataSource()->GetOutput());

  render(); // Re-render display
}

///////////////////////////////////////////////////////////////////////////////

FreesteelWindow::FreesteelWindow(int window_id)
  : VtkWindow(window_id)
{
  pthanimated = NULL;
	getRenderer()->GetActiveCamera()->ParallelProjectionOn(); 
	getRenderer()->SetBackground(0.1, 0.2, 0.4);
  render(); // Re-render display
}

FreesteelWindow::~FreesteelWindow()
{
}

int FreesteelWindow::add(GSTbase* gst)
{
  gstees.push_back(gst);
  gst->AddToRenderer(getRenderer());

  return(gstees.size() - 1);
}

int FreesteelWindow::LoadSTL(const char* str)
{
	GSTsurface* surf = new GSTsurface();
	surf->LoadSTL(str);
	return add(surf);
}

void FreesteelWindow::addPathxSeries(PathXSeries* xser)
{
	GSTtoolpath* tp = new GSTtoolpath();
	tp->ftpaths.push_back(*xser);
	tp->UpdateFromPax();
	add(tp);
}

void FreesteelWindow::PushTrianglesIntoSurface(int ix, SurfX* sx)
{
	GSTsurface* surf = dynamic_cast<GSTsurface*>(gstees[ix]);
	ASSERT(surf != NULL);
	surf->PushTrianglesIntoSurface(*sx);
}

void FreesteelWindow::showAll()
{
  I1 xrg(1000,-1000), yrg(1000,-1000), zrg(1000,-1000);
  for (size_t i = 0; i < gstees.size(); ++i)
  {
      xrg.Absorb(gstees[i]->xrg, i == 0);
      yrg.Absorb(gstees[i]->yrg, i == 0);
      zrg.Absorb(gstees[i]->zrg, i == 0);
  }

  getRenderer()->GetActiveCamera()->SetFocalPoint(xrg.Half(), yrg.Half(), zrg.Half());
  getRenderer()->GetActiveCamera()->SetPosition(xrg.Half(), yrg.Half(), zrg.hi + 100);
  
  // scale to fill window
  ASSERT(getRenderer()->GetActiveCamera()->GetParallelProjection() != 0);

  double scale = getRenderer()->GetActiveCamera()->GetParallelScale();
  getRenderer()->GetActiveCamera()->SetParallelScale(max(xrg.Leng(), yrg.Leng())); 
}

bool Advance(AnimatedPos& res, const vector<P2>& pths, const vector< vector<P3> >& links, const vector<int>& brks, double z, double adv, double& advanced, int fcut = -1, int fretract = -1, double tol = 0.0)
{
  advanced = 0;
	res.ilink = -1;

	int j = 0; 
	ASSERT((brks.size() == 0) || (brks.back() == pths.size()));
	int i;
	for (i = 1; i < pths.size(); i++) 
	{
		if ((j == brks.size()) || (i < brks[j]))
		{
			double lenseg = (pths[i] - pths[i - 1]).Len();
			if ((adv >= 0) && (adv - lenseg) <= 0.0)
			{
				// stop along this segment
				advanced += adv;
				res.ilink = j;
				res.isegOnLink = 0;
				res.isegOnPath = i;
				res.ptOnPath = Along((adv / lenseg), pths[i - 1], pths[i]);
				res.bOnPath = true;
				return false;
			}

			advanced += lenseg;
			adv -= lenseg;
			res.ptOnPath = pths[i];
			res.isegOnPath = i;
		}
		// advance through possible multiple markings on this segment.  
		else
		{
			ASSERT(i == brks[j]); 
			do
			{
				res.ilink = j;
				const vector<P3>& link = links[j];
				for (int il = 1; il < link.size(); ++il) 
				{
					double lenseg = (link[il] - link[il - 1]).Len();
					if ((adv >= 0) && (adv - lenseg) <= 0.0)
					{
						// stop along this segment
						advanced += adv;
						res.isegOnLink = il;
						res.ptOnLink = Along((adv /	lenseg), link[il - 1], link[il]);
						res.bOnPath = false;
						return false;
					}

					advanced += lenseg;
					adv -= lenseg;
					res.ptOnLink = link[il];
					res.isegOnLink = il;
				}
				res.isegOnLink = link.size();
				if (!link.empty())
					res.ptOnLink = link.back();
				j++;
			}
			while ((j < brks.size()) && (i == brks[j])); 
		}
	}

	do
	{
		res.ilink = j;
		const vector<P3>& link = links[j];
		for (int il = 1; il < link.size(); ++il) 
		{
			double lenseg = (link[il] - link[il - 1]).Len();
			if ((adv >= 0) && (adv - lenseg) <= 0.0)
			{
				// stop along this segment
				advanced += adv;
				res.isegOnLink = il;
				res.ptOnLink = Along((adv /	lenseg), link[il - 1], link[il]);
				res.bOnPath = false;
				return false;
			}

			advanced += lenseg;
			adv -= lenseg;
			res.ptOnLink = link[il];
			res.isegOnLink = il;
		}
		res.isegOnLink = link.size();
		if (!link.empty())
			res.ptOnLink = link.back();
		j++;
	}
	while ((j < brks.size()) && (i == brks[j]));

  return true; 
}

void FreesteelWindow::setAnimated(GSTtoolpath* gst)
{
  if (pthanimated)
    pthanimated->SetDrawWhole();

  pthanimated = gst;
  totallen = 0;
  for (int ip = 0; ip < pthanimated->ftpaths.size(); ++ip)
  {
    double levellen = 0;
    const PathXSeries& pathxs = pthanimated->ftpaths[ip];
    Advance(pthanimated->ftpolydataMap->pos, pathxs.pths, pathxs.linkpths, pathxs.brks, pathxs.z, -1.0, levellen);
    totallen += levellen;            
  }
  pthanimated->ftpolydataMap->pos.ipathx = pthanimated->ftpaths.size() - 1;

  animatedlast = 0;

  pthanimated->ftpolydataMap->xrg = gst->xrg;
  pthanimated->ftpolydataMap->yrg = gst->yrg;
  pthanimated->ftpolydataMap->bAnimated = true;
  pthanimated->ftpolydataMap->bForward = false;
}

void FreesteelWindow::setProgress(double start, double end, bool onelevel /* = false*/) {
  // TODO: Implement start...

  if( !pthanimated ) return;
  if( !pthanimated->ftpaths.size() ) return;

  // draw toolpath up to that length
  double animatedlength = (totallen * end);

  // set new end position from animated length
  double advanced;
  int ip = 0;
  for (; ip < pthanimated->ftpaths.size(); ++ip)
  {
      PathXSeries& pathxs = pthanimated->ftpaths[ip];
      bool bFinish = Advance(pthanimated->ftpolydataMap->pos, pathxs.pths, pathxs.linkpths, pathxs.brks, pathxs.z, animatedlength, advanced);
      animatedlength -= advanced;
      if (!bFinish)
      {
          TOL_ZERO(animatedlength);
          break;
      }
  }
  pthanimated->ftpolydataMap->pos.ipathx = min(ip, (int)pthanimated->ftpaths.size() - 1);

  // starting from
  if (onelevel)
    pthanimated->ftpolydataMap->poslast.ipathx = pthanimated->ftpolydataMap->pos.ipathx;
  else
    pthanimated->ftpolydataMap->poslast.ipathx = 0;

  render();
}

///////////////////////////////////////////////////////////////////////////////

FreesteelWindow2::FreesteelWindow2(int window_id)
  : VtkWindow(window_id)
{
	getRenderer()->GetActiveCamera()->ParallelProjectionOn(); 
	getRenderer()->SetBackground(0.1, 0.2, 0.4);
  render(); // Re-render display

//  surface = new GSTsurface();
  boundary = new GSTtoolpath();
  toolpath = new GSTtoolpath();
}

FreesteelWindow2::~FreesteelWindow2()
{
//  if( surface )  delete surface;
  if( boundary ) delete boundary;
  if( toolpath ) delete toolpath;
}

void FreesteelWindow2::setSurface(GSTsurface* surface) {
  this->surface = surface;
  surface->AddToRenderer(getRenderer());
}

void FreesteelWindow2::setBoundary(const PathXSeries* boundpath) {
  boundary->ftpaths.push_back(*boundpath);
  boundary->UpdateFromPax();
  boundary->AddToRenderer(getRenderer());
}

void FreesteelWindow2::addToolpath(const PathXSeries* toolpath) {
  this->toolpath->ftpaths.push_back(*toolpath);
  this->toolpath->UpdateFromPax();
  this->toolpath->AddToRenderer(getRenderer());
}

void FreesteelWindow2::clear() {
  this->toolpath->ftpaths.clear();
  this->toolpath->UpdateFromPax();
  this->toolpath->AddToRenderer(getRenderer());
}

void FreesteelWindow2::setTool(const ToolShape* tool) {
  toolpath->toolshape = *tool;
}

void FreesteelWindow2::showAll()
{
  I1 xrg(1000,-1000), yrg(1000,-1000), zrg(1000,-1000);
  xrg.Absorb(surface->xrg, true);
  yrg.Absorb(surface->yrg, true);
  zrg.Absorb(surface->zrg, true);

  getRenderer()->GetActiveCamera()->SetFocalPoint(xrg.Half(), yrg.Half(), zrg.Half());
  getRenderer()->GetActiveCamera()->SetPosition(xrg.Half(), yrg.Half(), zrg.hi + 100);
  
  // scale to fill window
  ASSERT(getRenderer()->GetActiveCamera()->GetParallelProjection() != 0);

  double scale = getRenderer()->GetActiveCamera()->GetParallelScale();
  getRenderer()->GetActiveCamera()->SetParallelScale(max(xrg.Leng(), yrg.Leng())); 
}

// EOF ////////////////////////////////////////////////////////////////////////
