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

#include "vtkFloatArray.h"
#include "vtkClipPolyData.h"
#include "vtkRenderWindow.h"
#include "vtkCellArray.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkActor.h"
#include "vtkActorCollection.h"
#include "vtkSTLReader.h"
#include "vtkPolyDataMapper.h"
#include "vtkOpenGLPolyDataMapper.h"

#include "bolts/bolts.h"
#include "cages/cages.h"
#include "pits/pits.h"
#include "visuals/MakeToolpath.h"
#include "visuals/MakeToolpath.h"
#include "visuals/fsvtkToolpathMapper.h"
#include "visuals/gstsurface.h"
#include "visuals/gst.h"

#include "MainFrame.h"
#include "AnimationFrame.h"

static char *cformat = "LX%.3fY%.3fZ%.3fF%d\n";
static char *cformatNoF = "LX%.3fY%.3fZ%.3f\n";
static char *cformatNoFZ = "LX%.3fY%.3f\n";

class ThinAlg
{
public:
	double z;
	int fcut;
	double sqdevtol;
	vector<P2> pts;
	int ixstart;

	ThinAlg(double devtol, double lz, int lfcut)
	{
		z = lz;
		ixstart = 0;
		sqdevtol = devtol * devtol;
		fcut = lfcut;
	}

	void AddPoint(const P2& ptnew, FILE* fn)
	{
		pts.push_back(ptnew);
		if ((pts.size() - ixstart) == 1)
		{
			if (pts.size() == 1)
				fprintf(fn, cformat, pts.back().u, pts.back().v, z, fcut);
			else
				fprintf(fn, cformatNoFZ, pts.back().u, pts.back().v);
		}
		else
		{
			P2 vec = ptnew - pts[ixstart];
			double veclen = vec.Len();
			ASSERT(veclen > 0);

			for (int ix = ixstart + 1; ix < pts.size() - 1; ++ix)
			{
				double d = Dot(vec, (pts[ix] - pts[ixstart])) / veclen;
				double lam = d / veclen;
				lam = I1unit.PushInto(lam);
				P2 vperp(pts[ix] - Along(lam, pts[ixstart], ptnew));
				double sqdev = vperp.Lensq();
				if (sqdev > sqdevtol)
				{
					fprintf(fn, cformatNoFZ, pts[pts.size() - 2].u, pts[pts.size() - 2].v);
					ixstart = pts.size() - 2;
					break;
				}
			}
		}
	}

	void End(FILE* fn)
	{
		fprintf(fn, cformatNoFZ, pts.back().u, pts.back().v);
		pts.clear();
		ixstart = 0;
	}
};

bool Advance(AnimatedPos& res, const vector<P2>& pths, const vector< vector<P3> >& links, const vector<int>& brks, double z, double adv, double& advanced, FILE* fn = NULL, int fcut = -1, int fretract = -1, double tol = 0.0)
{
    advanced = 0;
	res.ilink = -1;

	ThinAlg thin(tol, z, fcut);
	if (fn)
		thin.AddPoint(pths[0], fn);

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

			if (fn)
			{
				if (thin.pts.empty())
					thin.AddPoint(pths[i - 1], fn);
				thin.AddPoint(res.ptOnPath, fn);
			}
		}
		// advance through possible multiple markings on this segment.  
		else
		{
			ASSERT(i == brks[j]); 
			do
			{
				res.ilink = j;
				const vector<P3>& link = links[j];
				if (fn && !link.empty())
				{
					thin.End(fn);
					fprintf(fn,	cformat, link[0].x, link[0].y, link[0].z, fretract);
				}
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
					if (fn)
						fprintf(fn,	cformatNoF, res.ptOnLink.x, res.ptOnLink.y, res.ptOnLink.z);
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
		if (fn && !link.empty())
		{
			thin.End(fn);
			fprintf(fn,	cformat, link[0].x, link[0].y, link[0].z, fretract);
		}
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
			if (fn)
				fprintf(fn,	cformatNoF, res.ptOnLink.x, res.ptOnLink.y, res.ptOnLink.z);
		}
		res.isegOnLink = link.size();
		if (!link.empty())
			res.ptOnLink = link.back();
		j++;
	}
	while ((j < brks.size()) && (i == brks[j]));
	
	 return true; 
}

void PostProcess(FILE* file, const vector<PathXSeries>& pathxseries, const MachineParams& params)
{
	AnimatedPos pos;
	for (int ip = 0; ip < pathxseries.size(); ++ip)
	{
		double levellen = 0;
		const PathXSeries& pathxs = pathxseries[ip];
		Advance(pos, pathxs.pths, pathxs.linkpths, pathxs.brks, pathxs.z, -1.0, levellen, file, params.fcut, params.fretract, params.thintol);
	}
}


BEGIN_EVENT_TABLE(AnimationFrame, wxFrame)
    EVT_MENU(MENU_ANIMATE,		      AnimationFrame::OnAnimate)
    EVT_BUTTON(CALC_STOCK,                    AnimationFrame::OnCalculateStock)
    EVT_MENU(wxID_EXIT,                       AnimationFrame::OnExit)
    EVT_COMMAND_SCROLL(SLIDER_ANIMATION,      AnimationFrame::OnAnimateTP)
END_EVENT_TABLE()

/*
    This array takes the EXACT order of the declarations in
    include/wx/event.h
    (section "wxScrollBar and wxSlider event identifiers")
*/
static const wxChar *eventNames[] =
{
    wxT("wxEVT_SCROLL_TOP"),
    wxT("wxEVT_SCROLL_BOTTOM"),
    wxT("wxEVT_SCROLL_LINEUP"),
    wxT("wxEVT_SCROLL_LINEDOWN"),
    wxT("wxEVT_SCROLL_PAGEUP"),
    wxT("wxEVT_SCROLL_PAGEDOWN"),
    wxT("wxEVT_SCROLL_THUMBTRACK"),
    wxT("wxEVT_SCROLL_THUMBRELEASE"),
    wxT("wxEVT_SCROLL_ENDSCROLL")
};


AnimationFrame::AnimationFrame(MainFrame* mf, const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame(mf, -1, title, pos, size)
{
    mainfr = mf;
    CreateStatusBar();
    SetStatusText(status);
    
    wxBoxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);
    sldTP = new wxSlider(this, SLIDER_ANIMATION, 0L, 0L, 1000L); 
    wxButton *stock = new wxButton(this, CALC_STOCK, _T("Stock"));  

    sizerTop->Add(sldTP, 1, wxEXPAND | wxALL, 5);
    sizerTop->Add(stock, 1, wxEXPAND | wxALL, 5);

    SetAutoLayout(TRUE);
    SetSizer(sizerTop);

    sizerTop->SetSizeHints(this);
    sizerTop->Fit(this);

    pthanimated = NULL; // nothing animated to start with
    ClearAnimated();

    style = FROMSTART;

    sldTP->Disable();
}

void AnimationFrame::UpdateControls()
{
}


void AnimationFrame::OnAnimate(wxCommandEvent& event)
{
	Show(TRUE);
	sldTP->Enable();
}

void AnimationFrame::OnExit(wxCommandEvent& WXUNUSED(event) )
{
//	Show(FALSE);
//	sld->Disable();
}

void AnimationFrame::OnAnimateTP(wxScrollEvent& event)
{
    pthanimated->ftpolydataMap->bDrawStock = false;
    
    wxEventType eventType = event.GetEventType();
    int index = eventType - wxEVT_SCROLL_TOP;

    switch(index)
    {
        case 6:
        case 7:
        {
            // draw toolpath up to that length
            double animatedlength = (totallen * (event.GetPosition() / 1000.0));
            
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
            status.Clear();
//            status << _T("Length: ") << animatedlength << _T("     Location: ") << ppos.first.x << _T(", ") << ppos.first.y << _T(", ") << ppos.first.z;

            // starting from
            if (style == ONELEVEL)
            {
				pthanimated->ftpolydataMap->poslast.ipathx = pthanimated->ftpolydataMap->pos.ipathx;
            }
            else
            {
				pthanimated->ftpolydataMap->poslast.ipathx = 0;
            }

            mainfr->GetGst()->renWin->Render();
            break;
        }

        default:
            event.Skip();            
	}	
}

void AnimationFrame::OnCalculateStock(wxCommandEvent& WXUNUSED(event) )
{
	pthanimated->ftpolydataMap->CalculateStock((*pthanimated->ftpolydataMap->pftpaths)[pthanimated->ftpolydataMap->pos.ipathx].z + pthanimated->ftpolydataMap->ptoolshape->cornerrad);
    pthanimated->ftpolydataMap->bDrawStock = true;
    mainfr->GetGst()->renWin->Render();    
}

void AnimationFrame::SetAnimated(class GSTtoolpath* gst)
{
    ClearAnimated();
    
    pthanimated = gst;
    ASSERT(pthanimated);

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

    sldTP->Enable();
}

void AnimationFrame::ClearAnimated()
{
    if (pthanimated)
        pthanimated->SetDrawWhole();
    pthanimated = NULL;

    sldTP->SetValue(0);
    sldTP->Disable();
}
