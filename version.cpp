#include "version.h"
#include <filesystem>
#include <windows.h>
#include <thread>
#include <chrono>

HMODULE version_dll;
HMODULE dummy_dll;


auto dllName = "\\Arial.ttf";
auto gamename1 = "GrandChase.exe";
auto gamename2 = "SO3D.exe";
auto gamename3 = "SO3DPlus.exe";
auto gamename4 = "Main.exe";
auto gamename5 = "Solo_Leveling_ARISE.exe";

#define WRAPPER_FUNC(name) o##name = GetProcAddress(version_dll, ###name)
#define GET_MY_CUSTOM_FUNCTION(type, funcCall, funcName) typedef void (*type)(); type funcCall = (type)GetProcAddress(dummy_dll, #funcName); if(funcCall) funcCall() 


#ifdef _M_AMD64
#pragma warning (disable: 4081)
#define STRINGIFY(name) #name
#define EXPORT_FUNCTION comment(linker, "/EXPORT:" __FUNCTION__ "=" __FUNCDNAME__)
#define WRAPPER_GENFUNC(name) \
    FARPROC o##name; \
    __declspec(dllexport) void WINAPI _##name() \
    { \
        __pragma(STRINGIFY(EXPORT_FUNCTION)); \
        o##name(); \
    }
#else
#define WRAPPER_GENFUNC(name) \
	FARPROC o##name; \
	__declspec(naked) void _##name() \
	{ \
		__asm jmp[o##name] \
	}
#endif

WRAPPER_GENFUNC(GetFileVersionInfoA);
WRAPPER_GENFUNC(GetFileVersionInfoByHandle);
WRAPPER_GENFUNC(GetFileVersionInfoExW);
WRAPPER_GENFUNC(GetFileVersionInfoExA);
WRAPPER_GENFUNC(GetFileVersionInfoSizeA);
WRAPPER_GENFUNC(GetFileVersionInfoSizeExA);
WRAPPER_GENFUNC(GetFileVersionInfoSizeExW);
WRAPPER_GENFUNC(GetFileVersionInfoSizeW);
WRAPPER_GENFUNC(GetFileVersionInfoW);
WRAPPER_GENFUNC(VerFindFileA);
WRAPPER_GENFUNC(VerFindFileW);
WRAPPER_GENFUNC(VerInstallFileA);
WRAPPER_GENFUNC(VerInstallFileW);
WRAPPER_GENFUNC(VerLanguageNameA);
WRAPPER_GENFUNC(VerLanguageNameW);
WRAPPER_GENFUNC(VerQueryValueA);
WRAPPER_GENFUNC(VerQueryValueW);


void new_console() {
	AllocConsole();
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	// solve the disorder codes in the CJK character show
	SetConsoleOutputCP(CP_UTF8);
}

std::filesystem::path getApplicationPath() {
	TCHAR buff[MAX_PATH];
	GetModuleFileName(NULL, buff, MAX_PATH);
	return std::filesystem::path(buff);
}

void load_version() {
	char systemPath[MAX_PATH];
	GetSystemDirectoryA(systemPath, MAX_PATH);
	strcat_s(systemPath, "\\version.dll");
	version_dll = LoadLibraryA(systemPath);	
	if (!version_dll) return;
	
	WRAPPER_FUNC(GetFileVersionInfoA);
	WRAPPER_FUNC(GetFileVersionInfoByHandle);
	WRAPPER_FUNC(GetFileVersionInfoExW);
	WRAPPER_FUNC(GetFileVersionInfoExA);
	WRAPPER_FUNC(GetFileVersionInfoSizeA);
	WRAPPER_FUNC(GetFileVersionInfoSizeExW);
	WRAPPER_FUNC(GetFileVersionInfoSizeExA);
	WRAPPER_FUNC(GetFileVersionInfoSizeW);
	WRAPPER_FUNC(GetFileVersionInfoW);
	WRAPPER_FUNC(VerFindFileA);
	WRAPPER_FUNC(VerFindFileW);
	WRAPPER_FUNC(VerInstallFileA);
	WRAPPER_FUNC(VerInstallFileW);
	WRAPPER_FUNC(VerLanguageNameA);
	WRAPPER_FUNC(VerLanguageNameW);
	WRAPPER_FUNC(VerQueryValueA);
	WRAPPER_FUNC(VerQueryValueW);
}

void load_custom_dll() {
	char customDllPath[MAX_PATH];
	// Replace "your_custom_dll.dll" with the path to your custom DLL
	if (GetModuleFileNameA(NULL, customDllPath, MAX_PATH) != 0)
	{
		// Remove the executable's filename to get the directory
		char* lastSlash = strrchr(customDllPath, '\\');
		if (lastSlash != NULL)
			*lastSlash = '\0';

		// Append the desired filename to the application directory
		strcat_s(customDllPath , dllName);

		// Display a message box with the constructed path
		//MessageBoxA(NULL, customDllPath, "Constructed Path", MB_ICONINFORMATION);
	}
	else
	{
		// Handle the error if the executable file's path cannot be retrieved
		MessageBoxA(NULL, "Failed to get the executable file path.", "Error", MB_ICONERROR);
	}
	dummy_dll = LoadLibraryA(customDllPath);
	if (!dummy_dll) {
		// Handle the error if the custom DLL couldn't be loaded
	}
	// Load additional functions or do any setup here
}


DWORD WINAPI Load(LPVOID lpParam) {
	auto applicationPath = getApplicationPath();
	load_version();
	if (!version_dll)
		return 0;

	if (applicationPath.filename() != gamename3) {
		return 0;
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(7000));
	/*load_version();
	if (!version_dll)
		return 0;*/
	//MessageBox(0, L"Load Helper After Press OK", L"Loader", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
label:
	load_custom_dll(); // Load your custom DLL
	if (!dummy_dll) {
		// Handle the error if your custom DLL couldn't be loaded
		int result = MessageBox(0, L"Not Loaded\nYes to Call Again !", L"DLL Caller", MB_YESNO | MB_ICONINFORMATION | MB_SYSTEMMODAL);

		if (result == IDYES) {
			goto label;
		}
		else if (result == IDCANCEL) {
			return 0;
		}
	}

#ifdef _DEBUG
	MessageBox(NULL, L"It's working!", L"version proxy", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
#endif

	// load some dll
	// Call your custom function from myDLL
	//typedef void (*MyCustomFunctionType)();
	//MyCustomFunctionType myCallFunc = (MyCustomFunctionType)GetProcAddress(dummy_dll, "MyCustomFunction");
	////DUMMY_WRAPPER_FUNC(MyCustomFunction);
	//if (myCallFunc) {
	//	myCallFunc();
	//};;
	//GET_MY_CUSTOM_FUNCTION(MyCustomFunctionType, myCallFunc, MyCustomFunction);


	return 0;
}
