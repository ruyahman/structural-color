
#include "systemclass.h"


SystemClass::SystemClass()
{
    m_applicationName = L"Diff";
    m_hinstance = nullptr;
    m_hwnd = nullptr;
	m_Input = 0;
	m_Graphics = 0;
	m_Fps = 0;
	m_Cpu = 0;
	m_Timer = 0;
	m_Position = 0;
}

//コピー



SystemClass::~SystemClass()
{
    Shutdown();
}


bool SystemClass::Initialize()
{
	int screenWidth, screenHeight;
	bool result;

	screenWidth = 0;
	screenHeight = 0;

	InitializeWindows(screenWidth, screenHeight);

	m_Input = new InputClass;
	if(!m_Input)
	{
		return false;
	}

	result = m_Input->Initialize(m_hinstance, m_hwnd, screenWidth, screenHeight);
	if(!result)
	{
		MessageBox(m_hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}

	m_Graphics = new GraphicsClass;
	if(!m_Graphics)
	{
		return false;
	}

	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if(!result)
	{
		return false;
	}

	m_Fps = new FpsClass;
	if(!m_Fps)
	{
		return false;
	}
	m_Fps->Initialize();

	m_Cpu = new CpuClass();
	if(!m_Cpu)
	{
		return false;
	}

	m_Cpu->Initialize();

	m_Timer = new TimerClass;
	if(!m_Timer)
	{
		return false;
	}

	result = m_Timer->Initialize();
	if(!result)
	{
		MessageBox(m_hwnd, L"Could not initialize the Timer object.", L"Error", MB_OK);
		return false;
	}

	m_Position = new PositionClass;
	if(!m_Position)
	{
		return false;
	}


	return true;
}


void SystemClass::Shutdown()
{
	if(m_Position)
	{
		delete m_Position;
		m_Position = 0;
	}

	if(m_Timer)
	{
		delete m_Timer;
		m_Timer = 0;
	}

	if(m_Cpu)
	{
		m_Cpu->Shutdown();
		delete m_Cpu;
		m_Cpu = 0;
	}

	if(m_Fps)
	{
		delete m_Fps;
		m_Fps = 0;
	}

	if(m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	if(m_Input)
	{
		m_Input->Shutdown();
		delete m_Input;
		m_Input = 0;
	}

	ShutdownWindows();
	
	return;
}


void SystemClass::Run()
{
	MSG msg;
	bool done, result;


	ZeroMemory(&msg, sizeof(MSG));
	
	done = false;
	while(!done)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if(msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			result = Frame();
			if(!result)
			{
				done = true;
			}
		}
	
		if(m_Input->IsKeyDown(DIK_ESCAPE))
		{
			done = true;
		}



	}

	return;
}


bool SystemClass::Frame()
{
    m_Timer->Frame();
    m_Fps->Frame();
    m_Cpu->Frame();
    if (!m_Input->Frame())
        return false;

    PositionClass::Controls controls;
    controls.pitch = float(m_Input->IsKeyDown(DIK_UP)) - float(m_Input->IsKeyDown(DIK_DOWN));
    controls.yaw = float(m_Input->IsKeyDown(DIK_LEFT)) - float(m_Input->IsKeyDown(DIK_RIGHT));
    controls.moveX = float(m_Input->IsKeyDown(DIK_D)) - float(m_Input->IsKeyDown(DIK_A));
    controls.moveZ = float(m_Input->IsKeyDown(DIK_W)) - float(m_Input->IsKeyDown(DIK_S));
    controls.increasePatternFrequency = m_Input->IsKeyDown(DIK_SPACE);
    controls.reset = m_Input->IsKeyDown(DIK_R);
    m_Position->Update(m_Timer->GetTime() / 1000.0f, controls);
    if (controls.reset)
        m_Input->ResetView();

    int mouseX, mouseY;
    float yaw, pitch;
    m_Input->GetMouseLocation(mouseX, mouseY);
    m_Input->GetCameraYawPitch(yaw, pitch);
    if (!m_Graphics->Frame(mouseX, mouseY, m_Position->GetCameraOffset(), yaw, pitch,
        m_Fps->GetFps(), m_Cpu->GetCpuPercentage(), m_Position->GetPatternFrequency()))
        return false;
    return m_Graphics->Render(m_Position->GetModelRotation());
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}

//ウィンドウ設定
void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	ZeroMemory(&wc,sizeof(wc));

	ApplicationHandle = this;

	m_hinstance = GetModuleHandle(NULL);

	m_applicationName = L"Diff";

	wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = m_hinstance;
	wc.hIcon		 = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm       = wc.hIcon;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	//wc.lpszMenuName  = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize        = sizeof(WNDCLASSEX);
	
	RegisterClassEx(&wc);

	screenWidth  = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	if(FULL_SCREEN)
	{
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth  = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;			
		dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		posX = posY = 0;
	}
	else
	{
	
		screenWidth  = 800;
		screenHeight = 600;

		// 真ん中に表示
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth)  / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	m_hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, m_applicationName, m_applicationName, 
						    WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
						    posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	 //ウィンドウの作成
	/*m_hwnd=CreateWindow(m_applicationName,m_applicationName,WS_OVERLAPPEDWINDOW,
		 0,0,screenWidth,screenHeight,0,0,m_hinstance,0);*/

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	//ShowCursor(false);

	return;
}


void SystemClass::ShutdownWindows()
{


	if(FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	if (m_hwnd) DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	if (m_hinstance) UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	ApplicationHandle = NULL;

	return;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch(umessage)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		case WM_CLOSE:
		{
			PostQuitMessage(0);		
			return 0;
		}

		default:
		{
			return ApplicationHandle ? ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam)
                : DefWindowProc(hwnd, umessage, wparam, lparam);
		}
	}
}