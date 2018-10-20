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
    string str(difficulty, '0');
	
    auto start = system_clock::now();

    while (_hash.substr(0, difficulty) != str)
    {
        ++_nonce;
        _hash = calculate_hash();
    }

    auto end = system_clock::now();
    duration<double> diff = end - start;
    cout << "Block " << _index << " mined: " << _hash << " in " << diff.count() << " seconds" << endl;
}

std::string block::calculate_hash() const noexcept
{
	string ss;
	ss.append(to_string(_index));
	ss.append(to_string(_time));
	ss.append(_data);
	ss.append(to_string(_nonce));
	ss.append(prev_hash);
	return sha256(ss);
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