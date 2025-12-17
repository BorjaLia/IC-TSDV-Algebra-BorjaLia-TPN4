#pragma once

#include "raylib.h"
#include "raymath.h"
#include "AABB.h"

struct Plane
{
	Vector3 normal;
	float distance;
};

struct Frustum
{
	Plane planes[6];
};

bool IsAABBInFrustum(Frustum& frustum, MyAABB& aabb);
void UpdateFrustum(Frustum& frustum, Camera camera, float aspect, float nearDist, float farDist);