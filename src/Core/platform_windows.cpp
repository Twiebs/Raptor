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

static inline bool IsInvalidHandle(HANDLE handle) {
	if (handle == INVALID_HANDLE_VALUE || handle == NULL) {
		LOG_ERROR(GetLastError());
		return true;
	}
	return false;
}

#define CHECK_HANDLE_ERRORS(handle) if (IsInvalidHandle(handle)) { assert(false && "Invalid WinAPI handle"); }



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

	CHECK_HANDLE_ERRORS(file);

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

#define CHECK_RETURN_ERRORS(function) if (!function) { LOG_ERROR("WinAPI Error: " << GetLastError()); }


void GetFilesInDirectory(const std::string& directory, std::vector<std::string> files, std::vector<std::string>* directories) {
	std::string path = directory + "/*";
	WIN32_FIND_DATA findData;
	HANDLE handle = FindFirstFile(path.c_str(), &findData);
	CHECK_HANDLE_ERRORS(handle);
	do {
		if (strcmp(findData.cFileName, ".") == 0) continue;
		else if (strcmp(findData.cFileName, "..") == 0) continue;

		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			directories->emplace_back(findData.cFileName);
		} else {
			files.emplace_back(findData.cFileName);
		}

	} while (FindNextFile(handle, &findData));


	FindClose(handle);
}


void ListFilesInDirectory(const std::string& directory, unsigned int level) {
	std::string path = directory + "/*";
	WIN32_FIND_DATA findData;
	HANDLE handle = FindFirstFile(path.c_str(), &findData);
	CHECK_HANDLE_ERRORS(handle);

	static auto PrintSpaces = [](int level) {
		for (auto i = 0; i < level; i++) {
			std::cout << "  ";
		}
	};


	do {
		if (strcmp(findData.cFileName, ".") == 0) continue;
		else if (strcmp(findData.cFileName, "..") == 0) continue;

		//PrintSpaces(level);
		//std::cout << findData.cFileName << "\n";
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			ListFilesInDirectory(directory + "/" + findData.cFileName, level + 1);
		}
	} while (FindNextFile(handle, &findData));
	

	FindClose(handle);
}

FileWriteTime GetLastWriteTimeForDirectory(const std::string& path) {
	HANDLE handle = CreateFile(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS, NULL);
	CHECK_HANDLE_ERRORS(handle);
	FILETIME lastWriteTime;
	CHECK_RETURN_ERRORS(GetFileTime(handle, NULL, NULL, &lastWriteTime));
	CHECK_RETURN_ERRORS(CloseHandle(handle));

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