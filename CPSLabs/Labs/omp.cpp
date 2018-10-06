#include <iostream>
#include <thread>
#include <vector>
#include <random>
#include <chrono>
#include <fstream>
#include <omp.h>

using namespace std;
using namespace chrono;

constexpr int THREADS = 10;			// Number of threads to run

void hello()
{
	auto my_rank = omp_get_thread_num();			// Get the thread number
	auto thread_count = omp_get_num_threads();		// Get the number of threads in operation
	cout << "Hello from thread " << my_rank << " of " << thread_count << endl;		// Display a message
}

vector<unsigned int> generate_values(size_t size)	// Generates a vector of random values
{
	random_device r;								// Create random engine
	default_random_engine e(r());
	vector<unsigned int> data;						// Generate random numbers
	for (size_t i = 0; i < size; ++i) {
		data.push_back(e());
	}
	return data;
}

void bubble_sort(vector<unsigned int>& values)
{
	for (size_t count = values.size(); count >= 2; --count) {
		for (size_t i = 0; i < count - 1; ++i) {
			if (values[i] > values[i + 1]) {
				auto tmp = values[i];
				values[i] = values[i + 1];
				values[i + 1] = tmp;
			}
		}
	}
}

void parallel_sort(vector<unsigned int>& values)
{
	auto num_threads = thread::hardware_concurrency();			// Get the number of threads
	auto n = values.size();										// Get the number of elements in the vector
	int i, tmp, phase;											// Declare the variables used in the loop
#pragma omp parallel num_threads(num_threads) default(none) shared(values, n) private(i, tmp, phase)		// Declare parallel section
	for (phase = 0; phase < n; ++phase) {
		if (phase % 2 == 0) {									// Determine which phase of the sort we are in
#pragma omp for
			for (i = 1; i < n; i += 2) {						// Parallel for loop.  Each thread jumps forward 2 so no conflict
				if (values[i - 1] > values[i]) {				// Check if we should swap values
					tmp = values[i - 1];						// Swap values
					values[i - 1] = values[i];
					values[i] = tmp;
				}
			}
		}
		else {
#pragma omp for
			for (i = 1; i < n; i += 2) {						// Parallel for loop.  Each thread jumps forward 2 so no conflict
				if (values[i] > values[i + 1]) {				// Check is we should swap values
					tmp = values[i + 1];						// Swap values
					values[i + 1] = values[i];
					values[i] = tmp;
				}
			}
		}
	}
}

int main(int argc, char **argv)
{
	/* Example 1 - Hello World Threading /
	// Run hello THREADS times
#pragma omp parallel num_threads(THREADS)
	hello();

	return 0;
	/*************************************/

	/* Example 2 - Montecarlo Pi /
	auto num_threads = thread::hardware_concurrency();		// Get number of supported threads

	const int n = static_cast<int>(pow(2, 30));				// Number of iteration values to perform
	double factor = 0.0;									// Factor value
	double pi = 0.0;										// Calculated pi

#pragma omp parallel for num_threads(num_threads) reduction(+:pi) private(factor)		// Parallelised for loop that does the work
	for (int k = 0; k < n; ++k) {
		if (k % 2 == 0) {									// Determine sign of factor
			factor = 1.0;
		}
		else {
			factor = -1.0;
		}
		pi += factor / (2.0 * k + 1);						// Add this iteration value to pi sum
	}

	pi *= 4.0;												// Get the final value of pi

	cout.precision(numeric_limits<double>::digits10);		// Show more precision of pi
	cout << "pi = " << pi << endl;

	return 0;
	/*************************************/

	/* Example 3 - Bubble Sorting */
	ofstream results("bubble.csv", ofstream::out);			// Create results file
	for (size_t size = 8; size <= 16; ++size) {				// Gather results for 2^8 to 2^16 results
		results << pow(2, size) << ", ";					// Output data size
		for (size_t i = 0; i < 100; ++i) {					// Gather 100 results
			cout << "Generating " << i << " for " << pow(2, size) << " values" << endl;		// Generate vector of random values
			auto data = generate_values(static_cast<size_t>(pow(2, size)));
			cout << "Sorting" << endl;						// Sort the vector
			auto start = system_clock::now();
			parallel_sort(data);
			auto end = system_clock::now();
			auto total = duration_cast<milliseconds>(end - start).count();
			results << total << ",";						// Output time
		}
		results << endl;
	}
	results.close();

	return 0;
	/*************************************/
}