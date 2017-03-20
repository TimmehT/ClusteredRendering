#pragma once

class Direct3DManager
{
public:
	Direct3DManager();
	Direct3DManager(const Direct3DManager&);
	~Direct3DManager();

	bool Initialize();
	void Unitialize();
};
