#pragma once
//#include <DirectXPCH.h>

class Camera
{
public:
	Camera();
	~Camera();

	// Get & set world cam position
	XMVECTOR GetPositionXM()const;
	XMFLOAT3 GetPosition()const;
	void SetPosition(float x, float y, float z);
	void SetPosition(const XMFLOAT3& v);

	// Get Camera vectors
	XMVECTOR GetRightXM()const;
	XMFLOAT3 GetRight()const;
	XMVECTOR GetUpXM()const;
	XMFLOAT3 GetUp()const;
	XMVECTOR GetLookXM()const;
	XMFLOAT3 GetLook()const;

	// Get frustum proerties
	float GetNearZ()const;
	float GetFarZ()const;
	float GetAspect()const;
	float GetFovY()const;
	float GetFovX()const;

	// Get near and far plane dimensions in view space coordinates.
	float GetNearWindowWidth()const;
	float GetNearWindowHeight()const;
	float GetFarWindowWidth()const;
	float GetFarWindowHeight()const;

	// Set frustum.
	void SetFrustum(float fovY, float aspect, float zn, float zf);

	// Define camera space via LookAt parameters.
	void LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp);
	void LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up);


	// Get view & proj matrices
	XMMATRIX View()const;
	XMMATRIX Proj()const;
	XMMATRIX ViewProj()const;

	void Move(XMFLOAT3 dir);

	void Rotate(XMFLOAT3 axis, float deg);

	void UpdateViewMatrix();

private:
	// Camera parameters
	XMFLOAT3 m_position;
	XMFLOAT3 m_right;
	XMFLOAT3 m_up;
	XMFLOAT3 m_look;

	// Frustum properties
	float m_nearZ;
	float m_farZ;
	float m_aspect;
	float m_fovY;
	float m_nearWindowHeight;
	float m_farWindowHeight;

	// view/proj matrices
	XMFLOAT4X4 m_view;
	XMFLOAT4X4 m_proj;
};
