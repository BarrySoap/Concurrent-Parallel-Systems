#include <iostream>
#include <immintrin.h>
#include <cassert>
#include <chrono>

using namespace std;
using namespace std::chrono;

union v4			// Union can flip between the types within.
{
	__m128 v;		// SSE 4 x float vector
	float a[4];		// scalar array of 4 floats
};

union v8
{
	__m256 v;
	float a[8];
};

int main(int argc, char **argv)
{
	/* Example 1 - Memory Alignment /
	alignas(16) v4 x;						// Declare a single 128-bit value aligned to 16 bytes (size of 128-bits)
	x.a[0] = 10.0f;							// We can treat x as a collection of four floats
	x.a[1] = 20.0f;							// Or other combinations of values for 128-bits
	x.a[2] = 30.0f;
	x.a[3] = 40.0f;
	cout << "Original values" << endl;		// We can print out individual values
	for (size_t i = 0; i < 4; ++i) {
		cout << x.a[i] << endl;
	}

	alignas(16) v4 y;						// Declared a second 128-bit value aligned to 16 bytes (size of 128-bits)
	y.a[0] = 10.0f;
	y.a[1] = 20.0f;
	y.a[2] = 30.0f;
	y.a[3] = 40.0f;
	x.v = _mm_add_ps(x.v, y.v);				// Add y to x
	cout << "New values" << endl;			// Print out individual values
	for (size_t i = 0; i < 4; ++i) {
		cout << x.a[i] << endl;
	}

	float *data = (float*)_aligned_malloc(100 * sizeof(float), 4);		// Create array of 100 floats, aligned to 4 bytes.
	cout << data[0] << endl;											// Access just like an array

	v4 *big_data = (v4*)_aligned_malloc(100 * sizeof(v4), 16);			// Create an array of 100 128-bit values aligned to 16 bytes

	cout << big_data[0].a[0] << endl;									// Access just like an array of __m128

	_aligned_free(data);												// Free the data - ALWAYS REMEMBER TO FREE YOUR MEMORY
	_aligned_free(big_data);											// We are dealing at a C level here

	return 0;
	/*************************************/

	/* Example 2 - SIMD Operations */
	{											// Test all types to ensure add is working
		alignas(16) v4 x;
		alignas(16) v4 y;
		for (size_t i = 0; i < 4; ++i) {
			x.a[i] = static_cast<float>(i);
			y.a[i] = static_cast<float>(i);
		}
		x.v = _mm_add_ps(x.v, y.v);
		for (size_t i = 0; i < 4; ++i) {
			assert(x.a[i] == static_cast<float>(2 * i));
		}
	}
	{
		alignas(32) v8 x;
		alignas(32) v8 y;
		for (size_t i = 0; i < 8; ++i) {
			x.a[i] = static_cast<float>(i);
			y.a[i] = static_cast<float>(i);
		}
		x.v = _mm256_add_ps(x.v, y.v);
		for (size_t i = 0; i < 8; ++i) {
			assert(x.a[i] == static_cast<float>(2 * i));
		}
	}

	{	//Add 1 million float values, first using floats
		float *d1 = (float*)_aligned_malloc(sizeof(float) * 1000000, 4);
		float *d2 = (float*)_aligned_malloc(sizeof(float) * 1000000, 4);
		auto start = system_clock::now();
		for (size_t count = 0; count < 100; ++count) {
			for (size_t i = 0; i < 1000000; ++i) {
				d1[i] = d1[i] + d2[i];
			}
		}
		auto total = duration_cast<nanoseconds>((system_clock::now() - start)).count() / 100;
		cout << "float time: " << total << "ns" << endl;
		_aligned_free(d1);
		_aligned_free(d2);
	}
	{	// Now using _m128
		v4 *d1 = (v4*)_aligned_malloc(sizeof(v4) * 500000, 16);
		v4 *d2 = (v4*)_aligned_malloc(sizeof(v4) * 500000, 16);
		auto start = system_clock::now();
		for (size_t count = 0; count < 100; ++count) {
			for (size_t i = 0; i < 500000; ++i)
				d1[i].v = _mm_add_ps(d1[i].v, d2[i].v);
		}
		auto total = duration_cast<nanoseconds>((system_clock::now() - start)).count() / 100;
		cout << "m128 time: " << total << "ns" << endl;
		_aligned_free(d1);
		_aligned_free(d2);
	}
	{	// Now using _m256
		v8 *d1 = (v8*)_aligned_malloc(sizeof(v8) * 250000, 32);
		v8 *d2 = (v8*)_aligned_malloc(sizeof(v8) * 250000, 32);
		auto start = system_clock::now();
		for (size_t count = 0; count < 100; ++count) {
			for (size_t i = 0; i < 250000; ++i) {
				d1[i].v = _mm256_add_ps(d1[i].v, d2[i].v);
			}
		}
		auto total = duration_cast<nanoseconds>((system_clock::now() - start)).count() / 100;
		cout << "m256 time: " << total << "ns" << endl;
		_aligned_free(d1);
		_aligned_free(d2);
	}
	return 0;
	/*************************************/
}