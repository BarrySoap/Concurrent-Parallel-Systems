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

void trap(function<double(double)> f, double start, double end, size_t iterations, shared_ptr<double> p)
{
	/*	f			the function we are using to generate the curve.
		start		the starting value we will place in the function.
		end			the end value we will place in the function.
		iterations	the number of iterations(or trapezoids) we will generate.
		p			a shared piece of data to store the result.					*/

	auto my_rank = omp_get_thread_num();						// Get thread number
	auto thread_count = omp_get_num_threads();					// Get number of threads
	auto slice_size = (end - start) / iterations;				// Calculation iteration slice size
	auto iterations_thread = iterations / thread_count;			// Calculate number of iterations per thread
	auto local_start = start + ((my_rank * iterations_thread) * slice_size);		// Calculate this thread's start point
	auto local_end = local_start + iterations_thread * slice_size;					// Calculate this thread's end point
	auto my_result = (f(local_start) + f(local_end)) / 2.0;		// Calculate initial result

	double x;													// Declare x before the loop - stops it being allocated and destroyed each iteration
	for (size_t i = 0; i <= iterations_thread - 1; ++i) {		// Sum each iteration
		x = local_start + i * slice_size;						// Calculate next slice to calculate x
		my_result += f(x);										// Add to current result
	}
	my_result *= slice_size;									// Multiply the result by the slice size

#pragma omp critical											// Critical section - add to the shared data// Critical section - add to the shared data
	*p += my_result;
}

double f(unsigned int i)							// Let's create a function that relies on i to determine the amount of work
{
	auto start = i * (i + 1) / 2;					// Calculate start and end values
	auto end = start + i;
	auto result = 0.0;								// Declare return value

	for (auto j = start; j <= end; ++j) {			// Loop for number of iterations, calculating sin
		result += sin(j);
	}

	return result;									// Return result
}

int main(int argc, char **argv)
{
	/* Example 1 - Hello World Threading */
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

	/* Example 3 - Bubble Sorting/Parallel Sorting */
	ofstream results("bubble.csv", ofstream::out);			// Create results file
	//ofstream results("parallel.csv", ofstream::out);
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

	/* Example 4 - Trapezoidal Function (Area Under a Curve) /
	auto result = make_shared<double>(0.0);					// Declare shared result
	auto start = 0.0;										// Define start and end values
	auto end = 3.14159265359;								// pi
	auto trapezoids = static_cast<size_t>(pow(2, 24));		// Defined number of trapezoids to generation
	auto thread_count = thread::hardware_concurrency();		// Get number of threads

	auto f = [](double x) { return cos(x); };				// Create function to calculate integral. Use cos

#pragma omp parallel num_threads(thread_count)				// Run trap in parallel
	trap(f, start, end, trapezoids, result);

	cout << "Using " << trapezoids << " trapezoids. ";		// Output result
	cout << "Estimated integral of function " << start << " to " << end << " = " << *result << endl;

	return 0;
	/*************************************/

	/* Example 5 - Task Scheduling /
	auto thread_count = thread::hardware_concurrency();		// Get number of hardware threads
	auto n = static_cast<size_t>(pow(2, 14));				// Define number of iterations to calculate
	auto sum = 0.0;											// Declare sum value

	auto start = system_clock::now();						// Get start time
#pragma omp parallel for num_threads(thread_count) reduction(+:sum) schedule(static, 1)
	for (auto i = 0; i <= n; ++i) {
		sum += f(i);
	}
	auto end = system_clock::now();							// Get end time
	auto total = duration_cast<milliseconds>(end - start).count();		// Calculate and output total time
	cout << "Total time: " << total << "ms" << endl;

	return 0;
	/*************************************/
}