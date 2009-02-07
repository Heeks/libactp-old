// PythonStuff.cpp

// written by Dan Heeks, January 2009, license: GPL version 3 http://www.gnu.org/licenses/gpl-3.0.txt

#ifdef WIN32
#include "windows.h"
#endif

#include "pits/CoreRoughGeneration.h"

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

static MachineParams params;
static vector<PathXSeries> boundary;
static vector<PathXSeries> toolpath;

static void MakeRectBoundary(SurfX &sx, vector<PathXSeries> &b) 
{
	b.push_back(PathXSeries());
	b.back().z = sx.gzrg.hi + 1.0;
	b.back().Add(P2(sx.gxrg.lo, sx.gyrg.lo)); 
	b.back().Add(P2(sx.gxrg.hi, sx.gyrg.lo)); 
	b.back().Add(P2(sx.gxrg.hi, sx.gyrg.hi)); 
	b.back().Add(P2(sx.gxrg.lo, sx.gyrg.hi)); 
	b.back().Add(P2(sx.gxrg.lo, sx.gyrg.lo)); 
	b.back().Break(); 
}

static int make_roughing(const char* filepath)
{
	// clear the toolpath
	toolpath.clear();

	// define the surface
	SurfX sx;
	sx.ReadStlFile(filepath);
	if(params.minz > sx.gzrg.lo)sx.gzrg.lo = params.minz;
	sx.BuildComponents(); // compress thing 

	// make a rectangle boundary
	if(boundary.size() == 0)
	{
		MakeRectBoundary(sx, boundary);
	}

	// make the roughing toolpath
	MakeCorerough(toolpath, sx, boundary[0], params);

	return 0; // success
}

static PyObject* actp_makerough(PyObject* self, PyObject* args)
{
	char* filepath;
	if (!PyArg_ParseTuple(args, "s", &filepath)) return NULL;

	int result = make_roughing(filepath);

	// return int result
	PyObject *pValue = PyInt_FromLong(result);
	Py_INCREF(pValue);
	return pValue;
}

static PyObject* actp_getnumpaths(PyObject* self, PyObject* args)
{
	PyObject *pValue = PyInt_FromLong(toolpath.size());
	Py_INCREF(pValue);
	return pValue;
}

static PyObject* actp_getnumpoints(PyObject* self, PyObject* args)
{
	int path_index;
	if (!PyArg_ParseTuple(args, "i", &path_index)) return NULL;

	PathXSeries& path = toolpath[path_index];

	PyObject *pValue = PyInt_FromLong(path.pths.size());
	Py_INCREF(pValue);
	return pValue;
}

static PyObject* actp_getz(PyObject* self, PyObject* args)
{
	int path_index;
	if (!PyArg_ParseTuple(args, "i", &path_index)) return NULL;

	PathXSeries& path = toolpath[path_index];

	PyObject *pValue = PyFloat_FromDouble(path.z);
	Py_INCREF(pValue);
	return pValue;
}

static PyObject* actp_getpoint(PyObject* self, PyObject* args)
{
	int path_index, point_index;
	if (!PyArg_ParseTuple(args, "ii", &path_index, &point_index)) return NULL;

	PathXSeries& path = toolpath[path_index];
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

	PathXSeries& path = toolpath[path_index];

	PyObject *pValue = PyInt_FromLong(path.brks.size());
	Py_INCREF(pValue);
	return pValue;
}

static PyObject* actp_getbreak(PyObject* self, PyObject* args)
{
	int path_index, break_index;
	if (!PyArg_ParseTuple(args, "ii", &path_index, &break_index)) return NULL;

	PathXSeries& path = toolpath[path_index];
	int brk = path.brks[break_index];

	PyObject *pValue = PyInt_FromLong(brk);
	Py_INCREF(pValue);
	return pValue;
}

static PyObject* actp_getnumlinkpths(PyObject* self, PyObject* args)
{
	int path_index;
	if (!PyArg_ParseTuple(args, "i", &path_index)) return NULL;

	PathXSeries& path = toolpath[path_index];

	PyObject *pValue = PyInt_FromLong(path.linkpths.size());
	Py_INCREF(pValue);
	return pValue;
}

static PyObject* actp_getnumlinkpoints(PyObject* self, PyObject* args)
{
	int path_index, link_index;
	if (!PyArg_ParseTuple(args, "ii", &path_index, &link_index)) return NULL;

	PathXSeries& path = toolpath[path_index];
	vector<P3> &link_path = path.linkpths[link_index];

	PyObject *pValue = PyInt_FromLong(link_path.size());
	Py_INCREF(pValue);
	return pValue;
}

static PyObject* actp_getlinkpoint(PyObject* self, PyObject* args)
{
	int path_index, link_index, point_index;
	if (!PyArg_ParseTuple(args, "iii", &path_index, &link_index, &point_index)) return NULL;

	PathXSeries& path = toolpath[path_index];
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

static PyObject* actp_setleadoffdz(PyObject* self, PyObject* args)
{
	double d;
	if (!PyArg_ParseTuple(args, "d", &d)) return NULL;
	params.leadoffdz = d;

	Py_RETURN_NONE;
}

static PyObject* actp_setleadofflen(PyObject* self, PyObject* args)
{
	double d;
	if (!PyArg_ParseTuple(args, "d", &d)) return NULL;
	params.leadofflen = d;

	Py_RETURN_NONE;
}

static PyObject* actp_setleadoffrad(PyObject* self, PyObject* args)
{
	double d;
	if (!PyArg_ParseTuple(args, "d", &d)) return NULL;
	params.leadoffrad = d;

	Py_RETURN_NONE;
}

static PyObject* actp_setretractzheight(PyObject* self, PyObject* args)
{
	double d;
	if (!PyArg_ParseTuple(args, "d", &d)) return NULL;
	params.retractzheight = d;

	Py_RETURN_NONE;
}

static PyObject* actp_setleadoffsamplestep(PyObject* self, PyObject* args)
{
	double d;
	if (!PyArg_ParseTuple(args, "d", &d)) return NULL;
	params.leadoffsamplestep = d;

	Py_RETURN_NONE;
}

static PyObject* actp_settoolcornerrad(PyObject* self, PyObject* args)
{
	double d;
	if (!PyArg_ParseTuple(args, "d", &d)) return NULL;
	params.toolcornerrad = d;

	Py_RETURN_NONE;
}

static PyObject* actp_settoolflatrad(PyObject* self, PyObject* args)
{
	double d;
	if (!PyArg_ParseTuple(args, "d", &d)) return NULL;
	params.toolflatrad = d;

	Py_RETURN_NONE;
}

static PyObject* actp_setsamplestep(PyObject* self, PyObject* args)
{
	double d;
	if (!PyArg_ParseTuple(args, "d", &d)) return NULL;
	params.samplestep = d;

	Py_RETURN_NONE;
}

static PyObject* actp_setstepdown(PyObject* self, PyObject* args)
{
	double d;
	if (!PyArg_ParseTuple(args, "d", &d)) return NULL;
	params.stepdown = d;

	Py_RETURN_NONE;
}

static PyObject* actp_setclearcuspheight(PyObject* self, PyObject* args)
{
	double d;
	if (!PyArg_ParseTuple(args, "d", &d)) return NULL;
	params.clearcuspheight = d;

	Py_RETURN_NONE;
}

static PyObject* actp_settriangleweaveres(PyObject* self, PyObject* args)
{
	double d;
	if (!PyArg_ParseTuple(args, "d", &d)) return NULL;
	params.triangleweaveres = d;

	Py_RETURN_NONE;
}

static PyObject* actp_setflatradweaveres(PyObject* self, PyObject* args)
{
	double d;
	if (!PyArg_ParseTuple(args, "d", &d)) return NULL;
	params.flatradweaveres = d;

	Py_RETURN_NONE;
}

static PyObject* actp_setdchangright(PyObject* self, PyObject* args)
{
	double d;
	if (!PyArg_ParseTuple(args, "d", &d)) return NULL;
	params.dchangright = d;

	Py_RETURN_NONE;
}

static PyObject* actp_setdchangrightoncontour(PyObject* self, PyObject* args)
{
	double d;
	if (!PyArg_ParseTuple(args, "d", &d)) return NULL;
	params.dchangrightoncontour = d;

	Py_RETURN_NONE;
}

static PyObject* actp_setdchangleft(PyObject* self, PyObject* args)
{
	double d;
	if (!PyArg_ParseTuple(args, "d", &d)) return NULL;
	params.dchangleft = d;

	Py_RETURN_NONE;
}

static PyObject* actp_setdchangefreespace(PyObject* self, PyObject* args)
{
	double d;
	if (!PyArg_ParseTuple(args, "d", &d)) return NULL;
	params.dchangefreespace = d;

	Py_RETURN_NONE;
}

static PyObject* actp_setsidecutdisplch(PyObject* self, PyObject* args)
{
	double d;
	if (!PyArg_ParseTuple(args, "d", &d)) return NULL;
	params.sidecutdisplch = d;

	Py_RETURN_NONE;
}

static PyObject* actp_setfcut(PyObject* self, PyObject* args)
{
	int i;
	if (!PyArg_ParseTuple(args, "i", &i)) return NULL;
	params.fcut = i;

	Py_RETURN_NONE;
}

static PyObject* actp_setfretract(PyObject* self, PyObject* args)
{
	int i;
	if (!PyArg_ParseTuple(args, "i", &i)) return NULL;
	params.fretract = i;

	Py_RETURN_NONE;
}

static PyObject* actp_setthintol(PyObject* self, PyObject* args)
{
	double d;
	if (!PyArg_ParseTuple(args, "d", &d)) return NULL;
	params.thintol = d;

	Py_RETURN_NONE;
}

static PyObject* actp_setstartpoint(PyObject* self, PyObject* args)
{
	double x, y, vx, vy;
	if (!PyArg_ParseTuple(args, "dddd", &x, &y, &vx, &vy)) return NULL;
	params.use_given_start_point = true;
	params.start_point = P2(x, y);
	P2 v(vx, vy);
	if(v.Len() < 0.000000001)v = P2(1.0, 0.0);
	params.start_direction = v / v.Len(); 

	Py_RETURN_NONE;
}

static PyObject* actp_setminz(PyObject* self, PyObject* args)
{
	double d;
	if (!PyArg_ParseTuple(args, "d", &d)) return NULL;
	params.minz = d;

	Py_RETURN_NONE;
}

static PyObject* actp_boundaryclear(PyObject* self, PyObject* args)
{
	double d = 0.0;
	PyArg_ParseTuple(args, "d", &d);
	boundary.clear();
	boundary.push_back(PathXSeries());
	boundary.back().z = d;

	Py_RETURN_NONE;
}

static PyObject* actp_boundaryadd(PyObject* self, PyObject* args)
{
	double x, y;
	if (!PyArg_ParseTuple(args, "dd", &x, &y)) return NULL;
	boundary.back().Add(P2(x, y)); 

	Py_RETURN_NONE;
}

static PyObject* actp_boundarybreak(PyObject* self, PyObject* args)
{
	// i'm not sure yet if you need one of these at the end of every boundary
	boundary.back().Break(); 
	Py_RETURN_NONE;
}

static PyObject* actp_resetparams(PyObject* self, PyObject* args)
{
	// reset all the parameters to their default values
	params = MachineParams();

	Py_RETURN_NONE;
}

static PyMethodDef ActpMethods[] = {
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
	{"setleadoffdz", actp_setleadoffdz, METH_VARARGS , ""},
	{"setleadofflen", actp_setleadofflen, METH_VARARGS , ""},
	{"setleadoffrad", actp_setleadoffrad, METH_VARARGS , ""},
	{"setretractzheight", actp_setretractzheight, METH_VARARGS , ""},
	{"setleadoffsamplestep", actp_setleadoffsamplestep, METH_VARARGS , ""},
	{"settoolcornerrad", actp_settoolcornerrad, METH_VARARGS , ""},
	{"settoolflatrad", actp_settoolflatrad, METH_VARARGS , ""},
	{"setsamplestep", actp_setsamplestep, METH_VARARGS , ""},
	{"setstepdown", actp_setstepdown, METH_VARARGS , ""},
	{"setclearcuspheight", actp_setclearcuspheight, METH_VARARGS , ""},
	{"settriangleweaveres", actp_settriangleweaveres, METH_VARARGS , ""},
	{"setflatradweaveres", actp_setflatradweaveres, METH_VARARGS , ""},
	{"setdchangright", actp_setdchangright, METH_VARARGS , ""}, 
	{"setdchangrightoncontour", actp_setdchangrightoncontour, METH_VARARGS , ""},
	{"setdchangleft", actp_setdchangleft, METH_VARARGS , ""},
	{"setdchangefreespace", actp_setdchangefreespace, METH_VARARGS , ""},
	{"setsidecutdisplch", actp_setsidecutdisplch, METH_VARARGS , ""},
	/*int*/{"setfcut", actp_setfcut, METH_VARARGS , ""},
	/*int*/{"setfretract", actp_setfretract, METH_VARARGS , ""},
	{"setthintol", actp_setthintol, METH_VARARGS , ""},
	{"setstartpoint", actp_setstartpoint, METH_VARARGS , ""},
	{"setminz", actp_setminz, METH_VARARGS , ""},
	{"boundaryclear", actp_boundaryclear, METH_VARARGS , ""},
	{"boundaryadd", actp_boundaryadd, METH_VARARGS , ""},
	{"boundarybreak", actp_boundarybreak, METH_VARARGS , ""},
	{"resetdefaults", actp_resetparams, METH_VARARGS , ""},
	{NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC
initactp(void)
{
	Py_InitModule("actp", ActpMethods);
}
