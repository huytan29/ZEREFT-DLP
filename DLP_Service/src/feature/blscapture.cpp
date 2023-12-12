// #include "define_header.h"
#include "../headers/BlScreenlib/blockScreen.h"
#include "../headers/BlScreenlib/blockScreencore.h"
#include "../headers/json.hpp"
#include <cerrno>
#include <codecvt> 

using json = nlohmann::json;

bool IsProcessInConfig(const std::string &processName)
{
    std::ifstream configFile("..\\..\\..\\config\\screen.json");
    if (!configFile.is_open())
    {
        std::cerr << "Failed to open config file." << strerror(errno) << std::endl;
        return false;
    }

    try
    {
        json configData;
        configFile >> configData;

        // Check if the process is in the list
        for (const auto &app : configData)
        {
            if (app["ProcessName"] == processName)
            {
                return true;
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error parsing config file: " << e.what() << std::endl;
    }

    return false;
}

BOOL CALLBACK EnumWindowsProcHide(HWND hWnd, LPARAM lParam)
{
    if (IsWindowVisible(hWnd))
    {
        wchar_t windowTitle[256];
        GetWindowTextW(hWnd, windowTitle, sizeof(windowTitle) / sizeof(wchar_t));

        // Get the process name of the hWnd window
        DWORD processId;
        GetWindowThreadProcessId(hWnd, &processId);
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
        if (hProcess != NULL)
        {
            wchar_t processName[MAX_PATH];
            if (GetProcessImageFileNameW(hProcess, processName, MAX_PATH) > 0)
            {
                std::wstring processNameStr = processName;
                size_t found = processNameStr.find_last_of(L"\\");
                if (found != std::wstring::npos)
                {
                    processNameStr = processNameStr.substr(found + 1); // get file .exe of process
                    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
                    if (IsProcessInConfig(converter.to_bytes(processNameStr)))
                    {
                        // ...
                    }
                }
            }
            CloseHandle(hProcess);
        }
    }
    return TRUE;
}

BOOL CALLBACK EnumWindowsProcUnhide(HWND hWnd, LPARAM lParam)
{
	if (IsWindowVisible(hWnd))
	{
		AdjustDebugPrivs();
		if (SetWindowDisplayAffinityForExternelProcess(hWnd, WDA_NONE))
			Sleep(10);
	}
	return TRUE;
}

//
void CALLBACK TimerCallback(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	EnumWindows(EnumWindowsProcHide, 0);
	std::cout << "_____________DISABLE-SCREEN-SHOT_____________" << std::endl;
}

void Disable_ScreenShot()
{

	UINT_PTR timerId = SetTimer(NULL, 0, 2000, TimerCallback);

	if (timerId == 0)
	{
		std::cerr << "Failed to create timer." << std::endl;
		return;
	}

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	KillTimer(NULL, timerId);
}

void Enable_ScreenShot()
{
	EnumWindows(EnumWindowsProcUnhide, 0);
	std::cout << "_____________ENABLE-SCREEN-SHOT_____________" << std::endl;
}

// main
int blockScreen(int argc, char *argv[])
{
	if (argc < 2)
	{
		std::string x;
		std::cout << "Disable Screenshots (ON/OFF)" << std::endl;
		std::cin >> x;
		if (x == "1" || x == "ON" || x == "On" || x == "on")
		{
			EnumWindows(EnumWindowsProcHide, 0);
			Disable_ScreenShot();
		}
		else if (x == "0" || x == "OFF" || x == "Off" || x == "off")
		{
			EnumWindows(EnumWindowsProcUnhide, 0);
			Enable_ScreenShot();
		}
	}
	else
	{
		if (strcmp(argv[1], "ON") == 0)
		{
			EnumWindows(EnumWindowsProcHide, 0);
			Disable_ScreenShot();
		}
		else if (strcmp(argv[1], "OFF") == 0)
		{
			EnumWindows(EnumWindowsProcUnhide, 0);
			Enable_ScreenShot();
		}
		else
		{
			std::cout << "argument: " << argv[1] << " FAILED !" << std::endl;
		}
	}
	return 1;
}

/*void blockScreen()
{
	std::string x;
	std::cout << "Disable Screenshots (ON/OFF)" << std::endl;
	std::cin >> x;
	if (x == "1" || x == "ON" || x == "On" || x == "on")
	{
		EnumWindows(EnumWindowsProcHide, 0);
		Disable_ScreenShot();
	}
	else if (x == "0" || x == "OFF" || x == "Off" || x == "off")
	{
	   EnumWindows(EnumWindowsProcUnhide, 0);
	   Enable_ScreenShot();
	}

}*/
