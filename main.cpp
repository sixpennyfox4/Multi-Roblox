#include <Windows.h>
#include <TlHelp32.h>
#include <fstream>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

bool isRobloxRunning() {
	HANDLE hSnapshot{ CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL) };

	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);

	if (Process32First(hSnapshot, &pe32)) {

		do {
			if (std::wstring(pe32.szExeFile) == L"RobloxPlayerBeta.exe") {
				return true;
			}
		} while (Process32Next(hSnapshot, &pe32));

	}

	return false;
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);

		break;

	default:
		return DefWindowProcW(hWnd, msg, wp, lp);
	}
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow) {
	bool closeInsntancesOnAppClose{ true };

	WNDCLASSW winClass{};
	winClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	winClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winClass.hInstance = hInst;
	winClass.lpszClassName = L"MultiRobloxWC";
	winClass.lpfnWndProc = WinProc;

	RegisterClassW(&winClass);

	HWND window{ CreateWindowW(
		L"MultiRobloxWC",
		L"Multi Roblox",
		WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_VISIBLE,
		150,
		150,
		300,
		344,
		NULL,
		NULL,
		NULL,
		NULL
	) };

	HWND mainText{ CreateWindowW(
		L"STATIC",
		L"- This should be opened before roblox!\n\n- By default closing this will forcefully close all opened roblox instances expect one! (you can change that by modifying settings.json file)\n\nSource Code: https://github.com/sixpen\nnyfox4/Multi-Roblox",
		WS_VISIBLE | WS_CHILD,
		20,
		10,
		240,
		320,
		window,
		NULL,
		NULL,
		NULL
	) };

	SendMessage(mainText, WM_SETFONT, (WPARAM)CreateFont(22, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
		VARIABLE_PITCH, TEXT("Arial")), TRUE);

	if (isRobloxRunning())
		MessageBox(window, L"Roblox is running which may cause Multi Roblox to not work!", L"Roblox Running", MB_OK | MB_ICONINFORMATION);

	std::ifstream iF("settings.json");
	if (!iF.fail()) {
		std::string settingsC((std::istreambuf_iterator<char>(iF)), std::istreambuf_iterator<char>());

		json settingsJSON{ json::parse(settingsC) };
		if (settingsJSON.contains("closeInsntancesOnAppClose"))
			closeInsntancesOnAppClose = settingsJSON["closeInsntancesOnAppClose"];
	}
	else {
		MessageBox(window, L"Failed to open settings.json, using default setting! (close instances on Multi Roblox close)", L"Failed to Open Settings File", MB_OK | MB_ICONINFORMATION);
	}

	if (closeInsntancesOnAppClose)
		CreateMutex(NULL, TRUE, L"ROBLOX_singletonMutex");
	else
		CreateMutex(NULL, FALSE, L"ROBLOX_singletonEvent");

	MSG msg{ 0 };
	while (GetMessage(&msg, NULL, NULL, NULL)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}