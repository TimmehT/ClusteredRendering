#include <DirectXPCH.h>
#include "Camera.h"

using namespace EngineMath;

Camera::Camera()
	: m_position(0.0f, 0.0f, -10.0f),
	m_right(1.0f, 0.0f, 0.0f),
	m_up(0.0f, 1.0f, 0.0f),
	m_look(0.0f, 0.0f, 1.0f),
	m_camSpeed(3.0f),
	m_camSensitivity(2.9f)
{	
}

Camera::~Camera()
{
}

void Camera::Update()
{
	XMVECTOR pos = FloatToVector(m_position);
	XMVECTOR look = FloatToVector(m_look);
	XMVECTOR up = FloatToVector(m_up);

	XMMATRIX view = Float4X4ToMatrix(m_camData.viewMat);
	XMMATRIX proj = Float4X4ToMatrix(m_camData.projMat);

	XMStoreFloat4x4(&m_camData.viewMat,	XMMatrixLookAtLH(pos, pos + look, up));
	XMStoreFloat4x4(&m_camData.viewProjMat, view * proj);
}

void Camera::SetLens(float fov, float nearPlane, float farPlane, unsigned int width, unsigned int height)
{
	XMStoreFloat4x4(&m_camData.projMat, XMMatrixPerspectiveFovLH(fov, (float)width/(float)height, nearPlane, farPlane));
}

void Camera::Pitch(float angle)
{
	XMVECTOR right = FloatToVector(m_right);
	XMVECTOR look = FloatToVector(m_look);
	XMVECTOR up = FloatToVector(m_up);
	XMMATRIX rot = XMMatrixRotationAxis(right, angle * m_camSensitivity);

	XMStoreFloat3(&m_look, XMVector3TransformNormal(look, rot));
	XMStoreFloat3(&m_up, XMVector3TransformNormal(up, rot));
	
}

void Camera::Yaw(float angle)
{
	XMVECTOR right = FloatToVector(m_right);
	XMVECTOR look = FloatToVector(m_look);
	XMVECTOR up = FloatToVector(m_up);
	XMMATRIX rot = XMMatrixRotationAxis(FloatToVector(XMFLOAT3(0, 1, 0)), angle * m_camSensitivity);

	XMStoreFloat3(&m_look, XMVector3TransformNormal(look, rot));
	XMStoreFloat3(&m_up, XMVector3TransformNormal(up, rot));
	XMStoreFloat3(&m_right, XMVector3TransformNormal(right, rot));
}

void Camera::MoveForward(float dt)
{
	XMVECTOR pos = FloatToVector(m_position);
	XMVECTOR look = FloatToVector(m_look);

	pos += m_camSpeed * dt * look;
	XMStoreFloat3(&m_position, pos);
}

void Camera::MoveBackward(float dt)
{
	XMVECTOR pos = FloatToVector(m_position);
	XMVECTOR look = FloatToVector(m_look);

	pos -= m_camSpeed * dt * look;
	XMStoreFloat3(&m_position, pos);
}

void Camera::MoveUp(float dt)
{
	XMVECTOR pos = FloatToVector(m_position);
	XMVECTOR up = FloatToVector(m_up);

	pos += m_camSpeed * dt * up;
	XMStoreFloat3(&m_position, pos);
}

void Camera::MoveDown(float dt)
{
	XMVECTOR pos = FloatToVector(m_position);
	XMVECTOR up = FloatToVector(m_up);

	pos -= m_camSpeed * dt * up;
	XMStoreFloat3(&m_position, pos);
}

void Camera::MoveLeft(float dt)
{
	XMVECTOR pos = FloatToVector(m_position);
	XMVECTOR right = FloatToVector(m_right);

	pos -= m_camSpeed * dt * right;
	XMStoreFloat3(&m_position, pos);
}

void Camera::MoveRight(float dt)
{
	XMVECTOR pos = FloatToVector(m_position);
	XMVECTOR right = FloatToVector(m_right);

	pos += m_camSpeed * dt * right;
	XMStoreFloat3(&m_position, pos);
}

const CameraData & Camera::GetCamData()
{
	return m_camData;
}



