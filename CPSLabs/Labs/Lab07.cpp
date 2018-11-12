#define __CL_ENABLE_EXCEPTIONS

#include <iostream>
#include <vector>
#include <CL/cl.hpp>

using namespace std;
using namespace cl;

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

	/* Exercise 2: Print OpenCL Info */
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
}