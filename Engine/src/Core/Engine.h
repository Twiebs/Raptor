#pragma once

#include<string>
#include<stdint.h>

#include<Core\Engine.inc.h>

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

class Engine : public IApplicationStartable{
public:
	Engine();
	~Engine();

	void LoadScene(IScene* scene);
	static void StartScene (IScene* scene);

	void Start(IApplication* app) override;
	void Update(float deltaTime) override;

	IApplication* GetApp() { return app; }

	//Debug
	void Log(std::string message);
	void Error(std::string message);

private:
	IApplication* app;
	IScene* currentScene;

	LogLevel logLevel;
	bool isRunning = true;
};
