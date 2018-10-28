#include <iostream>
#include <string>
#include <chrono>
#include <fstream>
#include "block_chain.h"

using namespace std;
using namespace chrono;

int main()
{
	block_chain bchain;
	bchain.results.open("CourseworkTestOriginal.csv", ofstream::out);
	bchain.results << "Average Block Time" << "," << "Difficulty" << endl;
	for (uint32_t difficulty = 1; difficulty < 6; difficulty++)
	{
		auto start = system_clock::now();
		for (uint32_t i = 1; i < 100u; ++i)
		{
			cout << "Mining block " << i << "..." << endl;
			bchain.add_block(block(i, string("Block ") + to_string(i) + string(" Data")), difficulty);
		}
		auto end = system_clock::now();

		duration<double> diff = end - start;
		bchain.results << diff.count() << "," << difficulty << endl;
	}
	bchain.results.close();
	return 0;
}