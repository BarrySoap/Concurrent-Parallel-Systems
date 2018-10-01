#include <iostream>
#include <thread>
#include <omp.h>

using namespace std;

constexpr int THREADS = 10;			// Number of threads to run

void hello()
{
	auto my_rank = omp_get_thread_num();			// Get the thread number
	auto thread_count = omp_get_num_threads();		// Get the number of threads in operation
	cout << "Hello from thread " << my_rank << " of " << thread_count << endl;		// Display a message
}

int main(int argc, char **argv)
{
	/* Example 1 - Hello World Threading /
	// Run hello THREADS times
#pragma omp parallel num_threads(THREADS)
	hello();

	return 0;
	/*************************************/

	/* Example 2 - Montecarlo Pi */
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
}