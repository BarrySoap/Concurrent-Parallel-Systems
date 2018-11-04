#include <iostream>
#include <sstream>
#include <random>
#include <chrono>
#include <mpi.h>

using namespace std;
using namespace chrono;

const unsigned int MAX_STRING = 100;
constexpr size_t SIZE = 1 << 20;

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

// Randomly generate vector values
void generate_data(vector<float> &data)
{
	// Create random engine
	auto millis = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	default_random_engine e(static_cast<unsigned int>(millis.count()));
	// Fill data
	for (unsigned int i = 0; i < data.size(); ++i)
		data[i] = e();
}

// Normalises 4D vectors
void normalise_vector(vector<float> &data)
{
	// Iterate through each 4-dimensional vector
	for (unsigned int i = 0; i < (data.size() / 4); ++i)
	{
		// Sum the squares of the 4 components
		float sum = 0.0f;
		for (unsigned int j = 0; j < 4; ++j)
			sum += powf(data[(i * 4) + j], 2.0f);
		// Get the square root of the result
		sum = sqrtf(sum);
		// Divide each component by sum
		for (unsigned int j = 0; j < 4; ++j)
			data[(i * 4) + j] /= sum;
	}
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

	/* Map Reduce /
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

	// Shutdown MPI
	MPI_Finalize();

	return 0;
	/**************************/

	/* Vector Normalisation */
	// Initialise MPI
	auto result = MPI_Init(nullptr, nullptr);
	if (result != MPI_SUCCESS)
	{
		cout << "ERROR - initialising MPI" << endl;
		MPI_Abort(MPI_COMM_WORLD, result);
		return -1;
	}

	int my_rank, num_procs;
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

	// Vector containing values to normalise
	vector<float> data;
	// Local storage.  Allocate enough space
	vector<float> my_data(SIZE / num_procs);

	// Check if main process
	if (my_rank == 0)
	{
		// Generate data
		data.resize(SIZE);
		generate_data(data);
		// Scatter the data
		MPI_Scatter(&data[0], SIZE / num_procs, MPI_FLOAT,  // Source
			&my_data[0], SIZE / num_procs, MPI_FLOAT,       // Destination
			0, MPI_COMM_WORLD);
	}
	else
	{
		MPI_Scatter(nullptr, SIZE / num_procs, MPI_FLOAT,  // Source
			&my_data[0], SIZE / num_procs, MPI_FLOAT,      // Destination
			0, MPI_COMM_WORLD);
	}

	// Normalise local data
	normalise_vector(my_data);

	// Gather the results

	if (my_rank == 0)
	{
		MPI_Gather(&my_data[0], SIZE / num_procs, MPI_FLOAT,// Source
			&data[0], SIZE / num_procs, MPI_FLOAT,          // Dest
			0, MPI_COMM_WORLD);
	}
	else
	{
		MPI_Gather(&my_data[0], SIZE / num_procs, MPI_FLOAT,// Source
			nullptr, SIZE / num_procs, MPI_FLOAT,           // Dest
			0, MPI_COMM_WORLD);
	}

	// Check if main process
	if (my_rank == 0)
	{
		// Display results - first 10
		for (unsigned int i = 0; i < 10; ++i)
		{
			cout << "<";
			for (unsigned int j = 0; j < 3; ++j)
				cout << data[(i * 4) + j] << ", ";
			cout << data[(i * 4) + 3] << ">" << endl;
		}
	}

	// Shutdown MPI
	MPI_Finalize();

	return 0;
	/**************************/
}