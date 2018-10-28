#include "block_chain.h"
#include "sha256.h"

#include <iostream>
#include <sstream>
#include <chrono>
#include <fstream>
#include <thread>

using namespace std;
using namespace std::chrono;

// Note that _time would normally be set to the time of the block's creation.
// This is part of the audit a block chain.  To enable consistent results
// from parallelisation we will just use the index value, so time increments
// by one each time: 1, 2, 3, etc.
block::block(uint32_t index, const string &data)
	: _index(index), _data(data), _nonce(0), _time(static_cast<long>(index))
{
}

void block::mine_block(uint32_t difficulty) noexcept
{
	auto num_threads = thread::hardware_concurrency();
	vector<thread> threads;

	auto start = system_clock::now();

	for (unsigned int i = 0; i < num_threads; ++i)
	{
		threads.push_back(thread(&block::calculate_hash, this, difficulty));
	}

	for (auto &t : threads)
	{
		t.join();
	}

	auto end = system_clock::now();
	duration<double> diff = end - start;
	cout << "Block " << _index << " mined: " << _hash << " in " << diff.count() << " seconds" << endl;
}

void block::calculate_hash(uint32_t difficulty) noexcept
{
	string str(difficulty, '0');

	while (flag != true)
	{
		stringstream ss;
		ss << _index << _time << _data << ++_nonce << prev_hash;

		string newHash = sha256(ss.str());
		if (newHash.substr(0, difficulty) == str)
		{
			flag = true;
			_hash = newHash;
		}
	}
}

block_chain::block_chain()
{
	_chain.emplace_back(block(0, "Genesis Block"));
}

void block_chain::add_block(block &&new_block, uint32_t difficulty) noexcept
{
	new_block.prev_hash = get_last_block().get_hash();
	new_block.mine_block(difficulty);
	_chain.push_back(new_block);
}