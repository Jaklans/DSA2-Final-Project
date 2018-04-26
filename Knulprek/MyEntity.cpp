#include "MyEntity.h"

#define NormalMagnitude 10.0f

using namespace Simplex;
std::map<String, MyEntity*> MyEntity::m_IDMap;
//  Accessors
matrix4 Simplex::MyEntity::GetModelMatrix(void){ return m_m4ToWorld; }
void Simplex::MyEntity::SetModelMatrix(matrix4 a_m4ToWorld)
{
	if (!m_bInMemory)
		return;

	m_m4ToWorld = a_m4ToWorld;
	m_pModel->SetModelMatrix(m_m4ToWorld);
	m_pRigidBody->SetModelMatrix(m_m4ToWorld);
}
Model* Simplex::MyEntity::GetModel(void){return m_pModel;}
MyRigidBody* Simplex::MyEntity::GetRigidBody(void){	return m_pRigidBody; }
bool Simplex::MyEntity::IsInitialized(void){ return m_bInMemory; }
String Simplex::MyEntity::GetUniqueID(void) { return m_sUniqueID; }
void Simplex::MyEntity::SetAxisVisible(bool a_bSetAxis) { m_bSetAxis = a_bSetAxis; }
//  MyEntity
void Simplex::MyEntity::Init(void)
{
	m_pMeshMngr = MeshManager::GetInstance();
	m_bInMemory = false;
	m_bSetAxis = false;
	m_pModel = nullptr;
	m_pRigidBody = nullptr;
	m_DimensionArray = nullptr;
	m_m4ToWorld = IDENTITY_M4;
	m_sUniqueID = "";
	m_nDimensionCount = 0;
}
void Simplex::MyEntity::Swap(MyEntity& other)
{
	m_bInMemory = false;
	std::swap(m_pModel, other.m_pModel);
	std::swap(m_pRigidBody, other.m_pRigidBody);
	std::swap(m_m4ToWorld, other.m_m4ToWorld);
	std::swap(m_pMeshMngr, other.m_pMeshMngr);
	std::swap(m_bInMemory, other.m_bInMemory);
	std::swap(m_sUniqueID, other.m_sUniqueID);
	std::swap(m_bSetAxis, other.m_bSetAxis);
	std::swap(m_nDimensionCount, other.m_nDimensionCount);
	std::swap(m_DimensionArray, other.m_DimensionArray);
}
void Simplex::MyEntity::Release(void)
{
	m_pMeshMngr = nullptr;
	//it is not the job of the entity to release the model, 
	//it is for the mesh manager to do so.
	m_pModel = nullptr;
	if (m_DimensionArray)
	{
		delete[] m_DimensionArray;
		m_DimensionArray = nullptr;
	}
	SafeDelete(m_pRigidBody);
	m_IDMap.erase(m_sUniqueID);
}
void Simplex::MyEntity::AddForce(vector3 force)
{
	//Everything has mass of 1 :D
	if (m_pRigidBody->collider != sphere) return;
	_force += force;
}
void Simplex::MyEntity::ApplyPhysics(float deltaTime)
{
	if (m_pRigidBody->collider != sphere) return;
	_velocity = _force * deltaTime;
	_force = vector3();
	//_velocity = glm::clamp( _velocity, 0.f, 5.5f);
	SetModelMatrix(glm::translate(_velocity) * m_m4ToWorld);
}
void Simplex::MyEntity::SetOctAddress(OctreeAddress & val)
{
	_octAddress = val;
}
OctreeAddress * Simplex::MyEntity::GetOctAddress()
{
	return &_octAddress;
}
//The big 3
Simplex::MyEntity::MyEntity(String a_sFileName, colliderType type, String a_sUniqueID)
{
	Init();

	//Inverse Cylinder has no model
	if (type == inverseCylinder) {
		GenUniqueID(a_sUniqueID);
		m_sUniqueID = a_sUniqueID;
		m_IDMap[a_sUniqueID] = this;

		//Rigid body
		m_pRigidBody = new MyRigidBody(type); //generate a rigid body

		return;
	}

	m_pModel = new Model();
	m_pModel->Load(a_sFileName);
	//if the model is loaded
	if (m_pModel->GetName() != "")
	{
		GenUniqueID(a_sUniqueID);
		m_sUniqueID = a_sUniqueID;
		m_IDMap[a_sUniqueID] = this;

		
		//Rigid body
		m_pRigidBody = new MyRigidBody(type); //generate a rigid body
		m_bInMemory = true; //mark this entity as viable
	}
}
Simplex::MyEntity::MyEntity(MyEntity const& other)
{
	m_bInMemory = other.m_bInMemory;
	m_pModel = other.m_pModel;
	//generate a new rigid body we do not share the same rigid body as we do the model
	m_pRigidBody = new MyRigidBody(*other.m_pRigidBody); 
	m_m4ToWorld = other.m_m4ToWorld;
	m_pMeshMngr = other.m_pMeshMngr;
	m_sUniqueID = other.m_sUniqueID;
	m_bSetAxis = other.m_bSetAxis;
	m_nDimensionCount = other.m_nDimensionCount;
	m_DimensionArray = other.m_DimensionArray;

}
MyEntity& Simplex::MyEntity::operator=(MyEntity const& other)
{
	if(this != &other)
	{
		Release();
		Init();
		MyEntity temp(other);
		Swap(temp);
	}
	return *this;
}
MyEntity::~MyEntity(){Release();}
//--- Methods
void Simplex::MyEntity::AddToRenderList(bool a_bDrawRigidBody)
{
	//if not in memory return
	if (!m_bInMemory)
		return;

	//draw model
	m_pModel->AddToRenderList();
	
	//draw rigid body
	if(a_bDrawRigidBody)
		m_pRigidBody->AddToRenderList();

	if (m_bSetAxis)
		m_pMeshMngr->AddAxisToRenderList(m_m4ToWorld);
}
MyEntity* Simplex::MyEntity::GetEntity(String a_sUniqueID)
{
	//look the entity based on the unique id
	auto entity = m_IDMap.find(a_sUniqueID);
	//if not found return nullptr, if found return it
	return entity == m_IDMap.end() ? nullptr : entity->second;
}
void Simplex::MyEntity::GenUniqueID(String& a_sUniqueID)
{
	static uint index = 0;
	String sName = a_sUniqueID;
	MyEntity* pEntity = GetEntity(a_sUniqueID);
	//while MyEntity exists keep changing name
	while (pEntity)
	{
		a_sUniqueID = sName + "_" + std::to_string(index);
		index++;
		pEntity = GetEntity(a_sUniqueID);
	}
	return;
}
bool Simplex::MyEntity::IsColliding(MyEntity* const other)
{
	//if not in memory return
	if (!m_bInMemory || (!other->m_bInMemory && other->m_pRigidBody->collider != inverseCylinder))
		return true;

	//if the entities are not living in the same dimension
	//they are not colliding
	if (!_octAddress.Compare(other->_octAddress))
		return false;

	vector3 collisionForce;
	bool result = m_pRigidBody->IsColliding(other->GetRigidBody(), collisionForce);

	if (result) {
		AddForce(collisionForce * -NormalMagnitude);
		other->AddForce(collisionForce * NormalMagnitude);
	}

	return result;
}
void Simplex::MyEntity::ClearCollisionList(void)
{
	m_pRigidBody->ClearCollidingList();
}