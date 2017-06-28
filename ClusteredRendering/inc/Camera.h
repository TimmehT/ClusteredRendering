#pragma once


struct CameraData
{
	XMFLOAT4X4 viewMat;
	XMFLOAT4X4 projMat;
	XMFLOAT4X4 viewProjMat;
};

class Camera
{
public:
	Camera();
	~Camera();
	
	void Update();
	void SetLens(float fov, float nearPlane, float farPlane, unsigned int width, unsigned int height);
	void Pitch(float angle);
	void Yaw(float angle);
	void MoveForward(float dt);
	void MoveBackward(float dt);
	void MoveUp(float dt);
	void MoveDown(float dt);
	void MoveLeft(float dt);
	void MoveRight(float dt);
	
	CameraData& GetCamData();
	
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetRight();


private:
	XMFLOAT3 m_up;
	XMFLOAT3 m_right;
	XMFLOAT3 m_look;
	XMFLOAT3 m_position;

	float m_camSpeed;
	float m_camSensitivity;
	CameraData m_camData;

};
