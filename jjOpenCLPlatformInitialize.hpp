#ifndef _GLIBCXX_IOSTREAM
	#include <iostream>
	#define _GLIBCXX_IOSTREAM
#endif
#ifndef __OPENCL_CL_H
	#ifdef __APPLE__
		#include <OpenCL/opencl.h>
	#else
		#include <CL/cl.h>
	#endif
	#define __OPENCL_CL_H
#endif
#include <cstdlib>
#define JJ_OPENCL_PLATFORM_INITIALIZE

typedef struct platformInformation{
	cl_platform_id platformID;
	char* fullSupportOrNot;
	char* clVersion;
	char* clPlatformName;
	char* clVendor;
	char* clPlatformExtensions;
}JJ_CL_PLAT_INFO;//Structure which has information about each platform

typedef struct clPlatforms{
	cl_uint platformsNum;
	JJ_CL_PLAT_INFO* platforms;
}JJ_CL_PLATFORMS;//Structure which has the number of platforms and array of platform

cl_int jjOpenCLPlatformAsk(cl_platform_info informationMacro,
						   JJ_CL_PLATFORMS* input,
						   int index);
cl_int jjOpenCLPlatformInitialize(JJ_CL_PLATFORMS* input,
								  bool displayInformations);