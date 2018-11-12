#define __CL_ENABLE_EXCEPTIONS

#include <iostream>
#include <vector>
#include <CL/cl.hpp>
#include <fstream>
#include <string>
#include <array>

using namespace std;
using namespace cl;

constexpr int ELEMENTS = 2048;
constexpr std::size_t DATA_SIZE = sizeof(int) * ELEMENTS;

int main(int argc, char **argv)
{
	/* Exercise 1: Print onboard GPU /
	try
	{
		// Get the platforms
		vector<Platform> platforms;
		Platform::get(&platforms);

		// Assume only one platform.  Get GPU devices.
		vector<Device> devices;
		platforms[0].getDevices(CL_DEVICE_TYPE_GPU, &devices);

		// Just to test, print out device 0 name
		cout << devices[0].getInfo<CL_DEVICE_NAME>() << endl;

		// Create a context with these devices
		Context context(devices);

		// Create a command queue for device 0
		CommandQueue queue(context, devices[0]);
	}
	catch (Error error)
	{
		cout << error.what() << "(" << error.err() << ")" << endl;
	}
	return 0;
	/***********************************/

	/* Exercise 2: Print OpenCL Info /
	try
	{
		// Get the platforms
		vector<Platform> platforms;
		Platform::get(&platforms);

		// Iterate through each platform
		for (auto &p : platforms)
		{
			// Print out platform name
			cout << "********************" << endl;
			cout << p.getInfo<CL_PLATFORM_NAME>() << endl;

			// Get all devices for the platform
			vector<Device> devices;
			p.getDevices(CL_DEVICE_TYPE_ALL, &devices);

			// Iterate through all the devices
			for (auto &d : devices)
			{
				cout << endl;
				cout << d.getInfo<CL_DEVICE_NAME>() << endl;
				cout << "Vendor: " << d.getInfo<CL_DEVICE_VENDOR>() << endl;
				cout << "Max Compute: " << d.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>() << endl;
				cout << "Max Memory: " << d.getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>() / (1024 * 1024) << "MB" << endl;
				cout << "Clock Freq: " << d.getInfo<CL_DEVICE_MAX_CLOCK_FREQUENCY>() << "MHz" << endl;
				cout << "Available: " << (d.getInfo<CL_DEVICE_AVAILABLE>() ? "True" : "False") << endl;
			}

			cout << endl;
			cout << "********************" << endl;
		}
	}
	catch (Error error)
	{
		cout << error.what() << "(" << error.err() << ")" << endl;
	}
	return 0;
	/***********************************/

	/* Exercise 3: Kernel Loading and Execution */
	// Initialise memory
	array<int, ELEMENTS> A;
	array<int, ELEMENTS> B;
	array<int, ELEMENTS> C;
	for (std::size_t i = 0; i < ELEMENTS; ++i)
		A[i] = B[i] = i;

	try
	{
		// Get the platforms
		vector<Platform> platforms;
		Platform::get(&platforms);

		// Assume only one platform.  Get GPU devices.
		vector<Device> devices;
		platforms[0].getDevices(CL_DEVICE_TYPE_GPU, &devices);

		// Just to test, print out device 0 name
		cout << devices[0].getInfo<CL_DEVICE_NAME>() << endl;

		// Create a context with these devices
		Context context(devices);

		// Create a command queue for device 0
		CommandQueue queue(context, devices[0]);

		// Create the buffers
		Buffer bufA(context, CL_MEM_READ_ONLY, DATA_SIZE);
		Buffer bufB(context, CL_MEM_READ_ONLY, DATA_SIZE);
		Buffer bufC(context, CL_MEM_WRITE_ONLY, DATA_SIZE);

		// Copy data to the GPU
		queue.enqueueWriteBuffer(bufA, CL_TRUE, 0, DATA_SIZE, &A);
		queue.enqueueWriteBuffer(bufB, CL_TRUE, 0, DATA_SIZE, &B);

		// Read in kernel source
		ifstream file("vec-add.cl");
		string code(istreambuf_iterator<char>(file), (istreambuf_iterator<char>()));

		// Create program
		Program::Sources source(1, make_pair(code.c_str(), code.length() + 1));
		Program program(context, source);

		// Build program for devices
		program.build(devices);

		// Create the kernel
		Kernel vecadd_kernel(program, "vecadd");

		// Set kernel arguments
		vecadd_kernel.setArg(0, bufA);
		vecadd_kernel.setArg(1, bufB);
		vecadd_kernel.setArg(2, bufC);

		// Execute kernel
		NDRange global(ELEMENTS);
		NDRange local(256);
		queue.enqueueNDRangeKernel(vecadd_kernel, NullRange, global, local);

		// Copy result back.
		queue.enqueueReadBuffer(bufC, CL_TRUE, 0, DATA_SIZE, &C);

		// Test that the results are correct
		for (int i = 0; i < 2048; ++i)
			if (C[i] != i + i)
				cout << "Error: " << i << endl;

		cout << "Finished" << endl;
	}
	catch (Error error)
	{
		cout << error.what() << "(" << error.err() << ")" << endl;
	}
	return 0;
	/***********************************/
}