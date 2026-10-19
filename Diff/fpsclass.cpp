#include "fpsclass.h"


FpsClass::FpsClass(){}

FpsClass::FpsClass(const FpsClass& other){}

FpsClass::~FpsClass(){}

void FpsClass::Initialize()
{
	m_fps = 0;
	m_cout = 0; 
	m_startTime = timeGetTime();

}


void FpsClass::Frame()
{
	m_cout++;

	if(timeGetTime() >= (m_startTime + 1000))
	{
		m_fps = m_cout;
		m_cout = 0;

		m_startTime = timeGetTime();
	}
}

int FpsClass::GetFps()
{
	return m_fps;
}






