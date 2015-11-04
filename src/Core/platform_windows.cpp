#include <Windows.h>
#include <string>

#include <Core/logging.h>
#include <Core/Platform.h>

bool operator==(const FileWriteTime& a, const FileWriteTime& b) {
	bool result = (a.highDateTime == b.highDateTime) && (a.lowDateTime == b.lowDateTime);
	return result;
}

bool operator!=(const FileWriteTime& a, const FileWriteTime& b) {
	bool result = (a.highDateTime != b.highDateTime) || (a.lowDateTime != b.lowDateTime);
	return result;
}

FileWriteTime GetLastModifedTime(const std::string& filename) {
	HANDLE file = CreateFile(
		filename.c_str(),
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);

	if (file == NULL) {
		LOG_ERROR(GetLastError());
	}


	FILETIME lastWriteTime;
	

	if (!GetFileTime(file, NULL, NULL, &lastWriteTime)) {
		LOG_ERROR(GetLastError());
	}

	
	BOOL closeHandleSucuess = CloseHandle(file);
	if (!closeHandleSucuess) {
		LOG_ERROR(GetLastError());
	}

	FileWriteTime result;
	result.lowDateTime = lastWriteTime.dwLowDateTime;
	result.highDateTime = lastWriteTime.dwHighDateTime;
	return result;
}

//CodeModule LoadModule (const std::string& filename) {
//	HMODULE module = LoadLibrary(filename.c_str());
//	if (module == NULL) {
//		LOG_ERROR("Error loading code module: " << filename << GetLastError());
//		return CodeModule { 0 };
//	}
//
//	get_height_fun addr = (get_height_fun)GetProcAddress(module, "get_height");
//
//	CodeModule result = { };
//	result.module = module;
//	result.testfunction = addr;
//	return result;
//}
//
//void UnloadModule(CodeModule* module) { 
//	FreeLibrary(module->module);
//
//
//}