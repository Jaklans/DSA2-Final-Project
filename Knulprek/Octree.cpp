#include "Octree.h"
#include "Simplex\Simplex.h"

std::vector<bool> Octree::DisplayOctant;

bool Octree::display = false;

Octree::Octree()
{
	//Set up the draw logic
	int size = pow(8, OctreeAddress::depth);
	DisplayOctant = std::vector<bool>(size);
}


Octree::~Octree()
{
}

void Octree::Initialize(vector3 min, vector3 max)
{
	//The center is between the minimum and the maximum
	_center = vector3((min.x + max.x) / 2, (min.y + max.y) / 2, (min.z + max.z) / 2);
	//The half distance is the largest half distance
	_halfDistance = max - _center;
	_min = _center;
	_max = _center;
}

//Get an address from the Octree
OctreeAddress Octree::GetAddress(vector3 min, vector3 max)
{
	//optimization
	if (min.y < _min.y)
		_min.y = min.y;
	if (_max.y < max.y)
		_max.y = max.y;
	if (min.x < _min.x)
		_min.x = min.x;
	if (_max.x < max.x)
		_max.x = max.x;
	if (min.z < _min.z)
		_min.z = min.z;
	if (_max.z < max.z)
		_max.z = max.z;
	OctreeAddress ret = OctreeAddress();

	vector3 cen = _center;
	vector3 bounds = _halfDistance;

	//For each level of recursion
	for (size_t i = 0; i < OctreeAddress::depth; i++)
	{
		//Get the octant of the max and the octant of the min
		unsigned _int8 a = Oct(cen, min);
		unsigned _int8 b = Oct(cen, max);
		//If they are the same and neither is zero, 
		if (a == b && a != 0) {
			//We have another level of specificity, so assign it to the address and subdivide
			ret.Set(i, a);
			bounds.x /= 2;
			bounds.y /= 2;
			bounds.z /= 2;
			if (min.x > cen.x)
				cen.x += bounds.x;
			else
				cen.x -= bounds.x;
			if (min.y > cen.y)
				cen.y += bounds.y;
			else
				cen.y -= bounds.y;
			if (min.z > cen.z)
				cen.z += bounds.z;
			else
				cen.z -= bounds.z;
		}
		else
		{
			Declare(ret, i);
			return ret;
		}
	}
	Declare(ret, OctreeAddress::depth - 1);
	return ret;
}

//Resets the draw logic
void Octree::Refresh()
{
	
	int size = pow(8, OctreeAddress::depth);
	DisplayOctant = std::vector<bool>(size);DisplayOctant[0] = true;
}

//Declare that an octant will be shown
void Octree::Declare(OctreeAddress& address, int depth)
{
	//If we aren't displaying, don't bother
	if (!display)
		return;
	int index = 0;
	for (size_t i = 0; i < depth; i++)
	{

		int oct = address.Get(i);
		if (oct & 1)
		{
			oct = oct >> 1;
			index = index * 8 + oct + 1;
			DisplayOctant[index] = true;
		}
		else
			break;
	}
}

//Trigger the recursion
void Octree::Display()
{
	Draw(0, _center, _halfDistance);
}

void Octree::ReOptimize()
{
	Initialize(_min, _max);
}

//Draw the subnodes recursively
void Octree::Draw(int index, vector3 center, vector3 halfSize)
{
	if (index < DisplayOctant.size() && DisplayOctant[index]) {
		MeshManager::GetInstance()->AddWireCubeToRenderList(glm::translate(center) * glm::scale(halfSize * 2.0f), vector3(1, .5f, .3f));
		halfSize.y /= 2;
		vector3 mod (0, -halfSize.y, 0);
		for (size_t i = 1; i <= 8; i++)
		{
			Draw(index * 8 + i, center + mod, halfSize);
			if (mod.y > 0 && mod.x > 0)
				mod.z *= -1;
			if(mod.x > 0)
				mod.y *= -1;
			mod.x *= -1;
		}
	}
}


//Returns a 4 bit bitset that is as follows:
//index: meaning
//0: Is specific
//1: x is high
//2: y is high
//3: z is high
unsigned _int8 Octree::Oct(vector3 center, vector3 location)
{
	//if(location.x == center.x || location.y == center.y || location.z == center.z)
	//	return 0;
	_int8 ret = 1;
	//if (location.x > center.x)
	//	ret = ret | (1 << 1);
	if (location.y > center.y)
		ret = ret | (1 << 2);
	//if (location.z > center.z)
	//	ret = ret | (1 << 3);
	return ret;
}
