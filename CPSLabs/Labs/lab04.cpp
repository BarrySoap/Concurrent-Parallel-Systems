#include <iostream>
#include <immintrin.h>
#include <cassert>
#include <chrono>
#include <random>

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

void generate_data(v4 *data, size_t n)		// Randomly generate vector values
{
	random_device r;						// Create random engine
	default_random_engine e(r());

	for (size_t i = 0; i < n; ++i) {		// Fill data
		for (size_t j = 0; j < 4; ++j) {
			data[i].a[j] = e();
		}
	}
}

void normalise_vectors(v4 *data, v4 *result, size_t n)			// Normalises the vector
{
	for (size_t i = 0; i < n; ++i) {							// Normalise the vectors
		result[i].v = _mm_mul_ps(data[i].v, data[i].v);			// Square each component - simply multiply the vectors by themselves
		result[i].v = _mm_hadd_ps(result[i].v, result[i].v);	// Calculate sum of the components.
		result[i].v = _mm_hadd_ps(result[i].v, result[i].v);	// See notes to explain hadd.

		result[i].v = _mm_rsqrt_ps(result[i].v);				// Calculate reciprocal square root of the values
																// That is 1.0f / sqrt(value) - or the length of the vector

		result[i].v = _mm_mul_ps(data[i].v, result[i].v);		// Multiply result by the original data
																// As we have the reciprocal it is the same as dividing each component by the length
	}															// All vectors now normalised
}

void check_results(v4 *data, v4 *result)						// Check the first 100 results
{
	for (size_t i = 0; i < 100; ++i) {							// Check first 100 values
		float l = 0.0f;											// Calculate the length of the vector
		for (size_t j = 0; j < 4; ++j) {
			l += powf(data[i].a[j], 2.0f);						// Square each component and add to l
		}
		l = sqrtf(l);											// Get sqrt of the length
		for (size_t j = 0; j < 4; ++j) {
			cout << data[i].a[j] / l << " : " << result[i].a[j] << endl;	// Now check that the individual results
		}
	}
}

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

	/* Example 2 - SIMD Operations /
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

	/* Example 3 - Vector Normalisation */
	v4 *data = (v4*)_aligned_malloc(sizeof(v4) * 1000000, 16);
	v4 *result = (v4*)_aligned_malloc(sizeof(v4) * 1000000, 16);
	generate_data(data, 1000000);
	normalise_vectors(data, result, 1000000);
	check_results(data, result);
	_aligned_free(data);
	_aligned_free(result);
	return 0;
	/*************************************/
}