#pragma once

void Inputs(Camera& camera, float& nearPlane, float& farPlane);

void Draw(Camera& camera, std::vector<SceneObject>& sceneObjects, int visibleCount, float nearPlane, float farPlane);

void Update(Frustum& cameraFrustum, const Camera& camera, float aspectRatio, float nearPlane, float farPlane, std::vector<SceneObject>& sceneObjects, int& visibleCount);
