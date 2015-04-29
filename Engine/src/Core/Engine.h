#pragma once

#include<string>
#include<stdint.h>

#include<Core\Input.h>
#include<Core\Scene.h>

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef unsigned int uint;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef uint32 flag;

class IScene;

enum LogLevel {
	LOG_DFEAULT,
	LOG_ERROR_ONLY
};

class Engine {
public:
	InputProcessor inputProcessor;

	Engine();
	~Engine();

	void LoadScene(IScene* scene);
	static void StartScene (IScene* scene);

	void Update(float deltaTime);

	bool IsRunning() { return isRunning; };
	void Exit();

	//Debug
	void Log(std::string message);
	void Error(std::string message);

private:
	LogLevel logLevel;
	bool isRunning = true;
	IScene* currentScene;
};
