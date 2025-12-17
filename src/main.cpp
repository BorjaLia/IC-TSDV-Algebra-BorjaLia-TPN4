#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <string>
#include <cmath>

#include "AABB.h"
#include "Frustum.h"
#include "SceneObject.h"
#include "main.h"

static bool IsMeshInFrustum(Frustum& frustum, SceneObject& object);

int main()
{
	const int screenWidth = 1600;
	const int screenHeight = 900;

	InitWindow(screenWidth, screenHeight, "Frustum Culling");
	SetWindowState(FLAG_WINDOW_RESIZABLE);

	Camera camera = { 0 };
	camera.position = { 20.0f, 15.0f, 20.0f };
	camera.target = { 0.0f, 0.0f, 0.0f };
	camera.up = { 0.0f, 1.0f, 0.0f };
	camera.fovy = 45.0f;
	camera.projection = CAMERA_PERSPECTIVE;

	float nearPlane = 2.5f;
	float farPlane = 50.0f;

	std::vector<SceneObject> sceneObjects;
	Frustum cameraFrustum;

	SceneObject obj1;
	obj1.model = LoadModel("res/decahedron.obj");
	obj1.position = { 5.0f, 5.0f, 5.0f };
	obj1.aabb = CalculateLocalAABB(*obj1.model.meshes);
	sceneObjects.push_back(obj1);

	SceneObject obj2;
	obj2.model = LoadModel("res/dodecahedron.obj");
	obj2.position = { 3.0f, 2.0f, 0.0f };
	obj2.aabb = CalculateLocalAABB(*obj2.model.meshes);
	sceneObjects.push_back(obj2);

	for (int i = 0; i < 5; i++)
	{
		SceneObject objRand;
		objRand.model = LoadModel("res/dodecahedron.obj");
		objRand.position = { (float)(GetRandomValue(-50, 50)), 0.0f, (float)(GetRandomValue(-50, 50)) };
		objRand.aabb = CalculateLocalAABB(*objRand.model.meshes);
		sceneObjects.push_back(objRand);
	}

	DisableCursor();

	while (!WindowShouldClose())
	{
		Inputs(camera, nearPlane, farPlane);

		float aspectRatio = (float)GetScreenWidth() / (float)GetScreenHeight();
		int visibleCount = 0;

		Update(cameraFrustum, camera, aspectRatio, nearPlane, farPlane, sceneObjects, visibleCount);

		Draw(camera, sceneObjects, visibleCount, nearPlane, farPlane);
	}

	for (int i = 0; i < sceneObjects.size() - 1; i++)
	{
		SceneObject& obj = sceneObjects[i];
		if (IsModelValid(obj.model))
		{
			UnloadModel(obj.model);
		}
	}

	CloseWindow();

	return 0;
}

void Update(Frustum& cameraFrustum, const Camera& camera, float aspectRatio, float nearPlane, float farPlane, std::vector<SceneObject>& sceneObjects, int& visibleCount)
{
	UpdateFrustum(cameraFrustum, camera, aspectRatio, nearPlane, farPlane);

	for (int i = 0; i < sceneObjects.size(); i++)
	{
		Matrix matTranslate = MatrixTranslate(sceneObjects[i].position.x, sceneObjects[i].position.y, sceneObjects[i].position.z);
		MyAABB worldAABB = GetUpdatedAABB(sceneObjects[i].aabb, matTranslate);

		if (IsAABBInFrustum(cameraFrustum, worldAABB))
		{
			sceneObjects[i].isVisible = IsMeshInFrustum(cameraFrustum, sceneObjects[i]);
		}
		else {
			sceneObjects[i].isVisible = false;
		}

		if (sceneObjects[i].isVisible)
		{
			visibleCount++;
		}
	}
}

void Draw(Camera& camera, std::vector<SceneObject>& sceneObjects, int visibleCount, float nearPlane, float farPlane)
{
	BeginDrawing();
	ClearBackground(RAYWHITE);

	BeginMode3D(camera);

	DrawGrid(100, 1.0f);

	for (int i = 0; i < sceneObjects.size(); i++)
	{
		Matrix matTranslate = MatrixTranslate(sceneObjects[i].position.x, sceneObjects[i].position.y, sceneObjects[i].position.z);
		MyAABB worldAABB = GetUpdatedAABB(sceneObjects[i].aabb, matTranslate);

		if (sceneObjects[i].isVisible)
		{
			DrawModel(sceneObjects[i].model, sceneObjects[i].position, 1.0f, RED);
			DrawModelWires(sceneObjects[i].model, sceneObjects[i].position, 1.0f, MAROON);
			DrawAABB(worldAABB, BLUE);
		}
		else
		{
			DrawModelWires(sceneObjects[i].model, sceneObjects[i].position, 1.0f, LIGHTGRAY);
			DrawAABB(worldAABB, GRAY);
		}
	}

	EndMode3D();

	DrawText(TextFormat("Visible objects: %d/%d", visibleCount, sceneObjects.size()), 10, 10, 20, BLACK);
	DrawText("WASD to Move, Mouse to Look, Q/E to Change FOV", 10, 35, 15, DARKGRAY);
	DrawText("R/F to Change NearPlane, T/G to Change FarPlane", 10, 55, 15, DARKGRAY);

	DrawText("FOV:", 10, 80, 15, DARKGRAY);
	DrawText(TextFormat("%s", std::to_string((int)camera.fovy).c_str()), 50, 80, 15, BLACK);
	DrawText("Planes:", 10, 95, 15, DARKGRAY);
	DrawText(TextFormat("%s", std::to_string((int)nearPlane).c_str()), 10, 125, 15, BLACK);
	DrawText(TextFormat("%s", std::to_string((int)farPlane).c_str()), 10, 150, 15, BLACK);

	EndDrawing();
}

void Inputs(Camera& camera, float& nearPlane, float& farPlane)
{
	UpdateCamera(&camera, CAMERA_FREE);

	if (IsKeyPressed(KEY_Q))
	{
		camera.fovy += 2.0f;
	}
	if (IsKeyPressed(KEY_E))
	{
		camera.fovy -= 2.0f;
	}
	camera.fovy = Clamp(camera.fovy, 1.0f, 180.0f);

	if (IsKeyPressed(KEY_R))
	{
		nearPlane++;
	}
	if (IsKeyPressed(KEY_F))
	{
		nearPlane--;
	}
	nearPlane = Clamp(nearPlane, 0.0f, farPlane - 1.0f);

	if (IsKeyPressed(KEY_T))
	{
		farPlane++;
	}
	if (IsKeyPressed(KEY_G))
	{
		farPlane--;
	}
	farPlane = Clamp(farPlane, nearPlane + 1.0f, farPlane);
}

bool IsMeshInFrustum(Frustum& frustum, SceneObject& object)
{
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < object.model.meshes[0].vertexCount; j += 3)
		{
			Vector3 v0 = { object.model.meshes[0].vertices[(j + 0)], object.model.meshes[0].vertices[(j + 1)], object.model.meshes[0].vertices[(j + 2)] };
			float dot = Vector3DotProduct(frustum.planes[i].normal, v0);

			if (dot > EPSILON) {
				return true;
			}
		}
	}
	return false;
}
