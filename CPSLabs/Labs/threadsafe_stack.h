#pragma once

#include <exception>
#include <stack>
#include <memory>
#include <mutex>

template<typename T>
class threadsafe_stack
{
private:
	std::stack<T> data;			// The actual stack object we are using
	mutable std::mutex mut;		// The mutex to control access
public:
	threadsafe_stack() { }								// Normal constructor
	threadsafe_stack(const threadsafe_stack &other)		// Copy constructor
	{
		std::lock_guard<std::mutex> lock(other.mut);	// We need to copy the data from the other stack. Lock other stack
		data = other.data;
	}

	void push(T value)								// Push method.  Adds to the stack
	{
		std::lock_guard<std::mutex> lock(mut);		// Lock access to the object
		data.push(value);							// Push value onto the internal stack
	}

	T pop()											// Pop method.  Removes from the stack
	{
		std::lock_guard<std::mutex> lock(mut);		// Lock access to the object
		if (data.empty()) throw std::exception();	// Check if stack is empty
		auto res = data.top();						// Access value at the top of the stack.
		data.pop();									// Remove the top item from the stack
		return res;									// Return resource
	}

	bool empty() const								// Checks if the stack is empty
	{
		std::lock_guard<std::mutex> lock(mut);
		return data.empty();
	}
};