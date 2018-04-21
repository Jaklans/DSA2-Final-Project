#pragma once
//An adress that corresponds to one quadrant of the octree
class OctreeAddress
{
public:
	//The depth that address are
	static const unsigned int depth = 5;
private:
	//Masks that need to be used a lot
	const unsigned _int8 masks[2] = {15, 15 << 4};
	//bitset
	unsigned _int8 _address[(depth + 1) / 2];

public:
	OctreeAddress();
	~OctreeAddress();

	//Copy assignment operator
	OctreeAddress& OctreeAddress::operator= (OctreeAddress);
	
	//Sets a coordinate by index
	bool Set(int index, unsigned _int8 state);
	//Fetches a coordinate by index
	unsigned _int8 Get(int index);

	//Compares two Addresses to detect if they share space
	bool Compare(OctreeAddress &other);

};

