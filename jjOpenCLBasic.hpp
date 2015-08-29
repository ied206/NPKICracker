
#include <fstream>
#include <sstream>
#ifndef JJ_OPENCL_PLATFORM_INITIALIZE
	#include "jjOpenCLPlatformInitialize.hpp"
	#define JJ_OPENCL_PLATFORM_INITIALIZE
#endif

using namespace std;
cl_context createContext();
cl_command_queue createCommandqueue(cl_context context, cl_device_id* device);
cl_program CreateProgram(cl_context context, cl_device_id device, const char* filename);
cl_kernel CreateKernel(cl_program program, const char* kernel_name);