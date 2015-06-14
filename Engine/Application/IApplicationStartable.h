#pragma once

//Foward decl
class IApplication;

class IApplicationStartable {
public:
	IApplicationStartable();
	~IApplicationStartable();

	virtual void Start(IApplication* app) = 0;
	virtual void Update(float deltaTime) = 0;
};
