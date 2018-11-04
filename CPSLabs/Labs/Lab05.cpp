#include <iostream>
#include <sstream>
#include <random>
#include <mpi.h>

using namespace std;

const unsigned int MAX_STRING = 100;

double monte_carlo_pi(size_t iterations)
{
	// Seed with real random number if available
	random_device r;
	// Create random number generator
	default_random_engine e(r());
	// Create a distribution - we want doubles between 0.0 and 1.0
	uniform_real_distribution<double> distribution(0.0, 1.0);

	// Keep track of number of points in circle
	unsigned int in_circle = 0;
	// Iterate
	for (size_t i = 0; i < iterations; ++i)
	{
		// Generate random point
		auto x = distribution(e);
		auto y = distribution(e);
		// Get length of vector defined - use Pythagarous
		auto length = sqrt((x * x) + (y * y));
		// Check if in circle
		if (length <= 1.0)
			++in_circle;
	}
	// Calculate pi
	return (4.0 * in_circle) / static_cast<double>(iterations);
}

int main(int argc, char **argv)
{
	/* Basic MPI Application /
	// Initialise MPI
	auto result = MPI_Init(nullptr, nullptr);
	// Check that we initialised correctly
	if (result != MPI_SUCCESS)
	{
		// Display error and abort
		cout << "ERROR - initialising MPI!" << endl;
		MPI_Abort(MPI_COMM_WORLD, result);
		return -1;
	}

	// Get MPI information
	int num_procs, rank, length;
	char host_name[MPI_MAX_PROCESSOR_NAME];
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Get_processor_name(host_name, &length);

	// Display information
	cout << "Number of processors = " << num_procs << endl;
	cout << "My rank = " << rank << endl;
	cout << "Running on = " << host_name << endl;

	// Shutdown MPI
	MPI_Finalize();

	return 0;
	/**************************/

	/* Sending And Receiving /
	int num_procs, my_rank;
	num_procs = 4;

	// Initialise MPI
	auto result = MPI_Init(nullptr, nullptr);
	if (result != MPI_SUCCESS)
	{
		cout << "ERROR - initialising MPI" << endl;
		MPI_Abort(MPI_COMM_WORLD, result);
		return -1;
	}

	// Get MPI Information
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	// Check if we are the main process
	if (my_rank != 0)
	{
		// Not main process - send message
		// Generate message
		stringstream buffer;
		buffer << "Greetings from process " << my_rank << " of " << num_procs << "!";
		// Get the character array from the string
		auto data = buffer.str().c_str();
		// Send to the main node
		MPI_Send((void*)data, buffer.str().length() + 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
	}
	else
	{
		// Main process - print message
		cout << "Greetings from process " << my_rank << " of " << num_procs << "!" << endl;
		// Read in data from each worker process
		char message[MAX_STRING];
		for (int i = 1; i < num_procs; ++i)
		{
			// Receive message into buffer
			MPI_Recv(message, MAX_STRING, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			// Display message
			cout << message << endl;
		}
	}

	// Shutdown MPI
	MPI_Finalize();

	return 0;
	/**************************/

	/* Map Reduce */
	// Initialise MPI
	auto result = MPI_Init(nullptr, nullptr);
	if (result != MPI_SUCCESS)
	{
		cout << "ERROR - initialising MPI" << endl;
		MPI_Abort(MPI_COMM_WORLD, result);
		return -1;
	}

	int my_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	double local_sum, global_sum;

	// Calculate local sum - use previously defined function
	local_sum = monte_carlo_pi(static_cast<unsigned int>(pow(2, 24)));
	// Print out local sum
	cout.precision(numeric_limits<double>::digits10);
	cout << my_rank << ":" << local_sum << endl;
	// Reduce
	MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	// If main process display global reduced sum
	if (my_rank == 0)
	{
		global_sum /= 4.0;
		cout << "Pi=" << global_sum << endl;
	}
	/**************************/


}