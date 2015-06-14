#include "Table.h"


Table::Table() {
}


Table::~Table() {
}

void Table::AddWidget(Widget* widget) {
	widget->SetParent(this);
	children.push_back(widget);
}
