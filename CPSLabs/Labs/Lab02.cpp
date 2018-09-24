#include <thread>
#include <iostream>
#include <vector>
#include <memory>
#include <mutex>

using namespace std;
using namespace std::this_thread;

mutex mut;

void increment(shared_ptr<int> value)
{
	for (unsigned int i = 0; i < 1000000; ++i) {	// Loop 1 million times, incrementing value
		mut.lock();									// Acquire the lock
		*value = *value + 1;						// Increment value
		mut.unlock();								// Release the lock
	}
}

int main(int argc, char **argv)
{
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
}