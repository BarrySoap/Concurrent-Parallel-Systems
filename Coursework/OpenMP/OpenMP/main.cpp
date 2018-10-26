#include <iostream>
#include <string>
#include <chrono>
#include <fstream>
#include <thread>
#include "block_chain.h"

using namespace std;
using namespace chrono;

int main()
{
    block_chain bchain;
	duration<double> totalTime;
	bchain.results.open("OpenMP.csv", ofstream::out);
	bchain.results << "Average Block Time" << "," << "Difficulty" << endl;

#pragma omp parallel for num_threads(8) default(none)
	for (int difficulty = 1; difficulty < 6; difficulty++)
	{
		auto start = system_clock::now();
		for (int i = 1; i < 100u; ++i)
		{
			bchain.add_block(block(i, string("Block ") + to_string(i) + string(" Data")), difficulty);
		}
		auto end = system_clock::now();
		duration<double> diff = (end - start) / 100;
		bchain.results << diff.count() << "," << difficulty << endl;
	}
	
	bchain.results.close();

    return 0;
}