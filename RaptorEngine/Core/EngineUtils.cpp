#include<iostream>
#include<fstream>

#include<Core/EngineUtils.h>

//TODO Move to custom shader parser
//Research disabling #include error in GLSL
bool ReadFile(const char* filename, string& outFile) {
	ifstream fileStream(filename);

	if (fileStream.is_open()) {
		string line;
		while (getline(fileStream, line)) {
			//Check if the file has a include directive
			if (line.find("#") != std::string::npos) {
				if (line.find("include") != std::string::npos) {
					size_t begin = line.find("<") + 1;
					size_t end = line.find(">") - begin;
					string includeFilename = line.substr(begin, end);
					string includeFile;
					ReadFile(includeFilename.c_str(), includeFile);
					outFile.append(includeFile);
					continue;	//Dont add the include line...
				}
			}
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