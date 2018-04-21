#pragma once
#include "OctreeAddress.h"
#include "Definitions.h"

using namespace Simplex;
//An Octree that handles all of the manager level work of the Octree system
class Octree
{
	//The center
	vector3 _center;
	//The bounds
	vector3 _halfDistance;
	//The boolean of visual information (yes this is completely neccesary to draw the Octree)
	static std::vector<bool> DisplayOctant;

public:
	//The bool that flips the recording of Display logic
	static bool display;
	Octree();
	~Octree();

	//Sets up the center and bounds
	void Initialize(vector3 min, vector3 max);
	//Gets an address based on positional information
	OctreeAddress GetAddress(vector3 min, vector3 max);
	//Clears visual information
	static void Refresh();
	//Declares a path to need drawing
	static void Declare(OctreeAddress &address, int depth);
	//Draws the Octree
	void Display();



private:
	//Recursive visual function
	void Draw( int index, vector3 center, vector3 bounds);
	//Helper function that identifies the octant.
	unsigned _int8 Oct(vector3 center, vector3 location);

};

