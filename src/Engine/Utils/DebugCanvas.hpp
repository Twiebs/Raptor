// #pragma once
//
// #include<string>
//
// #include <Graphics/Camera.hpp>
// #include <Graphics/GL/GLSLProgram.hpp>
//
// #include <UI/Canvas.hpp>
// #include <Assets/Font.hpp>
// #include <UI/Label.hpp>
// #include <UI/Widget.hpp>
// #include <UI/Group.hpp>
//
// #include <Utils/CommonScene.hpp>
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
