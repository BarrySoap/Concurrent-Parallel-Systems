#include <thread>
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <functional>
#include <fstream>
#include <cstddef>
#include <future>

using namespace std;
using namespace std::chrono;
using namespace std::this_thread;

constexpr std::size_t num_threads = 100;		// constexpr = The constexpr specifier declares that it is possible to evaluate the value of the function or variable at compile time

/* This is the function called by the thread in example 1 */
void hello_world()
{
	cout << "Hello from thread " << this_thread::get_id() << endl;
}

void task_one()	// Example 2
{
	cout << "Task one starting" << endl;
	cout << "Task one sleeping for 3 seconds" << endl;
	sleep_for(seconds(3));
	cout << "Task one awake again" << endl;
	cout << "Task one sleeping for 5 seconds" << endl;
	sleep_for(milliseconds(5000));
	cout << "Task one awake again" << endl;
	cout << "Task one ending" << endl;
}

void task_two()	// Example 2
{
	cout << "Task two starting" << endl;
	cout << "Task two sleeping for 2 seconds" << endl;
	sleep_for(microseconds(2000000));
	cout << "Task two awake again" << endl;
	cout << "Task two sleeping for 10 seconds" << endl;
	sleep_for(seconds(10));
	cout << "Task two awake again" << endl;
	cout << "Task two ending" << endl;
}

void task(std::size_t n, int val)
{
	cout << "Thread: " << n << " Random Value: " << val << endl;
}

void work()
{
	int n = 0;
	for (int i = 0; i < 1000000; ++i) {
		++n;								// Do some spinning - no actual processing but will make the CPU work
	}
}

void monte_carlo_pi(std::size_t iterations)
{
	std::random_device r;										// Seed with real random number if available
	default_random_engine e(r());								// Create random number generator
	uniform_real_distribution<double> distribution(0.0, 1.0);	// Create a distribution - we want doubles between 0.0 and 1.0

	unsigned int in_circle = 0;									// Keep track of number of points in circle
		
	for (std::size_t i = 0; i < iterations; ++i) {					// Iterate	
		auto x = distribution(e);								// Generate random point(s)
		auto y = distribution(e);
		
		auto length = sqrt((x * x) + (y * y));					// Get length of vector defined - use Pythagoras
		
		if (length <= 1.0) {									// Check if in circle
			++in_circle;
		}
	}
	auto pi = (4.0 * in_circle) / static_cast<double>(iterations);		// Calculate pi (not returned)
}

void new_monte_carlo_pi(size_t iterations, promise<double> pi)
{
	random_device r;											// Seed with real random number if available
	default_random_engine e(r());								// Create random number generator
	uniform_real_distribution<double> distribution(0.0, 1.0);	// Create a distribution - we want doubles between 0.0 and 1.0

	unsigned int in_circle = 0;									// Keep track of number of points in circle
	for (size_t i = 0; i < iterations; ++i) {					// Iterate
		auto x = distribution(e);								// Generate random point(s)
		auto y = distribution(e);
		auto length = sqrt((x * x) + (y * y));					// Get length of vector defined - use Pythagoras
		if (length <= 1.0) {									// Check if in circle
			++in_circle;
		}
	}
	pi.set_value((4.0 * in_circle) / static_cast<double>(iterations));	// Calculate pi
}


int main(int argc, char **argv)
{
	/* Example 1 - Thread Creation / 
	thread t(hello_world);		// Create a new thread
	t.join();					// Wait for thread to finish (join it)
	return 0;					// Return 0 (OK)
	// ************************* //

	/* Example 2 - Thread Sleeping /
	cout << "Starting task one" << endl;
	thread t1(task_one);
	cout << "Starting task two" << endl;
	thread t2(task_two);
	cout << "Joining task one" << endl;
	t1.join();
	cout << "Task one joined" << endl;
	cout << "Joining task two" << endl;
	t2.join();
	cout << "Task two joined" << endl;
	cout << "Exiting" << endl;
	return 0;
	// ************************* //

	/* Example 3 - Giving Threads Parameters /
	std::random_device r;				// Create a random, seed with real random number if available
	default_random_engine e(r());		// Create random number generator

	vector<thread> threads;
	for (std::size_t i = 0; i < num_threads; ++i) {
		threads.push_back(thread(task, i, e()));		// Create 100 threads in a vector
	}

	for (auto &t : threads) {			// Use ranged for loop to join the threads
		t.join();
	}
		
	return 0;
	// ************************* //

	/* Example 4 - Lambda Expressions /
	auto add = [](int x, int y) { return x + y; };		// Create lambda expression
	auto x = add(10, 12);								// Call the defined function
	cout << "10 + 12 = " << x << endl;					// Display answer - should be 22

	function<int(int, int)> add_function = [](int x, int y) { return x + y; };		// Create function object with same lambda expression
	x = add_function(20, 12);							// Call the function object
	cout << "20 + 12 = " << x << endl;					// Display the answer - should be 32

	int a = 5;
	int b = 10;
	
	auto add_fixed = [a, b] { return a + b; };			// Define the values passed to the function
	x = add_fixed();									// Call the function
	cout << "5 + 10 = " << x << endl;					// Display the answer - should be 15

	a = 20;												// Change values of a and b
	b = 30;
	
	x = add_fixed();									// Call the fixed function again
	cout << "20 + 30 = " << x << endl;					// Display the answer - will come out as 15

	auto add_reference = [&a, &b] { return a + b; };	// Define the values passed to the function , but pass as reference
	x = add_reference();								// Call the function
	cout << "20 + 30 = " << x << endl;					// Display the answer - should be 50

	a = 30;												// Change the values of a and b
	b = 5;
	
	x = add_reference();								// Call the reference based function again
	cout << "30 + 5 = " << x << endl;					// Display the answer - should be 35

	return 0;
	// ************************* //

	/* Example 5 - Thread Creation using Lambda Expressions /
	thread t([] { cout << "Hello from lambda thread!" << endl; });		// Create a thread using a lambda expression
	t.join();			// Join thread
	
	return 0;
	// ************************* //

	/* Example 6 - CSV Serialisation */
	ofstream data("data.csv", ofstream::out);		// Create a new file
	for (int i = 0; i < 100; ++i) {					// We’re going to gather 100 readings, so create a thread and join it 100 times
		auto start = system_clock::now();			// Get start time
		thread t(work);								// Start thread
		t.join();									
		auto end = system_clock::now();				// Get end time
		auto total = end - start;					// Calculate the duration
		data << total.count() << endl;				// Write to file
	}
	
	data.close();									// 100 iterations complete.
	return 0;
	// ************************* //

	/* Example 7 - Monte Carlo Pi Distributions /
	ofstream data("montecarlo.csv", ofstream::out);					// Create data file

	for (std::size_t num_threads = 0; num_threads <= 6; ++num_threads) {
		auto total_threads = static_cast<unsigned int>(pow(2.0, num_threads));	// Calculate number of threads
		cout << "Number of threads = " << total_threads << endl;	// Output number of threads
		data << "num_threads_" << total_threads;					// Serialise number of threads to the file
		
		for (std::size_t iters = 0; iters < 100; ++iters) {			// Now execute 100 iterations
			auto start = system_clock::now();						// Get the start time
			vector<thread> threads;									// We need to create total_threads threads
			for (std::size_t n = 0; n < total_threads; ++n) {
				threads.push_back(thread(monte_carlo_pi, static_cast<unsigned int>(pow(2.0, 24.0 - num_threads))));		// Working in base 2 to make things a bit easier
			}
			for (auto &t : threads) {
				t.join();											// Join the threads (wait for them to finish)
			}
			auto end = system_clock::now();							// Get the end time
			auto total = end - start;								// Get the total time
			data << ", " << duration_cast<milliseconds>(total).count();		// Convert to milliseconds and output to file
		}
		data << endl;
	}
	data.close();				// Close the file
	return 0;
	// ************************* //

	/* EXTRA Example - Monte Carlo Pi Distributions using Futures/Promises /
	ofstream data("montecarlo.csv", ofstream::out);					// Create data file

	for (std::size_t num_threads = 0; num_threads <= 6; ++num_threads) {
		auto total_threads = static_cast<unsigned int>(pow(2.0, num_threads));
		cout << "Number of threads = " << total_threads << endl;	// Write number of threads
		data << "num_threads_" << total_threads;					// Write number of threads to the file

		for (size_t iters = 0; iters < 100; ++iters) {				// Now execute 100 iterations
			auto start = system_clock::now();						// Get the start time
			vector<future<double>> futures;							// We need to create total_threads threads
			vector<thread> threads;
			for (size_t n = 0; n < total_threads; ++n) {
				promise<double> pi;									// Create promise
				futures.push_back(pi.get_future());					// Store future
				threads.push_back(thread(new_monte_carlo_pi, static_cast<unsigned int>(pow(2.0, 24.0 - num_threads)), move(pi)));	// Working in base 2 to make things a bit easier
			}
			double pi = 0.0;										// Calculate pi from futures
			for (auto &f : futures) {
				pi += f.get();
			}
			pi /= static_cast<double>(total_threads);
			for (auto &t : threads) {								// Join the threads (wait for them to finish)
				t.join();
			}
			auto end = system_clock::now();							// Get the end time
			auto total = end - start;								// Get the total time
			cout << "pi = " << pi << " in " << duration_cast<milliseconds>(total).count() << " ms" << endl;		// Print result
			data << ", " << duration_cast<milliseconds>(total).count();											// Convert to milliseconds and output to file
		}
		data << endl;
	}
	data.close();				// Close the file
	return 0;
	// ************************* //
	*/
}