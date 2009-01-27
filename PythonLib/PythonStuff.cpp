// PythonStuff.cpp

#ifdef WIN32
#include "windows.h"
#endif

#include "MakeToolpath.h"

#if _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif


#ifdef WIN32

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;

	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

#endif

static void PushTestTrianglesIntoSurface(SurfX& sx) 
{
	P3 p0(0.0, 0.0, 0.0); 
	P3 p1(20.0, 0.0, 0.0); 
	P3 p2(7.0, 7.0, 4.0); 
	P3 p3(13.0, 7.0, 4.0); 
	P3 p4(10.0, 10.0, 15.0); 
	P3 p5(7.0, 13.0, 4.0); 
	P3 p6(13.0, 13.0, 4.0); 
	P3 p7(0.0, 20.0, 0.0); 
	P3 p8(20.0, 20.0, 0.0); 

	sx.PushTriangle(p0, p1, p3); 
	sx.PushTriangle(p0, p3, p2); 
	sx.PushTriangle(p0, p2, p7); 
	sx.PushTriangle(p2, p5, p7); 
	sx.PushTriangle(p1, p8, p3); 
	sx.PushTriangle(p3, p8, p6); 
	sx.PushTriangle(p5, p6, p8); 
	sx.PushTriangle(p5, p8, p7); 
	sx.PushTriangle(p2, p3, p4); 
	sx.PushTriangle(p3, p6, p4); 
	sx.PushTriangle(p6, p5, p4); 
	sx.PushTriangle(p2, p4, p5); 
}


static void MakeRectBoundary(SurfX &sx, vector<PathXSeries> &boundary_ftpaths) 
{
	boundary_ftpaths.push_back(PathXSeries());
	boundary_ftpaths.back().z = sx.gzrg.hi + 1.0;
	boundary_ftpaths.back().Add(P2(sx.gxrg.lo, sx.gyrg.lo)); 
	boundary_ftpaths.back().Add(P2(sx.gxrg.hi, sx.gyrg.lo)); 
	boundary_ftpaths.back().Add(P2(sx.gxrg.hi, sx.gyrg.hi)); 
	boundary_ftpaths.back().Add(P2(sx.gxrg.lo, sx.gyrg.hi)); 
	boundary_ftpaths.back().Add(P2(sx.gxrg.lo, sx.gyrg.lo)); 
	boundary_ftpaths.back().Break(); 
}

static int test_roughing(const char* filepath = NULL)
{
	double cr = 3.0; // corner radius
	double fr = 0.0; // flat radius
	double sd = 5.0;// step down
	double si = cr / 2.0;// step in
	//if (RunCoreRoughDlg(cr, fr, sd, si))
	{
		MachineParams params;
	// linking parameters
		params.leadoffdz = 0.1; 
		params.leadofflen = 1.1;
		params.leadoffrad = 2.0;
		params.retractzheight = /*gstsurf->zrg.hi*/ 15.0 + 5.0;
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



		//GSTtoolpath* gsttpath = new GSTtoolpath;
		//gst->gstees.push_back(gsttpath);
		vector<PathXSeries> toolpath_ftpaths;
		
		// define the surface
		SurfX sx;
		if(filepath)
		{
			sx.ReadStlFile(filepath);
		}
		else
		{
			PushTestTrianglesIntoSurface(sx);
		}

		sx.BuildComponents(); // compress thing 

		// make a rectangle boundary
		vector<PathXSeries> boundary_ftpaths;
		MakeRectBoundary(sx, boundary_ftpaths);

		// make the roughing toolpath
		MakeCorerough(toolpath_ftpaths, sx, boundary_ftpaths[0], params);

		// print them
		for(vector<PathXSeries>::iterator It = toolpath_ftpaths.begin(); It != toolpath_ftpaths.end(); It++)
		{
			PathXSeries &ps = *It;
			cout<<"PathXSeries z = "<<ps.z<<"\n";
			for(vector<P2>::iterator It2 = ps.pths.begin(); It2 != ps.pths.end(); It2++)
			{
				P2 &p2 = *It2;
				cout<<"X"<<p2.u<<"Y"<<p2.v<<" ";
			}
			cout<<"end PathXSeries\n";
		}

		// write result to a file
		//FILE* fpost = fopen("freesteel.tp", "w");
		//ASSERT(fpost);
		//PostProcess(fpost, toolpath_ftpaths, params);
		//fclose(fpost);


		//gsttpath->toolshape = ToolShape(params.toolflatrad, params.toolcornerrad, params.toolcornerrad, params.toolcornerrad / 10.0);
		//gsttpath->bound.Append(boundary_ftpaths[0].pths);
		//gsttpath->UpdateFromPax();
		//gsttpath->AddToRenderer(&gst->ren1);
	}

	return 0; // success
}

static PyObject* actp_test(PyObject* self, PyObject* args)
{
	int result = test_roughing();

	// return int result
	PyObject *pValue = PyInt_FromLong(result);
	Py_INCREF(pValue);
	return pValue;
}

static PyObject* actp_makerough(PyObject* self, PyObject* args)
{
	char* filepath;
	if (!PyArg_ParseTuple(args, "s", &filepath)) return NULL;

	int result = test_roughing(filepath);

	// return int result
	PyObject *pValue = PyInt_FromLong(result);
	Py_INCREF(pValue);
	return pValue;
}

static PyMethodDef ActpMethods[] = {
	{"test", actp_test, METH_VARARGS , ""},
	{"makerough", actp_makerough, METH_VARARGS , ""},
	{NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC
initactp(void)
{
	Py_InitModule("actp", ActpMethods);
}
