#pragma once

#include<iostream>
#include<string>
#include<stdint.h>

#include<Application\Application.h>
#include<Application\IApplicationStartable.h>
#include<Core\IScene.h>
#include<Core\Types.h>

class IScene;

enum LogLevel {
	LOG_DFEAULT,
	LOG_ERROR_ONLY
};

class Engine : public IApplicationStartable {
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
