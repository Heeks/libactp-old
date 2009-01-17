/* File : VtkObjects.i */
%module freesteel

%{
#include "freesteel-python.h"
#include "src/bolts/P2.h"
#include "src/bolts/P3.h"
%}

/* Let's just grab the original header file here */
%include "freesteel-python.h"
%include "src/bolts/P2.h"
%include "src/bolts/P3.h"
