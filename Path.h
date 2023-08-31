#pragma once

#include <Windows.h>
#include <string>
#include <filesystem>

static int __dllVar = 0;

static std::filesystem::path getGamePath() {
	return std::filesystem::current_path();
}

static std::filesystem::path getDllPath() {
	char path[MAX_PATH];
	HMODULE hm = NULL;

	if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
		GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
		(LPCSTR)&__dllVar, &hm) == 0) {
		int ret = GetLastError();
		fprintf(stderr, "GetModuleHandle failed, error = %d\n", ret);
		// Return or however you want to handle an error.
	}
	if (GetModuleFileNameA(hm, path, sizeof(path)) == 0) {
		int ret = GetLastError();
		fprintf(stderr, "GetModuleFileName failed, error = %d\n", ret);
		// Return or however you want to handle an error.
	}
	return std::filesystem::path(std::string(path));
}