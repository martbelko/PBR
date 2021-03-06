#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
	enum class Direction {
		Forward,
		Backward,
		Left,
		Right
	};
public:
	Camera(const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f), const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = DEFAULT_YAW, float pitch = DEFAULT_PITCH);
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

	float GetZoom() const { return mZoom; }
	const glm::vec3& GetPosition() const { return mPosition; }
	const float GetSpeed() const { return mMovementSpeed; }
	const float GetMouseSensitivity() const { return mMouseSensitivity; }
	glm::mat4 GetViewMatrix() const;

	void SetSpeed(float speed) { mMovementSpeed = speed; }
	void SetMouseSensitivity(float mouseSensitivity) { mMouseSensitivity = mouseSensitivity; }
	void SetPosition(const glm::vec3& position);

	void ProcessKeyboard(Direction direction, float deltaTime);
	void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
	void ProcessMouseScroll(float yoffset);
private:
	void UpdateCameraVectors();
private:
	glm::vec3 mPosition;
	glm::vec3 mFront;
	glm::vec3 mUp;
	glm::vec3 mRight;
	glm::vec3 mWorldUp;

	float mYaw;
	float mPitch;
	float mMovementSpeed;
	float mMouseSensitivity;
	float mZoom;
private:
	static constexpr float DEFAULT_YAW = -90.0f;
	static constexpr float DEFAULT_PITCH = 0.0f;
	static constexpr float DEFAULT_SPEED = 20.0f;
	static constexpr float DEFAULT_SENSITIVITY = 0.1f;
	static constexpr float DEFAULT_ZOOM = 45.0f;
};