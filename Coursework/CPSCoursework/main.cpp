#include <iostream>
#include <string>
#include <chrono>
#include <fstream>
#include "block_chain.h"

using namespace std;

int main()
{
    block_chain bchain;
	bchain.results.open("CourseworkTest.csv", ofstream::out);
	bchain.results << "Individual Block Times" << endl;

    for (uint32_t i = 1; i < 10u; ++i)
    {
        bchain.add_block(block(i, string("Block ") + to_string(i) + string(" Data")));
		bchain.results << endl;
    }

	bchain.results.close();

    return 0;
}