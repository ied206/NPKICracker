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
#ifndef JJ_OPENCL_PLATFORM_INITIALIZE
	#include "jjOpenCLPlatformInitialize.hpp"
	#define JJ_OPENCL_PLATFORM_INITIALIZE
#endif



using namespace std;



cl_int jjOpenCLPlatformAsk(cl_platform_info informationMacro,
						   JJ_CL_PLATFORMS* input,
						   int index)
{
	/*
	 * This function ask platform to give it's property
	 * This function take informationMacro, and return to platforms in structure which pointed by second argument input pointer.
	 * Index is used to both case, showing error message and selecting platform in structure pointed by input pointer
	 */
	string macroToString;//This variable store Error Message's platform's property name. This variable is set by below's switch statement
	char** informationToKnow = NULL;//This variable store pointer to pass to clGetPlagroemInfo. This variable is set by below's switch statement
	size_t platformInformationSize;//DONT give attention to this variable. This variable store size to pass to clGetPlatformInfo.
	cl_int errNum;//DONT give attention to this variable. This variable store error message returned from function called by this function. 


	switch (informationMacro)
	{
		case CL_PLATFORM_NAME: macroToString.assign("NAME");
							   informationToKnow = &(input->platforms[index].clPlatformName);
							   break;
		case CL_PLATFORM_VENDOR: macroToString.assign("VENDOR");
								 informationToKnow = &(input->platforms[index].clVendor);
								 break;
		case CL_PLATFORM_VERSION: macroToString.assign("VERSION");
								  informationToKnow = &(input->platforms[index].clVersion);
								  break;
		case CL_PLATFORM_PROFILE: macroToString.assign("PROFILE");
			 					  informationToKnow = &(input->platforms[index].fullSupportOrNot);
			 					  break;
		case CL_PLATFORM_EXTENSIONS: macroToString.assign("EXTENSIONS");
									 informationToKnow = &(input->platforms[index].clPlatformExtensions);
									 break;
		default: return -1;
	}//Determine the error message and information to return using pointer, by macro given from the function calls this function


	errNum = clGetPlatformInfo(input->platforms[index].platformID, informationMacro,
							   0, NULL,
							   &platformInformationSize);//Getting information's size and checking error
	if(errNum != CL_SUCCESS)
	{
		cerr << "Error happened at getting " << index << "'th platform's CL_PLATFORM_" << macroToString << "'s size" << endl;
		return -1;
	}//Error message


	*informationToKnow = (char*) malloc(sizeof(char) * platformInformationSize);

	errNum = clGetPlatformInfo(input->platforms[index].platformID, informationMacro,
							   platformInformationSize, *informationToKnow,
							   NULL);//Getting information and checking error
	if(errNum != CL_SUCCESS)
	{
		cerr << "Error happened at getting " << index << "'th platform's CL_PLATFORM_" << macroToString << endl;
		return -1;
	}//Error message


	return CL_SUCCESS;
}





cl_int jjOpenCLPlatformInitialize(JJ_CL_PLATFORMS* input,
								  bool displayInformations)
{
	/*
	 * This function get platform's number and platform array and return it to structure first argument pointer points to.
	 * Second argument decides either or not to display platforms' information to terminal (or console).
	 */
	cl_int errNum;//DONT give attention to this variable. This variable store size to pass to clGetPlatformInfo.
	cl_platform_id * platformIds;//This pointer store array of platform id.


	errNum = clGetPlatformIDs(0, NULL,
							  &(input -> platformsNum));//Get the number of platforms
	platformIds = (cl_platform_id*)malloc(sizeof(cl_platform_id) * input -> platformsNum);//Allocate memory space to store platform IDs' array

	errNum = clGetPlatformIDs(input -> platformsNum, platformIds,
							  NULL);//Store platform ID's array
	input->platforms = (JJ_CL_PLAT_INFO*)malloc(sizeof(JJ_CL_PLAT_INFO) * input -> platformsNum);//Allocate memory space to store array of each platform's information structure 


	if(displayInformations == true)
		cout << "Number of all platform: " << input->platformsNum << "\n\n" << endl;//If user of this function wants to display information about platform, show it. In this line, it is the number of platforms


	for(int i = 0; i < input -> platformsNum; ++i)
	{
		input->platforms[i].platformID = platformIds[i];//Pass platform ID to array's each element

		if(CL_SUCCESS != jjOpenCLPlatformAsk(CL_PLATFORM_NAME, input, i))
			return -1;
		if(CL_SUCCESS != jjOpenCLPlatformAsk(CL_PLATFORM_VENDOR, input, i))
			return -1;
		if(CL_SUCCESS != jjOpenCLPlatformAsk(CL_PLATFORM_VERSION, input, i))
			return -1;
		if(CL_SUCCESS != jjOpenCLPlatformAsk(CL_PLATFORM_PROFILE, input, i))
			return -1;
		if(CL_SUCCESS != jjOpenCLPlatformAsk(CL_PLATFORM_EXTENSIONS, input, i))
			return -1;
		//Pass each platform element's properties
		
		if(displayInformations == true){
			cout << "------------------------------------------------------------\n" << i+1 << "th device's information:\n" << endl;
			cout << "Kind of OpenCL Platform: " << input -> platforms[i].clPlatformName << endl;
			cout << "Vendor of this OpenCL platform: " << input -> platforms[i].clVendor << endl;
			cout << "OpenCL version: " << input -> platforms[i].clVersion << endl;
			cout << "Embeded profile which support part of OpenCL standard or full profile: " << input -> platforms[i].fullSupportOrNot << endl;
			cout << "Extension which this OpenCL platform supports: " << input -> platforms[i].clPlatformExtensions << endl;
			cout << "\n" << endl;
			//If user of this function wants to display information about platform, show it. Int this if statement, it is properties of each platform
		}
	}


	return CL_SUCCESS;
}