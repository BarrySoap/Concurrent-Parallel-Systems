#include <iostream>
#include <string>
#include <chrono>
#include <fstream>
#include "block_chain.h"

using namespace std;

int main()
{
    block_chain bchain;
	auto start = std::chrono::system_clock::now();

    for (uint32_t i = 1; i < 5u; ++i)
    {
        cout << "Mining block " << i << "..." << endl;
        bchain.add_block(block(i, string("Block ") + to_string(i) + string(" Data")));
    }
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> diff = end - start;
	cout << "Blocks mined in: " << diff.count() << " seconds" << endl;
	ofstream results("CourseworkTest.csv", ofstream::out);
	results << diff.count() << endl;
	results.close();

    return 0;
}