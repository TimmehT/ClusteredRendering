#include <DirectXPCH.h>
#include "Camera.h"


Camera::Camera()
	: m_position(0.0f, 0.0f, 0.0f),
	m_right(1.0f, 0.0f, 0.0f),
	m_up(0.0f, 1.0f, 0.0f),
	m_look(0.0f, 0.0f, 1.0f)
{
	
}

Camera::~Camera()
{
}

XMVECTOR Camera::GetPositionXM() const
{
	return XMLoadFloat3(&m_position);
}

XMFLOAT3 Camera::GetPosition() const
{
	return m_position;
}

void Camera::SetPosition(float x, float y, float z)
{
	m_position = XMFLOAT3(x, y, z);
}

void Camera::SetPosition(const XMFLOAT3 & v)
{
	m_position = v;
}

XMVECTOR Camera::GetRightXM() const
{
	return XMLoadFloat3(&m_right);
}

XMFLOAT3 Camera::GetRight() const
{
	return m_right;
}

XMVECTOR Camera::GetUpXM() const
{
	return XMLoadFloat3(&m_up);
}

XMFLOAT3 Camera::GetUp() const
{
	return m_up;
}

XMVECTOR Camera::GetLookXM() const
{
	return XMLoadFloat3(&m_look);
}

XMFLOAT3 Camera::GetLook() const
{
	return m_look;
}

float Camera::GetNearZ() const
{
	return m_nearZ;
}

float Camera::GetFarZ() const
{
	return m_farZ;
}

float Camera::GetAspect() const
{
	return m_aspect;
}

float Camera::GetFovY() const
{
	return m_fovY;
}

float Camera::GetFovX() const
{
	float halfWidth = 0.5f*GetNearWindowWidth();
	return 2.0f*atan(halfWidth / m_nearZ);
}

float Camera::GetNearWindowWidth() const
{
	return m_aspect * m_nearWindowHeight;
}

float Camera::GetNearWindowHeight() const
{
	return m_nearWindowHeight;
}

float Camera::GetFarWindowWidth() const
{
	return m_aspect * m_farWindowHeight;
}

float Camera::GetFarWindowHeight() const
{
	return m_farWindowHeight;
}

void Camera::SetFrustum(float fovY, float aspect, float zn, float zf)
{
	m_fovY = fovY;
	m_aspect = aspect;
	m_nearZ = zn;
	m_farZ = zf;

	m_nearWindowHeight = 2.0f * m_nearZ * tanf(0.5f*m_fovY);
	m_farWindowHeight = 2.0f * m_farZ * tanf(0.5f*m_fovY);

	XMMATRIX proj = XMMatrixPerspectiveLH(m_fovY, m_aspect, m_nearZ, m_farZ);
	XMStoreFloat4x4(&m_proj, proj);
}

void Camera::LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp)
{
	XMVECTOR look = XMVector3Normalize(XMVectorSubtract(target, pos));
	XMVECTOR right = XMVector3Normalize(XMVector3Cross(worldUp, look));
	XMVECTOR up = XMVector3Cross(look, right);

	XMStoreFloat3(&m_position, pos);
	XMStoreFloat3(&m_look, look);
	XMStoreFloat3(&m_right, right);
	XMStoreFloat3(&m_up, up);
}

void Camera::LookAt(const XMFLOAT3 & pos, const XMFLOAT3 & target, const XMFLOAT3 & up)
{
	XMVECTOR p = XMLoadFloat3(&pos);
	XMVECTOR t = XMLoadFloat3(&target);
	XMVECTOR u = XMLoadFloat3(&up);

	LookAt(p, t, u);
}

XMMATRIX Camera::View() const
{
	return XMLoadFloat4x4(&m_view);
}

XMMATRIX Camera::Proj() const
{
	return XMLoadFloat4x4(&m_proj);
}

XMMATRIX Camera::ViewProj() const
{
	return XMMatrixMultiply(View(),Proj());
}

void Camera::Move(XMFLOAT3 dir)
{
}

void Camera::Rotate(XMFLOAT3 axis, float deg)
{
}

void Camera::UpdateViewMatrix()
{
	XMVECTOR r = XMLoadFloat3(&m_right);
	XMVECTOR u = XMLoadFloat3(&m_up);
	XMVECTOR l = XMLoadFloat3(&m_look);
	XMVECTOR p = XMLoadFloat3(&m_position);

	l = XMVector3Normalize(l);
	u = XMVector3Normalize(XMVector3Cross(l, r));
	r = XMVector3Cross(u, l);

	float x = -XMVectorGetX(XMVector3Dot(p, r));
	float y = -XMVectorGetX(XMVector3Dot(p, u));
	float z = -XMVectorGetX(XMVector3Dot(p, l));

	XMStoreFloat3(&m_right, r);
	XMStoreFloat3(&m_up, u);
	XMStoreFloat3(&m_look, l);

	m_view(0, 0) = m_right.x;
	m_view(1, 0) = m_right.y;
	m_view(2, 0) = m_right.z;
	m_view(3, 0) = x;

	m_view(0, 1) = m_up.x;
	m_view(1, 1) = m_up.y;
	m_view(2, 1) = m_up.z;
	m_view(3, 1) = y;

	m_view(0, 2) = m_look.x;
	m_view(1, 2) = m_look.y;
	m_view(2, 2) = m_look.z;
	m_view(3, 2) = z;

	m_view(0, 3) = 0.0f;
	m_view(1, 3) = 0.0f;
	m_view(2, 3) = 0.0f;
	m_view(3, 3) = 1.0f;
}

