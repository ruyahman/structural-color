#include "systemclass.h"
#include <exception>

int WINAPI WinMain(HINSTANCE, HINSTANCE, PSTR, int)
{
    // SystemClass owns startup/shutdown, including partial initialization failures.
    SystemClass application;
    try
    {
        if (!application.Initialize())
            return 1;
        application.Run();
    }
    catch (const std::exception& error)
    {
        MessageBoxA(nullptr, error.what(), "Diff", MB_OK | MB_ICONERROR);
        return 1;
    }
    return 0;
}
