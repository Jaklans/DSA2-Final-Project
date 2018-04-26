#include "AppClass.h"
using namespace Simplex;
void Application::InitVariables(void)
{
	srand(time(NULL));

	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUp(
		vector3(0.0f, 0.0f, 100.0f), //Position
		vector3(0.0f, 0.0f, 19.0f),	//Target
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

	float cylinderDiameter = 15;

#pragma region GenerateSpheres
	// GIVENS
	// radius ?
	uint numBalls = 100;
	float ballDiameter = 3;
	float ballBaseHeight = 20;

	// calculated values
	float innerRadius = (cylinderDiameter - ballDiameter) / 2;

	// GENERATE SPHERES
	for (uint i = 0; i < numBalls; i++) {
		float randAngle = rand() / (float)RAND_MAX * 2 * PI;
		float randMag = rand() / (float)RAND_MAX * innerRadius;
		float currX = randMag * cosf(randAngle);
		float currZ = randMag * sinf(randAngle);
		float currY = ballBaseHeight + ballDiameter * i;
		m_pEntityMngr->AddEntity("Knulprek//Sphere.fbx", sphere, "Ball" + i);
		m_pEntityMngr->SetModelMatrix(glm::translate(vector3(currX, currY, currZ)));
	}
#pragma endregion GenerateSpheres

#pragma region GenerateCylinders
	// GIVENS
	// height 7, width ?
	uint numHolesPerRing = 36;
	uint numHoles = numHolesPerRing * 5;
	uint numPegs = numHoles / 2;// numPegs <= numHoles / 2
	float ringDiameter = cylinderDiameter;
	float ringBaseHeight = 0;
	float ringHeightIncrement = 1.5f;
	float minPegAngle = 140;// 0 - 180
	float pegLength = /*ringDiameter * 1.5f / 8*/1;

	// PRE-CALCULATED VALUES
	float ringRadius = ringDiameter / 2;
	float holeAngleIncrement = 2 * PI / (float)numHolesPerRing;
	float ringHalfHeight = ringHeightIncrement / 2;
	float rad2Deg = 180 / PI;
	float minHorizontalHoleLength = ringRadius * sinf(minPegAngle / rad2Deg) / sinf((90 - minPegAngle / 2) / rad2Deg);// (r * sin(T)) / (sin(90 - T / 2))

	// HOLE CENTERPOINT LOCATIONS
	std::vector<vector3> holes;
	for (uint i = 0; i < numHoles; i++) {
		uint holeIndexInRing = i % numHolesPerRing;
		uint holeRingIndex = i / numHolesPerRing;
		float holeAngle = holeAngleIncrement * holeIndexInRing;
		holes.push_back(vector3(
			cosf(holeAngle) * ringRadius,// X
			ringBaseHeight + ringHeightIncrement * holeRingIndex,// Y
			sinf(holeAngle) * ringRadius// Z
		));

		//// DRAW HOLES
		//m_pEntityMngr->AddEntity("Knulprek//Sphere.fbx", cylinder);
		//m_pEntityMngr->SetModelMatrix(glm::translate(holes[i]) * glm::scale(vector3(0.3f, 0.5f, 0.3f)));
	}
	std::vector<vector3> shuffledHoles = holes;
	std::random_shuffle(shuffledHoles.begin(), shuffledHoles.end());


	// ARRANGE LIST OF HOLES
	for (uint i = 0; i < numPegs * 2; i += 2)
	{
		vector3 hole1 = shuffledHoles[i];
		vector3 hole2;
		float holeXDiff;
		float holeZDiff;
		float horizontalLength;

		for (uint j = i + 1; j < numHoles; j++)
		{
			hole2 = shuffledHoles[j];
			holeXDiff = (hole2.x - hole1.x);
			holeZDiff = (hole2.z - hole1.z);
			horizontalLength = sqrtf(powf(holeXDiff, 2) + powf(holeZDiff, 2));
			if (horizontalLength >= minHorizontalHoleLength)
			{
				if (j != i + 1)
				{
					vector3 temp = shuffledHoles[j];
					shuffledHoles[j] = shuffledHoles[i + 1];
					shuffledHoles[i + 1] = temp;
				}
				break;
			}
		}
	}

	// PEG TRANSFORMATION CALCULATIONS
	for (uint i = 0; i < numPegs * 2; i += 2)
	{
		vector3 hole1 = shuffledHoles[i];
		vector3 hole2 = shuffledHoles[i + 1];

		// calculated variables pt 1
		float holeXDiff = (hole2.x - hole1.x);;
		float holeZDiff = (hole2.z - hole1.z);;
		float horizontalLength = sqrtf(powf(holeXDiff, 2) + powf(holeZDiff, 2));;

		// if we couldn't find a hole far enough away
		if (horizontalLength < minHorizontalHoleLength)
			continue;

		// calculated variables pt 2
		float verticalLength = (hole2.y - hole1.y);
		float horizontalAngle = atanf(holeXDiff / holeZDiff);
		float verticalAngle = atanf(horizontalLength / verticalLength);
		float distanceToCenter = abs(hole2.x * hole1.z - hole2.z * hole1.x) / horizontalLength;

		// final transformations
		glm::mat4 pegRotation = glm::rotate(horizontalAngle * rad2Deg, vector3(0, 1, 0)) * glm::rotate(verticalAngle * rad2Deg, vector3(1, 0, 0));
		glm::mat4 pegVerticalRotation = glm::rotate(verticalAngle * rad2Deg, vector3(1, 0, 0));
		glm::mat4 pegHorizontalRotation = glm::rotate(horizontalAngle * rad2Deg, vector3(0, 1, 0));
		glm::mat4 pegTranslation = glm::translate(vector3(
			-distanceToCenter * cosf(horizontalAngle),
			hole1.y + verticalLength / 2,
			distanceToCenter * sinf(horizontalAngle)
		));
		glm::mat4 pegScale = glm::scale(vector3(1, pegLength, 1));

		// generate pegs
		m_pEntityMngr->AddEntity("Knulprek//Cylinder.fbx", cylinder, "Peg" + i);
		m_pEntityMngr->SetModelMatrix(pegTranslation * pegHorizontalRotation * pegVerticalRotation * pegScale);

		tags.push_back("Peg" + i);
	}

	selectedPeg = 0;
#pragma endregion GenerateCylinders

	m_pEntityMngr->AddEntity("Knulprek//Cylinder.fbx", inverseCylinder);

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

	m_pMeshMngr->AddCylinderToRenderList(m_pEntityMngr->GetEntity(m_pEntityMngr->GetEntityIndex(tags.at(selectedPeg)))->GetModelMatrix() * glm::scale(vector3(1, 16, 1)), vector3(1, 0, 1), RENDER_WIRE);

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