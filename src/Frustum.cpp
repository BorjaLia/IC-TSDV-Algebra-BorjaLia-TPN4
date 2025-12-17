#include "Frustum.h"

bool IsAABBInFrustum(Frustum& frustum, MyAABB& aabb)
{
	for (int i = 0; i < 6; i++)
	{
		Vector3 vertex;

		if (frustum.planes[i].normal.x > 0)
		{
			vertex.x = aabb.max.x;
		}
		else
		{
			vertex.x = aabb.min.x;
		}

		if (frustum.planes[i].normal.y > 0)
		{
			vertex.y = aabb.max.y;
		}
		else
		{
			vertex.y = aabb.min.y;
		}

		if (frustum.planes[i].normal.z > 0)
		{
			vertex.z = aabb.max.z;
		}
		else
		{
			vertex.z = aabb.min.z;
		}

		if ((Vector3DotProduct(frustum.planes[i].normal, vertex) + frustum.planes[i].distance) < 0)
		{
			return false;
		}
	}
	return true;
}

void UpdateFrustum(Frustum& frustum, Camera camera, float aspect, float nearDist, float farDist)
{
	Vector3 forward = Vector3Subtract(camera.target, camera.position);
	forward = Vector3Normalize(forward);

	Vector3 right = Vector3CrossProduct(forward, camera.up);
	right = Vector3Normalize(right);

	Vector3 up = Vector3CrossProduct(right, forward);

	float halfHeight = tanf(camera.fovy * 0.5f * DEG2RAD);
	float halfWidth = halfHeight * aspect;

	Vector3 topSlope = Vector3Add(forward, Vector3Scale(up, halfHeight));
	frustum.planes[3].normal = Vector3Normalize(Vector3CrossProduct(topSlope, right)); // plano arriba

	Vector3 botSlope = Vector3Subtract(forward, Vector3Scale(up, halfHeight));
	frustum.planes[2].normal = Vector3Normalize(Vector3CrossProduct(right, botSlope));// plano abajo

	Vector3 leftSlope = Vector3Subtract(forward, Vector3Scale(right, halfWidth));
	frustum.planes[0].normal = Vector3Normalize(Vector3CrossProduct(leftSlope, up));// plano izquierda

	Vector3 rightSlope = Vector3Add(forward, Vector3Scale(right, halfWidth));
	frustum.planes[1].normal = Vector3Normalize(Vector3CrossProduct(up, rightSlope));// plano derecha

	frustum.planes[4].normal = forward;// plano near
	frustum.planes[5].normal = Vector3Negate(forward); // plano far

	frustum.planes[0].distance = -Vector3DotProduct(frustum.planes[0].normal, camera.position);
	frustum.planes[1].distance = -Vector3DotProduct(frustum.planes[1].normal, camera.position);
	frustum.planes[2].distance = -Vector3DotProduct(frustum.planes[2].normal, camera.position);
	frustum.planes[3].distance = -Vector3DotProduct(frustum.planes[3].normal, camera.position);

	Vector3 nearCenter = Vector3Add(camera.position, Vector3Scale(forward, nearDist));
	Vector3 farCenter = Vector3Add(camera.position, Vector3Scale(forward, farDist));

	frustum.planes[4].distance = -Vector3DotProduct(frustum.planes[4].normal, nearCenter);
	frustum.planes[5].distance = -Vector3DotProduct(frustum.planes[5].normal, farCenter);
}