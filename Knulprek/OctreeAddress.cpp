#include "OctreeAddress.h"
#include <cstring>
#include "Octree.h"

//Initialize the bitset
OctreeAddress::OctreeAddress()
{
	for (size_t i = 0; i < (depth + 1) / 2; i++)
	{
		_address[i] = 0;
	}
}


OctreeAddress::~OctreeAddress()
{

}

//Memcopy the bitset
OctreeAddress & OctreeAddress::operator=(OctreeAddress other)
{
	std::memcpy(_address, other._address, (depth + 1) / 2);

	return *this;
}

//Set a coordinate
bool OctreeAddress::Set(int index, unsigned _int8 state)
{
	if(index > depth)
		return false;
	if (index & 1)
		state = state << 4;
	//Wipe the address and override it with the new state
	_address[index / 2] = (_address[index / 2] & (~masks[index & 1])) | state;
	return true;
}

//Fetch a coordinate
unsigned _int8 OctreeAddress::Get(int index)
{
	unsigned _int8 ret = 0;
	if (index > depth)
		return 0;
	ret = _address[index / 2] & masks[index & 1];
	if (index & 1)
		ret = ret >> 4;
	return ret;
}

//Compares the addresses. Returns true if the addresses are the same or one contains the other
bool OctreeAddress::Compare(OctreeAddress & other)
{
	//For each part of the address
	for (size_t i = 0; i < depth; i++)
	{
		unsigned _int8 a = _address[i / 2] & masks[i & 1];
		unsigned _int8 b = other._address[i / 2] & masks[i & 1];
		//If either address is broad as to its position
		if (a == 0 || b == 0) {
			return true;
		}
		else if (a != b)
		{
			return false;
		}
	}
	return true;
}
