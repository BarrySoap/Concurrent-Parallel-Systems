#include <thread>
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <functional>

using namespace std;
using namespace std::chrono;
using namespace std::this_thread;

constexpr size_t num_threads = 100;		// constexpr = The constexpr specifier declares that it is possible to evaluate the value of the function or variable at compile time

/* This is the function called by the thread in example 1 */
void hello_world()
{
	cout << "Hello from thread " << this_thread::get_id() << endl;
}

void task_one()
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

void task_two()
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

void task(size_t n, int val)
{
	cout << "Thread: " << n << " Random Value: " << val << endl;
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
	for (size_t i = 0; i < num_threads; ++i) {
		threads.push_back(thread(task, i, e()));		// Create 100 threads in a vector
	}

	for (auto &t : threads) {			// Use ranged for loop to join the threads
		t.join();
	}
		
	return 0;
	// ************************* //

	/* Example 4 - Lambda Expressions */
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
}