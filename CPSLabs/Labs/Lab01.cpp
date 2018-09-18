#include <thread>
#include <iostream>
#include <vector>
#include <random>
#include <chrono>

using namespace std;
using namespace std::chrono;
using namespace std::this_thread;

constexpr size_t num_threads = 100;		// constexpr = The constexpr specifier declares that it is possible to evaluate the value of the function or variable at compile time

/* This is the function called by the thread */
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

	/* Example 3 - Giving Threads Parameters*/
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
}