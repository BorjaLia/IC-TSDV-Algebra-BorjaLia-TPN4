#pragma once

struct SceneObject
{
	Model model;
	Vector3 position;
	MyAABB aabb;
	bool isVisible;
};