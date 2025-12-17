#pragma once

#include "raylib.h"
#include "raymath.h"

struct MyAABB
{
	Vector3 min;
	Vector3 max;
};

MyAABB CalculateLocalAABB(Mesh mesh);
MyAABB GetUpdatedAABB(MyAABB localBB, Matrix transform);
void DrawAABB(MyAABB aabb, Color color);
