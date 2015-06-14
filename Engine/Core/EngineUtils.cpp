#include<iostream>
#include<fstream>

#include<Core/EngineUtils.h>

//TODO Move to custom shader parser
//Research disabling #include error in GLSL
bool ReadFile(const char* filename, string& outFile) {

}



void LOG_DEFAULT(const char* message) {
	cout << message;
}