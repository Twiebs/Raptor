#include<iostream>
#include<fstream>

#include<Core/EngineUtils.h>

bool ReadFile(const char* filename, string& outFile) {
	ifstream fileStream(filename);

	if (fileStream.is_open()) {
		string line;
		while (getline(fileStream, line)) {
			outFile.append(line);
			outFile.append("\n");
		}

		fileStream.close();
		return true;
	}

	cout << "Error opening file: " << filename;	//XXX Error message
	return false;
}

void LOG_DEFAULT(const char* message) {
	cout << message;
}