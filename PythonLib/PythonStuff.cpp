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

static vector<PathXSeries> toolpath_ftpaths;

static int test_roughing(const char* filepath = NULL)
{
	double cr = 3.0; // corner radius
	double fr = 0.0; // flat radius
	double sd = 5.0;// step down

	// unused for now
        //double si = cr / 2.0;// step in

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

		// clear the toolpath
		toolpath_ftpaths.clear();

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

static PyObject* actp_getnumpaths(PyObject* self, PyObject* args)
{
	PyObject *pValue = PyInt_FromLong(toolpath_ftpaths.size());
	Py_INCREF(pValue);
	return pValue;
}

static PyObject* actp_getnumpoints(PyObject* self, PyObject* args)
{
	int path_index;
	if (!PyArg_ParseTuple(args, "i", &path_index)) return NULL;

	PathXSeries& path = toolpath_ftpaths[path_index];

	PyObject *pValue = PyInt_FromLong(path.pths.size());
	Py_INCREF(pValue);
	return pValue;
}

static PyObject* actp_getz(PyObject* self, PyObject* args)
{
	int path_index;
	if (!PyArg_ParseTuple(args, "i", &path_index)) return NULL;

	PathXSeries& path = toolpath_ftpaths[path_index];

	PyObject *pValue = PyFloat_FromDouble(path.z);
	Py_INCREF(pValue);
	return pValue;
}

static PyObject* actp_getpoint(PyObject* self, PyObject* args)
{
	int path_index, point_index;
	if (!PyArg_ParseTuple(args, "ii", &path_index, &point_index)) return NULL;

	PathXSeries& path = toolpath_ftpaths[path_index];
	P2& point = path.pths[point_index];


	// return point a tuple ( x, y )
	PyObject *pTuple = PyTuple_New(2);
	{
		PyObject *pValue = PyFloat_FromDouble(point.u);
		if (!pValue){
			Py_DECREF(pTuple);return NULL;
		}
		PyTuple_SetItem(pTuple, 0, pValue);
	}
	{
		PyObject *pValue = PyFloat_FromDouble(point.v);
		if (!pValue){
			Py_DECREF(pTuple);return NULL;
		}
		PyTuple_SetItem(pTuple, 1, pValue);
	}

	Py_INCREF(pTuple);
	return pTuple;
}

static PyObject* actp_getnumbreaks(PyObject* self, PyObject* args)
{
	int path_index;
	if (!PyArg_ParseTuple(args, "i", &path_index)) return NULL;

	PathXSeries& path = toolpath_ftpaths[path_index];

	PyObject *pValue = PyInt_FromLong(path.brks.size());
	Py_INCREF(pValue);
	return pValue;
}

static PyObject* actp_getbreak(PyObject* self, PyObject* args)
{
	int path_index, break_index;
	if (!PyArg_ParseTuple(args, "ii", &path_index, &break_index)) return NULL;

	PathXSeries& path = toolpath_ftpaths[path_index];
	int brk = path.brks[break_index];

	PyObject *pValue = PyInt_FromLong(brk);
	Py_INCREF(pValue);
	return pValue;
}

static PyObject* actp_getnumlinkpths(PyObject* self, PyObject* args)
{
	int path_index;
	if (!PyArg_ParseTuple(args, "i", &path_index)) return NULL;

	PathXSeries& path = toolpath_ftpaths[path_index];

	PyObject *pValue = PyInt_FromLong(path.linkpths.size());
	Py_INCREF(pValue);
	return pValue;
}

static PyObject* actp_getnumlinkpoints(PyObject* self, PyObject* args)
{
	int path_index, link_index;
	if (!PyArg_ParseTuple(args, "ii", &path_index, &link_index)) return NULL;

	PathXSeries& path = toolpath_ftpaths[path_index];
	vector<P3> &link_path = path.linkpths[link_index];

	PyObject *pValue = PyInt_FromLong(link_path.size());
	Py_INCREF(pValue);
	return pValue;
}

static PyObject* actp_getlinkpoint(PyObject* self, PyObject* args)
{
	int path_index, link_index, point_index;
	if (!PyArg_ParseTuple(args, "iii", &path_index, &link_index, &point_index)) return NULL;

	PathXSeries& path = toolpath_ftpaths[path_index];
	vector<P3> &link_path = path.linkpths[link_index];
	P3 &point = link_path[point_index];

	// return point a tuple ( x, y, z )
	PyObject *pTuple = PyTuple_New(3);
	{
		PyObject *pValue = PyFloat_FromDouble(point.x);
		if (!pValue){
			Py_DECREF(pTuple);return NULL;
		}
		PyTuple_SetItem(pTuple, 0, pValue);
	}
	{
		PyObject *pValue = PyFloat_FromDouble(point.y);
		if (!pValue){
			Py_DECREF(pTuple);return NULL;
		}
		PyTuple_SetItem(pTuple, 1, pValue);
	}
	{
		PyObject *pValue = PyFloat_FromDouble(point.z);
		if (!pValue){
			Py_DECREF(pTuple);return NULL;
		}
		PyTuple_SetItem(pTuple, 2, pValue);
	}

	Py_INCREF(pTuple);
	return pTuple;
}

static PyMethodDef ActpMethods[] = {
	{"test", actp_test, METH_VARARGS , ""},
	{"makerough", actp_makerough, METH_VARARGS , ""},
	{"getnumpaths", actp_getnumpaths, METH_VARARGS , ""},
	{"getnumpoints", actp_getnumpoints, METH_VARARGS , ""},
	{"getz", actp_getz, METH_VARARGS , ""},
	{"getpoint", actp_getpoint, METH_VARARGS , ""},
	{"getnumbreaks", actp_getnumbreaks, METH_VARARGS , ""},
	{"getbreak", actp_getbreak, METH_VARARGS , ""},
	{"getnumlinkpths", actp_getnumlinkpths, METH_VARARGS , ""},
	{"getnumlinkpoints", actp_getnumlinkpoints, METH_VARARGS , ""},
	{"getlinkpoint", actp_getlinkpoint, METH_VARARGS , ""},
	{NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC
initactp(void)
{
	Py_InitModule("actp", ActpMethods);
}
