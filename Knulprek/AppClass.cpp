#include "AppClass.h"
using namespace Simplex;
void Application::InitVariables(void)
{
	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUp(
		vector3(0.0f, 0.0f, 100.0f), //Position
		vector3(0.0f, 0.0f, 99.0f),	//Target
		AXIS_Y);					//Up

	m_pLightMngr->SetPosition(vector3(0.0f, 3.0f, 13.0f), 1); //set the position of first light (0 is reserved for ambient light)

	/*
#ifdef DEBUG
	uint uInstances = 900;
#else
	uint uInstances = 1849;
#endif
	int nSquare = static_cast<int>(std::sqrt(uInstances));
	m_uObjects = nSquare * nSquare;
	uint uIndex = -1;
	for (int i = 0; i < nSquare; i++)
	{
		for (int j = 0; j < nSquare; j++)
		{
			uIndex++;
			m_pEntityMngr->AddEntity("Minecraft\\Cube.obj");
			vector3 v3Position = vector3(glm::sphericalRand(34.0f));
			matrix4 m4Position = glm::translate(v3Position);
			m_pEntityMngr->SetModelMatrix(m4Position);
		}
	}

	*/

	// Generate Spheres
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			for (int k = 0; k < 3; k++) {
				m_pEntityMngr->AddEntity("Knulprek//Sphere.fbx", sphere);
				m_pEntityMngr->SetModelMatrix(glm::translate(vector3(i * 4 - 4, j * 4 + 25, k * 4 - 4)));
			}
		}
	}

	//// Generate Cylinders
	//for (int i = 0; i < 10; i++) {
	//	m_pEntityMngr->AddEntity("Knulprek//Cylinder.fbx", cylinder);
	//	m_pEntityMngr->SetModelMatrix(glm::translate(vector3(0, -i, 0)) * glm::rotate(18.0f * float(i), vector3(0, 1, 0)) * glm::rotate(90.0f, vector3(1, 0, 0)));
	//}

	//// changeable parameters
	//int numPegs = 50;
	//float pegAngleVariationFactor = 0.2f; // maximum peg angular variation, factor of pegAngleIncrement
	//float pegHorizontalVariationFactor = 8;
	//float pegVerticalCompressionFactor = 0.2f;

	////calculated values
	//float pegAngleIncrement = 180 / (float)numPegs;
	//std::vector<float> pegHeights;
	//for (int i = 0; i < numPegs; i++)
	//{
	//	float currentPegAngleVariation = pegAngleIncrement * pegAngleVariationFactor * std::rand() / (float)RAND_MAX;
	//	pegHeights.push_back(i * pegAngleIncrement + currentPegAngleVariation);
	//}
	//std::random_shuffle(pegHeights.begin(), pegHeights.end()); // shuffle angles so that we can generate heights in linear order

	//// DON'T SCALE, ask for different maya model instead
	//// generate cylinders
	//glm::mat4 flipPegToVertical = glm::rotate(90.0f, vector3(1, 0, 0));
	//for (int i = 0; i < numPegs; i++) {
	//	glm::mat4 rotatePegHorizontally = glm::rotate(pegHeights[i], vector3(0, 1, 0));
	//	glm::mat4 translatePegHeight = glm::translate(vector3(0, -i * pegVerticalCompressionFactor, 0));
	//	m_pEntityMngr->AddEntity("Knulprek//Cylinder.fbx", cylinder);
	//	m_pEntityMngr->SetModelMatrix(translatePegHeight * rotatePegHorizontally * flipPegToVertical);
	//}






	// given: numHolesPerRing, ringDiameter, ringBaseHeight, ringHeightIncrement, numHoles
	// BASE VALUES
	// holeAngleIncrement = 360 / numHolesPerRing
	// ringHalfHeight = ringHeightIncrement / 2
	// LIST VALUES - for i 0:numHoles
	// holes = vector<vector3>
	// holes.push_back(vector3)
	// holes[i].X = cos(holeAngleIncrement * i) * ringDiameter
	// holes[i].Y = sin(holeAngleIncrement * i) * ringDiameter
	// holes[i].Z = ringBaseHeight + ringHeightIncrement * (i / numHolesPerRing)
	// PEG CALCULATION VALUES, given hole1 and hole2
	// holeXDiff = (hole2.X - hole1.X)
	// holeYDiff = (hole2.Y - hole1.Y)
	// holeZDiff = (hole2.Z - hole1.Z)
	// horizontalLength = sqrt(holeXDiff^2 + holeYDiff^2)
	// horizontalAngle = arctan(holeYDiff / holeXDiff)
	// verticalAngle = arctan(holeZDiff / horizontalLength)
	// !!!!! pegRotation = rotate(horizontalAngle, vector3(0, 1, 0)) * rotate(verticalAngle, vector3(1, 0, 0))
	// distanceToCenter = abs(hole2.X * hole1.Y - hole2.Y * hole1.X) / horizontalLength
	// pegXTranslation = distanceToCenter * cos(horizontalAngle)
	// pegYTranslation = distanceToCenter * sin(horizontalAngle)
	// pegZTranslation = hole1.Z + ringHalfHeight
	// !!!!! pegTranslation = translate(vector3(pegXTranslation, pegYTranslation, pegZTranslation))

	// GIVENS
	// height 7, width ?
	uint numHolesPerRing = 36;
	uint numHoles = 36 * 5;
	uint numPegs = 30;// numPegs <= numHoles / 2
	float ringDiameter = 10;
	float ringBaseHeight = 5;
	float ringHeightIncrement = 1;

	// PRE-CALCULATED VALUES
	float holeAngleIncrement = 360 / (float)numHolesPerRing;
	float ringHalfHeight = ringHeightIncrement / 2;

	// HOLE CENTERPOINT LOCATIONS
	std::vector<vector3> holes;
	for (uint i = 0; i < numHoles; i++)	{
		holes.push_back(vector3(
			cosf(holeAngleIncrement * i) * ringDiameter,// X
			sinf(holeAngleIncrement * i) * ringDiameter,// Y
			ringBaseHeight + ringHeightIncrement * i / numHolesPerRing// Z
		));
	}
	std::vector<vector3> shuffledHoles = holes;
	std::random_shuffle(shuffledHoles.begin(), shuffledHoles.end());

	// PEG TRANSFORMATION CALCULATIONS
	for (uint i = 0; i < numPegs; i++)
	{
		// start and end holes
		vector3 hole1 = shuffledHoles[i];
		vector3 hole2 = shuffledHoles[i + numPegs];

		// calculated variables
		float holeXDiff = (hole2.x - hole1.x);
		float holeYDiff = (hole2.y - hole1.y);
		float holeZDiff = (hole2.z - hole1.z);
		float horizontalLength = sqrtf(powf(holeXDiff, 2) + powf(holeYDiff, 2));
		float horizontalAngle = atanf(holeYDiff / holeXDiff);
		float verticalAngle = atanf(holeZDiff / horizontalLength);
		float distanceToCenter = abs(hole2.x * hole1.y - hole2.y * hole1.x) / horizontalLength;

		// final transformations
		glm::mat4 pegRotation = glm::rotate(horizontalAngle, vector3(0, 1, 0)) * glm::rotate(verticalAngle, vector3(1, 0, 0));
		glm::mat4 pegTranslation = glm::translate(vector3(
			distanceToCenter * cosf(horizontalAngle),
			distanceToCenter * sinf(horizontalAngle),
			hole1.z + ringHalfHeight
		));

		// generate pegs
		m_pEntityMngr->AddEntity("Knulprek//Cylinder.fbx", cylinder);
		m_pEntityMngr->SetModelMatrix(pegTranslation * pegRotation);
	}


	//m_pEntityMngr->AddEntity("Knulprek//Cylinder.fbx", inverseCylinder);

	m_uOctantLevels = 1;
	m_pEntityMngr->Update();
}

void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the ArcBall active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();

	//Update Entity Manager
	m_pEntityMngr->Update();

	//Add objects to render list
	m_pEntityMngr->AddEntityToRenderList(-1, true);
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	//display octree
	Octree::display = true;
	m_pEntityMngr->GetOctree()->Display();
	m_pEntityMngr->GetOctree()->Refresh();

	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui,
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	//release GUI
	ShutdownGUI();
}