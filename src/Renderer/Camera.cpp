#include "Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(const glm::vec3& position, const glm::vec3& up, float yaw, float pitch)
	: mFront(glm::vec3(0.0f, 0.0f, -1.0f)), mMovementSpeed(DEFAULT_SPEED), mMouseSensitivity(DEFAULT_SENSITIVITY), mZoom(DEFAULT_ZOOM)
{
	mPosition = position;
	mWorldUp = up;
	mYaw = yaw;
	mPitch = pitch;

	UpdateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
	: mFront(glm::vec3(0.0f, 0.0f, -1.0f)), mMovementSpeed(DEFAULT_SPEED), mMouseSensitivity(DEFAULT_SENSITIVITY), mZoom(DEFAULT_ZOOM)
{
	mPosition = glm::vec3(posX, posY, posZ);
	mWorldUp = glm::vec3(upX, upY, upZ);
	mYaw = yaw;
	mPitch = pitch;
	UpdateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() const
{
	return glm::lookAt(mPosition, mPosition + mFront, mUp);
}

void Camera::ProcessKeyboard(Direction direction, float deltaTime)
{
	float velocity = mMovementSpeed * deltaTime;
	if (direction == Direction::Forward)
		mPosition += mFront * velocity;
	if (direction == Direction::Backward)
		mPosition -= mFront * velocity;
	if (direction == Direction::Left)
		mPosition -= mRight * velocity;
	if (direction == Direction::Right)
		mPosition += mRight * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
	xoffset *= mMouseSensitivity * mZoom / 45.0f;
	yoffset *= mMouseSensitivity * mZoom / 45.0f;

	mYaw += xoffset;
	mPitch += yoffset;

	if (constrainPitch)
	{
		if (mPitch > 89.0f)
			mPitch = 89.0f;
		if (mPitch < -89.0f)
			mPitch = -89.0f;
	}

	UpdateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
{
	mZoom -= yoffset;
	if (mZoom < 1.0f)
		mZoom = 1.0f;
	if (mZoom > 45.0f)
		mZoom = 45.0f;
}

void Camera::UpdateCameraVectors()
{
	glm::vec3 nextFront;
	nextFront.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
	nextFront.y = sin(glm::radians(mPitch));
	nextFront.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));
	mFront = glm::normalize(nextFront);
	mRight = glm::normalize(glm::cross(mFront, mWorldUp));
	mUp = glm::normalize(glm::cross(mRight, mFront));
}
