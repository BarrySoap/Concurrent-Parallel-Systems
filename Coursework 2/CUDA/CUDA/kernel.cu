#include <iostream>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <vector>

using namespace std;

//constexpr size_t ELEMENTS = 2048;

__global__ void vecadd(const int *A, const int *B, int *C)
{
	// Get block index
	unsigned int block_idx = blockIdx.x;
	// Get thread index
	unsigned int thread_idx = threadIdx.x;
	// Get the number of threads per block
	unsigned int block_dim = blockDim.x;
	// Get the thread's unique ID - (block_idx * block_dim) + thread_idx;
	unsigned int idx = (block_idx * block_dim) + thread_idx;
	// Add corresponding locations of A and B and store in C
	C[idx] = A[idx] + B[idx];
}

int main(int argc, char **argv)
{
	/* Example 1: GPU Specifications */
	// Get number of devices on system
	int deviceCount;
	cudaGetDeviceCount(&deviceCount);

	cout << "Number of devices: " << deviceCount << endl;
	for (int i = 0; i < deviceCount; ++i)
	{
		// Get properties for device
		cudaDeviceProp deviceProp;
		cudaGetDeviceProperties(&deviceProp, i);

		cout << "Device " << i << endl;
		cout << "Name " << deviceProp.name << endl;
		cout << "Revision " << deviceProp.major << "." << deviceProp.minor << endl;
		cout << "Memory " << deviceProp.totalGlobalMem / 1024 / 1024 << "MB" << endl;
		cout << "Warp Size " << deviceProp.warpSize << endl;
		cout << "Clock " << deviceProp.clockRate << endl;
		cout << "Multiprocessors " << deviceProp.multiProcessorCount << endl;
	}
	return 0;
	/********************************/

	/* Example 2: Vector Addition /
	// Create host memory
	auto data_size = sizeof(int) * ELEMENTS;
	vector<int> A(ELEMENTS);    // Input aray
	vector<int> B(ELEMENTS);    // Input array
	vector<int> C(ELEMENTS);    // Output array

	// Initialise input data
	for (unsigned int i = 0; i < ELEMENTS; ++i)
		A[i] = B[i] = i;

	// Declare buffers
	int *buffer_A, *buffer_B, *buffer_C;

	// Initialise buffers
	cudaMalloc((void**)&buffer_A, data_size);
	cudaMalloc((void**)&buffer_B, data_size);
	cudaMalloc((void**)&buffer_C, data_size);

	// Write host data to device
	cudaMemcpy(buffer_A, &A[0], data_size, cudaMemcpyHostToDevice);
	cudaMemcpy(buffer_B, &B[0], data_size, cudaMemcpyHostToDevice);

	// Run kernel with one thread for each element
	// First value is number of blocks, second is threads per block.  Max 1024 threads per block
	vecadd << <ELEMENTS / 1024, 1024 >> > (buffer_A, buffer_B, buffer_C);

	// Wait for kernel to complete
	cudaDeviceSynchronize();

	// Read output buffer back to the host
	cudaMemcpy(&C[0], buffer_C, data_size, cudaMemcpyDeviceToHost);

	// Clean up resources
	cudaFree(buffer_A);
	cudaFree(buffer_B);
	cudaFree(buffer_C);

	// Test that the results are correct
	for (int i = 0; i < 2048; ++i)
		if (C[i] != i + i)
			cout << "Error: " << i << endl;

	cout << "Finished" << endl;

	return 0;
	/********************************/
}