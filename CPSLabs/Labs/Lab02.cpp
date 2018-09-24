#include <thread>
#include <iostream>
#include <vector>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <atomic>
#include "guarded.h"
#include "threadsafe_stack.h"

using namespace std;
using namespace chrono;
using namespace std::this_thread;

mutex mut;
atomic<int> value;
constexpr unsigned int NUM_ITERATIONS = 1000000;
constexpr unsigned int NUM_THREADS = 4;

void atomic_increment(shared_ptr<atomic<int>> value)
{
	for (unsigned int i = 0; i < 1000000; ++i) {		// Loop 1 million times, incrementing value
		(*value)++;										// Increment value
	}
}

void flag_task(unsigned int id, shared_ptr<atomic_flag> flag)
{
	for (unsigned int i = 0; i < 10; ++i) {						// Do 10 iterations
		while (flag->test_and_set());							// Test the flag is available, and grab when it is. Notice this while loops keeps spinning until flag is clear
		cout << "Thread " << id << " running " << i << endl;	// Flag is available.  Thread displays message
		this_thread::sleep_for(seconds(1));						// Sleep for 1 second
		flag->clear();											// Clear the flag
	}
}

void increment(shared_ptr<int> value)
{
	for (unsigned int i = 0; i < 1000000; ++i) {	// Loop 1 million times, incrementing value
		lock_guard<mutex> lock(mut);				// Create the lock guard - automatically acquires mutex
		*value = *value + 1;						// Increment value
		// lock guard is automatically destroyed at the end of the loop scope
		// This will release the lock
	}
}

void task(shared_ptr<guarded> g)
{
	for (unsigned int i = 0; i < NUM_ITERATIONS; ++i) {
		g->increment();								// Increment guarded object NUM_ITERATIONS times
	}
}

void task_1(condition_variable &condition)
{
	cout << "Task 1 sleeping for 3 seconds" << endl;					// Task one will initially sleep for a few seconds
	this_thread::sleep_for(seconds(3));
	auto lock = unique_lock<mutex>(mut);								// Create lock - thread 2 should manage to grab it first
	cout << "Task 1 notifying waiting thread" << endl;
	condition.notify_one();												// Notify waiting thread
	cout << "Task 1 waiting for notification" << endl;					// Now wait for notification
	condition.wait(lock);												// Wait, freeing lock as we do.
	cout << "Task 1 notified" << endl;									// We are free to continue
	cout << "Task 1 sleeping for 3 seconds" << endl;					// Sleep for 3 seconds
	this_thread::sleep_for(seconds(3));
	cout << "Task 1 notifying waiting thread" << endl;					// Notify any waiting thread
	condition.notify_one();
	
	cout << "Task 1 waiting 3 seconds for notification" << endl;		// Now wait 3 seconds for notification
	if (condition.wait_for(lock, seconds(3)) == cv_status::no_timeout) {
		cout << "Task 1 notified before 3 seconds" << endl;
	}	
	else {
		cout << "Task 1 got tired waiting" << endl;
	}
	
	cout << "Task 1 finished" << endl;									// Print finished message
}

void task_2(condition_variable &condition)
{
	auto lock = unique_lock<mutex>(mut);								// Create lock
	cout << "Task 2 waiting for notification" << endl;					// Task two will initially wait for notification
	condition.wait(lock);												// Wait, releasing the lock as we do.
	cout << "Task 2 notified" << endl;									// We are free to continue
	cout << "Task 2 sleeping for 5 seconds" << endl;					// Sleep for 5 seconds
	this_thread::sleep_for(seconds(5));
	cout << "Task 2 notifying waiting thread" << endl;					// Notify waiting thread
	condition.notify_one();
	cout << "Task 2 waiting 5 seconds for notification" << endl;		// Now wait 5 seconds for notification
	if (condition.wait_for(lock, seconds(5)) == cv_status::no_timeout) {
		cout << "Task 2 notified before 5 seconds" << endl;
	}
	else {
		cout << "Task 2 got tired waiting" << endl;
	}
	cout << "Task 2 sleeping for 5 seconds" << endl;					// Sleep for 5 seconds
	this_thread::sleep_for(seconds(5));
	cout << "Task 2 notifying waiting thread" << endl;					// Notify any waiting thread
	condition.notify_one();
	cout << "Task 2 finished" << endl;									// Print finished message
}

void pusher(shared_ptr<threadsafe_stack<unsigned int>> stack)
{
	for (unsigned int i = 0; i < 1000000; ++i) {		// Pusher will push 1 million values onto the stack
		stack->push(i);	
		this_thread::yield();							// Make the pusher yield.  Will give priority to another thread
	}
}

void popper(shared_ptr<threadsafe_stack<unsigned int>> stack)
{
	// Popper will pop 1 million values from the stack.
	// We do this using a counter and a while loop
	unsigned int count = 0;
	while (count < 1000000) {
		try {
			auto val = stack->pop();		// Try and pop a value
			++count;						// Item popped.  Increment count
		}
		catch (exception e) {
			cout << e.what() << endl;		// Item not popped.  Display message
		}
	}
}

int main(int argc, char **argv)
{
	/* Example 1 - Thread Incrementing to Output /
	auto value = make_shared<int>(0);					// Create a shared int value

	auto num_threads = thread::hardware_concurrency();	// Create number of threads hardware natively supports
	vector<thread> threads;
	for (unsigned int i = 0; i < num_threads; ++i) {
		threads.push_back(thread(increment, value));
	}

	for (auto &t : threads) {
		t.join();									// Join the threads
	}

	cout << "Value = " << *value << endl;			// Display the value
	// ************************* //

	/* Example 2 - Condition Variables /
	condition_variable condition;					// Create condition variable

	thread t1(task_1, ref(condition));				// Create two threads
	thread t2(task_2, ref(condition));				// ref is used to create a reference to pass to our thread functions

	t1.join();										// Join two threads
	t2.join();

	return 0;
	// ************************* //

	/* Example 3 - Object Guarding /
	auto g = make_shared<guarded>();				// Create guarded object

	vector<thread> threads;							// Create threads
	for (unsigned int i = 0; i < NUM_THREADS; ++i) {
		threads.push_back(thread(task, g));
	}
	for (auto &t : threads) {
		t.join();									// Join threads
	}

	cout << "Value = " << g->get_value() << endl;	// Display value stored in guarded object

	return 0;
	// ************************* //

	/* Example 4 - Threadsafe Stacks /
	auto stack = make_shared<threadsafe_stack<unsigned int>>();	// Create a threadsafe_stack

	thread t1(popper, stack);									// Create two threads
	thread t2(pusher, stack);

	t1.join();													// Join two threads
	t2.join();

	cout << "Stack empty = " << stack->empty() << endl;			// Check if stack is empty

	return 0;
	// ************************* //

	/* Example 5 - Atomics /
	auto value = make_shared<atomic<int>>();				// Create a shared int value

	auto num_threads = thread::hardware_concurrency();		// Create number of threads hardware natively supports
	vector<thread> threads;
	for (unsigned int i = 0; i < num_threads; ++i) {
		threads.push_back(thread(atomic_increment, value));
	}

	for (auto &t : threads) {
		t.join();											// Join the threads
	}

	cout << "Value = " << *value << endl;					// Display the value
	// ************************* //

	/* Example 6 - Atomic Flags */
	auto flag = make_shared<atomic_flag>();					// Create shared flag

	auto num_threads = thread::hardware_concurrency();		// Get number of hardware threads

	vector<thread> threads;									// Create threads
	for (unsigned int i = 0; i < num_threads; ++i) {
		threads.push_back(thread(flag_task, i, flag));
	}

	for (auto &t : threads) {								// Join threads
		t.join();
	}

	return 0;
	// ************************* //
}