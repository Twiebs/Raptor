// #pragma once
//
// #include<string>
//
// #include <Graphics/Camera.h>
// #include <Graphics/GL/GLSLProgram.h>
//
// #include <UI/Canvas.h>
// #include <Assets/Font.hpp>
// #include <UI/Label.h>
// #include <UI/Widget.h>
// #include <UI/Group.h>
//
// #include <Utils/CommonScene.h>
//
// class DebugCanvas : public Canvas{
// public:
// 	Camera* camera;
//
// 	DebugCanvas(float width, float height);
// 	~DebugCanvas();
//
// 	void Setup() override;
// 	void Update(CommonScene* scene, float deltaTime);
//
// private:
// 	Font* font;
//
// 	VerticalGroup group;
// 	Label positionLabel;
// 	Label rotationLabel;
// 	Label fpsLabel;
// 	Label vertLabel;
//
// 	void Update(float deltaTime) override;
// };
