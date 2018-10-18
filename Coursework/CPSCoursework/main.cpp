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
	bchain.results.open("CourseworkTest2.csv", ofstream::out);
	bchain.results << "Individual Block Times" << "," << "Index" << endl;

	auto start = system_clock::now();
    for (uint32_t i = 1; i < 100u; ++i)
    {
        bchain.add_block(block(i, string("Block ") + to_string(i) + string(" Data")));
		bchain.results << endl;
    }
	auto end = system_clock::now();

	duration<double> diff = end - start;
	bchain.results << "Overall Time" << endl << diff.count() << endl;
	bchain.results.close();

    return 0;
}