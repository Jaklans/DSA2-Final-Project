#include "MyEntityManager.h"
using namespace Simplex;


void Simplex::MyEntityManager::SetGrav(vector3 grav)
{
	_gravity = grav;
}
vector3 Simplex::MyEntityManager::GetGrav()
{
	return _gravity;
}
void Simplex::MyEntityManager::SetPressure(float pressure)
{
	_pressure = pressure;
}
float Simplex::MyEntityManager::GetPressure()
{
	return _pressure;
}
Octree * Simplex::MyEntityManager::GetOctree()
{
	return &_oct;
}

//  MyEntityManager
Simplex::MyEntityManager* Simplex::MyEntityManager::m_pInstance = nullptr;
void Simplex::MyEntityManager::Init(void)
{
	_gravity = vector3(0, -2.5f, 0);//vector3(0, -9.8f, 0);
	_pressure = 4;
	_oct.Initialize(vector3(-34.0f, -34.0f, -34.0f), vector3(34.0f, 34.0f, 34.0f));
	m_uEntityCount = 0;
	m_mEntityArray = nullptr;
}
void Simplex::MyEntityManager::Release(void)
{
	for (uint uEntity = 0; uEntity < m_uEntityCount; ++uEntity)
	{
		MyEntity* pEntity = m_mEntityArray[uEntity];
		SafeDelete(pEntity);
	}
	m_uEntityCount = 0;
	m_mEntityArray = nullptr;
}
Simplex::MyEntityManager* Simplex::MyEntityManager::GetInstance()
{
	if(m_pInstance == nullptr)
	{
		m_pInstance = new MyEntityManager();
	}
	return m_pInstance;
}
void Simplex::MyEntityManager::ReleaseInstance()
{
	if(m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}
int Simplex::MyEntityManager::GetEntityIndex(String a_sUniqueID)
{
	//look one by one for the specified unique id
	for (uint uIndex = 0; uIndex < m_uEntityCount; ++uIndex)
	{
		if (a_sUniqueID == m_mEntityArray[uIndex]->GetUniqueID())
			return uIndex;
	}
	//if not found return -1
	return -1;
}
//Accessors
Simplex::uint Simplex::MyEntityManager::GetEntityCount(void) {	return m_uEntityCount; }
Simplex::Model* Simplex::MyEntityManager::GetModel(uint a_uIndex)
{
	//if the list is empty return
	if (m_uEntityCount == 0)
		return nullptr;

	// if out of bounds
	if (a_uIndex >= m_uEntityCount)
		a_uIndex = m_uEntityCount - 1;

	return m_mEntityArray[a_uIndex]->GetModel();
}
Simplex::Model* Simplex::MyEntityManager::GetModel(String a_sUniqueID)
{
	//Get the entity
	MyEntity* pTemp = MyEntity::GetEntity(a_sUniqueID);
	//if the entity exists
	if (pTemp)
	{
		return pTemp->GetModel();
	}
	return nullptr;
}
Simplex::MyRigidBody* Simplex::MyEntityManager::GetRigidBody(uint a_uIndex)
{
	//if the list is empty return
	if (m_uEntityCount == 0)
		return nullptr;

	// if out of bounds
	if (a_uIndex >= m_uEntityCount)
		a_uIndex = m_uEntityCount - 1;

	return m_mEntityArray[a_uIndex]->GetRigidBody();
}
Simplex::MyRigidBody* Simplex::MyEntityManager::GetRigidBody(String a_sUniqueID)
{
	//Get the entity
	MyEntity* pTemp = MyEntity::GetEntity(a_sUniqueID);
	//if the entity exists
	if (pTemp)
	{
		return pTemp->GetRigidBody();
	}
	return nullptr;
}
Simplex::matrix4 Simplex::MyEntityManager::GetModelMatrix(uint a_uIndex)
{
	//if the list is empty return
	if (m_uEntityCount == 0)
		return matrix4();

	// if out of bounds
	if (a_uIndex >= m_uEntityCount)
		a_uIndex = m_uEntityCount - 1;

	return m_mEntityArray[a_uIndex]->GetModelMatrix();
}
Simplex::matrix4 Simplex::MyEntityManager::GetModelMatrix(String a_sUniqueID)
{
	//Get the entity
	MyEntity* pTemp = MyEntity::GetEntity(a_sUniqueID);
	//if the entity exists
	if (pTemp)
	{
		return pTemp->GetModelMatrix();
	}
	return IDENTITY_M4;
}
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, String a_sUniqueID)
{
	//Get the entity
	MyEntity* pTemp = MyEntity::GetEntity(a_sUniqueID);
	//if the entity exists
	if (pTemp)
	{
		pTemp->SetModelMatrix(a_m4ToWorld);
	}
}
void Simplex::MyEntityManager::SetAxisVisibility(bool a_bVisibility, uint a_uIndex)
{
	//if the list is empty return
	if (m_uEntityCount == 0)
		return;

	//if the index is larger than the number of entries we are asking for the last one
	if (a_uIndex >= m_uEntityCount)
		a_uIndex = m_uEntityCount - 1;

	return m_mEntityArray[a_uIndex]->SetAxisVisible(a_bVisibility);
}
void Simplex::MyEntityManager::SetAxisVisibility(bool a_bVisibility, String a_sUniqueID)
{
	//Get the entity
	MyEntity* pTemp = MyEntity::GetEntity(a_sUniqueID);
	//if the entity exists
	if (pTemp)
	{
		pTemp->SetAxisVisible(a_bVisibility);
	}
}
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, uint a_uIndex)
{
	//if the list is empty return
	if (m_uEntityCount == 0)
		return;

	//if the index is larger than the number of entries we are asking for the last one
	if (a_uIndex >= m_uEntityCount)
		a_uIndex = m_uEntityCount - 1;

	m_mEntityArray[a_uIndex]->SetModelMatrix(a_m4ToWorld);
}
//The big 3
Simplex::MyEntityManager::MyEntityManager(){Init();}
Simplex::MyEntityManager::MyEntityManager(MyEntityManager const& a_pOther){ }
Simplex::MyEntityManager& Simplex::MyEntityManager::operator=(MyEntityManager const& a_pOther) { return *this; }
Simplex::MyEntityManager::~MyEntityManager(){Release();};
// other methods
void Simplex::MyEntityManager::Update(void)
{
	//_oct.ReOptimize();

	//Clear all collisions
	for (uint i = 0; i < m_uEntityCount; i++)
	{
		m_mEntityArray[i]->ClearCollisionList();
	}

	//Physics
	for (uint i = 0; i < m_uEntityCount; i++)
	{
		//Apply gravity
		m_mEntityArray[i]->AddForce(_gravity);
		//Move everything
		m_mEntityArray[i]->ApplyPhysics(.1f);
		m_mEntityArray[i]->SetOctAddress(_oct.GetAddress(m_mEntityArray[i]->GetRigidBody()->GetMinGlobal(), m_mEntityArray[i]->GetRigidBody()->GetMaxGlobal()));
	}


	//check collisions
	for (int i = 0; i < m_uEntityCount - 1; i++)
	{
		for (int j = i + 1; j < m_uEntityCount; j++)
		{
			if (m_mEntityArray[i]->IsColliding(m_mEntityArray[j])) 
			{
				//Resolve collisions
				//vector3 overlap;
				//m_mEntityArray[i]->AddForce(overlap * _pressure);
				//m_mEntityArray[j]->AddForce(overlap * -_pressure);
			}
		}
	}
}
void Simplex::MyEntityManager::AddEntity(String a_sFileName, colliderType type, String a_sUniqueID)
{
	//Create a temporal entity to store the object
	MyEntity* pTemp = new MyEntity(a_sFileName, type, a_sUniqueID);
	//if I was able to generate it add it to the list
	if (pTemp->IsInitialized() || type == inverseCylinder)
	{
		//create a new temp array with one extra entry
		PEntity* tempArray = new PEntity[m_uEntityCount + 1];
		//start from 0 to the current count
		uint uCount = 0;
		for (uint i = 0; i < m_uEntityCount; ++i)
		{
			tempArray[uCount] = m_mEntityArray[i];
			++uCount;
		}
		tempArray[uCount] = pTemp;
		//if there was an older array delete
		if (m_mEntityArray)
		{
			delete[] m_mEntityArray;
		}
		//make the member pointer the temp pointer
		m_mEntityArray = tempArray;
		//add one entity to the count
		++m_uEntityCount;
	}
}
void Simplex::MyEntityManager::RemoveEntity(uint a_uIndex)
{
	//if the list is empty return
	if (m_uEntityCount == 0)
		return;

	// if out of bounds choose the last one
	if (a_uIndex >= m_uEntityCount)
		a_uIndex = m_uEntityCount - 1;

	// if the entity is not the very last we swap it for the last one
	if (a_uIndex != m_uEntityCount - 1)
	{
		std::swap(m_mEntityArray[a_uIndex], m_mEntityArray[m_uEntityCount - 1]);
	}
	
	//and then pop the last one
	//create a new temp array with one less entry
	PEntity* tempArray = new PEntity[m_uEntityCount - 1];
	//start from 0 to the current count
	for (uint i = 0; i < m_uEntityCount - 1; ++i)
	{
		tempArray[i] = m_mEntityArray[i];
	}
	//if there was an older array delete
	if (m_mEntityArray)
	{
		delete[] m_mEntityArray;
	}
	//make the member pointer the temp pointer
	m_mEntityArray = tempArray;
	//add one entity to the count
	--m_uEntityCount;
}
void Simplex::MyEntityManager::RemoveEntity(String a_sUniqueID)
{
	int nIndex = GetEntityIndex(a_sUniqueID);
	RemoveEntity((uint)nIndex);
}
Simplex::String Simplex::MyEntityManager::GetUniqueID(uint a_uIndex)
{
	//if the list is empty return
	if (m_uEntityCount == 0)
		return "";

	//if the index is larger than the number of entries we are asking for the last one
	if (a_uIndex >= m_uEntityCount)
		a_uIndex = m_uEntityCount - 1;

	return m_mEntityArray[a_uIndex]->GetUniqueID();
}
Simplex::MyEntity* Simplex::MyEntityManager::GetEntity(uint a_uIndex)
{
	//if the list is empty return
	if (m_uEntityCount == 0)
		return nullptr;

	//if the index is larger than the number of entries we are asking for the last one
	if (a_uIndex >= m_uEntityCount)
		a_uIndex = m_uEntityCount - 1;

	return m_mEntityArray[a_uIndex];
}
void Simplex::MyEntityManager::AddEntityToRenderList(uint a_uIndex, bool a_bRigidBody)
{
	//if out of bounds will do it for all
	if (a_uIndex >= m_uEntityCount)
	{
		//add for each one in the entity list
		for (a_uIndex = 0; a_uIndex < m_uEntityCount; ++a_uIndex)
		{
			m_mEntityArray[a_uIndex]->AddToRenderList(a_bRigidBody);
		}
	}
	else //do it for the specified one
	{
		m_mEntityArray[a_uIndex]->AddToRenderList(a_bRigidBody);
	}
}