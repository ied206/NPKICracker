#include "jjOpenCLBasic.hpp"
cl_context createContext()
{
	cl_int errNum;
	JJ_CL_PLATFORMS platformsInformations;
	cl_context context = NULL;

	errNum = jjOpenCLPlatformInitialize(&platformsInformations, true);
	cl_context_properties contextProperties[] = {
		CL_CONTEXT_PLATFORM,
		(cl_context_properties)platformsInformations.platforms[0].platformID,
		0
	};
	context = clCreateContextFromType(contextProperties, CL_DEVICE_TYPE_GPU, NULL, NULL, &errNum);
	if(errNum != CL_SUCCESS)
	{
		cout << "This system has no OpenCL available GPU device" << endl;
		context = clCreateContextFromType(contextProperties, CL_DEVICE_TYPE_CPU, NULL, NULL, &errNum);

		if(errNum != CL_SUCCESS)
		{
			cout << "This system has no OpenCL available CPU device" << endl;
			cout << "This system isn't available to run this application" << endl;
			exit(1);
		}
		else{
			cout << "Application find OpenCL CPU device. Runnig on it...\n\n" << endl;
		}
	}
	else{
		cout << "Application find OpenCL GPU device. Running on it...\n\n" << endl;
	}
	return context;
}
cl_command_queue createCommandqueue(cl_context context, cl_device_id* device)
{
	cl_int errNum;
	cl_device_id* devices;
	cl_command_queue commandQueue = NULL;
	size_t deviceBufferSize = -1;

	errNum = clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &deviceBufferSize);
	if(errNum != CL_SUCCESS)
	{
		cerr << "Failed to call clGetContextInfo(..., CL_CONTEXT_DEVICES, ...)" << endl;
		exit(1);
	}
	if(deviceBufferSize <= 0)
	{
		cerr << "No available device" << endl;
		exit(1);
	}
	devices = new cl_device_id[deviceBufferSize / sizeof(cl_device_id)];
	errNum = clGetContextInfo(context, CL_CONTEXT_DEVICES, deviceBufferSize, devices, NULL);
	if(errNum != CL_SUCCESS)
	{
		cerr << "Failed to get device id" << endl;
		exit(1);
	}
	commandQueue = clCreateCommandQueue(context, devices[0], 0, NULL);

	if(commandQueue == NULL)
	{
		cerr << "Failed to create command queue for device 0" << endl;
		exit(1);
	}
	*device = devices[0];
	delete [] devices;
	return commandQueue;
}
cl_program CreateProgram(cl_context context, cl_device_id device, const char* filename)
{
	cl_int errNum;
	cl_program program;
	ifstream kernelFile(filename, ios::in);
	ostringstream oss;
	if(!kernelFile.is_open())
	{
		cerr << "kernel file " << filename << " isn't available to open." << endl;
		exit(1);
	}
	oss << kernelFile.rdbuf();

	string srcStdStr = oss.str();
	const char* srcStr = srcStdStr.c_str();
	program = clCreateProgramWithSource(context, 1, (const char**)&srcStr, NULL, &errNum);
	if((errNum != CL_SUCCESS) || program == NULL)
	{
		cerr << "Failed to create OpenCL Program" << endl;
		exit(1);
	}

	errNum = clBuildProgram(program, 1, &device, NULL, NULL, NULL);

	if(errNum != CL_SUCCESS)
	{
		char *buildLog;
		size_t errLength;
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, (size_t)NULL, NULL, &errLength);
		buildLog = (char*)malloc(sizeof(char) * (errLength + 1));
		errNum = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, sizeof(char)*errLength, buildLog, NULL);
		cerr << "OpenCL kernel build Error! Errors are here:\nclGetProgramBuildInfo return errorcode: " << errNum << " is";
		switch(errNum){
			case CL_INVALID_DEVICE: cerr << "CL_INVALID_DEVICE. Device is not valid with this program object." << endl; break;
			case CL_INVALID_VALUE: cerr << "CL_INVALID_VALUE. Parameter to clBuildProgram was wrong." << endl
										<< "If pfn_notify is NULL and user_data is not NULL, this can happen." << endl
										<< "device_list is NULL and num_devices is greater than 0, or device_list is not NULL and num_devices is zero, this can also happen" << endl;break;
			case CL_INVALID_PROGRAM: cerr << "CL_INVALID_PROGRAM. Program object is not valid" << endl; break;
			case CL_INVALID_BINARY: cerr << "CL_INVALID_BINARY. Given device_list is not matching binary given to clCreateProgramWithBinary, this can happen" << endl; break;
			case CL_INVALID_BUILD_OPTIONS: cerr << "CL_INVALID_BUILD_OPTIONS. Build option string given to clBuildProgram's options argument is wrong." << endl; break;
			case CL_INVALID_OPERATION: cerr << "CL_INVALID_OPERATION. Previous clBuildProgram call has not ended or kernel object is attatching to program object." << endl; break;
			case CL_COMPILER_NOT_AVAILABLE: cerr <<"CL_COMPILER_NOT_AVAILABLE" << endl; break;
			case CL_OUT_OF_RESOURCES: cerr << "CL_OUT_OF_RESOURCES" << endl; break;
			case CL_OUT_OF_HOST_MEMORY: cerr << "CL_OUT_OF_HOST_MEMORY" << endl; break;
			default: break;
		}
		cerr << endl;
		cerr << buildLog << endl;
		free(buildLog);
		clReleaseProgram(program);
		exit(1);
	}
	cout << "OpenCL program successfully built" << endl;
	return program;
}
cl_kernel CreateKernel(cl_program program, const char* kernel_name)
{
	cl_int errNum;
	cl_kernel kernel = NULL;

	kernel = clCreateKernel(program, kernel_name, &errNum);
	if(errNum != CL_SUCCESS){
		cerr << "Error code is this: " << errNum << endl;
		switch(errNum){
			case CL_INVALID_PROGRAM: cerr << "CL_INVALID_PROGRAM. Program object given first argument to CreateKernel is wrong." << endl; break;
			case CL_INVALID_PROGRAM_EXECUTABLE: cerr << "CL_INVALID_PROGRAM_EXECUTABLE." << endl; break;
			case CL_INVALID_KERNEL: cerr << "CL_INVALID_KERNEL_NAME. Kernel name given second argument CreateKernel is wrong." << endl; break;
			case CL_INVALID_KERNEL_DEFINITION: cerr << "CL_INVALID_KERNEL_DEFINITION. Kernel source code is not suitable for this OpenCL device" << endl; break;
			case CL_INVALID_VALUE: cerr << "CL_INVALID_VALUE. Are you sure you have not given NULL as second argument to CreateKernel?" << endl; break;
			case CL_OUT_OF_HOST_MEMORY: cerr << "CL_OUT_OF_HOST_MEMORY. There is no suitable memory for kernel memory allocation" << endl; break;
			default: break;
		}
		exit(1);
	}
	cout << "OpenCL kernel successfully built" << endl;
	return kernel;
}