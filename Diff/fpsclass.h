#ifndef _FPSCLASS_
#define _FPSCLASS_

#pragma comment(lib, "winmm.lib")

#include <Windows.h>
#include <MMSystem.h>

class FpsClass
{
public:
	FpsClass();
	FpsClass(const FpsClass&);
	~FpsClass();

	void Initialize();
	void Frame();
	int GetFps();

private:
	int m_fps, m_cout;
	unsigned long m_startTime;

};
#endif