#pragma once

#include <vector>
#include <UI/Widget.hpp>

class Table : public Widget{

public:
	Table();
	~Table();

	void AddWidget(Widget* widget);

private:
	std::vector<Widget*> children;

};
